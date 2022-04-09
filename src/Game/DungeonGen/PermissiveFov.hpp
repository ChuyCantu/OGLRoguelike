#pragma once

#include <glm/vec2.hpp>
#include <list>

class FovMap;

// Jonathon Duerig algorithm http://www.roguebasin.com/index.php/Precise_Permissive_Field_of_View
class PermissiveFov {
public:
    struct Line {
        glm::ivec2 nearPoint {0, 0};
        glm::ivec2 farPoint  {0, 0};

        // negative if the line is above the point.
        // positive if the line is below the point.
        // 0 if the line is on the point.
        int RelativeSlope(const glm::ivec2& point);
        bool IsBelow(const glm::ivec2& point);
        bool IsBelowOrContains(const glm::ivec2& point);
        bool IsAbove(const glm::ivec2& point);
        bool IsAboveOrContains(const glm::ivec2& point);
        bool Contain(const glm::ivec2& point);
    };

    struct Bump {
        glm::ivec2 position;
        Bump* parent {nullptr};
    };

    struct Field {
        Line steep {};
        Line shallow {};
        Bump* steepBump {nullptr};
        Bump* shallowBump {nullptr};
    };

    void ComputeFov(const glm::ivec2& origin, int radius, int fullyLitRadius, FovMap& map);

   private:
    void CalculateQuadrant();

    void VisitSquare(const glm::ivec2& dest, std::list<Field>::iterator& currentField, 
                     std::list<Bump>& steepBumps, std::list<Bump>& shallowBumps,
                     std::list<Field>& activeFields);
    std::list<Field>::iterator CheckField(std::list<Field>::iterator currentField, 
                                          std::list<Field>& activeFields);
    void AddShallowBump(const glm::ivec2& point, std::list<Field>::iterator currentField, 
                        std::list<Bump> & steepBumps, std::list<Bump> & shallowBumps);
    void AddSteepBump(const glm::ivec2& point, std::list<Field>::iterator currentField, 
                      std::list<Bump>& steepBumps, std::list<Bump>& shallowBumps);
    bool ActIsBlocked(const glm::ivec2& pos);

private: 
    glm::ivec2 source;
    glm::ivec2 quadrant;
    int radius;
    int fullyLitRadius;
    FovMap* map;
};

// Other implementations worth studying: https://github.com/libtcod/libtcod