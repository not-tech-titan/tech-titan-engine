#pragma once
#include <string>
#include <unordered_map>
#include "raylib.h"

class Input 
{
public:
    // Lifecycle
    static void Init();
    static void Update();
    static void Shutdown();

    // Action creation (editor will call these)
    static void RegisterVector2(const std::string& name);
    static void RegisterButton(const std::string& name);

    // Binding
    static void BindKey(const std::string& action, KeyboardKey key);
    static void BindMouseButton(const std::string& action, MouseButton button);
    static void BindVector2(const std::string& action, KeyboardKey negX, KeyboardKey posX, KeyboardKey negY, KeyboardKey posY);

    // Query
    static Vector2 GetVector2(const std::string& action);
    static bool GetButton(const std::string& action);
    static bool GetButtonPressed(const std::string& action);

private:
    struct Vector2Action 
    {
        KeyboardKey negX, posX;
        KeyboardKey negY, posY;
        Vector2 value;
    };

    struct ButtonAction 
    {
        KeyboardKey key;
        MouseButton mouse;
        bool value;
        bool pressed;
    };

    static std::unordered_map<std::string, Vector2Action> vector2Actions;
    static std::unordered_map<std::string, ButtonAction> buttonActions;
};
