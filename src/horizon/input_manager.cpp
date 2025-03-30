#include "horizon/input_manager.hpp"

#include "horizon/hid.hpp"
#include "horizon/kernel.hpp"

namespace Hydra::Horizon {

InputManager::InputManager() {
    shared_memory_id = Kernel::GetInstance().CreateSharedMemory(0x40000);
}

HID::SharedMemory* InputManager::GetHidSharedMemory() const {
    // vaddr addr =
    // Kernel::GetInstance().GetSharedMemory(shared_memory_id).GetRange().base;
    // return reinterpret_cast<HID::SharedMemory*>(mmu->UnmapAddr(addr));
    //  TODO: implement
    return nullptr;
}

} // namespace Hydra::Horizon
