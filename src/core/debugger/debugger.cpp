#include "core/debugger/debugger.hpp"

#define GET_THIS_THREAD()                                                      \
    std::unique_lock lock(thread_mutex);                                       \
    const auto thread_id = std::this_thread::get_id();                         \
    auto it = threads.find(thread_id);                                         \
    ASSERT_DEBUG(it != threads.end(), Debugger, "Thread {} not registered",    \
                 std::bit_cast<u64>(thread_id));

namespace hydra::debugger {

Thread::Thread(const std::string_view name_) : name{name_} {
    // TODO: make this configurable
    messages.resize(256);
}

void Thread::Log(const Message& msg) {
    messages[msg_ptr++] = msg;
    if (msg_ptr >= messages.size()) {
        msg_ptr = 0;
        msg_queue_filled = true;
    }
}

void Debugger::RegisterThisThread(const std::string_view name) {
    std::unique_lock lock(thread_mutex);
    threads.insert({std::this_thread::get_id(), Thread(name)});
}

void Debugger::UnregisterThisThread() {
    GET_THIS_THREAD();
    threads.erase(it);
}

void Debugger::LogOnThisThread(const Message& msg) {
    GET_THIS_THREAD();
    it->second.Log(msg);
}

} // namespace hydra::debugger
