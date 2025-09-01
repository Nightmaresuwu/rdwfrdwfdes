# PS5 Graphics Abstraction Layer (GNM/GNMX)

add_library(ps5_graphics STATIC
    ps5_renderer.cpp
    gnm_wrapper.cpp
    gnmx_wrapper.cpp
    ps5_shader_compiler.cpp
    ps5_texture_manager.cpp
    ps5_command_buffer.cpp
)

target_include_directories(ps5_graphics PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/video_core
)

target_link_libraries(ps5_graphics
    ps5_platform
)

# Link PS5 Graphics libraries when available
if (PLATFORM_PS5 AND EXISTS ${PS5_SDK_ROOT})
    target_link_libraries(ps5_graphics
        SceGnmDriver_stub_weak
        SceGnf_stub_weak
        SceShaderParser_stub_weak
    )
endif()

target_compile_definitions(ps5_graphics PRIVATE
    PS5_GRAPHICS_LAYER=1
    USE_GNM_GNMX=1
)