#include "horizon/services/service_base.hpp"

#include "horizon/cmif.hpp"
#include "horizon/const.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/domain_service.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon::Services {

u8* get_buffer_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::BufferDescriptor& descriptor) {
    u64 addr = descriptor.address_low | (u64)descriptor.address_mid << 32 |
               (u64)descriptor.address_high << 36;

    return reinterpret_cast<u8*>(mmu->UnmapAddr(addr));
}

void ServiceBase::Request(REQUEST_PARAMS) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* result = Cmif::write_out_header(writers.writer);
    *result = RESULT_SUCCESS;

    RequestImpl(readers, writers, add_service, *result, cmif_in.command_id);
}

void ServiceBase::Control(Reader& reader, Writer& writer) {
    auto cmif_in = reader.Read<Cmif::InHeader>();

    Result* result = Cmif::write_out_header(writer);
    *result = RESULT_SUCCESS;

    switch (cmif_in.command_id) {
    case 0: { // convert to domain
        auto domain_service = new DomainService();
        Kernel::GetInstance().SetHandle(handle_id, domain_service);
        domain_service->SetHandleId(handle_id);
        handle_id = domain_service->AddObject(this);

        // Out
        writer.Write(handle_id);

        break;
    }
    case 3: // query pointer buffer size
        // TODO: what's the point of this?
        writer.Write(0);
        break;
    case 4: { // clone current ex
        // TODO: u32 tag
        HandleId handle_id = Kernel::GetInstance().AddHandle(Clone());
        writer.Write(handle_id);
        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unimplemented control request {}",
                    cmif_in.command_id);
        break;
    }
}

} // namespace Hydra::Horizon::Services
