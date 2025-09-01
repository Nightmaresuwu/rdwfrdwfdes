# PS5 Audio System Abstraction

add_library(ps5_audio STATIC
    ps5_audio_manager.cpp
    ps5_audio_renderer.cpp
    ps5_audio_device.cpp
    ps5_audio_stream.cpp
)

target_include_directories(ps5_audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/audio_core
)

target_link_libraries(ps5_audio
    ps5_platform
)

# Link PS5 Audio libraries when available
if (PLATFORM_PS5 AND EXISTS ${PS5_SDK_ROOT})
    target_link_libraries(ps5_audio
        SceAudioOut_stub_weak
        SceAudioIn_stub_weak
        SceAudio3d_stub_weak
    )
endif()

target_compile_definitions(ps5_audio PRIVATE
    PS5_AUDIO_LAYER=1
    USE_PS5_AUDIO=1
)