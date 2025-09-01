// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <cstdint>

namespace PS5Input {

class PS5Controller;
class PS5Touchpad;
class PS5MotionSensor;

// Input event types
enum class InputEventType {
    ButtonPress,
    ButtonRelease,
    AxisMove,
    TouchpadTouch,
    TouchpadRelease,
    MotionUpdate
};

struct InputEvent {
    InputEventType type;
    uint32_t controller_id;
    uint32_t button_id;
    float value;
    float x, y; // For touchpad and motion
    uint64_t timestamp;
};

using InputEventCallback = std::function<void(const InputEvent&)>;

/**
 * PS5 Input Manager
 * Manages PS5 DualSense controllers, replacing SDL input
 */
class PS5InputManager {
public:
    PS5InputManager();
    ~PS5InputManager();
    
    // Core Input Interface
    bool Initialize();
    void Shutdown();
    void Update(); // Poll for input events
    
    bool IsInitialized() const { return initialized_; }
    
    // Controller Management
    std::vector<PS5Controller*> GetConnectedControllers();
    PS5Controller* GetController(uint32_t controller_id);
    uint32_t GetControllerCount() const;
    
    // Controller Events
    void SetControllerConnectedCallback(std::function<void(uint32_t)> callback);
    void SetControllerDisconnectedCallback(std::function<void(uint32_t)> callback);
    
    // Input Events
    void SetInputEventCallback(InputEventCallback callback);
    
    // Haptic Feedback
    void SetHapticFeedback(uint32_t controller_id, float left_motor, float right_motor);
    void SetTriggerFeedback(uint32_t controller_id, float left_trigger, float right_trigger);
    
    // Adaptive Triggers (DualSense specific)
    void SetAdaptiveTriggers(uint32_t controller_id, bool left_enabled, bool right_enabled);
    void SetTriggerResistance(uint32_t controller_id, float left_resistance, float right_resistance);
    
    // LED Control
    void SetControllerLED(uint32_t controller_id, uint8_t r, uint8_t g, uint8_t b);
    
    // Motion Control
    bool IsMotionControlAvailable(uint32_t controller_id);
    void EnableMotionControl(uint32_t controller_id, bool enable);
    
    // Touchpad
    bool IsTouchpadAvailable(uint32_t controller_id);
    PS5Touchpad* GetTouchpad(uint32_t controller_id);
    
    // Battery Status
    float GetBatteryLevel(uint32_t controller_id); // 0.0 - 1.0
    bool IsControllerCharging(uint32_t controller_id);
    
    // Input Mapping for Nintendo Switch emulation
    struct SwitchButtonMapping {
        uint32_t ps5_button;
        uint32_t switch_button;
    };
    
    void SetSwitchButtonMapping(const std::vector<SwitchButtonMapping>& mapping);
    void MapPS5ToSwitchInput(const InputEvent& ps5_event, InputEvent& switch_event);
    
private:
    bool InitializePS5InputSystem();
    void ShutdownPS5InputSystem();
    
    void PollControllers();
    void ProcessControllerEvents();
    
    void OnControllerConnected(uint32_t controller_id);
    void OnControllerDisconnected(uint32_t controller_id);
    
    std::vector<std::unique_ptr<PS5Controller>> controllers_;
    std::vector<SwitchButtonMapping> button_mapping_;
    
    InputEventCallback input_event_callback_;
    std::function<void(uint32_t)> controller_connected_callback_;
    std::function<void(uint32_t)> controller_disconnected_callback_;
    
    bool initialized_;
    uint64_t last_update_time_;
};

} // namespace PS5Input