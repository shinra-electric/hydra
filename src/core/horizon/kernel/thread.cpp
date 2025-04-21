#include "core/horizon/kernel/thread.hpp"

#include "core/horizon/kernel/kernel.hpp"
#include "core/hw/tegra_x1/cpu/cpu_base.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"
#include "core/hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra::Horizon::Kernel {

Thread::~Thread() {
    if (t) {
        t->join();
        delete t;
    }

    HW::TegraX1::CPU::MMUBase::GetInstance().Unmap(tls_addr,
                                                   tls_mem->GetSize());
    HW::TegraX1::CPU::MMUBase::GetInstance().FreeMemory(tls_mem);
}

void Thread::Start() {
    t = new std::thread([&]() {
        HW::TegraX1::CPU::ThreadBase* thread =
            HW::TegraX1::CPU::CPUBase::GetInstance().CreateThread(tls_mem);
        Kernel::GetInstance().InitializeThread(thread, entry_point, tls_addr,
                                               stack_top_addr);
        thread->SetRegX(0, args_addr);

        thread->Run();

        delete thread;
    });
}

} // namespace Hydra::Horizon::Kernel
