#include "horizon/loader/nro_loader.hpp"

#include "horizon/filesystem/file.hpp"
#include "horizon/filesystem/filesystem.hpp"
#include "horizon/kernel.hpp"
#include "hw/tegra_x1/cpu/memory.hpp"

namespace Hydra::Horizon::Loader {

static const std::string ROM_VIRTUAL_PATH =
    "/rom.nro"; // TODO: what should this be?

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

void NROLoader::LoadROM(const std::string& rom_filename) {
    // Parse file
    usize size;
    auto ifs = Hydra::open_file(rom_filename, size);

    NROHeader header;
    ifs.read(reinterpret_cast<char*>(&header), sizeof(header));

    // Validate
    if (strcmp(header.magic, "NRO0") != 0) {
        LOG_ERROR(HorizonLoader, "Invalid NRO magic \"{}\"", header.magic);
        return;
    }

    // Create executable memory
    uptr base;
    auto mem = Kernel::GetInstance().CreateExecutableMemory(
        size + header.bss_size, base);
    ifs.seekg(0, std::ios::beg);
    ifs.read(reinterpret_cast<char*>(mem->GetPtr()), size);

    // Set entrypoint
    Kernel::GetInstance().SetEntryPoint(
        base + sizeof(NROHeader) +
        header.GetSection(NROSectionType::Text).offset);

    // Filesystem
    Filesystem::Filesystem::GetInstance().AddEntry(
        new Filesystem::File(rom_filename), ROM_VIRTUAL_PATH);

    ifs.close();
}

} // namespace Hydra::Horizon::Loader
