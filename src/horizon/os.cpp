#include "horizon/os.hpp"

#include "horizon/services/sm/user_interface.hpp"

namespace Hydra::Horizon {

SINGLETON_DEFINE_GET_INSTANCE(OS, Horizon, "Horizon OS")

OS::OS(HW::Bus& bus, HW::TegraX1::CPU::MMUBase* mmu) : kernel(bus, mmu) {
    SINGLETON_SET_INSTANCE(Horizon, "Horizon OS");

    // Services
    sm_user_interface = new Services::Sm::IUserInterface();

    kernel.ConnectServiceToPort("sm:", sm_user_interface);
}

OS::~OS() { SINGLETON_UNSET_INSTANCE(); }

void OS::LoadROM(Rom* rom) { kernel.LoadROM(rom); }

} // namespace Hydra::Horizon
