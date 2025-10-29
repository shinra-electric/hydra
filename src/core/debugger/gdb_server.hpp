#pragma once

namespace hydra::debugger {

class Debugger;

class GdbServer {
  public:
    GdbServer(Debugger& debugger_);
    ~GdbServer();

  private:
    Debugger& debugger;

    i32 server_socket;
    i32 client_socket{-1};
    std::thread server_thread;
    std::atomic<bool> running{true};
    std::string receive_buffer;
    bool do_ack{true};

    std::thread::id current_thread_id;

    void ServerLoop();
    void Poll();

    void SendPacket(std::string_view data);
    void SendStatus(char status);

    void ProcessPackets();
    void HandleCommand(std::string_view command);

    // Commands
    void HandleVCont(std::string_view command);
    void HandleQuery(std::string_view command);
    void HandleThreadStatus();

    // Helpers
    void SetNonBlocking(i32 socket);
    void SendThreadStatus(std::thread::id thread_id, u8 signal);
    std::string PageFromBuffer(std::string_view buffer, std::string_view page);
};

} // namespace hydra::debugger
