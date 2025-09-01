// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ps5_renderer.h"
#include "gnm_wrapper.h"
#include "ps5_command_buffer.h"
#include "ps5_texture_manager.h"
#include "ps5_shader_compiler.h"
#include <iostream>

namespace PS5Graphics {

PS5Renderer::PS5Renderer()
    : initialized_(false)
    , display_width_(1920)
    , display_height_(1080)
    , current_frame_(0)
    , current_frame_buffer_(0) {
}

PS5Renderer::~PS5Renderer() {
    if (initialized_) {
        Shutdown();
    }
}

bool PS5Renderer::Initialize() {
    if (initialized_) {
        return true;
    }
    
    std::cout << "[PS5Renderer] Initializing PS5 Graphics System..." << std::endl;
    
    // Initialize GNM/GNMX wrapper
    gnm_wrapper_ = std::make_unique<GNMWrapper>();
    if (!gnm_wrapper_->Initialize()) {
        std::cout << "[PS5Renderer] Failed to initialize GNM wrapper" << std::endl;
        return false;
    }
    
    // Initialize command buffer system
    command_buffer_ = std::make_unique<PS5CommandBuffer>();
    if (!command_buffer_->Initialize()) {
        std::cout << "[PS5Renderer] Failed to initialize command buffer" << std::endl;
        return false;
    }
    
    // Initialize texture manager
    texture_manager_ = std::make_unique<PS5TextureManager>();
    if (!texture_manager_->Initialize()) {
        std::cout << "[PS5Renderer] Failed to initialize texture manager" << std::endl;
        return false;
    }
    
    // Initialize shader compiler
    shader_compiler_ = std::make_unique<PS5ShaderCompiler>();
    if (!shader_compiler_->Initialize()) {
        std::cout << "[PS5Renderer] Failed to initialize shader compiler" << std::endl;
        return false;
    }
    
    // Initialize GNM graphics context
    if (!InitializeGNM()) {
        std::cout << "[PS5Renderer] Failed to initialize GNM graphics context" << std::endl;
        return false;
    }
    
    // Create frame buffers
    if (!CreateFramebuffers()) {
        std::cout << "[PS5Renderer] Failed to create frame buffers" << std::endl;
        return false;
    }
    
    initialized_ = true;
    std::cout << "[PS5Renderer] Graphics system initialized successfully" << std::endl;
    std::cout << "[PS5Renderer] Display: " << display_width_ << "x" << display_height_ << std::endl;
    
    return true;
}

void PS5Renderer::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[PS5Renderer] Shutting down PS5 Graphics System..." << std::endl;
    
    // Wait for GPU to finish any pending work
    if (gnm_wrapper_) {
        gnm_wrapper_->WaitForIdle();
    }
    
    // Destroy frame buffers
    DestroyFramebuffers();
    
    // Shutdown subsystems
    if (shader_compiler_) {
        shader_compiler_->Shutdown();
    }
    
    if (texture_manager_) {
        texture_manager_->Shutdown();
    }
    
    if (command_buffer_) {
        command_buffer_->Shutdown();
    }
    
    // Shutdown GNM
    ShutdownGNM();
    
    if (gnm_wrapper_) {
        gnm_wrapper_->Shutdown();
    }
    
    initialized_ = false;
}

bool PS5Renderer::InitializeGNM() {
#ifdef PS5_BUILD
    // TODO: Initialize actual GNM graphics context
    std::cout << "[PS5Renderer] Initializing GNM graphics context..." << std::endl;
    
    // Set up display output
    if (!SetDisplayMode(display_width_, display_height_, 60)) {
        return false;
    }
    
    return true;
#else
    std::cout << "[PS5Renderer] GNM initialization (development mode)" << std::endl;
    return true;
#endif
}

void PS5Renderer::ShutdownGNM() {
#ifdef PS5_BUILD
    // TODO: Shutdown GNM graphics context
    std::cout << "[PS5Renderer] Shutting down GNM graphics context..." << std::endl;
#endif
}

bool PS5Renderer::CreateFramebuffers() {
    std::cout << "[PS5Renderer] Creating frame buffers..." << std::endl;
    
    for (size_t i = 0; i < MAX_FRAME_BUFFERS; ++i) {
        FrameBuffer& fb = frame_buffers_[i];
        
        // Calculate buffer sizes
        size_t color_buffer_size = display_width_ * display_height_ * 4; // RGBA8
        size_t depth_buffer_size = display_width_ * display_height_ * 4; // D24S8
        
        // Allocate GPU memory for color buffer
        fb.color_buffer_addr = AllocateGPUMemory(color_buffer_size, 4096);
        if (fb.color_buffer_addr == 0) {
            std::cout << "[PS5Renderer] Failed to allocate color buffer " << i << std::endl;
            return false;
        }
        
        // Allocate GPU memory for depth buffer
        fb.depth_buffer_addr = AllocateGPUMemory(depth_buffer_size, 4096);
        if (fb.depth_buffer_addr == 0) {
            std::cout << "[PS5Renderer] Failed to allocate depth buffer " << i << std::endl;
            return false;
        }
        
        fb.width = display_width_;
        fb.height = display_height_;
        
        std::cout << "[PS5Renderer] Frame buffer " << i << " created successfully" << std::endl;
    }
    
    return true;
}

