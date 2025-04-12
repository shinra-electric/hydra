#include "core/horizon/filesystem/filesystem.hpp"

#include "core/horizon/filesystem/directory.hpp"
#include "core/horizon/filesystem/file.hpp"

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
    auto entry = root.GetEntry(path.substr(1));
    ASSERT(entry, HorizonFilesystem, "Invalid path \"{}\"", path);

    return entry;
}

File* Filesystem::GetFile(const std::string& path) {
    auto file = dynamic_cast<File*>(GetEntry(path));
    ASSERT(file, HorizonFilesystem, "\"{}\" is not a file", path);

    return file;
}

Directory* Filesystem::GetDirectory(const std::string& path) {
    auto directory = dynamic_cast<Directory*>(GetEntry(path));
    ASSERT(directory, HorizonFilesystem, "\"{}\" is not a directory", path);

    return directory;
}

} // namespace Hydra::Horizon::Filesystem
