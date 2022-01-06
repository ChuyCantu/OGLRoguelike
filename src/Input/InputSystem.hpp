#ifndef __INPUTSYSTEM_H__
#define __INPUTSYSTEM_H__

#include <glm/vec2.hpp>
#include <SDL.h>
#include <unordered_map>
#include <vector>

enum class ButtonState {
    None,
    Pressed,
    Released,
    Held
};

class KeyboardState {
private:
    // Current state
    const uint8_t* currState;
    // State previous frame
    uint8_t prevState[SDL_NUM_SCANCODES];

public:
    // Get just the boolean true/false value of key
    bool GetKeyValue(SDL_Scancode keyCode) const;

    // Get a state based on current and previous frame
    ButtonState GetKeyState(SDL_Scancode keyCode) const;

    // Returns the first key pressed
    // Example:  if (isWaitingForInput && !state.Keyboard.GetKeyValue(rebindCommandScancode))
    static bool DetectScancode(SDL_Scancode* retScancode);

    // Friend so InputSystem can easily update it
    friend class InputSystem;
};

class MouseState {
private:
    // Store mouse position
    glm::vec2 mousePos;
    // Store button data
    uint32_t currButtons;
    uint32_t prevButtons;

    bool isRelative{false};
    glm::vec2 scrollWheel;

public:
    // For mouse position
    const glm::vec2& GetPosition() const { return mousePos; }
    bool IsRelative() const { return isRelative; }

    // For buttons
    bool GetButtonValue(int button) const;
    ButtonState GetButtonState(int button) const;

    const glm::vec2& GetScrollValue() const { return scrollWheel; }

    friend class InputSystem;
};

class ControllerState {
   private:
    // Current/previous buttons
    uint8_t currButtons[SDL_CONTROLLER_BUTTON_MAX];
    uint8_t prevButtons[SDL_CONTROLLER_BUTTON_MAX];
    // Is this controller connected?
    bool isConnected;

    float leftTrigger;
    float rightTrigger;

    glm::vec2 leftStick;
    glm::vec2 rightStick;

public:
    // For buttons
    bool GetButtonValue(SDL_GameControllerButton button) const;
    ButtonState GetButtonState(SDL_GameControllerButton button) const;

    bool IsConnected() const { return isConnected; }
    float GetLeftTrigger() const { return leftTrigger; }
    float GetRightTrigger() const { return rightTrigger; }
    const glm::vec2& GetLeftStick() const { return leftStick; }
    const glm::vec2& GetRightStick() const { return rightStick; }

    friend class InputSystem;
};

// Wrapper that contains current state of input
struct InputState {
    KeyboardState Keyboard;
    MouseState Mouse;
    ControllerState Controller;
};

class InputSystem {
private:
    InputState state;
    SDL_GameController* controller;
    //* For multiple controllers
    // std::vector<SDL_GameController*> controllers;
    std::unordered_map<std::string, SDL_Scancode> keyActions;

public:
    ~InputSystem();

    bool Initialize();
    void Shutdown();

    // Called right before SDL_PollEvents loop
    void PrepareForUpdate();
    // Called right after SDL_PollEvents loop
    void Update();
    void ProcessEvent(SDL_Event& event);

    // For controller filtering...
    float Filter1D(int input);
    glm::vec2 Filter2D(int inputX, int inputY);

    const InputState& GetState() const { return state; }

    void SetRelativeMouseMode(bool value);

    // For a better system, Input class should be static or either game should
    // have a function to return a ref
    // this way we can rebind the keys not only from UpdateInput()
    ButtonState GetActionButtonState(const std::string& name);
    const SDL_Scancode GetActionScancode(const std::string& name);
    void AddKeyAction(const std::string& name, SDL_Scancode scancode);
    void AddKeyAction(const std::string& name, const std::string& scancodeName);
    void RemoveKeyAction(const std::string& name);
    void RemapActionKey(const std::string& name, SDL_Scancode newScancode);
    void RemapActionKey(const std::string& name, const std::string& newScancodeName);
};

#endif  // __INPUTSYSTEM_H__