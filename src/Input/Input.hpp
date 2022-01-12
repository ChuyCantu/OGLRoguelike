#ifndef __INPUT_H__
#define __INPUT_H__

#include "Common.hpp"
#include "InputSystem.hpp"

class Input {
public:
    /**
     * @brief Returns true during the frames the key is down
     */
    static bool GetKey(SDL_Scancode key);
    /**
     * @brief Returns true the frame the key is pressed
     */
    static bool GetKeyDown(SDL_Scancode key);
    /**
     * @brief Returns true the frame the key is released
     */
    static bool GetKeyUp(SDL_Scancode key);

    /**
     * @brief Returns true during the frames the mouse button is down
     */
    static bool GetMouseButton(int button);
    /**
     * @brief Returns true the frame the mouse button is pressed
     */
    static bool GetMouseButtonDown(int button);
    /**
     * @brief Returns true the frame the mouse button is released
     */
    static bool GetMouseButtonUp(int button);

    /**
     * @brief Return current position of the mouse in relative to the window
     */
    static glm::ivec2 GetMousePosition();
    /**
     * @brief Return value of the mouse wheel as -1, 0 or 1
     */
    static glm::vec2 GetMouseWheel();
    /**
     * @brief If set to true, GetMousePosition returns a relative position from the center of the window each frame, if set to false, it returns the actual position of the mouse.
     */
    static void SetMouseRelativeMode(bool value);

    /**
     * @brief Return true if a key was pressed, false otherwise
     * 
     * @param scancode If the function returns true, this is the Scancode of the first key pressed, else it has the value SDL_SCANCODE_UNKNOWN
     */
    static bool DetectKeyPress(SDL_Scancode* scancode);

private:
    static Owned<InputSystem> system;

    friend class Engine;
};

#endif // __INPUT_H__