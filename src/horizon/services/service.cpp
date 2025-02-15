#include "horizon/services/service.hpp"

#include "horizon/cmif.hpp"
#include "horizon/const.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/domain_service.hpp"
#include "hw/tegra_x1/mmu/mmu.hpp"

namespace Hydra::Horizon::Services {

u8* get_buffer_ptr(const HW::MMU::MMUBase* mmu,
                   Hipc::BufferDescriptor* descriptor) {
    u64 addr = descriptor->address_low | (u64)descriptor->address_mid << 32 |
               (u64)descriptor->address_high << 36;

    return reinterpret_cast<u8*>(mmu->UnmapPtr(addr));
}

void ServiceBase::Control(Kernel& kernel, Reader& reader, Writer& writer) {
    auto cmif_in = reader.Read<Cmif::InHeader>();

    Result* res = Cmif::write_out_header(writer);

    switch (cmif_in.command_id) {
    case 0: { // convert to domain
        LOG_DEBUG(HorizonServices, "CONVERT TO DOMAIN");

        auto domain_service = new DomainService();
        kernel.SetService(handle, domain_service);
        handle = domain_service->AddObject(this);

        // Out
        writer.Write(handle);

        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unimplemented control request {}",
                    cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services
