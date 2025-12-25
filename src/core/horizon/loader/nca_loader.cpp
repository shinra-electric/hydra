#include "core/horizon/loader/nca_loader.hpp"

#include "core/horizon/filesystem/filesystem.hpp"
#include "core/horizon/kernel/kernel.hpp"
#include "core/horizon/loader/npdm.hpp"
#include "core/horizon/loader/nso_loader.hpp"

#define NINTENDO_LOGO_PATH "logo/NintendoLogo.png"
#define STARTUP_MOVIE_PATH "logo/StartupMovie.gif"

namespace hydra::horizon::loader {

NcaLoader::NcaLoader(const filesystem::ContentArchive& content_archive_)
    : content_archive{content_archive_} {
    filesystem::FileBase* file;
    auto res = content_archive.GetFile("code/main.npdm", file);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Loader, "Failed to load main.npdm: {}", res);
        return;
    }

    auto stream = file->Open(filesystem::FileOpenFlags::Read);
    auto reader = stream.CreateReader();

    const auto meta = reader.Read<NpdmMeta>();

    file->Close(stream);

    ASSERT(meta.magic == make_magic4('M', 'E', 'T', 'A'), Loader,
           "Invalid NPDM meta magic 0x{:08x}", meta.magic);

    // TODO: support 32-bit games
    if (!any(meta.flags & NpdmFlags::Is64BitInstruction)) {
        LOG_WARN(Loader, "32-bit games not supported");
        return;
    }

    LOG_DEBUG(Loader, "Name: {}", meta.name);
    LOG_DEBUG(Loader, "Main thread priority: 0x{:02x}",
              meta.main_thread_priority);
    LOG_DEBUG(Loader, "Main thread core number: {}",
              meta.main_thread_core_number);
    LOG_DEBUG(Loader, "Main thread stack size: 0x{:08x}",
              meta.main_thread_stack_size);
    LOG_DEBUG(Loader, "System resource size: 0x{:08x}",
              meta.system_resource_size);

    name = meta.name;
    main_thread_priority = meta.main_thread_priority;
    main_thread_core_number = meta.main_thread_core_number;
    main_thread_stack_size = meta.main_thread_stack_size;
    system_resource_size = meta.system_resource_size;

    // Nintendo logo
    res = content_archive.GetFile(NINTENDO_LOGO_PATH, nintendo_logo_file);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Loader, "Failed to get " NINTENDO_LOGO_PATH ": {}", res);
        return;
    }

    // Startup movie
    res = content_archive.GetFile(STARTUP_MOVIE_PATH, startup_movie_file);
    if (res != filesystem::FsResult::Success) {
        LOG_ERROR(Loader, "Failed to get " STARTUP_MOVIE_PATH ": {}", res);
        return;
    }
}

void NcaLoader::LoadProcess(kernel::Process* process) {
    // Title ID
    process->SetTitleID(content_archive.GetTitleID());

    for (const auto& [name, entry] : content_archive.GetEntries()) {
        if (name == "code") {
            auto dir = dynamic_cast<filesystem::Directory*>(entry);
            ASSERT(dir, Loader, "Code is not a directory");
            LoadCode(process, dir);
        } else if (name == "data") {
            const auto res = KERNEL_INSTANCE.GetFilesystem().AddEntry(
                FS_SD_MOUNT "/rom/romFS", entry, true);
            ASSERT(res == filesystem::FsResult::Success, Loader,
                   "Failed to add romFS entry: {}", res);
        } else if (name == "logo") {
            // Do nothing
        } else {
            LOG_NOT_IMPLEMENTED(Loader, "{}", name);
        }
    }
}

void NcaLoader::LoadCode(kernel::Process* process, filesystem::Directory* dir) {
    // HACK: if rtld is not present, use main as the entry point
    std::string entry_point = "rtld";
    filesystem::EntryBase* e;
    if (dir->GetEntry("rtld", e) == filesystem::FsResult::DoesNotExist)
        entry_point = "main";

    for (const auto& [filename, entry] : dir->GetEntries()) {
        auto file = dynamic_cast<filesystem::FileBase*>(entry);
        ASSERT(file, Loader, "Code entry is not a file");
        if (filename == "main.npdm") {
            // Do nothing
        } else {
            LOG_DEBUG(Loader, "Loading {}", filename);
            NsoLoader loader(file, filename, filename == entry_point);
            loader.SetMainThreadParams(main_thread_priority,
                                       main_thread_core_number,
                                       main_thread_stack_size);
            loader.LoadProcess(process);
        }
    }

    process->SetSystemResourceSize(system_resource_size);

    // TODO: ACI and ACID
}

} // namespace hydra::horizon::loader
