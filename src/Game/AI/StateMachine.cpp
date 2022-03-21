#include "StateMachine.hpp"

void StateMachine::AddState(Ref<State> state) {
    states[state->GetName()] = state;
}

void StateMachine::RemoveState(Ref<State> state) {
    auto iter {states.find(state->GetName())};
    if (iter != states.end())
        states.erase(iter);
}

void StateMachine::RemoveState(const std::string& stateName) {
    auto iter {states.find(stateName)};
    if (iter != states.end())
        states.erase(iter);
}

void StateMachine::AddTransition(Ref<State> from, Ref<State> to, std::function<bool()> condition) {
    auto iter {transitions.find(from->GetName())};
    if (iter != transitions.end())
        iter->second.emplace_back(Transition{to->GetName(), condition});
    else 
        transitions.emplace(from->GetName(), std::vector{Transition{to->GetName(), condition}});
}

void StateMachine::AddAnyTransition(Ref<State> to, std::function<bool()> condition) {
    anyTransitions.emplace_back(Transition{to->GetName(), condition});
}

void StateMachine::ClearStates() {
    states.clear();
}

void StateMachine::ClearTransitions() {
    transitions.clear();
}

void StateMachine::ClearAnyTransitions() {
    anyTransitions.clear();
}

void StateMachine::Update() {
    Ref<State> state;
    if (CheckStateTransition(state))
        SetState(state);

    if (currentState) currentState->Update();
}

void StateMachine::SetState(Ref<State> state) {
    if (state == currentState)
        return;

    if (currentState) currentState->OnExit();

    currentState = state;

    if (currentState) currentState->OnEnter();

    auto iter {transitions.find(currentState->GetName())};
    if (iter != transitions.end())
        currentTransitions = &iter->second;
    else
        currentTransitions = nullptr;
}

void StateMachine::SetState(const std::string& stateName) {
    Ref<State> state;
    auto siter {states.find(stateName)};
    if (siter != states.end())
        state = siter->second;

    if (state == currentState)
        return;

    if (currentState) currentState->OnExit();

    currentState = state;

    if (currentState) currentState->OnEnter();

    auto titer {transitions.find(currentState->GetName())};
    if (titer != transitions.end())
        currentTransitions = &titer->second;
    else
        currentTransitions = nullptr;
}

bool StateMachine::CheckStateTransition(Ref<State>& outState) {
    for (Transition transition : anyTransitions) {
        if (transition.condition()) {
            auto iter {states.find(transition.to)};
            if (iter != states.end()) {
                outState = iter->second;
                return true;
            }
        }
    }

    if (currentTransitions) {
        for (Transition transition : *currentTransitions) {
            if (transition.condition()) {
                auto iter{states.find(transition.to)};
                if (iter != states.end()) {
                    outState = iter->second;
                    return true;
                }
            }
        }
    }

    outState = nullptr;
    return false;
}