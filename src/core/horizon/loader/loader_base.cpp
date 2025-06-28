#include "core/horizon/loader/loader_base.hpp"

#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/loader/nca_loader.hpp"
#include "core/horizon/loader/nro_loader.hpp"
#include "core/horizon/loader/nso_loader.hpp"
#include "core/horizon/loader/xci_loader.hpp"

namespace hydra::horizon::loader {

LoaderBase* LoaderBase::CreateFromFile(const std::string& path) {
    // Check if the file exists
    if (!std::filesystem::exists(path)) {
        // TODO: return an error instead
        LOG_FATAL(Other, "Invalid path \"{}\"", path);
        return nullptr;
    }

    // Create loader
    auto file = new horizon::filesystem::HostFile(path, false);
    const auto extension =
        std::string_view(path).substr(path.find_last_of(".") + 1);

    horizon::loader::LoaderBase* loader{nullptr};
    if (extension == "nro") {
        loader = new horizon::loader::NroLoader(file);
    } else if (extension == "nso") {
        loader = new horizon::loader::NsoLoader(file);
    } else if (extension == "nca") {
        loader = new horizon::loader::NcaLoader(file);
    } else if (extension == "xci") {
        loader = new horizon::loader::XciLoader(file);
    } else {
        // TODO: return an error instead
        LOG_FATAL(Other, "Unknown ROM extension \"{}\"", extension);
        return nullptr;
    }

    return loader;
}

} // namespace hydra::horizon::loader
