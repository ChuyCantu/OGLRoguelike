#pragma once

#include <string>

class State {
public:
    State(const std::string& name) : name{name} { }

    virtual void OnEnter() { }
    virtual void Update()  { }
    virtual void OnExit()  { }

    const std::string& GetName() const { return name; }

protected:
    std::string name;
};