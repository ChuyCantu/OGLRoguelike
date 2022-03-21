#ifndef __STATEMACHINE_H__
#define __STATEMACHINE_H__

#include "Common.hpp"
#include "State.hpp"

#include <functional>

struct Transition {
    std::string_view to;
    std::function<bool()> condition;
};

class StateMachine {
public:
    void AddState(Ref<State> state);
    void RemoveState(Ref<State> state);
    void RemoveState(const std::string& stateName);
    void AddTransition(Ref<State> from, Ref<State> to, std::function<bool()> condition);
    void AddAnyTransition(Ref<State> to, std::function<bool()> condition);
    void ClearStates();
    void ClearTransitions();
    void ClearAnyTransitions();

    void Update();
    void SetState(Ref<State> state);
    void SetState(const std::string& stateName);

    const Ref<State> GetCurrentState() const { return currentState; }

private:
    bool CheckStateTransition(Ref<State>& outState);

private:
    Ref<State> currentState {nullptr};
    std::vector<Transition>* currentTransitions {nullptr};

    std::unordered_map<std::string_view, Ref<State>> states;
    std::unordered_map<std::string_view, std::vector<Transition>> transitions;
    std::vector<Transition> anyTransitions;
};

#endif // __STATEMACHINE_H__