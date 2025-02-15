#include "horizon/os.hpp"

#include "horizon/services/sm/user_interface.hpp"

namespace Hydra::Horizon {

static OS* s_instance = nullptr;

OS& OS::GetInstance() { return *s_instance; }

OS::OS(HW::Bus& bus) : kernel(bus) {
    ASSERT(s_instance == nullptr, Horizon, "Horizon OS already exists");
    s_instance = this;

    // Services
    sm_user_interface = new Services::Sm::IUserInterface();

    kernel.ConnectServiceToPort("sm:", sm_user_interface);
}

OS::~OS() {}

void OS::SetMMU(HW::MMU::MMUBase* mmu) { kernel.SetMMU(mmu); }

} // namespace Hydra::Horizon
