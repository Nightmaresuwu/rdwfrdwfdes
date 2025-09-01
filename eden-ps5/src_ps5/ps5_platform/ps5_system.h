// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <string>

namespace PS5Platform {

/**
 * PS5 System Information and Management
 */
class PS5System {
public:
    static bool Initialize();
    static void Shutdown();
    
    // System Information
    static std::string GetSystemVersion();
    static uint64_t GetAvailableMemory();
    static uint32_t GetCPUCoreCount();
    static bool IsJailbroken();
    
    // etaHEN Integration
    static bool InitializeEtaHEN();
    static bool IsEtaHENAvailable();
    
    // Power Management
    static void PreventSleep();
    static void AllowSleep();
    
    // System Resources
    static bool RequestHighPerformanceMode();
    static void ReleaseHighPerformanceMode();
    
private:
    static bool initialized_;
    static bool etahen_available_;
    static bool high_performance_mode_;
};

} // namespace PS5Platform