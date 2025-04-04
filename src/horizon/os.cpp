#include "horizon/os.hpp"

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
}

OS::~OS() { SINGLETON_UNSET_INSTANCE(); }

} // namespace Hydra::Horizon
