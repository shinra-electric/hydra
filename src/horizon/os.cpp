#include "horizon/os.hpp"

#include "horizon/services/sm/user_interface.hpp"

namespace Hydra::Horizon {

static OS* s_instance = nullptr;

OS& OS::GetInstance() { return *s_instance; }

OS::OS(HW::Bus& bus, HW::TegraX1::CPU::MMUBase* mmu) : kernel(bus, mmu) {
    ASSERT(s_instance == nullptr, Horizon, "Horizon OS already exists");
    s_instance = this;

    // Services
    sm_user_interface = new Services::Sm::IUserInterface();

    kernel.ConnectServiceToPort("sm:", sm_user_interface);
}

OS::~OS() { s_instance = nullptr; }

void OS::LoadROM(Rom* rom, HW::TegraX1::CPU::ThreadBase* thread) {
    kernel.LoadROM(rom, thread);
}

} // namespace Hydra::Horizon
