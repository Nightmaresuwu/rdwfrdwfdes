// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ps5_input_manager.h"
#include "ps5_controller.h"
#include "ps5_touchpad.h"
#include "ps5_motion_sensor.h"
#include <iostream>
#include <chrono>
#include <algorithm>

#ifdef PS5_BUILD
// Include PS5 input headers when building for actual PS5
// #include <pad.h>
// #include <libscetouch.h>
// #include <libscemotion.h>
#endif

namespace PS5Input {

PS5InputManager::PS5InputManager()
    : initialized_(false)
    , last_update_time_(0) {
}

PS5InputManager::~PS5InputManager() {
    if (initialized_) {
        Shutdown();
    }
}

bool PS5InputManager::Initialize() {
    if (initialized_) {
        return true;
    }
    
    std::cout << "[PS5InputManager] Initializing PS5 Input System..." << std::endl;
    
    if (!InitializePS5InputSystem()) {
        std::cout << "[PS5InputManager] Failed to initialize PS5 input system" << std::endl;
        return false;
    }
    
    // Set up default Nintendo Switch button mapping
    SetSwitchButtonMapping({
        {0x0001, 0x0001}, // Cross -> A
        {0x0002, 0x0002}, // Circle -> B  
        {0x0004, 0x0004}, // Square -> Y
        {0x0008, 0x0008}, // Triangle -> X
        {0x0010, 0x0010}, // L1 -> L
        {0x0020, 0x0020}, // R1 -> R
        {0x0040, 0x0040}, // L2 -> ZL
        {0x0080, 0x0080}, // R2 -> ZR
        {0x0100, 0x0100}, // Share -> Minus
        {0x0200, 0x0200}, // Options -> Plus
        {0x0400, 0x0400}, // L3 -> Left Stick
        {0x0800, 0x0800}, // R3 -> Right Stick
        {0x1000, 0x1000}, // PS Button -> Home
        {0x2000, 0x2000}, // Touchpad -> Capture
    });
    
    initialized_ = true;
    std::cout << "[PS5InputManager] Input system initialized successfully" << std::endl;
    
    return true;
}

void PS5InputManager::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[PS5InputManager] Shutting down PS5 Input System..." << std::endl;
    
    // Clear all controllers
    controllers_.clear();
    
    // Shutdown PS5 input system
    ShutdownPS5InputSystem();
    
    initialized_ = false;
}

bool PS5InputManager::InitializePS5InputSystem() {
#ifdef PS5_BUILD
    std::cout << "[PS5InputManager] Initializing PS5 native input system..." << std::endl;
    
    // TODO: Initialize PS5 input subsystems
    // int result = scePadInit();
    // if (result < 0) {
    //     std::cout << "[PS5InputManager] Failed to initialize pad system: " << std::hex << result << std::endl;
    //     return false;
    // }
    
    // result = sceTouchInit();
    // if (result < 0) {
    //     std::cout << "[PS5InputManager] Failed to initialize touch system: " << std::hex << result << std::endl;
    //     return false;
    // }
    
    // result = sceMotionInit();
    // if (result < 0) {
    //     std::cout << "[PS5InputManager] Failed to initialize motion system: " << std::hex << result << std::endl;
    //     return false;
    // }
    
    return true;
#else
    std::cout << "[PS5InputManager] PS5 input system initialized (development mode)" << std::endl;
    return true;
#endif
}

void PS5InputManager::ShutdownPS5InputSystem() {
#ifdef PS5_BUILD
    // TODO: Shutdown PS5 input subsystems
    // sceMotionTerm();
    // sceTouchTerm();
    // scePadTerm();
#endif
}

void PS5InputManager::Update() {
    if (!initialized_) {
        return;
    }
    
    // Update timestamp
    auto now = std::chrono::steady_clock::now();
    last_update_time_ = std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();
    
    // Poll for controller changes and input events
    PollControllers();
    ProcessControllerEvents();
}

void PS5InputManager::PollControllers() {
#ifdef PS5_BUILD
    // TODO: Poll for controller connections/disconnections
    // for (int i = 0; i < 4; ++i) {
    //     int handle = scePadOpen(SCE_USER_SERVICE_USER_ID_SYSTEM, i, 0, NULL);
    //     if (handle >= 0) {
    //         // Controller connected
    //         if (i >= controllers_.size() || !controllers_[i]) {
    //             OnControllerConnected(i);
    //         }
    //         scePadClose(handle);
    //     } else {
    //         // Controller disconnected
    //         if (i < controllers_.size() && controllers_[i]) {
    //             OnControllerDisconnected(i);
    //         }
    //     }
    // }
#else
    // Development mode - simulate one controller
    if (controllers_.empty()) {
        OnControllerConnected(0);
    }
#endif
}

void PS5InputManager::ProcessControllerEvents() {
    for (auto& controller : controllers_) {
        if (controller && controller->IsConnected()) {
            controller->Update();
            
            // Process input events from this controller
            auto events = controller->GetInputEvents();
            for (const auto& event : events) {
                if (input_event_callback_) {
                    // Map PS5 input to Nintendo Switch input if needed
                    InputEvent switch_event;
                    MapPS5ToSwitchInput(event, switch_event);
                    input_event_callback_(switch_event);
                }
            }
        }
    }
}

