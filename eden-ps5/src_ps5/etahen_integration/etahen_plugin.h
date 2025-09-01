// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace etaHEN {

/**
 * etaHEN Plugin Integration
 * Integrates Eden with the etaHEN homebrew enabler system
 */
class EtaHENPlugin {
public:
    EtaHENPlugin();
    ~EtaHENPlugin();
    
    // Plugin Lifecycle
    bool Initialize();
    void Shutdown();
    void Update();
    
    bool IsInitialized() const { return initialized_; }
    bool IsEtaHENAvailable() const { return etahen_available_; }
    
    // etaHEN Connection
    bool ConnectToEtaHEN();
    void DisconnectFromEtaHEN();
    bool IsConnectedToEtaHEN() const { return connected_; }
    
    // Plugin Registration
    bool RegisterWithEtaHEN(const std::string& plugin_name, const std::string& version);
    void UnregisterFromEtaHEN();
    
    // Jailbreak Functionality
    bool RequestJailbreak(int pid = -1);
    bool IsProcessJailbroken(int pid = -1);
    
    // File System Access
    bool EnableDataAccess(); // Enable /data access in sandbox
    bool EnableDevAccess();  // Enable /dev access
    
    // PKG Management
    bool InstallPKG(const std::string& pkg_path);
    bool UninstallPKG(const std::string& title_id);
    std::vector<std::string> GetInstalledPKGs();
    
    // FTP Integration
    bool StartFTPServer(uint16_t port = 1337);
    void StopFTPServer();
    bool IsFTPServerRunning() const;
    
    // Klog Integration
    bool StartKlogServer(uint16_t port = 9081);
    void StopKlogServer();
    bool IsKlogServerRunning() const;
    
    // ELF Loader
    bool LoadELF(const std::string& elf_path);
    bool LoadELFFromMemory(const void* elf_data, size_t size);
    
    // Debug Features
    bool EnablePS5Debug();
    void DisablePS5Debug();
    bool IsPS5DebugEnabled() const;
    
    // Cheats Integration (if Illusion plugin is available)
    bool EnableCheats();
    void DisableCheats();
    bool AreCheatsEnabled() const;
    
    // Game Management
    bool LaunchGame(const std::string& title_id);
    bool KillGame(const std::string& title_id);
    std::vector<std::string> GetRunningGames();
    
    // System Information
    std::string GetSystemVersion();
    std::string GetEtaHENVersion();
    uint64_t GetFreeMemory();
    
    // Event Callbacks
    using StatusChangeCallback = std::function<void(bool connected)>;
    using MessageCallback = std::function<void(const std::string& message)>;
    
    void SetStatusChangeCallback(StatusChangeCallback callback);
    void SetMessageCallback(MessageCallback callback);
    
private:
    bool InitializeIPC();
    void ShutdownIPC();
    
    void ProcessEtaHENMessages();
    bool SendIPCCommand(const std::string& command, std::string& response);
    
    bool initialized_;
    bool etahen_available_;
    bool connected_;
    bool registered_;
    
    std::string plugin_name_;
    std::string plugin_version_;
    
    // IPC connection details
    int ipc_socket_;
    uint16_t etahen_port_;
    
    // Service states
    bool ftp_server_running_;
    bool klog_server_running_;
    bool ps5_debug_enabled_;
    bool cheats_enabled_;
    
    StatusChangeCallback status_callback_;
    MessageCallback message_callback_;
};

/**
 * Global etaHEN plugin instance
 */
EtaHENPlugin& GetEtaHENPlugin();

} // namespace etaHEN