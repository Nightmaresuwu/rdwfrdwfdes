# PS5 Platform Abstraction Layer

add_library(ps5_platform STATIC
    ps5_system.cpp
    ps5_memory.cpp
    ps5_threading.cpp
    ps5_filesystem.cpp
    ps5_network.cpp
)

target_include_directories(ps5_platform PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link PS5-specific libraries when available
if (PLATFORM_PS5 AND EXISTS ${PS5_SDK_ROOT})
    target_link_libraries(ps5_platform
        SceKernel_stub_weak
        ScePosix_stub_weak
        SceNet_stub_weak
        SceNetCtl_stub_weak
    )
endif()

target_compile_definitions(ps5_platform PRIVATE
    PS5_PLATFORM_LAYER=1
)