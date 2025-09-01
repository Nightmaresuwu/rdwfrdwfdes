# PS5 Input System (Controller Management)

add_library(ps5_input STATIC
    ps5_input_manager.cpp
    ps5_controller.cpp
    ps5_touchpad.cpp
    ps5_motion_sensor.cpp
)

target_include_directories(ps5_input PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/input_common
)

target_link_libraries(ps5_input
    ps5_platform
)

# Link PS5 Input libraries when available
if (PLATFORM_PS5 AND EXISTS ${PS5_SDK_ROOT})
    target_link_libraries(ps5_input
        ScePad_stub_weak
        SceTouch_stub_weak
        SceMotion_stub_weak
    )
endif()

target_compile_definitions(ps5_input PRIVATE
    PS5_INPUT_LAYER=1
    USE_PS5_CONTROLLERS=1
)