// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>
#include <memory>

namespace PS5Graphics {

class PS5CommandBuffer;

/**
 * GNM (Graphics North) API Wrapper
 * Low-level PS5 graphics interface
 */
class GNMWrapper {
public:
    GNMWrapper();
    ~GNMWrapper();
    
    bool Initialize();
    void Shutdown();
    
    // GPU Memory Management
    uint64_t AllocateGPUMemory(size_t size, uint32_t alignment);
    void FreeGPUMemory(uint64_t address);
    
    // Command Buffer Submission
    void SubmitCommandBuffer(PS5CommandBuffer* cmd_buffer);
    void WaitForIdle();
    
    // Display Output
    bool Present(uint64_t color_buffer_addr, uint32_t width, uint32_t height);
    
    // Resource Creation
    uint64_t CreateBuffer(size_t size, uint32_t usage_flags);
    void DestroyBuffer(uint64_t buffer_handle);
    
    uint64_t CreateTexture(uint32_t width, uint32_t height, uint32_t format, uint32_t usage_flags);
    void DestroyTexture(uint64_t texture_handle);
    
    // Shader Management
    uint64_t CreateComputeShader(const void* shader_data, size_t size);
    uint64_t CreateGraphicsShader(const void* vs_data, size_t vs_size, 
                                  const void* fs_data, size_t fs_size);
    void DestroyShader(uint64_t shader_handle);
    
    // Render State Management
    void SetRenderTargets(uint64_t* color_targets, size_t num_color_targets, 
                         uint64_t depth_target);
    void SetViewport(float x, float y, float width, float height, 
                    float min_depth, float max_depth);
    void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
    
    // Drawing Commands
    void DrawIndexed(uint32_t index_count, uint32_t instance_count, 
                    uint32_t first_index, uint32_t vertex_offset, 
                    uint32_t first_instance);
    void Draw(uint32_t vertex_count, uint32_t instance_count, 
             uint32_t first_vertex, uint32_t first_instance);
    
    // Compute Dispatch
    void Dispatch(uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);
    
    // Synchronization
    void InsertWait();
    void InsertSignal(uint64_t signal_value);
    
private:
    bool InitializeDevice();
    void ShutdownDevice();
    
    bool CreateCommandQueue();
    void DestroyCommandQueue();
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
    
    bool initialized_;
};

} // namespace PS5Graphics