#include "Input.hpp"

Owned<InputSystem> Input::system {MakeOwned<InputSystem>()};