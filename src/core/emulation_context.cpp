#include "core/emulation_context.hpp"

#include "common/functions.hpp"
#include "hatch/hatch.hpp"

#include "core/horizon/loader/nca_loader.hpp"
#include "core/horizon/loader/nro_loader.hpp"
#include "core/horizon/loader/nso_loader.hpp"
#include "core/horizon/state_manager.hpp"
#include "core/hw/tegra_x1/cpu/dynarmic/cpu.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/cpu.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"
#include "core/hw/tegra_x1/cpu/thread_base.hpp"

namespace hydra {

EmulationContext::EmulationContext() {
    // Initialize
    switch (config.GetCpuBackend()) {
    case CpuBackend::AppleHypervisor:
        cpu = new hw::tegra_x1::cpu::hypervisor::CPU();
        break;
    case CpuBackend::Dynarmic:
        cpu = new hw::tegra_x1::cpu::dynarmic::CPU();
        break;
    default:
        LOG_FATAL(Other, "Unknown CPU backend");
        break;
    }

    gpu = new hw::tegra_x1::gpu::GPU(cpu->GetMMU());

    builtin_display = new hw::display::Display();

    bus = new hw::Bus();
    bus->ConnectDisplay(builtin_display, 0);

    os = new horizon::OS(*bus, cpu->GetMMU());

    // Filesystem
    /*
    for (const auto& root_path : CONFIG_INSTANCE.GetRootPaths()) {
        const auto res =
            horizon::Filesystem::Filesystem::GetInstance().AddEntry(
                root_path.guest_path, root_path.host_path, true);
        ASSERT(res == horizon::Filesystem::FsResult::Success, Services,
               "Failed to get root path: {}", res);
        LOG_INFO(Other, "Mapped {} to {}", root_path.guest_path,
                 root_path.host_path);
    }
    */
}

EmulationContext::~EmulationContext() {
    // TODO: delete objects
}

void EmulationContext::LoadRom(const std::string& rom_filename) {
    // Load ROM
    usize size;
    auto ifs = open_file(rom_filename, size);

    std::string extension =
        rom_filename.substr(rom_filename.find_last_of(".") + 1);
    horizon::loader::LoaderBase* loader{nullptr};
    if (extension == "nro")
        loader = new horizon::loader::NROLoader();
    else if (extension == "nso")
        loader = new horizon::loader::NSOLoader(true);
    else if (extension == "nca")
        loader = new horizon::loader::NCALoader();
    else
        LOG_FATAL(Other, "Unknown ROM extension \"{}\"", extension);

    StreamReader reader(ifs, 0, size);
    process = loader->LoadRom(reader, rom_filename);
    delete loader;

    ifs.close();

    LOG_INFO(Other, "-------- Title info --------");
    LOG_INFO(Other, "Title ID: {:016x}", os->GetKernel().GetTitleID());

    // Patch
    const auto target_patch_filename =
        fmt::format("{:016x}.hatch", os->GetKernel().GetTitleID());
    for (const auto& patch_directory :
         CONFIG_INSTANCE.GetPatchDirectories()) {
        for (const auto& dir_entry :
             std::filesystem::directory_iterator{patch_directory}) {
            if (to_lower(dir_entry.path().filename().string()) ==
                target_patch_filename) {
                LOG_INFO(Other, "Applying patch \"{}\"",
                         dir_entry.path().string());

                std::ifstream ifs(dir_entry);

                // Deserialize
                Hatch::Deserializer deserializer;
                deserializer.Deserialize(ifs);

                const auto& hatch = deserializer.GetHatch();

                // Memory patch
                for (const auto& entry : hatch.GetMemoryPatch())
                    cpu->GetMMU()->Store<u32>(entry.addr, entry.value);

                ifs.close();
            }
        }
    }
}

void EmulationContext::Run() {
    LOG_INFO(Other, "-------- Run --------");
    config.Log();

    // Enter focus
    auto& state_manager = horizon::StateManager::GetInstance();
    // HACK: games expect focus change to be the second message?
    state_manager.SendMessage(horizon::AppletMessage::Resume);
    state_manager.SetFocusState(horizon::AppletFocusState::InFocus);

    // Preselected user
    auto user_id = config.GetUserID();
    if (user_id == horizon::INVALID_USER_ID) {
        // If there is just a single user, use that
        if (USER_MANAGER_INSTANCE.GetCount() == 1)
            user_id = *USER_MANAGER_INSTANCE.Begin();
    }

    if (user_id != horizon::INVALID_USER_ID)
        state_manager.PushPreselectedUser(user_id);

    process->Run();
    is_running = true;
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

} // namespace hydra