void PS5Renderer::DestroyFramebuffers() {
    for (size_t i = 0; i < MAX_FRAME_BUFFERS; ++i) {
        FrameBuffer& fb = frame_buffers_[i];
        
        if (fb.color_buffer_addr != 0) {
            FreeGPUMemory(fb.color_buffer_addr);
            fb.color_buffer_addr = 0;
        }
        
        if (fb.depth_buffer_addr != 0) {
            FreeGPUMemory(fb.depth_buffer_addr);
            fb.depth_buffer_addr = 0;
        }
    }
}

void PS5Renderer::BeginFrame() {
    current_frame_++;
    current_frame_buffer_ = current_frame_ % MAX_FRAME_BUFFERS;
    
    if (command_buffer_) {
        command_buffer_->Begin();
    }
}

void PS5Renderer::EndFrame() {
    if (command_buffer_) {
        command_buffer_->End();
    }
}

void PS5Renderer::Present() {
    // Submit command buffer
    if (command_buffer_) {
        SubmitCommandBuffer(command_buffer_.get());
    }
    
    // Present current frame buffer to display
    if (gnm_wrapper_) {
        const FrameBuffer& fb = frame_buffers_[current_frame_buffer_];
        gnm_wrapper_->Present(fb.color_buffer_addr, display_width_, display_height_);
    }
}

uint64_t PS5Renderer::AllocateGPUMemory(size_t size, uint32_t alignment) {
    if (gnm_wrapper_) {
        return gnm_wrapper_->AllocateGPUMemory(size, alignment);
    }
    return 0;
}

void PS5Renderer::FreeGPUMemory(uint64_t address) {
    if (gnm_wrapper_) {
        gnm_wrapper_->FreeGPUMemory(address);
    }
}

bool PS5Renderer::SetDisplayMode(uint32_t width, uint32_t height, uint32_t refresh_rate) {
    display_width_ = width;
    display_height_ = height;
    
#ifdef PS5_BUILD
    // TODO: Set actual display mode on PS5
    std::cout << "[PS5Renderer] Setting display mode: " << width << "x" << height << "@" << refresh_rate << "Hz" << std::endl;
    return true;
#else
    std::cout << "[PS5Renderer] Display mode set (development): " << width << "x" << height << "@" << refresh_rate << "Hz" << std::endl;
    return true;
#endif
}

void PS5Renderer::GetDisplayDimensions(uint32_t& width, uint32_t& height) {
    width = display_width_;
    height = display_height_;
}

PS5CommandBuffer* PS5Renderer::GetCurrentCommandBuffer() {
    return command_buffer_.get();
}

void PS5Renderer::SubmitCommandBuffer(PS5CommandBuffer* cmd_buffer) {
    if (gnm_wrapper_ && cmd_buffer) {
        gnm_wrapper_->SubmitCommandBuffer(cmd_buffer);
    }
}

uint32_t PS5Renderer::CreateTexture(uint32_t width, uint32_t height, uint32_t format) {
    if (texture_manager_) {
        return texture_manager_->CreateTexture(width, height, format);
    }
    return 0;
}

void PS5Renderer::DestroyTexture(uint32_t texture_id) {
    if (texture_manager_) {
        texture_manager_->DestroyTexture(texture_id);
    }
}

uint32_t PS5Renderer::CreateShader(const void* shader_data, size_t size, uint32_t type) {
    if (shader_compiler_) {
        return shader_compiler_->CompileShader(shader_data, size, type);
    }
    return 0;
}

void PS5Renderer::DestroyShader(uint32_t shader_id) {
    if (shader_compiler_) {
        shader_compiler_->DestroyShader(shader_id);
    }
}

void PS5Renderer::BeginPerfCapture(const char* label) {
#ifdef PS5_BUILD
    // TODO: Begin performance capture using PS5 tools
    std::cout << "[PS5Renderer] Begin perf capture: " << label << std::endl;
#endif
}

void PS5Renderer::EndPerfCapture() {
#ifdef PS5_BUILD
    // TODO: End performance capture
    std::cout << "[PS5Renderer] End perf capture" << std::endl;
#endif
}

} // namespace PS5Graphics