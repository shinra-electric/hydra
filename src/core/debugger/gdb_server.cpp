#include "core/debugger/gdb_server.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "core/debugger/debugger_manager.hpp"
#include "core/horizon/kernel/guest_thread.hpp"
#include "core/hw/tegra_x1/cpu/thread.hpp"

namespace hydra::debugger {

namespace {

constexpr u16 GDB_PORT = 1234;

constexpr char GDB_START = '$';
constexpr char GDB_END = '#';
constexpr char GDB_ACK = '+';
constexpr char GDB_NACK = '-';

constexpr char GDB_INT3 = 0x03;
constexpr int GDB_SIGTRAP = 5;

constexpr char GDB_OK[] = "OK";
constexpr char GDB_ERROR[] = "E01";
constexpr char GDB_EMPTY[] = "";

constexpr u32 FP_REGISTER = 29;
constexpr u32 LR_REGISTER = 30;
constexpr u32 SP_REGISTER = 31;
constexpr u32 PC_REGISTER = 32;
constexpr u32 PSTATE_REGISTER = 33;
constexpr u32 Q0_REGISTER = 34;
constexpr u32 FPSR_REGISTER = 66;
constexpr u32 FPCR_REGISTER = 67;

} // namespace

GdbServer::GdbServer(Debugger& debugger_) : debugger{debugger_} {
    // Create the socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        LOG_ERROR(Debugger, "Failed to create GDB socket");
        return;
    }

    // Set the socket to reuse address
    i32 opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt,
               sizeof(opt));

    // Bind the socket to the address
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(GDB_PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) ==
        -1) {
        LOG_ERROR(Debugger, "Failed to bind GDB socket");
        close(server_socket);
        return;
    }

    // Start listening
    if (listen(server_socket, 1) == -1) {
        LOG_ERROR(Debugger, "Failed to listen on GDB socket");
        close(server_socket);
        return;
    }

    // Set the socket to non-blocking mode
    SetNonBlocking(server_socket);

    // Create server thread
    server_thread = std::thread(&GdbServer::ServerLoop, this);

    LOG_INFO(Debugger, "GDB server started on port 1234");

    // Thread ID
    current_thread_id = debugger.threads.begin()->first;
}

GdbServer::~GdbServer() {
    running = false;
    server_thread.join();
    close(server_socket);
}

void GdbServer::ServerLoop() {
    GET_CURRENT_PROCESS_DEBUGGER().RegisterThisThread("GDB server");
    while (running) {
        Poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    GET_CURRENT_PROCESS_DEBUGGER().UnregisterThisThread();
}

void GdbServer::Poll() {
    if (client_socket == -1) {
        sockaddr_in client_addr{};
        socklen_t addr_len = sizeof(client_addr);
        i32 new_client =
            accept(server_socket, (sockaddr*)&client_addr, &addr_len);

        if (new_client != -1) {
            client_socket = new_client;
            SetNonBlocking(client_socket);

            // Pause all threads
            // TODO: helper function?
            for (const auto& [_, thread] : debugger.threads)
                thread.guest_thread->Pause();
        }
    } else {
        char buffer[1024];
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read > 0) {
            receive_buffer += std::string_view(buffer, bytes_read);
            ProcessPackets();
        } else if (bytes_read == 0) {
            close(client_socket);
            client_socket = -1;
            // TODO: resume all threads?

            LOG_INFO(Debugger, "GDB client disconnected");
        }
    }
}

void GdbServer::ProcessPackets() {
    size_t str_start = 0;
    while (true) {
        const auto start = receive_buffer.find(GDB_START, str_start);
        if (start == std::string::npos)
            break;

        const auto end = receive_buffer.find(GDB_END, start);
        if (end == std::string::npos)
            break;

        std::string command = receive_buffer.substr(start + 1, end - start - 1);
        if (end + 2 < receive_buffer.size()) {
            SendStatus(GDB_ACK);
            HandleCommand(command);
        } else {
            SendStatus(GDB_NACK);
            break;
        }

        str_start = end + 3;
    }

    if (str_start > 0)
        receive_buffer = receive_buffer.substr(str_start);
}

void GdbServer::HandleCommand(std::string_view command) {
    // TODO: only log in debug
    LOG_INFO(Debugger, "COMMAND: {}", command);

    if (command.starts_with("vCont")) {
        HandleVCont(command.substr(5));
        return;
    }

    const auto body = command.substr(1);
    switch (command[0]) {
    case 'Q':
    case 'q':
        HandleQuery(body);
        break;
    case '?':
        HandleThreadStatus();
        break;
    default:
        LOG_WARN(Debugger, "Unknown command: {}", command);
        SendPacket(GDB_EMPTY);
        break;
    }
}

void GdbServer::HandleVCont(std::string_view command) {
    if (command == "?") {
        // TODO: what is this?
        SendPacket("vCont;c;C;s;S");
        return;
    }

    // TODO
    LOG_FATAL(Debugger, "Unhandled vCont command: {}", command);
}

void GdbServer::HandleQuery(std::string_view command) {
    if (command == "StartNoAckMode") {
        do_ack = false;
        SendPacket(GDB_OK);
    } else if (command.starts_with("Supported")) {
        // TODO: why packet size 4000?
        SendPacket("PacketSize=4000;qXfer:features:read+;qXfer:threads:read+;"
                   "qXfer:libraries:read+;"
                   "vContSupported+;QStartNoAckMode+");
    } else if (command.starts_with("fThreadInfo")) { // TODO: ==?
        std::vector<std::string> thread_ids;
        thread_ids.reserve(debugger.threads.size());
        for (const auto& [thread_id, thread] : debugger.threads)
            thread_ids.push_back(
                fmt::format("{:x}", std::bit_cast<u64>(thread_id)));
        SendPacket(fmt::format("m{}", fmt::join(thread_ids, ",")));
    } else {
        SendPacket(GDB_EMPTY);
    }
}

void GdbServer::HandleThreadStatus() {
    SendThreadStatus(current_thread_id, GDB_SIGTRAP);
}

void GdbServer::SendPacket(std::string_view data) {
    ASSERT_DEBUG(client_socket != -1, Debugger, "Client socket is not valid");

    u8 checksum = 0;
    for (char c : data)
        checksum += c;

    std::string packet = fmt::format("${}#{:02x}", data, checksum);
    send(client_socket, packet.data(), packet.size(), 0);
}

void GdbServer::SendStatus(char status) {
    if (!do_ack)
        return;

    ASSERT_DEBUG(client_socket != -1, Debugger, "Client socket is not valid");
    send(client_socket, &status, 1, 0);
}

void GdbServer::SetNonBlocking(i32 socket) {
    i32 flags = fcntl(socket, F_GETFL, 0);
    fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}

void GdbServer::SendThreadStatus(std::thread::id thread_id, u8 signal) {
    auto thread = debugger.threads.at(thread_id).guest_thread->GetThread();
    const auto& state = thread->GetState();
    SendPacket(fmt::format("T{:02x}{:02x}:{};{:02x}:{};{:02x}:{};thread:{:x};",
                           signal, PC_REGISTER, state.pc, SP_REGISTER, state.sp,
                           LR_REGISTER, state.lr,
                           std::bit_cast<u64>(thread_id)));
}

} // namespace hydra::debugger
