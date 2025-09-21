#include "core/horizon/kernel/thread.hpp"

#include "core/debugger/debugger_manager.hpp"
#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::kernel {

IThread::~IThread() {
    if (thread) {
        // Request stop
        state = ThreadState::Stopping;
        thread->join();
        delete thread;
    }
}

void IThread::Start() {
    thread = new std::thread([&]() {
        tls_current_thread = this;

        // TODO: don't allow null processes
        if (process)
            process->RegisterThread(this);
        Run();
        if (process)
            process->UnregisterThread(this);

        // Signal exit
        state = ThreadState::Stopped;
        Signal();
    });
}

ThreadAction IThread::ProcessMessages(i64 pause_timeout_ns) {
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

void IThread::SendMessage(ThreadMessage msg) {
    std::lock_guard lock(msg_mutex);
    msg_queue.push(msg);
    msg_cv.notify_all(); // TODO: notify one?
}

ThreadAction IThread::ProcessMessagesImpl() {
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
            action.payload.resume = {.reason = ThreadResumeReason::Signalled,
                                     .signalled_obj = signalled_obj};
            break;
        }
        }
    }

    return action;
}

void IThread::AddMutexWaiter(IThread* thread) {
    std::lock_guard<std::mutex> lock(mutex_wait_mutex);
    mutex_wait_list.AddLast(thread);
}

IThread* IThread::RelinquishMutex(uptr mutex_addr, u32& out_waiter_count) {
    std::lock_guard<std::mutex> lock(mutex_wait_mutex);

    // Find a new owner
    IThread* new_owner = nullptr;
    out_waiter_count = 0;
    for (auto waiter_node = mutex_wait_list.GetHead();
         waiter_node != nullptr;) {
        auto waiter = waiter_node->Get();
        if (waiter->mutex_wait_addr != mutex_addr) {
            waiter_node = waiter_node->GetNext();
            continue;
        }

        waiter_node = mutex_wait_list.Remove(waiter_node);
        if (new_owner) {
            new_owner->AddMutexWaiter(waiter);
            out_waiter_count++;
        } else {
            new_owner = waiter;
            waiter->mutex_wait_addr = 0x0;
        }
    }

    return new_owner;
}

} // namespace hydra::horizon::kernel
