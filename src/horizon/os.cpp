#include "horizon/os.hpp"

#include "horizon/services/hosbinder/hos_binder_driver.hpp"

namespace Hydra::Horizon {

static OS* s_instance = nullptr;

OS& OS::GetInstance() { return *s_instance; }

OS::OS(HW::Bus& bus) : kernel(bus) {
    ASSERT(s_instance == nullptr, Horizon, "Horizon OS already exists");
    s_instance = this;

    // Services
    hos_binder_driver = new Services::HosBinder::IHOSBinderDriver();
}

OS::~OS() {}

void OS::SetMMU(HW::MMU::MMUBase* mmu) { kernel.SetMMU(mmu); }

} // namespace Hydra::Horizon
