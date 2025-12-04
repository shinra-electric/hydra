#include "core/horizon/loader/homebrew_loader.hpp"

#include "core/debugger/debugger_manager.hpp"
#include "core/horizon/filesystem/file_view.hpp"
#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/kernel/kernel.hpp"

namespace hydra::horizon::loader {

namespace {

constexpr usize STACK_MEMORY_SIZE = 0x40000;

enum class ConfigEntryType : u32 {
    EndOfList = 0,
    MainThreadHandle = 1,
    NextLoadPath = 2,
    OverrideHeap = 3,
    OverrideService = 4,
    Argv = 5,
    SyscallAvailableHint = 6,
    AppletType = 7,
    AppletWorkaround = 8,
    Reserved9 = 9,
    ProcessHandle = 10,
    LastLoadResult = 11,
    RandomSeed = 14,
    UserIdStorage = 15,
    HosVersion = 16,
    SyscallAvailableHint2 = 17,
};

enum class ConfigEntryFlag : u32 {
    None = 0,
    IsMandatory = BIT(0),
};

struct ConfigEntry {
    ConfigEntryType type;
    ConfigEntryFlag flags;
    u64 values[2];
};

class HomebrewThread : public kernel::GuestThread {
  public:
    // TODO: don't hardcode priority
    HomebrewThread(kernel::Process* process, std::string_view path_)
        : kernel::GuestThread(
              process, kernel::STACK_REGION.begin + STACK_MEMORY_SIZE - 0x10,
              0x2c, "Homebrew thread"),
          path{path_} {}

  protected:
    void Run() override {
        // Next load
        // TODO: memory type
        // TODO: region
        vaddr_t next_load_base;
        auto next_load_ptr = process->CreateMemory(
            kernel::EXECUTABLE_REGION, 0x1000,
            static_cast<kernel::MemoryType>(4),
            kernel::MemoryPermission::ReadWrite, true, next_load_base);

        while (true) {
            // File
            filesystem::FileBase* file;
            const auto res =
                KERNEL_INSTANCE.GetFilesystem().GetFile(path, file);
            ASSERT(res == filesystem::FsResult::Success, Loader,
                   "Failed to get Homebrew file: {}", res);

            // NRO loader
            {
                NroLoader nro_loader(file, false);
                nro_loader.LoadProcess(process);
                const auto executable_ptr = nro_loader.GetExecutablePtr();

                // Args
                // TODO: create a separate memory for this?
                const u64 argv_offset = nro_loader.GetExecutableSize();

                std::string argv = fmt::format("\"{}\"", path);
                for (const auto& arg : CONFIG_INSTANCE.GetProcessArgs().Get())
                    argv += fmt::format(" \"{}\"", arg);

                char* argv_ptr =
                    reinterpret_cast<char*>(executable_ptr + argv_offset);
                memcpy(argv_ptr, argv.data(), argv.size());
                argv_ptr[argv.size()] = '\0';

                // Config
                const uptr config_offset = argv_offset + argv.size() + 1;

#define ADD_ENTRY(t, f, value0, value1)                                        \
    {                                                                          \
        entry->type = ConfigEntryType::t;                                      \
        entry->flags = ConfigEntryFlag::f;                                     \
        entry->values[0] = value0;                                             \
        entry->values[1] = value1;                                             \
        entry++;                                                               \
    }
#define ADD_ENTRY_MANDATORY(t, value0, value1)                                 \
    ADD_ENTRY(t, None, value0, value1)
#define ADD_ENTRY_NON_MANDATORY(t, value0, value1)                             \
    ADD_ENTRY(t, IsMandatory, value0, value1)

                // Entries
                ConfigEntry* entry = reinterpret_cast<ConfigEntry*>(
                    executable_ptr + config_offset);

                ADD_ENTRY_MANDATORY(MainThreadHandle, self_handle, 0);
                ADD_ENTRY_MANDATORY(NextLoadPath, next_load_base,
                                    next_load_base + 0x800);
                ADD_ENTRY_MANDATORY(
                    Argv, 0, nro_loader.GetExecutableBase() + argv_offset);
                // TODO: supply the actual availability
                ADD_ENTRY_MANDATORY(SyscallAvailableHint, UINT64_MAX,
                                    UINT64_MAX);
                ADD_ENTRY_MANDATORY(SyscallAvailableHint2, UINT64_MAX, 0);
                ADD_ENTRY_MANDATORY(EndOfList, 0, 0);

#undef ADD_ENTRY_NON_MANDATORY
#undef ADD_ENTRY_MANDATORY
#undef ADD_ENTRY

                // Params
                entry_point = nro_loader.GetEntryPoint();
                args[0] = nro_loader.GetExecutableBase() + config_offset;
                args[1] = std::numeric_limits<u64>::max();
            }

            // Run
            GET_CURRENT_PROCESS_DEBUGGER().GetThisThread().SetGuestThread(this);
            kernel::GuestThread::Run();
            GET_CURRENT_PROCESS_DEBUGGER().GetThisThread().SetGuestThread(
                nullptr);

            // Next load
            path = std::string(reinterpret_cast<const char*>(next_load_ptr));
            if (path.empty())
                break;
        }
    }

  private:
    std::string path;
    handle_id_t self_handle{INVALID_HANDLE_ID};

  public:
    SETTER(self_handle, SetSelfHandle);
};

} // namespace

HomebrewLoader::HomebrewLoader(filesystem::FileBase* file_)
    : file{file_}, nro_loader(file, false) {
    // Asset section
    const auto asset_begin = nro_loader.GetSize();
    TryLoadAssetSection(new filesystem::FileView(
        file, asset_begin, file->GetSize() - asset_begin));
}

void HomebrewLoader::LoadProcess(kernel::Process* process) {
    // Map file
    // TODO: use the actual app name?
    std::string mapped_path = FS_SD_MOUNT "/entry.nro";
    const auto res =
        KERNEL_INSTANCE.GetFilesystem().AddEntry(mapped_path, file);
    ASSERT(res == filesystem::FsResult::Success, Loader,
           "Failed to map Homebrew file: {}", res);

    // Stack memory
    process->CreateStackMemory(STACK_MEMORY_SIZE);

    // Main thread
    auto main_thread = new HomebrewThread(process, mapped_path);
    const auto main_thread_handle_id = process->SetMainThread(main_thread);
    main_thread->SetSelfHandle(main_thread_handle_id);
}

namespace {

struct AssetSection {
    u64 offset;
    u64 size;
};

struct AssetHeader {
    u32 magic;
    u32 format_version;
    AssetSection icon_section;
    AssetSection nacp_section;
    AssetSection romfs_section;
};

} // namespace

void HomebrewLoader::TryLoadAssetSection(filesystem::FileBase* file) {
    auto stream = file->Open(filesystem::FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    // Header
    const auto header = reader.Read<AssetHeader>();
    // TODO: is this the correct way to check if the asset section is present?
    if (header.magic != make_magic4('A', 'S', 'E', 'T'))
        return;

    LOG_DEBUG(Loader, "Asset section found");

    // Icon
    if (header.icon_section.size > 0)
        icon_file = new filesystem::FileView(file, header.icon_section.offset,
                                             header.icon_section.size);

    // NACP
    if (header.nacp_section.size > 0)
        nacp_file = new filesystem::FileView(file, header.nacp_section.offset,
                                             header.nacp_section.size);

    file->Close(stream);
}

} // namespace hydra::horizon::loader
