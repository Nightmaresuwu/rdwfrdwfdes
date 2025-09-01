// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ps5_audio_manager.h"
#include "ps5_audio_device.h"
#include "ps5_audio_stream.h"
#include <iostream>
#include <algorithm>

#ifdef PS5_BUILD
// Include PS5 audio headers when building for actual PS5
// #include <audioout.h>
// #include <audioin.h>
// #include <audio3d.h>
#endif

namespace PS5Audio {

PS5AudioManager::PS5AudioManager()
    : default_output_device_(nullptr)
    , default_input_device_(nullptr)
    , initialized_(false)
    , master_volume_(1.0f)
    , is_muted_(false)
    , is_3d_audio_enabled_(false)
    , is_3d_audio_available_(false) {
}

PS5AudioManager::~PS5AudioManager() {
    if (initialized_) {
        Shutdown();
    }
}

bool PS5AudioManager::Initialize() {
    if (initialized_) {
        return true;
    }
    
    std::cout << "[PS5AudioManager] Initializing PS5 Audio System..." << std::endl;
    
    if (!InitializePS5AudioSystem()) {
        std::cout << "[PS5AudioManager] Failed to initialize PS5 audio system" << std::endl;
        return false;
    }
    
    if (!EnumerateAudioDevices()) {
        std::cout << "[PS5AudioManager] Failed to enumerate audio devices" << std::endl;
        return false;
    }
    
    // Check for 3D audio support
#ifdef PS5_BUILD
    // TODO: Check if PS5 3D audio is available
    // is_3d_audio_available_ = sceAudio3dIsAvailable();
    is_3d_audio_available_ = true; // Assume available on PS5
#else
    is_3d_audio_available_ = false; // Not available in development
#endif
    
    initialized_ = true;
    std::cout << "[PS5AudioManager] Audio system initialized successfully" << std::endl;
    std::cout << "[PS5AudioManager] Output devices: " << output_devices_.size() << std::endl;
    std::cout << "[PS5AudioManager] Input devices: " << input_devices_.size() << std::endl;
    std::cout << "[PS5AudioManager] 3D Audio available: " << (is_3d_audio_available_ ? "Yes" : "No") << std::endl;
    
    return true;
}

void PS5AudioManager::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[PS5AudioManager] Shutting down PS5 Audio System..." << std::endl;
    
    // Destroy all streams
    for (auto& stream : streams_) {
        stream->Stop();
    }
    streams_.clear();
    
    // Clean up audio devices
    CleanupAudioDevices();
    
    // Shutdown PS5 audio system
    ShutdownPS5AudioSystem();
    
    initialized_ = false;
}

bool PS5AudioManager::InitializePS5AudioSystem() {
#ifdef PS5_BUILD
    std::cout << "[PS5AudioManager] Initializing PS5 native audio system..." << std::endl;
    
    // TODO: Initialize PS5 audio subsystems
    // int result = sceAudioOutInit();
    // if (result < 0) {
    //     std::cout << "[PS5AudioManager] Failed to initialize audio out: " << std::hex << result << std::endl;
    //     return false;
    // }
    
    // result = sceAudioInInit();
    // if (result < 0) {
    //     std::cout << "[PS5AudioManager] Failed to initialize audio in: " << std::hex << result << std::endl;
    //     return false;
    // }
    
    return true;
#else
    std::cout << "[PS5AudioManager] PS5 audio system initialized (development mode)" << std::endl;
    return true;
#endif
}

void PS5AudioManager::ShutdownPS5AudioSystem() {
#ifdef PS5_BUILD
    // TODO: Shutdown PS5 audio subsystems
    // sceAudioInTerm();
    // sceAudioOutTerm();
#endif
}

bool PS5AudioManager::EnumerateAudioDevices() {
    std::cout << "[PS5AudioManager] Enumerating audio devices..." << std::endl;
    
    // Clear existing devices
    output_devices_.clear();
    input_devices_.clear();
    
#ifdef PS5_BUILD
    // TODO: Enumerate actual PS5 audio devices
    
    // Create default output device (HDMI/Main)
    auto main_output = std::make_unique<PS5AudioDevice>(
        PS5AudioDevice::Type::Output, 0, "Main Output (HDMI)", true);
    default_output_device_ = main_output.get();
    output_devices_.push_back(std::move(main_output));
    
    // Create headphone output device
    auto headphone_output = std::make_unique<PS5AudioDevice>(
        PS5AudioDevice::Type::Output, 1, "Headphones", false);
    output_devices_.push_back(std::move(headphone_output));
    
    // Create default input device (Microphone)
    auto mic_input = std::make_unique<PS5AudioDevice>(
        PS5AudioDevice::Type::Input, 0, "Microphone", true);
    default_input_device_ = mic_input.get();
    input_devices_.push_back(std::move(mic_input));
    
#else
    // Development mode - create mock devices
    auto mock_output = std::make_unique<PS5AudioDevice>(
        PS5AudioDevice::Type::Output, 0, "Mock Output", true);
    default_output_device_ = mock_output.get();
    output_devices_.push_back(std::move(mock_output));
    
    auto mock_input = std::make_unique<PS5AudioDevice>(
        PS5AudioDevice::Type::Input, 0, "Mock Input", true);
    default_input_device_ = mock_input.get();
    input_devices_.push_back(std::move(mock_input));
#endif
    
    std::cout << "[PS5AudioManager] Found " << output_devices_.size() << " output devices" << std::endl;
    std::cout << "[PS5AudioManager] Found " << input_devices_.size() << " input devices" << std::endl;
    
    return !output_devices_.empty();
}

