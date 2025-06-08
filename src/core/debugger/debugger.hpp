#pragma once

#define DEBUGGER_INSTANCE debugger::Debugger::GetInstance()

namespace hydra::debugger {

class Thread {
  public:
    Thread(const std::string_view name_);

    void Log(const LogMessage& msg);

    // API
    usize GetMessageCount() const {
        return (msg_queue_filled ? messages.size() : msg_ptr);
    }
    const LogMessage& GetMessage(const u32 index) const {
        return messages[(msg_ptr + index) % messages.size()];
    }

  private:
    std::string name;

    std::vector<LogMessage> messages;
    u32 msg_ptr{0};
    bool msg_queue_filled{false};
};

class Debugger {
  public:
    static Debugger& GetInstance() {
        static Debugger s_instance;
        return s_instance;
    }

    void RegisterThisThread(const std::string_view name);
    void UnregisterThisThread();
    void LogOnThisThread(const LogMessage& msg);

    // API
    void InstallCallback();
    void Lock() { thread_mutex.lock(); }
    void Unlock() { thread_mutex.unlock(); }
    usize GetThreadCount() const { return threads.size(); }
    Thread& GetThread(const u32 index) {
        // TODO: not the best way to index into a map
        auto it = threads.begin();
        std::advance(it, index);
        return it->second;
    }

  private:
    std::mutex thread_mutex;
    std::map<std::thread::id, Thread> threads;
};

} // namespace hydra::debugger
