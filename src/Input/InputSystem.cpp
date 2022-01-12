#include "InputSystem.hpp"

#include <glm/common.hpp>

bool KeyboardState::GetKeyValue(SDL_Scancode keyCode) const {
    if (currState[keyCode] == 1)
        return true;

    return false;
}

ButtonState KeyboardState::GetKeyState(SDL_Scancode keyCode) const {
    if (prevState[keyCode] == 0) {
        if (currState[keyCode] == 0)
            return ButtonState::None;
        else
            return ButtonState::Pressed;
    } else {  // Prev state must be 1
        if (currState[keyCode] == 0)
            return ButtonState::Released;
        else
            return ButtonState::Held;
    }
}

bool KeyboardState::DetectScancode(SDL_Scancode* retScancode) {
    int numKeys;
    auto keys{SDL_GetKeyboardState(&numKeys)};
    if (keys) {
        for (int i{0}; i < numKeys; ++i) {
            if (keys[i] == 1) {
                *retScancode = static_cast<SDL_Scancode>(i);
                return true;
            }
        }
    }
    *retScancode = SDL_SCANCODE_UNKNOWN;
    return false;
}

bool MouseState::GetButtonValue(int button) const {
    return (SDL_BUTTON(button) & currButtons) != 0;
}

ButtonState MouseState::GetButtonState(int button) const {
    int mask = SDL_BUTTON(button);
    if ((mask & prevButtons) == 0) {
        if ((mask & currButtons) == 0)
            return ButtonState::None;
        else
            return ButtonState::Pressed;
    } 
    else {
        if ((mask & currButtons) == 0)
            return ButtonState::Released;
        else
            return ButtonState::Held;
    }
}

bool InputSystem::Initialize() {
    // Assign current state pointer
    state.Keyboard.currState = SDL_GetKeyboardState(nullptr);
    // Clear previous state memory
    memset(state.Keyboard.prevState, 0, SDL_NUM_SCANCODES);

    // Mouse (just set everything to 0)
    state.Mouse.currButtons = 0;
    state.Mouse.prevButtons = 0;

    // Controller
    // Try to open controller 0
    controller = SDL_GameControllerOpen(0);

    //* For multiple controllers:
    // for (int i {0}; i < SDL_NumJoysticks(); ++i) {
    //     // Is this joystick a controller
    //     if (SDL_IsGameController(i)) {
    //         // Open this controller for use
    //         SDL_GameController* c {SDL_GameControllerOpen(i)};
    //         // Add to vector of SDL_GameController* pointers
    //         controllers.push_back(c);
    //     }
    // }
    //! For hot swapping SDL generates 2 different events
    //* SDL_CONTROLLERDEVICEADDED and SDL_CONTROLLERDEVICEREMOVED

    state.Controller.isConnected = controller != nullptr;
    memset(state.Controller.currButtons, 0, SDL_CONTROLLER_BUTTON_MAX);
    memset(state.Controller.prevButtons, 0, SDL_CONTROLLER_BUTTON_MAX);

    return true;
}

InputSystem::~InputSystem() {
    Shutdown();
}

void InputSystem::Shutdown() {
}

void InputSystem::PrepareForUpdate() {
    // Copy all "current" keyboard data
    memcpy(state.Keyboard.prevState, state.Keyboard.currState, SDL_NUM_SCANCODES);

    // Copies current mouse button state to prev state
    state.Mouse.prevButtons = state.Mouse.currButtons;
    //! Set isRelative to false?
    // state.Mouse.isRelative = false;
    state.Mouse.scrollWheel = glm::vec2{0};

    // Controller
    memcpy(state.Controller.prevButtons, state.Controller.currButtons, SDL_CONTROLLER_BUTTON_MAX);
}

