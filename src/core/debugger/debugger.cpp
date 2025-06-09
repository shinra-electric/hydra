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

void Thread::Log(const LogMessage& msg) {
    messages[msg_head] = msg;
    msg_head = (msg_head + 1) % messages.size();
    has_messages = true;
}

void Debugger::RegisterThisThread(const std::string_view name) {
    std::unique_lock lock(thread_mutex);
    threads.insert({std::this_thread::get_id(), Thread(name)});
}

void Debugger::UnregisterThisThread() {
    GET_THIS_THREAD();
    threads.erase(it);
}

void Debugger::LogOnThisThread(const LogMessage& msg) {
    GET_THIS_THREAD();
    it->second.Log(msg);
}

void Debugger::InstallCallback() {
#ifdef HYDRA_DEBUG
    g_logger.InstallCallback(
        [this](const LogMessage& msg) { LogOnThisThread(msg); });
#else
    LOG_FATAL(Debugger, "Debugger not supported");
#endif
}

} // namespace hydra::debugger
