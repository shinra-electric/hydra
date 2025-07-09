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

    // TODO: notify process
}

Thread::~Thread() {
    if (t) {
        t->join();
        delete t;
    }

    delete tls_mem;

    // TODO: notify process
}

void Thread::Run() {
    ASSERT(entry_point != 0x0, Kernel, "Invalid entry point");

    t = new std::thread([&]() {
        auto thread = CPU_INSTANCE.CreateThread(process->GetMmu(), tls_mem);

        DEBUGGER_INSTANCE.RegisterThisThread("Guest",
                                             thread); // TODO: handle ID?

        thread->Initialize(
            [this](hw::tegra_x1::cpu::IThread* thread, u64 id) {
                return KERNEL_INSTANCE.SupervisorCall(process, this, thread,
                                                      id);
            },
            tls_addr, stack_top_addr);

        thread->SetPC(entry_point);
        for (u32 i = 0; i < sizeof_array(args); i++)
            thread->SetRegX(i, args[i]);

        thread->Run();

        DEBUGGER_INSTANCE.UnregisterThisThread();

        delete thread;
    });
}

} // namespace hydra::horizon::kernel