void PS5InputManager::OnControllerConnected(uint32_t controller_id) {
    std::cout << "[PS5InputManager] Controller " << controller_id << " connected" << std::endl;
    
    // Ensure we have space for this controller
    if (controller_id >= controllers_.size()) {
        controllers_.resize(controller_id + 1);
    }
    
    // Create new controller instance
    auto controller = std::make_unique<PS5Controller>(controller_id);
    if (controller->Initialize()) {
        controllers_[controller_id] = std::move(controller);
        
        if (controller_connected_callback_) {
            controller_connected_callback_(controller_id);
        }
    }
}

void PS5InputManager::OnControllerDisconnected(uint32_t controller_id) {
    std::cout << "[PS5InputManager] Controller " << controller_id << " disconnected" << std::endl;
    
    if (controller_id < controllers_.size() && controllers_[controller_id]) {
        controllers_[controller_id].reset();
        
        if (controller_disconnected_callback_) {
            controller_disconnected_callback_(controller_id);
        }
    }
}

std::vector<PS5Controller*> PS5InputManager::GetConnectedControllers() {
    std::vector<PS5Controller*> connected;
    for (const auto& controller : controllers_) {
        if (controller && controller->IsConnected()) {
            connected.push_back(controller.get());
        }
    }
    return connected;
}

PS5Controller* PS5InputManager::GetController(uint32_t controller_id) {
    if (controller_id < controllers_.size()) {
        return controllers_[controller_id].get();
    }
    return nullptr;
}

uint32_t PS5InputManager::GetControllerCount() const {
    uint32_t count = 0;
    for (const auto& controller : controllers_) {
        if (controller && controller->IsConnected()) {
            count++;
        }
    }
    return count;
}

void PS5InputManager::SetControllerConnectedCallback(std::function<void(uint32_t)> callback) {
    controller_connected_callback_ = callback;
}

void PS5InputManager::SetControllerDisconnectedCallback(std::function<void(uint32_t)> callback) {
    controller_disconnected_callback_ = callback;
}

void PS5InputManager::SetInputEventCallback(InputEventCallback callback) {
    input_event_callback_ = callback;
}

void PS5InputManager::SetHapticFeedback(uint32_t controller_id, float left_motor, float right_motor) {
    auto* controller = GetController(controller_id);
    if (controller) {
        controller->SetHapticFeedback(left_motor, right_motor);
    }
}

void PS5InputManager::SetTriggerFeedback(uint32_t controller_id, float left_trigger, float right_trigger) {
    auto* controller = GetController(controller_id);
    if (controller) {
        controller->SetTriggerFeedback(left_trigger, right_trigger);
    }
}

void PS5InputManager::SetAdaptiveTriggers(uint32_t controller_id, bool left_enabled, bool right_enabled) {
    auto* controller = GetController(controller_id);
    if (controller) {
        controller->SetAdaptiveTriggers(left_enabled, right_enabled);
    }
}

void PS5InputManager::SetTriggerResistance(uint32_t controller_id, float left_resistance, float right_resistance) {
    auto* controller = GetController(controller_id);
    if (controller) {
        controller->SetTriggerResistance(left_resistance, right_resistance);
    }
}

void PS5InputManager::SetControllerLED(uint32_t controller_id, uint8_t r, uint8_t g, uint8_t b) {
    auto* controller = GetController(controller_id);
    if (controller) {
        controller->SetLEDColor(r, g, b);
    }
}

bool PS5InputManager::IsMotionControlAvailable(uint32_t controller_id) {
    auto* controller = GetController(controller_id);
    return controller ? controller->IsMotionControlAvailable() : false;
}

void PS5InputManager::EnableMotionControl(uint32_t controller_id, bool enable) {
    auto* controller = GetController(controller_id);
    if (controller) {
        controller->EnableMotionControl(enable);
    }
}

bool PS5InputManager::IsTouchpadAvailable(uint32_t controller_id) {
    auto* controller = GetController(controller_id);
    return controller ? controller->IsTouchpadAvailable() : false;
}

PS5Touchpad* PS5InputManager::GetTouchpad(uint32_t controller_id) {
    auto* controller = GetController(controller_id);
    return controller ? controller->GetTouchpad() : nullptr;
}

float PS5InputManager::GetBatteryLevel(uint32_t controller_id) {
    auto* controller = GetController(controller_id);
    return controller ? controller->GetBatteryLevel() : 0.0f;
}

bool PS5InputManager::IsControllerCharging(uint32_t controller_id) {
    auto* controller = GetController(controller_id);
    return controller ? controller->IsCharging() : false;
}

void PS5InputManager::SetSwitchButtonMapping(const std::vector<SwitchButtonMapping>& mapping) {
    button_mapping_ = mapping;
    std::cout << "[PS5InputManager] Button mapping updated with " << mapping.size() << " mappings" << std::endl;
}

void PS5InputManager::MapPS5ToSwitchInput(const InputEvent& ps5_event, InputEvent& switch_event) {
    // Copy base event data
    switch_event = ps5_event;
    
    // Map PS5 button to Nintendo Switch button
    if (ps5_event.type == InputEventType::ButtonPress || ps5_event.type == InputEventType::ButtonRelease) {
        for (const auto& mapping : button_mapping_) {
            if (mapping.ps5_button == ps5_event.button_id) {
                switch_event.button_id = mapping.switch_button;
                break;
            }
        }
    }
    
    // Additional mappings for axes, touchpad, etc. can be added here
}

} // namespace PS5Input