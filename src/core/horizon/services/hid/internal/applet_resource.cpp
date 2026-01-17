#include "core/horizon/services/hid/internal/applet_resource.hpp"

#include "core/horizon/kernel/shared_memory.hpp"

namespace hydra::horizon::services::hid::internal {

#define NPAD_INTERNAL_STATE(i)                                                 \
    reinterpret_cast<SharedMemory*>(shared_mem->GetPtr())                      \
        ->npad.entries[i]                                                      \
        .internal_state

AppletResource::AppletResource()
    : shared_mem{new kernel::SharedMemory(sizeof(SharedMemory),
                                          "HID shared memory")},
      npads{{NPAD_INTERNAL_STATE(0), NPAD_INTERNAL_STATE(1),
             NPAD_INTERNAL_STATE(2), NPAD_INTERNAL_STATE(3),
             NPAD_INTERNAL_STATE(4), NPAD_INTERNAL_STATE(5),
             NPAD_INTERNAL_STATE(6), NPAD_INTERNAL_STATE(7),
             NPAD_INTERNAL_STATE(8), NPAD_INTERNAL_STATE(9)}} {}

#undef NPAD_INTERNAL_STATE

AppletResource::~AppletResource() { delete shared_mem; }

void AppletResource::SetupNpads() {
    for (u32 i = 0; i < NPAD_COUNT; i++) {
        // TODO: get this from the config
        NpadStyleSet style_set;
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            style_set = NpadStyleSet::FullKey;
            break;
        case 8:
            style_set = NpadStyleSet::Handheld;
            break;
        case 9:
            style_set = NpadStyleSet::Palma;
            break;
        }

        // Disable handheld npads in docked mode
        if (style_set == NpadStyleSet::Handheld &&
            !CONFIG_INSTANCE.GetHandheldMode())
            style_set = NpadStyleSet::None;

        // TODO: auto-remap ProController to JoyDual and vice-versa

        // Check if style set is supported
        if (!any(supported_style_sets & style_set))
            style_set = NpadStyleSet::None;

        npads[i].Setup(style_set);
    }
}

} // namespace hydra::horizon::services::hid::internal
