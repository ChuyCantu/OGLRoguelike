#include "Input.hpp"

Owned<InputSystem> Input::system {MakeOwned<InputSystem>()};

bool Input::GetKey(SDL_Scancode key) {
    return system->GetState().Keyboard.GetKeyValue(key);
}

bool Input::GetKeyDown(SDL_Scancode key) {
    return system->GetState().Keyboard.GetKeyState(key) == ButtonState::Pressed;
}

bool Input::GetKeyUp(SDL_Scancode key) {
    return system->GetState().Keyboard.GetKeyState(key) == ButtonState::Released;
}


bool Input::GetMouseButton(int button) {
    return system->GetState().Mouse.GetButtonValue(button);
}

bool Input::GetMouseButtonDown(int button) {
    return system->GetState().Mouse.GetButtonState(button) == ButtonState::Pressed;
}

bool Input::GetMouseButtonUp(int button) {
    return system->GetState().Mouse.GetButtonState(button) == ButtonState::Released;
}

glm::ivec2 Input::GetMousePosition() {
    return system->GetState().Mouse.GetPosition();
}

glm::vec2 Input::GetMouseWheel() {
    return system->GetState().Mouse.GetScrollValue();
}

void Input::SetMouseRelativeMode(bool value) {
    system->SetRelativeMouseMode(value);
}

bool Input::DetectKeyPress(SDL_Scancode* scancode) { 
    return system->GetState().Keyboard.DetectScancode(scancode);
}