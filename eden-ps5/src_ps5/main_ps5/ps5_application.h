// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>
#include <string>
#include <atomic>

namespace PS5Graphics {
class PS5Renderer;
}

namespace PS5Audio {
class PS5AudioManager;
}

namespace PS5Input {
class PS5InputManager;
}

class PS5Frontend;
class EdenCoreIntegration;

/**
 * Main PS5 Application Class
 * Replaces the Qt-based yuzu application
 */
class PS5Application {
public:
    PS5Application();
    ~PS5Application();
    
    // Application Lifecycle
    bool Initialize(int argc, char* argv[]);
    void Shutdown();
    int Run();
    
    // Application Control
    void RequestShutdown();
    bool IsShuttingDown() const { return shutdown_requested_; }
    
    // Game Management
    bool LoadGame(const std::string& game_path);
    void UnloadGame();
    bool IsGameLoaded() const { return game_loaded_; }
    
    // Emulation Control
    void StartEmulation();
    void PauseEmulation();
    void ResumeEmulation();
    void StopEmulation();
    
    bool IsEmulationRunning() const { return emulation_running_; }
    bool IsEmulationPaused() const { return emulation_paused_; }
    
    // System Management
    void SaveState(const std::string& slot_name = "quicksave");
    void LoadState(const std::string& slot_name = "quicksave");
    
    // Settings
    void LoadSettings();
    void SaveSettings();
    
    // Performance
    float GetFPS() const { return current_fps_; }
    float GetEmulationSpeed() const { return emulation_speed_; }
    
private:
    bool InitializeSubsystems();
    void ShutdownSubsystems();
    
    bool ProcessCommandLine(int argc, char* argv[]);
    
    void MainLoop();
    void UpdateFPS();
    
    // Subsystems
    std::unique_ptr<PS5Graphics::PS5Renderer> renderer_;
    std::unique_ptr<PS5Audio::PS5AudioManager> audio_manager_;
    std::unique_ptr<PS5Input::PS5InputManager> input_manager_;
    std::unique_ptr<PS5Frontend> frontend_;
    std::unique_ptr<EdenCoreIntegration> eden_core_;
    
    // Application state
    std::atomic<bool> shutdown_requested_{false};
    bool initialized_{false};
    bool game_loaded_{false};
    bool emulation_running_{false};
    bool emulation_paused_{false};
    
    // Performance metrics
    float current_fps_{0.0f};
    float emulation_speed_{1.0f};
    
    // Settings
    std::string config_path_;
    std::string game_path_;
};