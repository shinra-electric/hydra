#pragma once

#define DEBUGGER_INSTANCE debugger::Debugger::GetInstance()

namespace hydra::debugger {

class Thread {
    friend class Debugger;

  public:
    Thread(const std::string_view name_);

    void Log(const LogMessage& msg);

    // API
    void Lock() { msg_mutex.lock(); }
    void Unlock() { msg_mutex.unlock(); }

    const std::string& GetName() const { return name; }
    usize GetMessageCount() const { return msg_count; }
    const LogMessage& GetMessage(const u32 index) const {
        return messages[(msg_tail + index) % messages.size()];
    }

  private:
    std::string name;

    std::mutex msg_mutex;
    std::vector<LogMessage> messages;
    u32 msg_tail{0};
    usize msg_count{0};
};

class Debugger {
  public:
    static Debugger& GetInstance() {
        static Debugger s_instance;
        return s_instance;
    }

    Debugger();
    ~Debugger();

    // API
    void Enable();
    void Disable();

    void Lock() { thread_mutex.lock(); }
    void Unlock() { thread_mutex.unlock(); }

    // No need to lock the mutex for these
    void RegisterThisThread(const std::string_view name);
    void UnregisterThisThread();

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

    void LogOnThisThread(const LogMessage& msg);
};

} // namespace hydra::debugger
