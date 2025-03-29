#include "horizon/os.hpp"

#include "horizon/hid.hpp"
#include "horizon/services/sm/user_interface.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon {

SINGLETON_DEFINE_GET_INSTANCE(OS, Horizon, "Horizon OS")

OS::OS(HW::Bus& bus, HW::TegraX1::CPU::MMUBase* mmu_)
    : mmu{mmu_}, kernel(bus, mmu_) {
    SINGLETON_SET_INSTANCE(Horizon, "Horizon OS");

    // Services
    sm_user_interface = new Services::Sm::IUserInterface();

    kernel.ConnectServiceToPort("sm:", sm_user_interface);

    // Shared memories
    hid_shared_memory_id = kernel.CreateSharedMemory(0x40000);
}

OS::~OS() { SINGLETON_UNSET_INSTANCE(); }

HID::SharedMemory* OS::GetHidSharedMemory() const {
    vaddr addr = kernel.GetSharedMemory(hid_shared_memory_id).GetRange().base;
    return reinterpret_cast<HID::SharedMemory*>(mmu->UnmapAddr(addr));
}

} // namespace Hydra::Horizon
