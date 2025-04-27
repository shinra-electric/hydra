#include "core/horizon/loader/nro_loader.hpp"

#include "core/horizon/const.hpp"
#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/kernel/process.hpp"

namespace Hydra::Horizon::Loader {

static const std::string ROM_VIRTUAL_PATH =
    FS_SD_MOUNT "/rom.nro"; // TODO: what should this be?

namespace {

enum class NROSectionType {
    Text,
    Ro,
    Data,
};

struct NROSection {
    u32 offset;
    u32 size;
};

struct NROHeader {
    u8 rocrt[16];
    char magic[4];
    u32 version;
    u32 size;
    u32 flags;
    NROSection sections[3];
    u32 bss_size;
    u8 reserved1[4];
    u8 module_id[32];
    u32 dso_handle_offset;
    u8 reserved2[4];
    u32 embedded_offset;
    u32 embedded_size;
    u32 dyn_str_offset;
    u32 dyn_str_size;
    u32 dyn_sym_offset;
    u32 dyn_sym_size;

    const NROSection& GetSection(NROSectionType type) const {
        return sections[static_cast<u32>(type)];
    }
};

} // namespace

Kernel::Process* NROLoader::LoadRom(StreamReader& reader,
                                    const std::string& rom_filename) {
    // Header
    const auto header = reader.Read<NROHeader>();

    // Validate
    ASSERT(std::memcmp(header.magic, "NRO0", 4) == 0, HorizonLoader,
           "Invalid NRO magic \"{}\"", header.magic);

    // Create executable memory
    usize executable_size = reader.GetSize() + header.bss_size;
    uptr base;
    auto ptr = Kernel::Kernel::GetInstance().CreateExecutableMemory(
        executable_size, Kernel::MemoryPermission::ReadWriteExecute, true,
        base); // TODO: is the permission correct?
    reader.Seek(0);
    reader.Read(reinterpret_cast<u8*>(ptr), reader.GetSize());

    // Args
    const u64 argv_offset = executable_size;

    std::string args = fmt::format("\"{}\"", ROM_VIRTUAL_PATH);
    for (const auto& arg : Config::GetInstance().GetProcessArgs())
        args += fmt::format(" \"{}\"", arg);

    char* argv = reinterpret_cast<char*>(ptr + argv_offset);
    memcpy(argv, args.c_str(), args.size());
    argv[args.size()] = '\0';

    // Config
    const uptr config_offset = argv_offset + args.size() + 1;

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
    ConfigEntry* entry = reinterpret_cast<ConfigEntry*>(ptr + config_offset);

    ADD_ENTRY_MANDATORY(MainThreadHandle, 0x0000000f,
                        0); // TODO: what thread handle should be used?
    ADD_ENTRY_MANDATORY(Argv, 0, base + argv_offset);
    // TODO: supply the actual availability
    ADD_ENTRY_MANDATORY(SyscallAvailableHint, UINT64_MAX, UINT64_MAX);
    ADD_ENTRY_MANDATORY(SyscallAvailableHint2, UINT64_MAX, 0);
    ADD_ENTRY_MANDATORY(EndOfList, 0, 0);

#undef ADD_ENTRY_NON_MANDATORY
#undef ADD_ENTRY_MANDATORY
#undef ADD_ENTRY

    // Filesystem
    const auto res = Filesystem::Filesystem::GetInstance().AddEntry(
        ROM_VIRTUAL_PATH,
        new Filesystem::HostFile(rom_filename, reader.GetOffset(),
                                 reader.GetSize()));
    ASSERT(res == Filesystem::FsResult::Success, HorizonLoader,
           "Failed to add romFS entry: {}", res);

    // Process
    Kernel::Process* process = new Kernel::Process();
    auto& main_thread = process->GetMainThread();
    main_thread.handle->SetEntryPoint(
        base + sizeof(NROHeader) +
        header.GetSection(NROSectionType::Text).offset);
    main_thread.handle->SetArg(0, base + config_offset);
    main_thread.handle->SetArg(1, UINT64_MAX);

    return process;
}

} // namespace Hydra::Horizon::Loader
