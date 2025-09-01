// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>
#include <vector>
#include <cstdint>

namespace PS5Audio {

class PS5AudioDevice;
class PS5AudioStream;

/**
 * PS5 Audio Manager
 * Manages PS5 native audio system, replacing Cubeb/SDL audio
 */
class PS5AudioManager {
public:
    PS5AudioManager();
    ~PS5AudioManager();
    
    // Core Audio Interface
    bool Initialize();
    void Shutdown();
    
    bool IsInitialized() const { return initialized_; }
    
    // Device Management
    std::vector<PS5AudioDevice*> GetOutputDevices();
    std::vector<PS5AudioDevice*> GetInputDevices();
    
    PS5AudioDevice* GetDefaultOutputDevice();
    PS5AudioDevice* GetDefaultInputDevice();
    
    // Stream Management
    PS5AudioStream* CreateOutputStream(uint32_t sample_rate, uint32_t channels, 
                                      uint32_t buffer_size);
    PS5AudioStream* CreateInputStream(uint32_t sample_rate, uint32_t channels, 
                                     uint32_t buffer_size);
    void DestroyStream(PS5AudioStream* stream);
    
    // Audio Control
    void SetMasterVolume(float volume); // 0.0 - 1.0
    float GetMasterVolume() const;
    
    void SetMute(bool muted);
    bool IsMuted() const;
    
    // 3D Audio Support
    bool Is3DAudioAvailable() const;
    void Enable3DAudio(bool enable);
    bool Is3DAudioEnabled() const;
    
    // Performance Monitoring
    uint32_t GetOutputLatency() const; // in milliseconds
    uint32_t GetInputLatency() const;  // in milliseconds
    
private:
    bool InitializePS5AudioSystem();
    void ShutdownPS5AudioSystem();
    
    bool EnumerateAudioDevices();
    void CleanupAudioDevices();
    
    std::vector<std::unique_ptr<PS5AudioDevice>> output_devices_;
    std::vector<std::unique_ptr<PS5AudioDevice>> input_devices_;
    std::vector<std::unique_ptr<PS5AudioStream>> streams_;
    
    PS5AudioDevice* default_output_device_;
    PS5AudioDevice* default_input_device_;
    
    bool initialized_;
    float master_volume_;
    bool is_muted_;
    bool is_3d_audio_enabled_;
    bool is_3d_audio_available_;
};

} // namespace PS5Audio