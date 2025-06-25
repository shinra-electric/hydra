#include "core/horizon/applets/applet_base.hpp"

#include "core/debugger/debugger.hpp"

namespace hydra::horizon::applets {

namespace {

struct CommonArguments {
    u32 version; // Version of the struct
    u32 size;    // Size of the struct
    u32 library_applet_api_version;
    u32 theme_color;
    bool play_startup_sound;
    u64 system_tick;
};

} // namespace

void AppletBase::Start() {
    // TODO: not every applet uses common args (for instance
    // LibraryAppletMiiEdit)
    const auto common_args = PopInData<CommonArguments>();
    ASSERT(common_args.version == 1, Applets, "Unsupported version {}",
           common_args.version); // TODO: support version 0
    ASSERT(common_args.size == sizeof(CommonArguments), Applets,
           "Invalid struct size 0x{:x}", common_args.size);

    thread = new std::thread([&]() {
        DEBUGGER_INSTANCE.RegisterThisThread("Applet");
        result = Run();
        controller.GetStateChangedEvent().handle->Signal();
        DEBUGGER_INSTANCE.UnregisterThisThread();
    });
}

} // namespace hydra::horizon::applets
