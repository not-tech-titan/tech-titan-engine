#pragma once
#include <string>
#include <unordered_map>
#include "raylib.h"

struct VideoSettings
{
    int windowWidth = 800;
    int windowHeight = 600;
    int targetFPS = 60;
    bool fullscreen = false;

};

struct AudioSettings
{
    float masterVolume = 1.0f;
    float musicVolume  = 0.8f;
    float sfxVolume    = 0.8f;
};

struct ControlSettings
{
    // action name → key
    std::unordered_map<std::string, KeyboardKey> keyBindings;
};

class Settings
{
public:
    VideoSettings video;
    AudioSettings audio;
    ControlSettings controls;

    // lifecycle
    void Load();
    void Save() const;

    // apply to engine systems
    void ApplyVideo() const;
    void ApplyAudio() const;

    // control helpers
    bool IsActionDown(const std::string& action) const;
    bool IsActionPressed(const std::string& action) const;

private:
    std::string GetSettingsPath() const;
};
