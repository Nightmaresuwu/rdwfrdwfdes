# etaHEN Integration Layer

add_library(etahen_integration STATIC
    etahen_plugin.cpp
    etahen_ipc.cpp
    etahen_jailbreak.cpp
    etahen_util.cpp
)

target_include_directories(etahen_integration PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(etahen_integration
    ps5_platform
)

# Link networking libraries for IPC
if (PLATFORM_PS5 AND EXISTS ${PS5_SDK_ROOT})
    target_link_libraries(etahen_integration
        SceNet_stub_weak
        SceNetCtl_stub_weak
    )
endif()

target_compile_definitions(etahen_integration PRIVATE
    ETAHEN_INTEGRATION=1
    ETAHEN_PLUGIN_ENABLED=1
)