#include "horizon/filesystem/filesystem.hpp"

#include "horizon/filesystem/directory.hpp"
#include "horizon/filesystem/file.hpp"

#define VERIFY_PATH(path)                                                      \
    ASSERT(path.size() != 0, HorizonFilesystem, "Path cannot be empty");       \
    ASSERT(path[0] == '/', HorizonFilesystem,                                  \
           "Path must start with a \"/\" (path: \"{}\")", path);

namespace Hydra::Horizon::Filesystem {

SINGLETON_DEFINE_GET_INSTANCE(Filesystem, HorizonFilesystem, "Filesystem")

Filesystem::Filesystem() {
    SINGLETON_SET_INSTANCE(HorizonFilesystem, "Filesystem");
}

Filesystem::~Filesystem() { SINGLETON_UNSET_INSTANCE(); }

void Filesystem::AddEntry(EntryBase* entry, const std::string& path) {
    VERIFY_PATH(path);
    root.AddEntry(entry, path.substr(1));
}

EntryBase* Filesystem::GetEntry(const std::string& path) {
    VERIFY_PATH(path);
    return root.GetEntry(path.substr(1));
}

File* Filesystem::GetFile(const std::string& path) {
    auto file = dynamic_cast<File*>(GetEntry(path));
    if (!file) {
        LOG_ERROR(HorizonFilesystem, "\"{}\" is not a file", path);
        return nullptr;
    }

    return file;
}

Directory* Filesystem::GetDirectory(const std::string& path) {
    auto directory = dynamic_cast<Directory*>(GetEntry(path));
    if (!directory) {
        LOG_ERROR(HorizonFilesystem, "\"{}\" is not a directory", path);
        return nullptr;
    }

    return directory;
}

} // namespace Hydra::Horizon::Filesystem
