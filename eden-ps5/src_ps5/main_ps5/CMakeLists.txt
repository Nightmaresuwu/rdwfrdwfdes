# Eden PS5 Main Application

add_executable(eden-ps5
    main.cpp
    ps5_application.cpp
    eden_core_integration.cpp
)

target_include_directories(eden-ps5 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
)

target_link_libraries(eden-ps5
    ps5_platform
    ps5_graphics
    ps5_audio
    ps5_input
    etahen_integration
    ps5_frontend
    core_ps5
)

# Link original Eden core libraries (modified for PS5)
target_link_libraries(eden-ps5
    # Core Eden libraries will be linked here
    # These need to be modified to work without Qt/SDL
)

# PS5-specific executable properties
if (PLATFORM_PS5)
    set_target_properties(eden-ps5 PROPERTIES
        OUTPUT_NAME "eden_emulator"
        SUFFIX ".elf"
    )
    
    # PS5 executable flags
    target_link_options(eden-ps5 PRIVATE
        -Wl,--oformat=elf64-x86-64
    )
endif()

# Install target for PS5 PKG creation
install(TARGETS eden-ps5 DESTINATION bin)