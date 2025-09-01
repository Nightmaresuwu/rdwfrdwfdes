// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ps5_application.h"
#include "eden_core_integration.h"
#include "../ps5_graphics/ps5_renderer.h"
#include "../ps5_audio/ps5_audio_manager.h"
#include "../ps5_input/ps5_input_manager.h"
#include "../ps5_frontend/ps5_frontend.h"

#include <iostream>
#include <thread>
#include <chrono>

PS5Application::PS5Application()
    : config_path_("/data/eden/config") {
}

PS5Application::~PS5Application() {
    if (initialized_) {
        Shutdown();
    }
}

bool PS5Application::Initialize(int argc, char* argv[]) {
    if (initialized_) {
        return true;
    }
    
    std::cout << "[PS5Application] Initializing application..." << std::endl;
    
    // Process command line arguments
    if (!ProcessCommandLine(argc, argv)) {
        std::cout << "[PS5Application] Failed to process command line" << std::endl;
        return false;
    }
    
    // Load settings
    LoadSettings();
    
    // Initialize subsystems
    if (!InitializeSubsystems()) {
        std::cout << "[PS5Application] Failed to initialize subsystems" << std::endl;
        return false;
    }
    
    initialized_ = true;
    std::cout << "[PS5Application] Application initialized successfully" << std::endl;
    
    return true;
}

void PS5Application::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[PS5Application] Shutting down application..." << std::endl;
    
    // Stop emulation if running
    if (emulation_running_) {
        StopEmulation();
    }
    
    // Unload game if loaded
    if (game_loaded_) {
        UnloadGame();
    }
    
    // Save settings
    SaveSettings();
    
    // Shutdown subsystems
    ShutdownSubsystems();
    
    initialized_ = false;
}

bool PS5Application::InitializeSubsystems() {
    std::cout << "[PS5Application] Initializing subsystems..." << std::endl;
    
    // Initialize Graphics Renderer
    std::cout << "[PS5Application] Initializing graphics renderer..." << std::endl;
    renderer_ = std::make_unique<PS5Graphics::PS5Renderer>();
    if (!renderer_->Initialize()) {
        std::cout << "[PS5Application] Failed to initialize graphics renderer" << std::endl;
        return false;
    }
    
    // Initialize Audio Manager
    std::cout << "[PS5Application] Initializing audio manager..." << std::endl;
    audio_manager_ = std::make_unique<PS5Audio::PS5AudioManager>();
    if (!audio_manager_->Initialize()) {
        std::cout << "[PS5Application] Failed to initialize audio manager" << std::endl;
        return false;
    }
    
    // Initialize Input Manager
    std::cout << "[PS5Application] Initializing input manager..." << std::endl;
    input_manager_ = std::make_unique<PS5Input::PS5InputManager>();
    if (!input_manager_->Initialize()) {
        std::cout << "[PS5Application] Failed to initialize input manager" << std::endl;
        return false;
    }
    
    // Initialize Frontend
    std::cout << "[PS5Application] Initializing frontend..." << std::endl;
    frontend_ = std::make_unique<PS5Frontend>(renderer_.get(), audio_manager_.get(), input_manager_.get());
    if (!frontend_->Initialize()) {
        std::cout << "[PS5Application] Failed to initialize frontend" << std::endl;
        return false;
    }
    
    // Initialize Eden Core Integration
    std::cout << "[PS5Application] Initializing Eden core..." << std::endl;
    eden_core_ = std::make_unique<EdenCoreIntegration>(renderer_.get(), audio_manager_.get(), input_manager_.get());
    if (!eden_core_->Initialize()) {
        std::cout << "[PS5Application] Failed to initialize Eden core" << std::endl;
        return false;
    }
    
    std::cout << "[PS5Application] All subsystems initialized successfully" << std::endl;
    return true;
}

void PS5Application::ShutdownSubsystems() {
    std::cout << "[PS5Application] Shutting down subsystems..." << std::endl;
    
    if (eden_core_) {
        eden_core_->Shutdown();
        eden_core_.reset();
    }
    
    if (frontend_) {
        frontend_->Shutdown();
        frontend_.reset();
    }
    
    if (input_manager_) {
        input_manager_->Shutdown();
        input_manager_.reset();
    }
    
    if (audio_manager_) {
        audio_manager_->Shutdown();
        audio_manager_.reset();
    }
    
    if (renderer_) {
        renderer_->Shutdown();
        renderer_.reset();
    }
    
    std::cout << "[PS5Application] Subsystems shutdown complete" << std::endl;
}

bool PS5Application::ProcessCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--game" && i + 1 < argc) {
            game_path_ = argv[++i];
            std::cout << "[PS5Application] Game path specified: " << game_path_ << std::endl;
        } else if (arg == "--config" && i + 1 < argc) {
            config_path_ = argv[++i];
            std::cout << "[PS5Application] Config path specified: " << config_path_ << std::endl;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Eden Nintendo Switch Emulator - PS5 Port" << std::endl;
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  --game <path>    Load game from path" << std::endl;
            std::cout << "  --config <path>  Use config directory" << std::endl;
            std::cout << "  --help, -h       Show this help message" << std::endl;
            return false;
        }
    }
    
    return true;
}

int PS5Application::Run() {
    if (!initialized_) {
        std::cout << "[PS5Application] Cannot run - not initialized" << std::endl;
        return -1;
    }
    
    std::cout << "[PS5Application] Starting main loop..." << std::endl;
    
    // Load game if specified
    if (!game_path_.empty()) {
        if (LoadGame(game_path_)) {
            std::cout << "[PS5Application] Game loaded successfully, starting emulation..." << std::endl;
            StartEmulation();
        } else {
            std::cout << "[PS5Application] Failed to load game: " << game_path_ << std::endl;
        }
    }
    
    // Run main loop
    MainLoop();
    
    std::cout << "[PS5Application] Main loop finished" << std::endl;
    
    return 0;
}

