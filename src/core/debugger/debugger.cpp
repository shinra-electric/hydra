#include "core/debugger/debugger.hpp"

#include <future>

#include "core/debugger/gdb_server.hpp"
#include "core/horizon/kernel/guest_thread.hpp"
#include "core/hw/tegra_x1/cpu/thread.hpp"

#define GET_THIS_THREAD()                                                      \
    std::unique_lock lock(mutex);                                              \
    const auto thread_id = std::this_thread::get_id();                         \
    auto it = threads.find(thread_id);                                         \
    ASSERT_DEBUG(it != threads.end(), Debugger,                                \
                 "Thread {:016x} not registered",                              \
                 std::bit_cast<u64>(thread_id));                               \
    auto& thread = it->second;

namespace hydra::debugger {

namespace {

constexpr u16 GDB_PORT = 1234;

}

ResolvedStackFrame StackFrame::Resolve() const {
    switch (type) {
    case StackFrameType::Host:
        // TODO
        return {"libhydra.dylib", "", addr};
    case StackFrameType::Guest: {
        const auto& module = debugger->GetModuleTable().FindSymbol(addr);
        const auto& function = debugger->GetFunctionTable().FindSymbol(addr);
        return {module, function, addr};
    }
    }
}

Thread::Thread(const std::string_view name_,
               horizon::kernel::GuestThread* guest_thread_)
    : name{name_}, guest_thread{guest_thread_} {
    // TODO: make this configurable
    messages.resize(256);
}

void Thread::Log(const Message& msg) {
    std::unique_lock lock(msg_mutex);
    messages[(msg_tail + msg_count) % messages.size()] = msg;
    if (msg_count < messages.size())
        msg_count++;
    else
        msg_tail = (msg_tail + 1) % messages.size();
}

Debugger::~Debugger() {
    if (gdb_server)
        delete gdb_server;
}

void Debugger::RegisterThisThread(const std::string_view name,
                                  horizon::kernel::GuestThread* guest_thread) {
    std::unique_lock lock(mutex);
    threads.try_emplace(std::this_thread::get_id(), name, guest_thread);
}

void Debugger::UnregisterThisThread() {
    GET_THIS_THREAD();
    threads.erase(it);
}

void Debugger::BreakOnThisThread(const std::string_view reason) {
    {
        GET_THIS_THREAD();
        thread.status = ThreadStatus::Break;
        thread.break_reason = reason;
    }

    // TODO: abort after the debugger is closed?
    std::promise<void> promise;
    auto future = promise.get_future();
    future.wait();
}

void Debugger::ActivateGdbServer() { gdb_server = new GdbServer(*this); }

void Debugger::NotifySupervisorPaused(horizon::kernel::GuestThread* thread) {
    if (gdb_server)
        gdb_server->NotifySupervisorPaused(thread);
}

void Debugger::BreakpointHit(horizon::kernel::GuestThread* thread) {
    if (gdb_server)
        gdb_server->BreakpointHit(thread);
}

void Debugger::LogOnThisThread(const LogMessage& msg) {
    GET_THIS_THREAD();
    lock.unlock();
    auto stack_trace = GetStackTrace(thread);
    lock.lock();
    thread.Log({msg, stack_trace});
}

StackTrace Debugger::GetStackTrace(Thread& thread) {
    StackTrace stack_trace;

    // Host
    // TODO

    // Guest
    if (auto guest_thread = thread.guest_thread) {
        guest_thread->GetThread()->GetStackTrace([&](vaddr_t addr) {
            stack_trace.frames.emplace_back(this, StackFrameType::Guest, addr);
        });
    }

    return std::move(stack_trace);
}

} // namespace hydra::debugger
