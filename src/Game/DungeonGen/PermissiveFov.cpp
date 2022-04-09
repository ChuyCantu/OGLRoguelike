#include "PermissiveFov.hpp"

#include "Game/Constants.hpp"
#include "Fov.hpp"
#include "Utils/MathExtras.hpp"

#include <glm/gtx/norm.hpp>

int PermissiveFov::Line::RelativeSlope(const glm::ivec2& point) {
    //    if (point.x >= farPoint.x)
    //    {
    //      return (farPoint.y - nearPoint.y)*(point.x - nearPoint.x) - (point.y - nearPoint.y)*(farPoint.x - nearPoint.x);
    //    }
    //    else
    //    {
    return (farPoint.y - nearPoint.y) * (farPoint.x - point.x) - (farPoint.y - point.y) * (farPoint.x - nearPoint.x);
    //    }
}

bool PermissiveFov::Line::IsBelow(const glm::ivec2& point) {
    return RelativeSlope(point) > 0;
}

bool PermissiveFov::Line::IsBelowOrContains(const glm::ivec2& point) {
    return RelativeSlope(point) >= 0;
}

bool PermissiveFov::Line::IsAbove(const glm::ivec2& point) {
    return RelativeSlope(point) < 0;
}

bool PermissiveFov::Line::IsAboveOrContains(const glm::ivec2& point) {
    return RelativeSlope(point) <= 0;
}

bool PermissiveFov::Line::Contain(const glm::ivec2& point) {
    return RelativeSlope(point) == 0;
}

void PermissiveFov::ComputeFov(const glm::ivec2& origin, int radius, int fullyLitRadius, FovMap& map) {
    this->source = origin;
    this->radius = radius;
    this->fullyLitRadius = fullyLitRadius;
    this->map = &map;

    map.minAffectedTile = map.GetSize();
    map.maxAffectedTile = glm::ivec2{-1, -1};

    static const std::vector<glm::ivec2> quadrants {
        glm::ivec2{ 1,  1},
        glm::ivec2{-1,  1},
        glm::ivec2{-1, -1},
        glm::ivec2{ 1, -1}
    };

    for (auto& quadrant : quadrants) {
        this->quadrant = quadrant;
        CalculateQuadrant();
    }
}

void PermissiveFov::CalculateQuadrant() {
    std::list<Bump> steepBumps;
    std::list<Bump> shallowBumps;
    std::list<Field> activeFields; // sorted from shallow-to-steep
    activeFields.push_back(Field{});
    activeFields.back().steep.nearPoint = glm::ivec2{1, 0};
    activeFields.back().steep.farPoint = glm::ivec2{0, 100};
    activeFields.back().shallow.nearPoint = glm::ivec2{0, 1};
    activeFields.back().shallow.farPoint = glm::ivec2{100, 0};

    auto currentField {activeFields.begin()};

    glm::ivec2 dest {0, 0};
    ActIsBlocked(dest);
    int distance = 1;
    int i = 0;
    int j = 0;
    // For each square outline
    for (i = 1; i < 100 && !activeFields.empty(); ++i) {
        // Visit the nodes in the outline
        for (j = 0; j <= i; ++j) {
            dest.x = i - j;
            dest.y = j;
            VisitSquare(dest, currentField, steepBumps, shallowBumps, activeFields);
        }
        currentField = activeFields.begin();
    }
}

void PermissiveFov::VisitSquare(const glm::ivec2& dest, std::list<Field>::iterator& currentField,
                 std::list<Bump>& steepBumps, std::list<Bump>& shallowBumps,
                 std::list<Field>& activeFields) {
    // The top-left and bottom-right corners of the destination square.
    glm::ivec2 topLeft {dest.x, dest.y + 1};
    glm::ivec2 bottomRight {dest.x + 1, dest.y};
    while (currentField != activeFields.end() && currentField->steep.IsBelowOrContains(bottomRight)) {
        // The square is in case 'above'. This means that it is ignored for the currentField. But the steeper fields might need it.
        ++currentField;
    }
    if (currentField == activeFields.end()) {
        // case ABOVE
        // The square was in case 'above' for all fields. This means that we no longer care about it or any squares in its diagonal rank.
        // TODO: Add notification here for optimization purposes.
        return;
    }

    // Now we check for other cases.
    if (currentField->shallow.IsAboveOrContains(topLeft)) {
        // case BELOW
        // The shallow line is above the extremity of the square, so that square is ignored.
        return;
    }
    // The square is between the lines in some way. This means that we need to visit it and determine whether it is blocked.
    bool isBlocked = ActIsBlocked(dest);
    if (!isBlocked) {
        // We don't care what case might be left, because this square does not obstruct.
        return;
    }

    if (currentField->shallow.IsAbove(bottomRight) && currentField->steep.IsBelow(topLeft)) {
        // case BLOCKING
        // Both lines intersect the square. This current field has ended.
        currentField = activeFields.erase(currentField);
    } else if (currentField->shallow.IsAbove(bottomRight)) {
        // case SHALLOW BUMP
        // The square intersects only the shallow line.
        AddShallowBump(topLeft, currentField, steepBumps, shallowBumps);
        currentField = CheckField(currentField, activeFields);
    } else if (currentField->steep.IsBelow(topLeft)) {
        // case STEEP BUMP
        // The square intersects only the steep line.
        AddSteepBump(bottomRight, currentField, steepBumps, shallowBumps);
        currentField = CheckField(currentField, activeFields);
    } else {
        // case BETWEEN
        // The square intersects neither line. We need to split into two fields.
        std::list<Field>::iterator steeperField = currentField;
        std::list<Field>::iterator shallowerField = activeFields.insert(currentField, *currentField);
        AddSteepBump(bottomRight, shallowerField, steepBumps, shallowBumps);
        CheckField(shallowerField, activeFields);
        AddShallowBump(topLeft, steeperField, steepBumps, shallowBumps);
        currentField = CheckField(steeperField, activeFields);
    }
}