void InputSystem::Update() {
    // Mouse
    int x = 0, y = 0;
    state.Mouse.currButtons = state.Mouse.isRelative
                                  ? SDL_GetRelativeMouseState(&x, &y)
                                  : SDL_GetMouseState(&x, &y);
    state.Mouse.mousePos.x = x;
    state.Mouse.mousePos.y = y;

    // Controller
    for (int i{0}; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
        state.Controller.currButtons[i] = SDL_GameControllerGetButton(controller,
                                                                      SDL_GameControllerButton(i));
    }

    state.Controller.leftTrigger = Filter1D(
        SDL_GameControllerGetAxis(controller,
                                  SDL_CONTROLLER_AXIS_TRIGGERLEFT));
    state.Controller.leftTrigger = Filter1D(
        SDL_GameControllerGetAxis(controller,
                                  SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

    x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX);
    y = -SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY);
    state.Controller.leftStick = Filter2D(x, y);

    x = SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX);
    y = -SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY);
    state.Controller.rightStick = Filter2D(x, y);
}

void InputSystem::ProcessEvent(SDL_Event& event) {
    switch (event.type) {
        case SDL_MOUSEWHEEL:
            state.Mouse.scrollWheel = glm::vec2 {
                static_cast<float>(event.wheel.x),
                static_cast<float>(event.wheel.y)
            };
            break;
        default:
            break;
    }
}

void InputSystem::SetRelativeMouseMode(bool value) {
    SDL_SetRelativeMouseMode(value ? SDL_TRUE : SDL_FALSE);
    state.Mouse.isRelative = value;
    // Make initial call to get relative to clear out
    if (value)
        SDL_GetRelativeMouseState(nullptr, nullptr);
}

float InputSystem::Filter1D(int input) {
    //* In theory, analog inputs ranges from -32,768 to 32,767
    // A value < dead zone is interpreted as 0%
    const int deadZone{250};  // TODO: Make deadZone a user configuration
    // A value > max value is interpreted as 100%
    const int maxValue{30000};

    float retVal{0.f};

    // Take absolute value of input
    int absValue{input > 0 ? input : -input};
    // Ignore input within dead zone
    if (absValue > deadZone) {
        // Compute fractional value between dead zone and max value
        retVal = static_cast<float>(absValue - deadZone) /
                 (maxValue - deadZone);

        // Make sure sign matches original value
        retVal = input > 0 ? retVal : -1.f * retVal;

        // Clamp between -1.f and 1.f
        retVal = glm::clamp(retVal, -1.f, 1.f);
    }

    return retVal;
}

glm::vec2 InputSystem::Filter2D(int inputX, int inputY) {
    const float deadZone{8000.f};
    const float maxValue{30000.f};

    // Make into 2D vector
    glm::vec2 dir{
        static_cast<float>(inputX),
        static_cast<float>(inputY)};

    float length{dir.length()};
    // If length < deadZone, should no be input
    if (length < deadZone)
        dir = glm::vec2{0};
    else {
        // Calculate fractional value between
        // dead zone and max value circles
        float f{(length - deadZone) / (maxValue - deadZone)};
        // Clamp f between 0.f and 1.f
        f = glm::clamp(f, 0.f, 1.f);
        // Normalize the vector, and then scale it to the
        // fractional value
        dir *= f / length;
    }

    return dir;
}

ButtonState InputSystem::GetActionButtonState(const std::string& name) {
    return state.Keyboard.GetKeyState(keyActions[name]);
}

const SDL_Scancode InputSystem::GetActionScancode(const std::string& name) {
    return keyActions[name];
}

void InputSystem::AddKeyAction(const std::string& name, SDL_Scancode scancode) {
    keyActions.emplace(name, scancode);
}

void InputSystem::AddKeyAction(const std::string& name, const std::string& scancodeName) {
    keyActions.emplace(name, SDL_GetScancodeFromName(scancodeName.c_str()));
}

void InputSystem::RemoveKeyAction(const std::string& name) {
    auto found{keyActions.find(name)};
    if (found != keyActions.end())
        keyActions.erase(found);
}

void InputSystem::RemapActionKey(const std::string& name, SDL_Scancode newScancode) {
    auto found{keyActions.find(name)};
    if (found != keyActions.end())
        keyActions[name] = newScancode;
}

void InputSystem::RemapActionKey(const std::string& name, const std::string& newScancodeName) {
    auto found{keyActions.find(name)};
    if (found != keyActions.end())
        keyActions[name] = SDL_GetScancodeFromName(newScancodeName.c_str());
}