void PS5Application::MainLoop() {
    auto last_fps_time = std::chrono::steady_clock::now();
    uint32_t frame_count = 0;
    
    while (!shutdown_requested_) {
        auto frame_start = std::chrono::steady_clock::now();
        
        // Update input
        if (input_manager_) {
            input_manager_->Update();
        }
        
        // Update frontend
        if (frontend_) {
            frontend_->Update();
        }
        
        // Update Eden core (emulation)
        if (eden_core_ && emulation_running_ && !emulation_paused_) {
            eden_core_->Update();
        }
        
        // Render frame
        if (renderer_) {
            renderer_->BeginFrame();
            
            // Render Eden core if emulation is active
            if (eden_core_ && emulation_running_) {
                eden_core_->Render();
            }
            
            // Render frontend UI
            if (frontend_) {
                frontend_->Render();
            }
            
            renderer_->EndFrame();
            renderer_->Present();
        }
        
        // Update FPS counter
        frame_count++;
        auto now = std::chrono::steady_clock::now();
        auto fps_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_fps_time);
        
        if (fps_elapsed.count() >= 1000) {
            current_fps_ = static_cast<float>(frame_count) / (fps_elapsed.count() / 1000.0f);
            frame_count = 0;
            last_fps_time = now;
        }
        
        // Frame limiting (target 60 FPS)
        auto frame_end = std::chrono::steady_clock::now();
        auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds>(frame_end - frame_start);
        auto target_frame_time = std::chrono::microseconds(16667); // ~60 FPS
        
        if (frame_duration < target_frame_time) {
            std::this_thread::sleep_for(target_frame_time - frame_duration);
        }
    }
}

bool PS5Application::LoadGame(const std::string& game_path) {
    if (!eden_core_) {
        std::cout << "[PS5Application] Cannot load game - Eden core not initialized" << std::endl;
        return false;
    }
    
    std::cout << "[PS5Application] Loading game: " << game_path << std::endl;
    
    if (eden_core_->LoadGame(game_path)) {
        game_loaded_ = true;
        game_path_ = game_path;
        std::cout << "[PS5Application] Game loaded successfully" << std::endl;
        return true;
    } else {
        std::cout << "[PS5Application] Failed to load game" << std::endl;
        return false;
    }
}

void PS5Application::UnloadGame() {
    if (!game_loaded_) {
        return;
    }
    
    std::cout << "[PS5Application] Unloading game..." << std::endl;
    
    // Stop emulation first
    if (emulation_running_) {
        StopEmulation();
    }
    
    if (eden_core_) {
        eden_core_->UnloadGame();
    }
    
    game_loaded_ = false;
    game_path_.clear();
}

void PS5Application::StartEmulation() {
    if (!game_loaded_ || emulation_running_) {
        return;
    }
    
    std::cout << "[PS5Application] Starting emulation..." << std::endl;
    
    if (eden_core_ && eden_core_->StartEmulation()) {
        emulation_running_ = true;
        emulation_paused_ = false;
        std::cout << "[PS5Application] Emulation started" << std::endl;
    } else {
        std::cout << "[PS5Application] Failed to start emulation" << std::endl;
    }
}

void PS5Application::PauseEmulation() {
    if (!emulation_running_ || emulation_paused_) {
        return;
    }
    
    std::cout << "[PS5Application] Pausing emulation..." << std::endl;
    emulation_paused_ = true;
}

void PS5Application::ResumeEmulation() {
    if (!emulation_running_ || !emulation_paused_) {
        return;
    }
    
    std::cout << "[PS5Application] Resuming emulation..." << std::endl;
    emulation_paused_ = false;
}

void PS5Application::StopEmulation() {
    if (!emulation_running_) {
        return;
    }
    
    std::cout << "[PS5Application] Stopping emulation..." << std::endl;
    
    if (eden_core_) {
        eden_core_->StopEmulation();
    }
    
    emulation_running_ = false;
    emulation_paused_ = false;
}

void PS5Application::RequestShutdown() {
    std::cout << "[PS5Application] Shutdown requested" << std::endl;
    shutdown_requested_ = true;
}

void PS5Application::LoadSettings() {
    // TODO: Load settings from /data/eden/config/settings.ini
    std::cout << "[PS5Application] Loading settings from: " << config_path_ << std::endl;
}

void PS5Application::SaveSettings() {
    // TODO: Save settings to /data/eden/config/settings.ini
    std::cout << "[PS5Application] Saving settings to: " << config_path_ << std::endl;
}

void PS5Application::SaveState(const std::string& slot_name) {
    if (!emulation_running_) {
        std::cout << "[PS5Application] Cannot save state - emulation not running" << std::endl;
        return;
    }
    
    std::cout << "[PS5Application] Saving state: " << slot_name << std::endl;
    
    if (eden_core_) {
        eden_core_->SaveState(slot_name);
    }
}

void PS5Application::LoadState(const std::string& slot_name) {
    if (!game_loaded_) {
        std::cout << "[PS5Application] Cannot load state - no game loaded" << std::endl;
        return;
    }
    
    std::cout << "[PS5Application] Loading state: " << slot_name << std::endl;
    
    if (eden_core_) {
        eden_core_->LoadState(slot_name);
    }
}