std::list<PermissiveFov::Field>::iterator PermissiveFov::CheckField(std::list<Field>::iterator currentField,
                                                                    std::list<Field>& activeFields) {
    std::list<Field>::iterator result = currentField;
    // If the two slopes are colinear, and if they pass through either extremity, remove the field of view.
    if (currentField->shallow.Contain(currentField->steep.nearPoint) 
        && currentField->shallow.Contain(currentField->steep.farPoint) 
        && (currentField->shallow.Contain(glm::ivec2{0, 1}) 
        || currentField->shallow.Contain(glm::ivec2{1, 0}))) {
        result = activeFields.erase(currentField);
    }
    return result;
}

void PermissiveFov::AddShallowBump(const glm::ivec2& point, std::list<Field>::iterator currentField,
                    std::list<Bump>& steepBumps, std::list<Bump>& shallowBumps) {
    // First, the far point of shallow is set to the new point.
    currentField->shallow.farPoint = point;
    // Second, we need to add the new bump to the shallow bump list for future steep bump handling.
    shallowBumps.push_back(Bump());
    shallowBumps.back().position = point;
    shallowBumps.back().parent = currentField->shallowBump;
    currentField->shallowBump = &shallowBumps.back();
    // Now we have too look through the list of steep bumps and see if any of them are below the line.
    // If there are, we need to replace near point too.
    Bump* currentBump = currentField->steepBump;
    while (currentBump != NULL) {
        if (currentField->shallow.IsAbove(currentBump->position)) {
            currentField->shallow.nearPoint = currentBump->position;
        }
        currentBump = currentBump->parent;
    }
}

void PermissiveFov::AddSteepBump(const glm::ivec2& point, std::list<Field>::iterator currentField,
                  std::list<Bump>& steepBumps, std::list<Bump>& shallowBumps) {
    currentField->steep.farPoint = point;
    steepBumps.push_back(Bump());
    steepBumps.back().position = point;
    steepBumps.back().parent = currentField->steepBump;
    currentField->steepBump = &steepBumps.back();
    // Now look through the list of shallow bumps and see if any of them are below the line.
    Bump* currentBump = currentField->shallowBump;
    while (currentBump != NULL) {
        if (currentField->steep.IsBelow(currentBump->position)) {
            currentField->steep.nearPoint = currentBump->position;
        }
        currentBump = currentBump->parent;
    }
}

bool PermissiveFov::ActIsBlocked(const glm::ivec2& pos) {
    glm::vec2 posf {pos};
    // float distance {glm::distance(std::max(posf.x, posf.y), std::min(posf.x, posf.y))};
    // if (radius >= 0 && distance > radius) 
    float distance {glm::distance2(std::max(posf.x, posf.y), std::min(posf.x, posf.y))};
    if (radius >= 0 && distance > radius * radius) 
        return true;
    int x = pos.x * quadrant.x + source.x;
    int y = pos.y * quadrant.y + source.y;
    
    auto node {map->TryGetNode(x, y)};
    if (node) {
        node->lightLevel = 1.0f - SmoothStep(glm::distance2(glm::vec2{x, y}, glm::vec2{source}), 
                                             fullyLitRadius * fullyLitRadius, radius * radius) 
                                             * (1.0f - REVEALED_LIGHT_LEVEL + 0.01f); // the + 0.01 offset helps to differentiate tiles that should not be visible

        if (node->lightLevel <= REVEALED_LIGHT_LEVEL) {
            node->visible = false;
            node->lightLevel = 0.0f;
        }
        else {
            node->visible = true;
            node->revealed = true;

            map->minAffectedTile.x = std::min(map->minAffectedTile.x, x);
            map->minAffectedTile.y = std::min(map->minAffectedTile.y, y);
            map->maxAffectedTile.x = std::max(map->maxAffectedTile.x, x);
            map->maxAffectedTile.y = std::max(map->maxAffectedTile.y, y);
        }

        return node->blocksLight;
    }

    return true;
    // glm::ivec2 adjustedPos{pos.x * quadrant.x + source.x,
    //                        pos.y * quadrant.y + source.y};
    // return g::currentMap == NULL || action(source, adjustedPos) || (*g::currentMap)[adjustedPos].place.blocked;
}