#pragma once

#include "core/horizon/services/hid/internal/npad.hpp"

namespace hydra::horizon::kernel {
class SharedMemory;
}

namespace hydra::horizon::services::hid::internal {

class AppletResource {
  public:
    AppletResource();
    ~AppletResource();

    void SetupNpads();

    // Update
    void UpdateNpad(NpadIdType type, const input::NpadState& new_state) {
        npads[static_cast<usize>(type)].Update(new_state);
    }
    // TODO: touch

    kernel::Event* GetNpadStyleSetUpdateEvent(NpadIdType type) {
        return npads[static_cast<usize>(type)].GetStyleSetUpdateEvent();
    }

  private:
    kernel::SharedMemory* shared_mem;

    bool input_enabled{true};
    NpadStyleSet supported_style_sets{
        NpadStyleSet::Standard}; // TODO: what should this be?
    NpadJoyHoldType joy_hold_type{NpadJoyHoldType::Vertical};

    std::array<Npad, NPAD_COUNT> npads;

  public:
    GETTER(shared_mem, GetSharedMemory);
    SETTER(input_enabled, EnableInput);
    GETTER_AND_SETTER(supported_style_sets, GetSupportedStyleSet,
                      SetSupportedStyleSet);
    GETTER_AND_SETTER(joy_hold_type, GetJoyHoldType, SetJoyHoldType);
};

} // namespace hydra::horizon::services::hid::internal
