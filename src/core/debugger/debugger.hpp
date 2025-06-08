#pragma once

namespace hydra::debugger {

struct Message {
    LogLevel level;
    LogClass c;
    std::string file;
    u32 line;
    std::string function;
    std::string str;
};

class Thread {
  public:
    Thread(const std::string_view name_);

    void Log(const Message& msg);

  private:
    std::string name;

    std::vector<Message> messages;
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
    void LogOnThisThread(const Message& msg);

    // API
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
