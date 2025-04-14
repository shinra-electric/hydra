#include "core/emulation_context.hpp"

#include "core/horizon/loader/nca_loader.hpp"
#include "core/horizon/loader/nro_loader.hpp"
#include "core/horizon/loader/nso_loader.hpp"
#include "core/horizon/state_manager.hpp"
#include "core/hw/tegra_x1/cpu/dynarmic/cpu.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/cpu.hpp"
#include "core/hw/tegra_x1/cpu/thread_base.hpp"

namespace Hydra {

EmulationContext::EmulationContext() {
    // Emulation
    switch (Config::GetInstance().GetCpuBackend()) {
    case CpuBackend::AppleHypervisor:
        cpu = new Hydra::HW::TegraX1::CPU::Hypervisor::CPU();
        break;
    case CpuBackend::Dynarmic:
        cpu = new Hydra::HW::TegraX1::CPU::Dynarmic::CPU();
        break;
    default:
        LOG_ERROR(Other, "Unknown CPU backend");
        break;
    }

    gpu = new Hydra::HW::TegraX1::GPU::GPU(cpu->GetMMU());

    builtin_display = new Hydra::HW::Display::Display();

    bus = new Hydra::HW::Bus();
    bus->ConnectDisplay(builtin_display, 0);

    os = new Hydra::Horizon::OS(*bus, cpu->GetMMU());

    // Filesystem
    for (const auto& root_dir : Config::GetInstance().GetRootDirectories()) {
        const auto entry_name =
            root_dir.path.substr(root_dir.path.find_last_of("/") + 1);
        const auto entry_path = fmt::format("/{}", entry_name);
        const auto res =
            Horizon::Filesystem::Filesystem::GetInstance().AddEntry(
                root_dir.path, entry_path);
        ASSERT(res == Horizon::Filesystem::FsResult::Success, HorizonServices,
               "Failed to get root path: {}", res);
        LOG_INFO(Other, "Mapped {} to {}", entry_path, root_dir.path);
    }
}

EmulationContext::~EmulationContext() {
    for (auto t : threads) {
        // Force the thead to exit
        delete t;
    }
}

void EmulationContext::LoadRom(const std::string& rom_filename) {
    // Load ROM
    usize size;
    auto ifs = Hydra::open_file(rom_filename, size);

    std::string extension =
        rom_filename.substr(rom_filename.find_last_of(".") + 1);
    Horizon::Loader::LoaderBase* loader{nullptr};
    if (extension == "nro")
        loader = new Horizon::Loader::NROLoader();
    else if (extension == "nso")
        loader = new Horizon::Loader::NSOLoader(true);
    else if (extension == "nca")
        loader = new Horizon::Loader::NCALoader();
    else
        LOG_ERROR(Other, "Unknown ROM extension \"{}\"", extension);

    FileReader reader(ifs, 0, size);
    loader->LoadROM(reader, rom_filename);
    delete loader;

    ifs.close();

    // HACK
#define BRK 0xd4200000
#define MOV_X0_XZR 0xd2800000

    // cpu->GetMMU()->Store<u32>(0x800112e4, MOV_X0_XZR);
    // cpu->GetMMU()->Store<u32>(0x803cf6b8, BRK);
    // cpu->GetMMU()->Store<u32>(0x803cf0c8, BRK);
    // cpu->GetMMU()->Store<u32>(0x803d569c, BRK);
}

void EmulationContext::Run() {
    // Main thread
    std::thread* t = new std::thread([&]() {
        // Main thread
        Hydra::HW::TegraX1::CPU::ThreadBase* main_thread =
            cpu->CreateThread(os->GetKernel().GetTlsMemory());
        os->GetKernel().InitializeMainThread(main_thread);

        // Run
        main_thread->Run();

        // Cleanup
        delete main_thread;

        // Notify that emulation has ended
        is_running = false;
    });

    is_running = true;

    // Enter focus
    auto& state_manager = Horizon::StateManager::GetInstance();
    // HACK: games expect focus change to be the second message?
    state_manager.SendMessage(Horizon::AppletMessage::None);
    state_manager.SetFocusState(Horizon::AppletFocusState::InFocus);

    // Select user account
    // HACK
    state_manager.PushPreselectedUser(0);
}

void EmulationContext::Present() {
    // TODO: correct?
    // Inform the app that we want to display the window
    Horizon::StateManager::GetInstance().SendMessage(
        Horizon::AppletMessage::RequestToDisplay);

    // TODO: don't hardcode the display id
    auto display = bus->GetDisplay(0);
    auto layer = display->GetPresentableLayer();
    if (!layer)
        return;

    u32 binder_id = layer->GetBinderId();
    auto& binder = os->GetDisplayBinderManager().GetBinder(binder_id);
    i32 slot = binder.ConsumeBuffer();
    if (slot == -1)
        return;
    const auto& buffer = binder.GetBuffer(slot);

    auto texture = gpu->GetTexture(buffer);
    gpu->GetRenderer()->Present(texture);
}

} // namespace Hydra