void PS5AudioManager::CleanupAudioDevices() {
    default_output_device_ = nullptr;
    default_input_device_ = nullptr;
    output_devices_.clear();
    input_devices_.clear();
}

std::vector<PS5AudioDevice*> PS5AudioManager::GetOutputDevices() {
    std::vector<PS5AudioDevice*> devices;
    for (const auto& device : output_devices_) {
        devices.push_back(device.get());
    }
    return devices;
}

std::vector<PS5AudioDevice*> PS5AudioManager::GetInputDevices() {
    std::vector<PS5AudioDevice*> devices;
    for (const auto& device : input_devices_) {
        devices.push_back(device.get());
    }
    return devices;
}

PS5AudioDevice* PS5AudioManager::GetDefaultOutputDevice() {
    return default_output_device_;
}

PS5AudioDevice* PS5AudioManager::GetDefaultInputDevice() {
    return default_input_device_;
}

PS5AudioStream* PS5AudioManager::CreateOutputStream(uint32_t sample_rate, 
                                                   uint32_t channels, 
                                                   uint32_t buffer_size) {
    if (!initialized_ || !default_output_device_) {
        return nullptr;
    }
    
    auto stream = std::make_unique<PS5AudioStream>(
        PS5AudioStream::Type::Output, default_output_device_, 
        sample_rate, channels, buffer_size);
    
    if (!stream->Initialize()) {
        std::cout << "[PS5AudioManager] Failed to create output stream" << std::endl;
        return nullptr;
    }
    
    PS5AudioStream* stream_ptr = stream.get();
    streams_.push_back(std::move(stream));
    
    std::cout << "[PS5AudioManager] Created output stream: " 
              << sample_rate << "Hz, " << channels << " channels" << std::endl;
    
    return stream_ptr;
}

PS5AudioStream* PS5AudioManager::CreateInputStream(uint32_t sample_rate, 
                                                  uint32_t channels, 
                                                  uint32_t buffer_size) {
    if (!initialized_ || !default_input_device_) {
        return nullptr;
    }
    
    auto stream = std::make_unique<PS5AudioStream>(
        PS5AudioStream::Type::Input, default_input_device_, 
        sample_rate, channels, buffer_size);
    
    if (!stream->Initialize()) {
        std::cout << "[PS5AudioManager] Failed to create input stream" << std::endl;
        return nullptr;
    }
    
    PS5AudioStream* stream_ptr = stream.get();
    streams_.push_back(std::move(stream));
    
    std::cout << "[PS5AudioManager] Created input stream: " 
              << sample_rate << "Hz, " << channels << " channels" << std::endl;
    
    return stream_ptr;
}

void PS5AudioManager::DestroyStream(PS5AudioStream* stream) {
    auto it = std::find_if(streams_.begin(), streams_.end(),
        [stream](const std::unique_ptr<PS5AudioStream>& s) {
            return s.get() == stream;
        });
    
    if (it != streams_.end()) {
        (*it)->Stop();
        streams_.erase(it);
        std::cout << "[PS5AudioManager] Stream destroyed" << std::endl;
    }
}

void PS5AudioManager::SetMasterVolume(float volume) {
    master_volume_ = std::clamp(volume, 0.0f, 1.0f);
    
#ifdef PS5_BUILD
    // TODO: Set actual PS5 master volume
    // sceAudioOutSetMainVolume(static_cast<int>(master_volume_ * 100));
#endif
    
    std::cout << "[PS5AudioManager] Master volume set to " << master_volume_ << std::endl;
}

float PS5AudioManager::GetMasterVolume() const {
    return master_volume_;
}

void PS5AudioManager::SetMute(bool muted) {
    is_muted_ = muted;
    
#ifdef PS5_BUILD
    // TODO: Set actual PS5 mute state
    // sceAudioOutSetMute(muted);
#endif
    
    std::cout << "[PS5AudioManager] Audio " << (muted ? "muted" : "unmuted") << std::endl;
}

bool PS5AudioManager::IsMuted() const {
    return is_muted_;
}

bool PS5AudioManager::Is3DAudioAvailable() const {
    return is_3d_audio_available_;
}

void PS5AudioManager::Enable3DAudio(bool enable) {
    if (!is_3d_audio_available_) {
        std::cout << "[PS5AudioManager] 3D Audio not available on this system" << std::endl;
        return;
    }
    
    is_3d_audio_enabled_ = enable;
    
#ifdef PS5_BUILD
    // TODO: Enable/disable PS5 3D audio
    // sceAudio3dSetEnabled(enable);
#endif
    
    std::cout << "[PS5AudioManager] 3D Audio " << (enable ? "enabled" : "disabled") << std::endl;
}

bool PS5AudioManager::Is3DAudioEnabled() const {
    return is_3d_audio_enabled_;
}

uint32_t PS5AudioManager::GetOutputLatency() const {
#ifdef PS5_BUILD
    // TODO: Get actual PS5 output latency
    // return sceAudioOutGetLatency();
    return 20; // Typical PS5 audio latency in ms
#else
    return 50; // Higher latency for development
#endif
}

uint32_t PS5AudioManager::GetInputLatency() const {
#ifdef PS5_BUILD
    // TODO: Get actual PS5 input latency
    // return sceAudioInGetLatency();
    return 15; // Typical PS5 input latency in ms
#else
    return 40; // Higher latency for development
#endif
}

} // namespace PS5Audio