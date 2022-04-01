#include "Action.hpp"

#include "Constants.hpp"
#include "Core/Log.hpp"
#include "Core/GameObject.hpp"
#include "DungeonGen/Dungeon.hpp"
#include "TurnManager.hpp"


//+ Action =============================================
Action::Action(Unit* owner) 
    : owner{owner} {
}

void Action::Cancel() {
    canceled = true;
}

//+ SkipAction ==========================================
SkipAction::SkipAction(Unit* owner) : Action{owner} {
    isCompleted = true;
    cost = owner->GetComponent<UnitComponent>().GetSpeed();
}

void SkipAction::Update() {
    LOG_TRACE("{} skipped turn.", owner->name);
}

//+ MoveAction =========================================
MoveAction::MoveAction(Unit* owner, const glm::vec3& destination, float duration) : Action{owner}, destination{destination}, duration{duration} {
    cost = 100;

    auto& move {owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Subscribe("OnDestinationReached", &MoveAction::OnDestinationReached, this);
    move.onCancelation.Subscribe("OnMoveCanceled", &MoveAction::OnMoveCanceled, this);

    // LOG_TRACE("Move created");
}

MoveAction::MoveAction(Unit* owner, const glm::vec2& destination, float duration) : Action{owner}, destination{glm::vec3{destination.x, destination.y, 0.0f}}, duration{duration} {
    cost = 100;

    auto& move{owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Subscribe("OnDestinationReached", &MoveAction::OnDestinationReached, this);
    move.onCancelation.Subscribe("OnMoveCanceled", &MoveAction::OnMoveCanceled, this);

    // LOG_TRACE("Move created");
}

MoveAction::~MoveAction() {
    auto& move{owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Unsubscribe("OnDestinationReached", this);
    move.onCancelation.Unsubscribe("OnMoveCanceled", this);

    // LOG_TRACE("Move deleted");
}

void MoveAction::OnStart()  {
    owner->GetComponent<MoveComponent>().Move(destination, duration);
    
    // LOG_TRACE("{}", owner->name);
}

void MoveAction::OnDestinationReached() {
    isCompleted = true;
}

void MoveAction::OnMoveCanceled() {
    canceled = true;
}

//+ MoveUnitAction =========================================
MoveUnitAction::MoveUnitAction(Unit* owner, const glm::ivec2& destination, float duration, Dungeon* dungeon
#ifdef ALTERNATIVE
    , DiagonalMovement diagonalMovement
#endif
    ) 
    : Action{owner}, tileDest{destination}, duration{duration}, dungeon{dungeon}
#ifdef ALTERNATIVE
    , diagonalMovement{diagonalMovement}
#endif
    {
    cost = 100;

    auto& move{owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Subscribe("OnDestinationReached", &MoveUnitAction::OnDestinationReached, this);
    move.onCancelation.Subscribe("OnMoveCanceled", &MoveUnitAction::OnMoveCanceled, this);
}

MoveUnitAction::~MoveUnitAction() {
    auto& move{owner->GetComponent<MoveComponent>()};
    move.onDestinationReached.Unsubscribe("OnDestinationReached", this);
    move.onCancelation.Unsubscribe("OnMoveCanceled", this);
}

void MoveUnitAction::OnStart()  {
    auto& destinationNode {dungeon->GetNode(tileDest.x, tileDest.y)};

    auto& ownerTransform {owner->GetComponent<Transform>()};
    auto& currPos {ownerTransform.GetPosition()};
    glm::ivec2 tilePos {static_cast<int>(currPos.x) / TILE_SIZE, static_cast<int>(currPos.y) / TILE_SIZE};


    if ((tileDest - tilePos).x > 0) {
        auto& scale {ownerTransform.GetScale()};
        ownerTransform.SetScale(glm::vec3{-1.0f, scale.y, scale.z});
    }
    else {
        auto& scale {ownerTransform.GetScale()};
        ownerTransform.SetScale(glm::vec3{1.0f, scale.y, scale.z});
    }

#ifdef ALTERNATIVE
    auto direction {tileDest - tilePos};

    if (destinationNode.unit || dungeon->pathfinding.GetNode(tileDest).isObstacle) {  // In the future this may change if any entity can go through obstacles
        // canceled = true;
        // onMoveActionCanceled.Invoke();
        isCompleted = true;
        cost = owner->GetComponent<UnitComponent>().GetSpeed();
        return;
    }

    bool canAdvance {true};
        
    if ((diagonalMovement != DiagonalMovement::Never || diagonalMovement != DiagonalMovement::Always)
        && direction.x != 0 && direction.y != 0) {  // diagonal movement
        AStar::Node& n1 {dungeon->pathfinding.GetNode(glm::ivec2{tilePos.x, tileDest.y})};
        AStar::Node& n2 {dungeon->pathfinding.GetNode(glm::ivec2{tileDest.x, tilePos.y})};

        switch (diagonalMovement) {
            case DiagonalMovement::AllowOneObstacle:
                if (n1.isObstacle && n2.isObstacle)
                    canAdvance = false;
                break;
            case DiagonalMovement::OnlyWhenNoObstacles:
                if (n1.isObstacle || n2.isObstacle)
                    canAdvance = false;
                break;
        }
    }

    if (canAdvance) {
#endif
        dungeon->GetNode(tilePos.x, tilePos.y).unit = nullptr;
        destinationNode.unit = owner;
        glm::vec3 destination {static_cast<float>(tileDest.x * TILE_SIZE), 
                               static_cast<float>(tileDest.y * TILE_SIZE), 0.0f};
        owner->GetComponent<MoveComponent>().Move(destination, duration);
        completeAsync = true;
#ifdef ALTERNATIVE
    }
    else { 
        isCompleted = true;
        cost = owner->GetComponent<UnitComponent>().GetSpeed();
        // canceled = true;
        // onMoveActionCanceled.Invoke();
    }
#endif
}

void MoveUnitAction::OnDestinationReached() {
    isCompleted = true;
    onMoveActionCompleted.Invoke();
}

void MoveUnitAction::OnMoveCanceled() {
    canceled = true;
    // auto& destinationNode {dungeon->GetNode(destination.x / TILE_SIZE, destination.y / TILE_SIZE)};
    auto& destinationNode{dungeon->GetNode(tileDest.x, tileDest.y)};

    if (destinationNode.unit == owner) {
        destinationNode.unit = nullptr;
        
        auto& srcPos {owner->GetComponent<MoveComponent>().GetSrcPosition()};
        auto& srcNode {dungeon->GetNode(static_cast<int>(srcPos.x) / TILE_SIZE, static_cast<int>(srcPos.y) / TILE_SIZE)};
        srcNode.unit = owner;
    }
    onMoveActionCanceled.Invoke();
}