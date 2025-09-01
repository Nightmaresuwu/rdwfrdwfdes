# PS5 Port Source Directory

# PS5 Platform Abstraction Layer
add_subdirectory(ps5_platform)

# PS5 Graphics Abstraction (GNM/GNMX)
add_subdirectory(ps5_graphics)

# PS5 Audio System
add_subdirectory(ps5_audio)

# PS5 Input System
add_subdirectory(ps5_input)

# etaHEN Integration
add_subdirectory(etahen_integration)

# Core Eden modifications
add_subdirectory(core_ps5)

# PS5 Frontend (replaces Qt GUI)
add_subdirectory(ps5_frontend)

# Main PS5 executable
add_subdirectory(main_ps5)