#include "core/emulation_context.hpp"

#include "core/horizon/loader/nca_loader.hpp"
#include "core/horizon/loader/nro_loader.hpp"
#include "core/horizon/loader/nso_loader.hpp"
#include "core/horizon/state_manager.hpp"
#include "core/hw/tegra_x1/cpu/dynarmic/cpu.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/cpu.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"
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
        LOG_FATAL(Other, "Unknown CPU backend");
        break;
    }

    gpu = new Hydra::HW::TegraX1::GPU::GPU(cpu->GetMMU());

    builtin_display = new Hydra::HW::Display::Display();

    bus = new Hydra::HW::Bus();
    bus->ConnectDisplay(builtin_display, 0);

    os = new Hydra::Horizon::OS(*bus, cpu->GetMMU());

    // Filesystem
    /*
    for (const auto& root_path : Config::GetInstance().GetRootPaths()) {
        const auto res =
            Horizon::Filesystem::Filesystem::GetInstance().AddEntry(
                root_path.guest_path, root_path.host_path, true);
        ASSERT(res == Horizon::Filesystem::FsResult::Success, HorizonServices,
               "Failed to get root path: {}", res);
        LOG_INFO(Other, "Mapped {} to {}", root_path.guest_path,
                 root_path.host_path);
    }
    */
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
        LOG_FATAL(Other, "Unknown ROM extension \"{}\"", extension);

    StreamReader reader(ifs, 0, size);
    loader->LoadRom(reader, rom_filename);
    delete loader;

    ifs.close();

    // HACK
#define BRK 0xd4200000
#define MOV_X0_XZR 0xd2800000
#define NOP 0xd503201f

    // HACK
    if (false) {                                    // Cave story+
        cpu->GetMMU()->Store<u32>(0x4127f50c, NOP); // Jump to heap
        // cpu->GetMMU()->Store<u32>(0x4009cbec, NOP);

        // cpu->GetMMU()->Store<u32>(0x40082bbc, NOP); // CoreMask?
        // cpu->GetMMU()->Store<u32>(0x40082d8c, NOP); // svcQueryMemory

        // cpu->GetMMU()->Store<u32>(0x40081764, NOP); // Reading save
        // screen.cfg

        // cpu->GetMMU()->Store<u32>(0x40093478,
        //                           NOP); // HID (probably shared memory?)

        // cpu->GetMMU()->Store<u32>(0x4001f118, NOP);

        cpu->GetMMU()->Store<u32>(0x42e81a48,
                                  MOV_X0_XZR); // InstructionAbortSameEl
    }

    if (true) {                                     // Puyo Puyo Tetris
        cpu->GetMMU()->Store<u32>(0x513e05d0, NOP); // Jump to heap
        cpu->GetMMU()->Store<u32>(0x402cbecc, NOP); // Audio
        // cpu->GetMMU()->Store<u32>(0x51b9da48, NOP); // InstructionAbortSameEl

        // Error 0x2a2 after loading a file and
        // allocating + mapping GPU memory
        cpu->GetMMU()->Store<u32>(0x4029f964, NOP);
        cpu->GetMMU()->Store<u32>(0x4029ff3c, NOP);
        cpu->GetMMU()->Store<u32>(0x4029ff44, NOP);
        cpu->GetMMU()->Store<u32>(0x402a004c, NOP);
        cpu->GetMMU()->Store<u32>(0x402a005c, NOP);
    }

    if (false) {                                    // Sonic Mania
        cpu->GetMMU()->Store<u32>(0x4144170c, NOP); // Jump to heap
    }

    if (false) {                                    // Shovel Knight
        cpu->GetMMU()->Store<u32>(0x40d9c66c, NOP); // Jump to heap
        // cpu->GetMMU()->Store<u32>(0x404f5a10, NOP); // NVN assert
        // cpu->GetMMU()->Store<u32>(0x404c21d4, NOP); // Crash after NVN
        // assert
        // cpu->GetMMU()->Store<u32>(0x404f5a88, NOP); // NVN assert
        cpu->GetMMU()->Store<u32>(0x405142c8, NOP); // Audio
        // cpu->GetMMU()->Store<u32>(0x404c21d4, NOP); // Crash after NVN assert

        cpu->GetMMU()->Store<u32>(
            0x42285e2c,
            MOV_X0_XZR); // Set result code to
                         // NVN_WINDOW_ACQUIRE_TEXTURE_RESULT_SUCCESS
                         // in nvnQueueAcquireTexture

        // cpu->GetMMU()->Store<u32>(0x404c21d4, NOP); // Singleton not
        // initialized
    }
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
    state_manager.SendMessage(Horizon::AppletMessage::Resume);
    state_manager.SetFocusState(Horizon::AppletFocusState::InFocus);

    // Select user account
    // HACK
    state_manager.PushPreselectedUser(0x01234567);
}

void EmulationContext::Present() {
    // TODO: don't hardcode the display id
    auto display = bus->GetDisplay(0);
    if (!display->IsOpen())
        return;

    display->GetVSyncEvent().handle->Signal(); // Signal V-Sync

    auto layer = display->GetPresentableLayer();
    if (!layer)
        return;

    u32 binder_id = layer->GetBinderId();
    auto& binder = os->GetDisplayDriver().GetBinder(binder_id);
    i32 slot = binder.ConsumeBuffer();
    if (slot == -1)
        return;
    const auto& buffer = binder.GetBuffer(slot);

    auto renderer = gpu->GetRenderer();
    renderer->LockMutex();
    auto texture = gpu->GetTexture(buffer.nv_buffer);
    renderer->Present(texture);
    renderer->EndCommandBuffer();
    renderer->UnlockMutex();
}

} // namespace Hydra
