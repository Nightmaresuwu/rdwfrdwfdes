// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project - PS5 Port
// SPDX-License-Identifier: GPL-3.0-or-later

#include "etahen_plugin.h"
#include "etahen_ipc.h"
#include <iostream>
#include <thread>
#include <chrono>

#ifdef PS5_BUILD
// Include PS5-specific networking headers
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#else
// Include standard networking headers for development
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

namespace etaHEN {

static std::unique_ptr<EtaHENPlugin> g_etahen_plugin;

EtaHENPlugin::EtaHENPlugin()
    : initialized_(false)
    , etahen_available_(false)
    , connected_(false)
    , registered_(false)
    , plugin_name_("Eden Nintendo Switch Emulator")
    , plugin_version_("1.0.0-PS5")
    , ipc_socket_(-1)
    , etahen_port_(9028)
    , ftp_server_running_(false)
    , klog_server_running_(false)
    , ps5_debug_enabled_(false)
    , cheats_enabled_(false) {
}

EtaHENPlugin::~EtaHENPlugin() {
    if (initialized_) {
        Shutdown();
    }
}

bool EtaHENPlugin::Initialize() {
    if (initialized_) {
        return true;
    }
    
    std::cout << "[EtaHENPlugin] Initializing etaHEN integration..." << std::endl;
    
    // Initialize IPC system
    if (!InitializeIPC()) {
        std::cout << "[EtaHENPlugin] Failed to initialize IPC system" << std::endl;
        return false;
    }
    
    // Try to detect etaHEN
    etahen_available_ = ConnectToEtaHEN();
    
    if (etahen_available_) {
        // Register as a plugin
        if (RegisterWithEtaHEN(plugin_name_, plugin_version_)) {
            std::cout << "[EtaHENPlugin] Successfully registered with etaHEN" << std::endl;
        } else {
            std::cout << "[EtaHENPlugin] Failed to register with etaHEN, but continuing..." << std::endl;
        }
    } else {
        std::cout << "[EtaHENPlugin] etaHEN not detected, running in standalone mode" << std::endl;
    }
    
    initialized_ = true;
    std::cout << "[EtaHENPlugin] etaHEN integration initialized" << std::endl;
    std::cout << "[EtaHENPlugin] etaHEN Available: " << (etahen_available_ ? "Yes" : "No") << std::endl;
    
    return true;
}

void EtaHENPlugin::Shutdown() {
    if (!initialized_) {
        return;
    }
    
    std::cout << "[EtaHENPlugin] Shutting down etaHEN integration..." << std::endl;
    
    // Stop any running services
    StopFTPServer();
    StopKlogServer();
    DisablePS5Debug();
    
    // Unregister from etaHEN
    if (registered_) {
        UnregisterFromEtaHEN();
    }
    
    // Disconnect from etaHEN
    if (connected_) {
        DisconnectFromEtaHEN();
    }
    
    // Shutdown IPC
    ShutdownIPC();
    
    initialized_ = false;
}

void EtaHENPlugin::Update() {
    if (!initialized_) {
        return;
    }
    
    // Process any incoming etaHEN messages
    if (connected_) {
        ProcessEtaHENMessages();
    }
}

bool EtaHENPlugin::InitializeIPC() {
#ifdef PS5_BUILD
    std::cout << "[EtaHENPlugin] Initializing PS5 IPC system..." << std::endl;
    
    // Initialize PS5 networking if needed
    // TODO: Add PS5-specific networking initialization
    
    return true;
#else
    std::cout << "[EtaHENPlugin] IPC system initialized (development mode)" << std::endl;
    return true;
#endif
}

void EtaHENPlugin::ShutdownIPC() {
    if (ipc_socket_ != -1) {
        close(ipc_socket_);
        ipc_socket_ = -1;
    }
}

bool EtaHENPlugin::ConnectToEtaHEN() {
    std::cout << "[EtaHENPlugin] Attempting to connect to etaHEN..." << std::endl;
    
#ifdef PS5_BUILD
    // Try to connect to etaHEN IPC server on localhost:9028
    ipc_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (ipc_socket_ < 0) {
        std::cout << "[EtaHENPlugin] Failed to create socket" << std::endl;
        return false;
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(etahen_port_);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(ipc_socket_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cout << "[EtaHENPlugin] Failed to connect to etaHEN (port " << etahen_port_ << ")" << std::endl;
        close(ipc_socket_);
        ipc_socket_ = -1;
        return false;
    }
    
    connected_ = true;
    std::cout << "[EtaHENPlugin] Connected to etaHEN successfully" << std::endl;
    
    if (status_callback_) {
        status_callback_(true);
    }
    
    return true;
#else
    std::cout << "[EtaHENPlugin] etaHEN connection simulated (development mode)" << std::endl;
    connected_ = false; // Not actually connected in dev mode
    return false;
#endif
}

void EtaHENPlugin::DisconnectFromEtaHEN() {
    if (!connected_) {
        return;
    }
    
    std::cout << "[EtaHENPlugin] Disconnecting from etaHEN..." << std::endl;
    
    if (ipc_socket_ != -1) {
        close(ipc_socket_);
        ipc_socket_ = -1;
    }
    
    connected_ = false;
    
    if (status_callback_) {
        status_callback_(false);
    }
}

bool EtaHENPlugin::RegisterWithEtaHEN(const std::string& plugin_name, const std::string& version) {
    if (!connected_) {
        return false;
    }
    
    std::cout << "[EtaHENPlugin] Registering plugin: " << plugin_name << " v" << version << std::endl;
    
    plugin_name_ = plugin_name;
    plugin_version_ = version;
    
    // TODO: Send registration command to etaHEN
    std::string command = "REGISTER_PLUGIN:" + plugin_name + ":" + version;
    std::string response;
    
    if (SendIPCCommand(command, response)) {
        registered_ = true;
        std::cout << "[EtaHENPlugin] Plugin registered successfully" << std::endl;
        return true;
    }
    
    std::cout << "[EtaHENPlugin] Failed to register plugin" << std::endl;
    return false;
}

void EtaHENPlugin::UnregisterFromEtaHEN() {
    if (!registered_ || !connected_) {
        return;
    }
    
    std::cout << "[EtaHENPlugin] Unregistering plugin..." << std::endl;
    
    std::string command = "UNREGISTER_PLUGIN:" + plugin_name_;
    std::string response;
    SendIPCCommand(command, response);
    
    registered_ = false;
}

bool EtaHENPlugin::RequestJailbreak(int pid) {
    if (!connected_) {
        std::cout << "[EtaHENPlugin] Cannot request jailbreak - not connected to etaHEN" << std::endl;
        return false;
    }
    
    if (pid == -1) {
        pid = getpid(); // Use current process ID
    }
    
    std::cout << "[EtaHENPlugin] Requesting jailbreak for PID " << pid << std::endl;
    
    std::string command = "JAILBREAK:" + std::to_string(pid);
    std::string response;
    
    if (SendIPCCommand(command, response)) {
        std::cout << "[EtaHENPlugin] Jailbreak request successful" << std::endl;
        return true;
    }
    
    std::cout << "[EtaHENPlugin] Jailbreak request failed" << std::endl;
    return false;
}

bool EtaHENPlugin::IsProcessJailbroken(int pid) {
    if (pid == -1) {
        pid = getpid();
    }
    
    // TODO: Check if process is jailbroken
    // This would involve checking process capabilities or making a test call
    
    return connected_; // Assume jailbroken if connected to etaHEN
}

bool EtaHENPlugin::EnableDataAccess() {
    if (!connected_) {
        return false;
    }
    
    std::cout << "[EtaHENPlugin] Enabling /data access..." << std::endl;
    
    std::string command = "ENABLE_DATA_ACCESS";
    std::string response;
    
    return SendIPCCommand(command, response);
}

bool EtaHENPlugin::StartFTPServer(uint16_t port) {
    if (ftp_server_running_) {
        return true;
    }
    
    if (!connected_) {
        std::cout << "[EtaHENPlugin] Cannot start FTP server - not connected to etaHEN" << std::endl;
        return false;
    }
    
    std::cout << "[EtaHENPlugin] Starting FTP server on port " << port << std::endl;
    
    std::string command = "START_FTP:" + std::to_string(port);
    std::string response;
    
    if (SendIPCCommand(command, response)) {
        ftp_server_running_ = true;
        std::cout << "[EtaHENPlugin] FTP server started successfully" << std::endl;
        return true;
    }
    
    std::cout << "[EtaHENPlugin] Failed to start FTP server" << std::endl;
    return false;
}

void EtaHENPlugin::StopFTPServer() {
    if (!ftp_server_running_ || !connected_) {
        return;
    }
    
    std::cout << "[EtaHENPlugin] Stopping FTP server..." << std::endl;
    
    std::string command = "STOP_FTP";
    std::string response;
    SendIPCCommand(command, response);
    
    ftp_server_running_ = false;
}

bool EtaHENPlugin::LoadELF(const std::string& elf_path) {
    if (!connected_) {
        std::cout << "[EtaHENPlugin] Cannot load ELF - not connected to etaHEN" << std::endl;
        return false;
    }
    
    std::cout << "[EtaHENPlugin] Loading ELF: " << elf_path << std::endl;
    
    std::string command = "LOAD_ELF:" + elf_path;
    std::string response;
    
    return SendIPCCommand(command, response);
}

bool EtaHENPlugin::SendIPCCommand(const std::string& command, std::string& response) {
    if (ipc_socket_ == -1) {
        return false;
    }
    
    // Send command
    if (send(ipc_socket_, command.c_str(), command.length(), 0) < 0) {
        std::cout << "[EtaHENPlugin] Failed to send IPC command" << std::endl;
        return false;
    }
    
    // Receive response
    char buffer[1024];
    int bytes_received = recv(ipc_socket_, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        std::cout << "[EtaHENPlugin] Failed to receive IPC response" << std::endl;
        return false;
    }
    
    buffer[bytes_received] = '\0';
    response = std::string(buffer);
    
    return true;
}

void EtaHENPlugin::ProcessEtaHENMessages() {
    // TODO: Process any incoming messages from etaHEN
    // This could include status updates, notifications, etc.
}

std::string EtaHENPlugin::GetSystemVersion() {
#ifdef PS5_BUILD
    // TODO: Get actual PS5 system version via etaHEN
    return "PS5 System Version 5.50";
#else
    return "Development Environment";
#endif
}

std::string EtaHENPlugin::GetEtaHENVersion() {
    if (!connected_) {
        return "Not Connected";
    }
    
    std::string command = "GET_VERSION";
    std::string response;
    
    if (SendIPCCommand(command, response)) {
        return response;
    }
    
    return "Unknown";
}

void EtaHENPlugin::SetStatusChangeCallback(StatusChangeCallback callback) {
    status_callback_ = callback;
}

void EtaHENPlugin::SetMessageCallback(MessageCallback callback) {
    message_callback_ = callback;
}

// Global instance accessor
EtaHENPlugin& GetEtaHENPlugin() {
    if (!g_etahen_plugin) {
        g_etahen_plugin = std::make_unique<EtaHENPlugin>();
    }
    return *g_etahen_plugin;
}

} // namespace etaHEN