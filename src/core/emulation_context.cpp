#include "core/emulation_context.hpp"

#include "hatch/hatch.hpp"

#include "core/audio/cubeb/core.hpp"
#include "core/audio/null/core.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/loader/nca_loader.hpp"
#include "core/horizon/loader/nro_loader.hpp"
#include "core/horizon/loader/nso_loader.hpp"
#include "core/horizon/state_manager.hpp"
#include "core/hw/tegra_x1/cpu/dynarmic/cpu.hpp"
#include "core/hw/tegra_x1/cpu/hypervisor/cpu.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"
#include "core/hw/tegra_x1/cpu/thread_base.hpp"
#include "core/input/device_manager.hpp"

namespace hydra {

EmulationContext::EmulationContext(horizon::ui::HandlerBase& ui_handler) {
    // Random
    srand(time(0));

    // Initialize
    switch (CONFIG_INSTANCE.GetCpuBackend()) {
    case CpuBackend::AppleHypervisor:
        cpu = new hw::tegra_x1::cpu::hypervisor::CPU();
        break;
    case CpuBackend::Dynarmic:
        cpu = new hw::tegra_x1::cpu::dynarmic::CPU();
        break;
    default:
        // TODO: return an error instead
        LOG_FATAL(Other, "Unknown CPU backend {}",
                  CONFIG_INSTANCE.GetCpuBackend());
        break;
    }

    gpu = new hw::tegra_x1::gpu::GPU(cpu->GetMMU());

    builtin_display = new hw::display::Display();

    bus = new hw::Bus();
    bus->ConnectDisplay(builtin_display, 0);

    switch (CONFIG_INSTANCE.GetAudioBackend()) {
    case AudioBackend::Null:
        audio_core = new audio::null::Core();
        break;
    case AudioBackend::Cubeb:
        audio_core = new audio::cubeb::Core();
        break;
    default:
        // TODO: return an error instead
        LOG_FATAL(Other, "Unknown audio backend {}",
                  CONFIG_INSTANCE.GetAudioBackend());
        break;
    }

    os = new horizon::OS(*bus, cpu->GetMMU(), *audio_core, ui_handler);

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

    // Check if the file exists
    if (!std::filesystem::exists(rom_filename)) {
        // TODO: return an error instead
        LOG_FATAL(Other, "Invalid ROM path {}", rom_filename);
        return;
    }

    // Load ROM
    auto file = new horizon::filesystem::HostFile(rom_filename, false);
    std::string extension =
        rom_filename.substr(rom_filename.find_last_of(".") + 1);

    horizon::loader::LoaderBase* loader{nullptr};
    if (extension == "nro") {
        loader = new horizon::loader::NroLoader(file);
    } else if (extension == "nso") {
        loader = new horizon::loader::NsoLoader(file);
    } else if (extension == "nca") {
        loader = new horizon::loader::NcaLoader(file);
    } else {
        // TODO: return an error instead
        LOG_FATAL(Other, "Unknown ROM extension \"{}\"", extension);
        return;
    }

    const auto process_params = loader->LoadProcess();
    delete loader;

    process = new horizon::kernel::Process(process_params.value());
    os->GetKernel().AddProcessHandle(process);

    LOG_INFO(Other, "-------- Title info --------");
    LOG_INFO(Other, "Title ID: {:016x}", os->GetKernel().GetTitleID());

    // Patch
    const auto target_patch_filename =
        fmt::format("{:016x}.hatch", os->GetKernel().GetTitleID());
    // TODO: iterate recursively
    for (const auto& patch_path : CONFIG_INSTANCE.GetPatchPaths().Get()) {
        if (!std::filesystem::is_directory(patch_path)) {
            // File
            TryApplyPatch(target_patch_filename, patch_path);
        } else {
            // Directory
            // TODO: iterate recursively
            for (const auto& dir_entry :
                 std::filesystem::directory_iterator{patch_path}) {
                TryApplyPatch(target_patch_filename, dir_entry.path().string());
            }
        }
    }
}

void EmulationContext::Run() {
    LOG_INFO(Other, "-------- Config --------");
    CONFIG_INSTANCE.Log();

    LOG_INFO(Other, "-------- Run --------");

    // Connect input devices
    INPUT_DEVICE_MANAGER_INSTANCE.ConnectDevices();

    // Enter focus
    auto& state_manager = horizon::StateManager::GetInstance();
    // HACK: games expect focus change to be the second message?
    state_manager.SendMessage(horizon::AppletMessage::Resume);
    state_manager.SetFocusState(horizon::AppletFocusState::InFocus);

    // Preselected user
    auto user_id = CONFIG_INSTANCE.GetUserID().Get();
    if (user_id == horizon::services::account::INVALID_USER_ID) {
        // If there is just a single user, use that
        if (USER_MANAGER_INSTANCE.GetCount() == 1) {
            user_id = *USER_MANAGER_INSTANCE.Begin();
        } else {
            // TODO: launch a select user applet in case the game requires it
            LOG_FATAL(Other, "Multiple user accounts");
        }
    }

    if (user_id != horizon::services::account::INVALID_USER_ID) {
        state_manager.PushPreselectedUser(user_id);
        LOG_INFO(Other, "Preselected user with ID {:032x}", user_id);
    }

    process->Run();
    is_running = true;
}

void EmulationContext::ProgressFrame(u32 width, u32 height,
                                     bool& out_dt_average_updated) {
    // Input
    INPUT_DEVICE_MANAGER_INSTANCE.Poll();

    // Present
    std::vector<u64> dt_ns_list;
    Present(width, height, dt_ns_list);

    // Delta time
    using namespace std::chrono_literals;

    for (const auto dt_ns : dt_ns_list) {
        accumulated_dt_ns += dt_ns;
        dt_sample_count++;
    }

    const auto now = clock_t::now();
    const auto time_since_last_dt_averaging = now - last_dt_averaging_time;
    if (time_since_last_dt_averaging > 1s) {
        if (dt_sample_count != 0)
            last_dt_average =
                (f32)accumulated_dt_ns / (f32)dt_sample_count / 1'000'000'000.f;
        else
            last_dt_average = 0.f;
        accumulated_dt_ns = 0;
        dt_sample_count = 0;
        last_dt_averaging_time = now;

        out_dt_average_updated = true;
    } else {
        out_dt_average_updated = false;
    }
}

void EmulationContext::Present(u32 width, u32 height,
                               std::vector<u64>& out_dt_ns_list) {
    // TODO: don't hardcode the display id
    auto display = bus->GetDisplay(0);
    if (!display->IsOpen())
        return;

    // Signal V-Sync
    display->GetVSyncEvent().handle->Signal();

    // Layer
    auto layer = display->GetPresentableLayer();
    if (!layer)
        return;

    // Get the buffer to present
    u32 binder_id = layer->GetBinderId();
    auto& binder = os->GetDisplayDriver().GetBinder(binder_id);

    horizon::BqBufferInput input;
    i32 slot = binder.ConsumeBuffer(input, out_dt_ns_list);
    if (slot == -1)
        return;
    const auto& buffer = binder.GetBuffer(slot);

    // Src rect
    IntRect2D src_rect;
    src_rect.origin.x() = input.rect.left;
    src_rect.origin.y() =
        input.rect.top; // Convert from top left to bottom left origin
    src_rect.size.x() = input.rect.right - input.rect.left;
    src_rect.size.y() = input.rect.bottom - input.rect.top;

    // HACK
    if (src_rect.size.x() == 0) {
        src_rect.size.x() = buffer.nv_buffer.planes[0].width;
        ONCE(LOG_WARN(Other, "Invalid src width"));
    }
    if (src_rect.size.y() == 0) {
        src_rect.size.y() = buffer.nv_buffer.planes[0].height;
        ONCE(LOG_WARN(Other, "Invalid src height"));
    }

    if (any(input.transform_flags & horizon::TransformFlags::FlipH)) {
        src_rect.origin.x() += src_rect.size.x();
        src_rect.size.x() = -src_rect.size.x();
    }
    if (any(input.transform_flags & horizon::TransformFlags::FlipV)) {
        src_rect.origin.y() += src_rect.size.y();
        src_rect.size.y() = -src_rect.size.y();
    }
    if (any(input.transform_flags & horizon::TransformFlags::Rot90)) {
        // TODO: how does this work? Is the aspect ratio kept intact?
        ONCE(LOG_NOT_IMPLEMENTED(Other, "Rotating by 90 degrees"));
    }

    // Dst rect
    const auto src_width = abs(src_rect.size.x());
    const auto src_height = abs(src_rect.size.y());

    IntRect2D dst_rect;
    auto scale_x = (f32)width / (f32)src_width;
    auto scale_y = (f32)height / (f32)src_height;
    if (scale_x > scale_y) {
        const auto dst_width = static_cast<i32>(src_width * scale_y);
        dst_rect.origin = int2({static_cast<i32>(width - dst_width) / 2, 0});
        dst_rect.size = int2({dst_width, static_cast<i32>(height)});
    } else {
        const auto dst_height = static_cast<i32>(src_height * scale_x);
        dst_rect.origin = int2({0, static_cast<i32>(height - dst_height) / 2});
        dst_rect.size = int2({static_cast<i32>(width), dst_height});
    }

    // Present
    auto renderer = gpu->GetRenderer();
    renderer->LockMutex();
    auto texture = gpu->GetTexture(buffer.nv_buffer);
    renderer->Present(texture, src_rect, dst_rect);
    renderer->EndCommandBuffer();
    renderer->UnlockMutex();
}

void EmulationContext::TryApplyPatch(const std::string_view target_filename,
                                     const std::filesystem::path path) {
    if (to_lower(path.filename().string()) != target_filename)
        return;

    LOG_INFO(Other, "Applying patch \"{}\"", path.string());

    std::ifstream ifs(path);

    // Deserialize
    Hatch::Deserializer deserializer;
    deserializer.Deserialize(ifs);

    const auto& hatch = deserializer.GetHatch();

    // Memory patch
    for (const auto& entry : hatch.GetMemoryPatch())
        cpu->GetMMU()->Store<u32>(entry.addr, entry.value);

    ifs.close();
}

} // namespace hydra
