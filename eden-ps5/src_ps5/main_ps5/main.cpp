// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#include <iostream>
#include <memory>
#include <signal.h>

#include "ps5_application.h"
#include "../ps5_platform/ps5_system.h"
#include "../etahen_integration/etahen_plugin.h"

// Global application instance
std::unique_ptr<PS5Application> g_ps5_app;

void SignalHandler(int signal) {
    std::cout << "[Main] Received signal " << signal << ", shutting down..." << std::endl;
    
    if (g_ps5_app) {
        g_ps5_app->RequestShutdown();
    }
}

void PrintBanner() {
    std::cout << "============================================" << std::endl;
    std::cout << "     Eden Nintendo Switch Emulator" << std::endl;
    std::cout << "           PlayStation 5 Port" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << "Version: 1.0.0-PS5" << std::endl;
    std::cout << "Build Date: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "Platform: PlayStation 5 (Homebrew)" << std::endl;
    std::cout << "============================================" << std::endl;
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    // Print startup banner
    PrintBanner();
    
    // Set up signal handlers
    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);
    
#ifdef PS5_BUILD
    std::cout << "[Main] Running on PlayStation 5" << std::endl;
#else
    std::cout << "[Main] Running in development environment" << std::endl;
#endif
    
    // Initialize PS5 platform layer
    std::cout << "[Main] Initializing PS5 platform..." << std::endl;
    if (!PS5Platform::PS5System::Initialize()) {
        std::cerr << "[Main] Failed to initialize PS5 platform" << std::endl;
        return -1;
    }
    
    // Initialize etaHEN integration
    std::cout << "[Main] Initializing etaHEN integration..." << std::endl;
    auto& etahen = etaHEN::GetEtaHENPlugin();
    if (!etahen.Initialize()) {
        std::cout << "[Main] Warning: etaHEN integration failed, continuing anyway..." << std::endl;
    }
    
    // Check if we're jailbroken (required for full functionality)
    if (etahen.IsEtaHENAvailable()) {
        std::cout << "[Main] etaHEN detected - requesting jailbreak..." << std::endl;
        if (etahen.RequestJailbreak()) {
            std::cout << "[Main] Jailbreak successful!" << std::endl;
            etahen.EnableDataAccess(); // Enable /data access for save files
        } else {
            std::cout << "[Main] Warning: Jailbreak failed, limited functionality" << std::endl;
        }
    } else {
        std::cout << "[Main] Warning: etaHEN not detected - running with limited functionality" << std::endl;
    }
    
    // Create and initialize PS5 application
    std::cout << "[Main] Creating PS5 application..." << std::endl;
    g_ps5_app = std::make_unique<PS5Application>();
    
    if (!g_ps5_app->Initialize(argc, argv)) {
        std::cerr << "[Main] Failed to initialize PS5 application" << std::endl;
        return -1;
    }
    
    std::cout << "[Main] Eden PS5 initialized successfully!" << std::endl;
    std::cout << "[Main] Starting emulator..." << std::endl;
    
    // Run the main application loop
    int result = g_ps5_app->Run();
    
    std::cout << "[Main] Application finished with result: " << result << std::endl;
    
    // Cleanup
    std::cout << "[Main] Shutting down..." << std::endl;
    
    if (g_ps5_app) {
        g_ps5_app->Shutdown();
        g_ps5_app.reset();
    }
    
    // Shutdown etaHEN integration
    etahen.Shutdown();
    
    // Shutdown PS5 platform
    PS5Platform::PS5System::Shutdown();
    
    std::cout << "[Main] Eden PS5 shutdown complete" << std::endl;
    
    return result;
}