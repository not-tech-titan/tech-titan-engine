#include "settings.h"
#include <fstream>
#include <filesystem>
#include "console.h"

// -------------------------------------
// CONFIG
// -------------------------------------
static const char* SETTINGS_FILE = "settings.cfg";

// -------------------------------------
// INTERNAL
// -------------------------------------
std::string Settings::GetSettingsPath() const
{
    // Later this can become:
    // %APPDATA%/TechTitan/<GameName>/settings.cfg
    // For now: local file next to exe
    return SETTINGS_FILE;
}

// -------------------------------------
// LOAD SETTINGS FROM DISK
// -------------------------------------
void Settings::Load()
{
    std::ifstream file(GetSettingsPath());
    if (!file.is_open())
        return;

    std::string token;

    while (file >> token)
    {
        // -------------------
        // VIDEO
        // -------------------
        if (token == "windowWidth")
            file >> video.windowWidth;
        else if (token == "windowHeight")
            file >> video.windowHeight;
        else if (token == "fullscreen")
            file >> video.fullscreen;

        // -------------------
        // AUDIO
        // -------------------
        else if (token == "masterVolume")
            file >> audio.masterVolume;
        else if (token == "musicVolume")
            file >> audio.musicVolume;
        else if (token == "sfxVolume")
            file >> audio.sfxVolume;

        // -------------------
        // INPUT BINDINGS
        // bind <ActionName> <KeyCode>
        // -------------------
        else if (token == "bind")
        {
            std::string action;
            int keyCode;

            file >> action >> keyCode;
            controls.keyBindings[action] = (KeyboardKey)keyCode;
        }
    }

    file.close();

    Console::PrintLine("Settings Loaded.");
}

// -------------------------------------
// SAVE SETTINGS TO DISK
// -------------------------------------
void Settings::Save() const
{
    std::ofstream file(GetSettingsPath());
    if (!file.is_open())
        return;

    // -------------------
    // VIDEO
    // -------------------
    file << "windowWidth " << video.windowWidth << "\n";
    file << "windowHeight " << video.windowHeight << "\n";
    file << "fullscreen " << video.fullscreen << "\n";

    // -------------------
    // AUDIO
    // -------------------
    file << "masterVolume " << audio.masterVolume << "\n";
    file << "musicVolume " << audio.musicVolume << "\n";
    file << "sfxVolume " << audio.sfxVolume << "\n";

    // -------------------
    // CONTROLS
    // -------------------
    for (const auto& binding : controls.keyBindings)
    {
        file << "bind "
             << binding.first << " "
             << (int)binding.second << "\n";
    }

    file.close();

    Console::PrintLine("Settings Saved.");
}

// -------------------------------------
// APPLY SETTINGS TO ENGINE
// -------------------------------------
void Settings::ApplyVideo() const
{
    SetWindowSize(video.windowWidth, video.windowHeight);

    bool currentlyFullscreen = IsWindowFullscreen();
    if (video.fullscreen != currentlyFullscreen)
    {
        ToggleFullscreen();
    }

    Console::PrintLine("Video Settings Applied.");
}

void Settings::ApplyAudio() const
{
    SetMasterVolume(audio.masterVolume);
    // music/sfx scaling handled by audio system later
    Console::PrintLine("Audio Settings Applied.");
}

// -------------------------------------
// ACTION-BASED INPUT
// -------------------------------------
bool Settings::IsActionDown(const std::string& action) const
{
    auto it = controls.keyBindings.find(action);
    if (it == controls.keyBindings.end())
        return false;

    return IsKeyDown(it->second);
}

bool Settings::IsActionPressed(const std::string& action) const
{
    auto it = controls.keyBindings.find(action);
    if (it == controls.keyBindings.end())
        return false;

    return IsKeyPressed(it->second);
}
