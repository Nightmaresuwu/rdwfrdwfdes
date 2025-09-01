// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <memory>
#include <vector>
#include <cstdint>

// Forward declarations for Eden video core
namespace VideoCore {
class RendererBase;
class RasterizerInterface;
}

namespace PS5Graphics {

class GNMWrapper;
class PS5CommandBuffer;
class PS5TextureManager;
class PS5ShaderCompiler;

/**
 * PS5 Renderer Implementation
 * Replaces Vulkan/OpenGL renderer with GNM/GNMX
 */
class PS5Renderer {
public:
    PS5Renderer();
    ~PS5Renderer();
    
    // Core Renderer Interface
    bool Initialize();
    void Shutdown();
    
    bool IsInitialized() const { return initialized_; }
    
    // Rendering Operations
    void BeginFrame();
    void EndFrame();
    void Present();
    
    // Resource Management
    uint32_t CreateTexture(uint32_t width, uint32_t height, uint32_t format);
    void DestroyTexture(uint32_t texture_id);
    
    uint32_t CreateShader(const void* shader_data, size_t size, uint32_t type);
    void DestroyShader(uint32_t shader_id);
    
    // Command Buffer Management
    PS5CommandBuffer* GetCurrentCommandBuffer();
    void SubmitCommandBuffer(PS5CommandBuffer* cmd_buffer);
    
    // Display Management
    bool SetDisplayMode(uint32_t width, uint32_t height, uint32_t refresh_rate);
    void GetDisplayDimensions(uint32_t& width, uint32_t& height);
    
    // Memory Management
    uint64_t AllocateGPUMemory(size_t size, uint32_t alignment);
    void FreeGPUMemory(uint64_t address);
    
    // Performance Monitoring
    void BeginPerfCapture(const char* label);
    void EndPerfCapture();
    
private:
    bool InitializeGNM();
    void ShutdownGNM();
    
    bool CreateFramebuffers();
    void DestroyFramebuffers();
    
    std::unique_ptr<GNMWrapper> gnm_wrapper_;
    std::unique_ptr<PS5CommandBuffer> command_buffer_;
    std::unique_ptr<PS5TextureManager> texture_manager_;
    std::unique_ptr<PS5ShaderCompiler> shader_compiler_;
    
    bool initialized_;
    uint32_t display_width_;
    uint32_t display_height_;
    uint32_t current_frame_;
    
    // Frame buffers
    struct FrameBuffer {
        uint64_t color_buffer_addr;
        uint64_t depth_buffer_addr;
        uint32_t width;
        uint32_t height;
    };
    
    static constexpr size_t MAX_FRAME_BUFFERS = 3; // Triple buffering
    FrameBuffer frame_buffers_[MAX_FRAME_BUFFERS];
    size_t current_frame_buffer_;
};

} // namespace PS5Graphics