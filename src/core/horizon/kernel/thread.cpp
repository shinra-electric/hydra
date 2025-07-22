#include "core/horizon/kernel/thread.hpp"

#include "core/debugger/debugger.hpp"
#include "core/horizon/kernel/process.hpp"
#include "core/hw/tegra_x1/cpu/cpu.hpp"
#include "core/hw/tegra_x1/cpu/mmu.hpp"
#include "core/hw/tegra_x1/cpu/thread.hpp"

namespace hydra::horizon::kernel {

Thread::Thread(Process* process_, vaddr_t stack_top_addr_, i32 priority_,
               const std::string_view debug_name)
    : SynchronizationObject(false, debug_name), process{process_},
      stack_top_addr{stack_top_addr_}, priority{priority_} {
    tls_mem = process->CreateTlsMemory(tls_addr);
}

Thread::~Thread() {
    if (thread) {
        // Request stop
        state = ThreadState::Stopping;
        thread->join();
        delete thread;
    }
    delete tls_mem;
}

void Thread::Start() {
    ASSERT(entry_point != 0x0, Kernel, "Invalid entry point");

    thread = new std::thread([&]() {
        auto thread = CPU_INSTANCE.CreateThread(
            process->GetMmu(),
            [this](hw::tegra_x1::cpu::IThread* thread, u64 id) {
                KERNEL_INSTANCE.SupervisorCall(process, this, thread, id);
            },
            [this]() {
                ProcessMessages();
                return state == ThreadState::Stopping;
            },
            tls_mem, tls_addr, stack_top_addr);

        process->RegisterThread(this);
        DEBUGGER_INSTANCE.RegisterThisThread("Guest",
                                             thread); // TODO: handle ID?

        thread->SetPC(entry_point);
        for (u32 i = 0; i < sizeof_array(args); i++)
            thread->SetRegX(i, args[i]);

        thread->Run();

        process->UnregisterThread(this);
        DEBUGGER_INSTANCE.UnregisterThisThread();

        // Signal exit
        state = ThreadState::Stopped;
        Signal();

        delete thread;
    });
}

ThreadAction Thread::ProcessMessages(i64 pause_timeout_ns) {
    const auto timeout_time = std::chrono::steady_clock::now() +
                              std::chrono::nanoseconds(pause_timeout_ns);

    std::unique_lock<std::mutex> lock(msg_mutex);
    while (!msg_queue.empty()) {
        const auto action = ProcessMessagesImpl();
        if (state != ThreadState::Paused)
            return action;

        if (pause_timeout_ns == INFINITE_TIMEOUT) {
            msg_cv.wait(lock);
        } else {
            msg_cv.wait_until(lock, timeout_time);
            const auto crnt_time = std::chrono::steady_clock::now();
            if (crnt_time >= timeout_time)
                return {
                    .type = ThreadActionType::Resume,
                    .payload = {
                        .resume = {.reason = ThreadResumeReason::TimedOut}}};
        }
    }

    return {};
}

void Thread::SendMessage(ThreadMessage msg) {
    std::lock_guard<std::mutex> lock(msg_mutex);
    msg_queue.push(msg);
    msg_cv.notify_all(); // TODO: notify one?
}

ThreadAction Thread::ProcessMessagesImpl() {
    ThreadAction action{};
    while (!msg_queue.empty()) {
        auto msg = msg_queue.front();
        msg_queue.pop();

        // Process the message
        switch (msg.type) {
        case ThreadMessageType::Stop:
            state = ThreadState::Stopping;
            return {.type = ThreadActionType::Stop};
        case ThreadMessageType::Pause:
            state = ThreadState::Paused;
            action = {};
            break;
        case ThreadMessageType::Resume: {
            const auto signalled_obj = msg.payload.resume.signalled_obj;

            state = ThreadState::Running;
            action.type = ThreadActionType::Resume;
            action.payload.resume = {.reason =
                                         (signalled_obj != nullptr
                                              ? ThreadResumeReason::Signalled
                                              : ThreadResumeReason::Cancelled),
                                     .signalled_obj = signalled_obj};
            break;
        }
        }
    }

    return action;
}

} // namespace hydra::horizon::kernel
