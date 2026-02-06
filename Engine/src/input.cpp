#include "input.h"
#include "console.h"

std::unordered_map<std::string, Input::Vector2Action> Input::vector2Actions;
std::unordered_map<std::string, Input::ButtonAction> Input::buttonActions;

void Input::Init() {}

void Input::Shutdown() {}

void Input::RegisterVector2(const std::string& name) 
{
    vector2Actions[name] = {};
    Console::PrintLine("Vector2 action registered: " + name);
}

void Input::RegisterButton(const std::string& name) 
{
    buttonActions[name] = {};
    Console::PrintLine("Button action registered: " + name);
}

void Input::BindVector2
(
    const std::string& action,
    KeyboardKey left,
    KeyboardKey right,
    KeyboardKey up,
    KeyboardKey down
)
{
    auto& a = vector2Actions[action];
    a.negX = left;
    a.posX = right;
    a.negY = up;
    a.posY = down;
    Console::PrintLine("Vector2 bound: " + action);
}


void Input::BindKey(const std::string& action, KeyboardKey key) 
{
    buttonActions[action].key = key;
    Console::PrintLine("Key action bound: " + action);
}

void Input::BindMouseButton(const std::string& action, MouseButton button) 
{
    buttonActions[action].mouse = button;
    Console::PrintLine("Mouse action bound: " + action);
}

void Input::Update() 
{
    // Vector2 actions
    for (auto& [name, action] : vector2Actions) 
    {
        float x = 0;
        float y = 0;

        if (IsKeyDown(action.negX)) x -= 1;
        if (IsKeyDown(action.posX)) x += 1;
        if (IsKeyDown(action.negY)) y -= 1;
        if (IsKeyDown(action.posY)) y += 1;

        action.value = { x, y };
    }

    // Button actions
    for (auto& [name, action] : buttonActions) 
    {
        bool down = false;

        if (action.key != 0)
            down |= IsKeyDown(action.key);

        if (action.mouse != 0)
            down |= IsMouseButtonDown(action.mouse);

        action.pressed = down && !action.value;
        action.value = down;
    }
}

Vector2 Input::GetVector2(const std::string& action) 
{
    return vector2Actions[action].value;
}

bool Input::GetButton(const std::string& action) 
{
    return buttonActions[action].value;
}

bool Input::GetButtonPressed(const std::string& action) 
{
    return buttonActions[action].pressed;
}
