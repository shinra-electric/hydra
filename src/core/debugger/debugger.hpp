#pragma once

#define DEBUGGER_INSTANCE debugger::Debugger::GetInstance()

namespace hydra::debugger {

class Thread {
  public:
    Thread(const std::string_view name_);

    void Log(const LogMessage& msg);

    // API
    usize GetMessageCount() const {
        if (!has_messages)
            return 0;

        return ((msg_head + messages.size()) - msg_tail) % messages.size();
    }
    const LogMessage& GetMessage(const u32 index) const {
        return messages[(msg_tail + index) % messages.size()];
    }

  private:
    std::string name;

    std::vector<LogMessage> messages;
    u32 msg_head{0};
    u32 msg_tail{0};
    bool has_messages{false};
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
