#include "horizon/services/service_base.hpp"

#include "horizon/cmif.hpp"
#include "horizon/const.hpp"
#include "horizon/kernel.hpp"
#include "horizon/services/domain_service.hpp"
#include "hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon::Services {

u8* get_buffer_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::BufferDescriptor& descriptor) {
    uptr addr = descriptor.address_low | (u64)descriptor.address_mid << 32 |
                (u64)descriptor.address_high << 36;
    if (addr == 0x0)
        return nullptr;

    usize size = descriptor.size_low | (usize)descriptor.size_high << 32;
    if (size == 0x0)
        return nullptr;

    return reinterpret_cast<u8*>(mmu->UnmapAddr(addr));
}

u8* get_static_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                   const Hipc::StaticDescriptor& descriptor) {
    uptr addr = descriptor.address_low | (u64)descriptor.address_mid << 32 |
                (u64)descriptor.address_high << 36;
    if (addr == 0x0)
        return nullptr;

    if (descriptor.size == 0x0)
        return nullptr;

    return reinterpret_cast<u8*>(mmu->UnmapAddr(addr));
}

u8* get_list_entry_ptr(const HW::TegraX1::CPU::MMUBase* mmu,
                       const Hipc::RecvListEntry& descriptor) {
    uptr addr = descriptor.address_low | (u64)descriptor.address_high << 32;
    if (addr == 0x0)
        return nullptr;

    if (descriptor.size == 0x0)
        return nullptr;

    return reinterpret_cast<u8*>(mmu->UnmapAddr(addr));
}

void ServiceBase::Request(REQUEST_PARAMS) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* result = Cmif::write_out_header(writers.writer);
    *result = RESULT_SUCCESS;

    RequestImpl(readers, writers, add_service, *result, cmif_in.command_id);
}

void ServiceBase::Control(Readers& readers, Writers& writers) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* result = Cmif::write_out_header(writers.writer);
    *result = RESULT_SUCCESS;

    const auto command =
        static_cast<Cmif::ControlCommandType>(cmif_in.command_id);
    LOG_DEBUG(HorizonServices, "Control request {}", command);
    switch (command) {
    case Cmif::ControlCommandType::ConvertCurrentObjectToDomain: {
        auto domain_service = new DomainService();
        Kernel::GetInstance().SetHandle(handle_id, domain_service);
        domain_service->SetHandleId(handle_id);
        handle_id = domain_service->AddObject(this);

        // Out
        writers.writer.Write(handle_id);

        break;
    }
    case Cmif::ControlCommandType::CloneCurrentObject: { // clone current object
        auto clone = Clone();
        HandleId handle_id = Kernel::GetInstance().AddHandle(clone);
        clone->SetHandleId(handle_id);
        writers.move_handles_writer.Write(handle_id);
        break;
    }
    case Cmif::ControlCommandType::QueryPointerBufferSize: // query pointer
                                                           // buffer size
        writers.writer.Write(GetPointerBufferSize());
        break;
    case Cmif::ControlCommandType::CloneCurrentObjectEx: { // clone current ex
        // TODO: u32 tag
        auto clone = Clone();
        HandleId handle_id = Kernel::GetInstance().AddHandle(clone);
        clone->SetHandleId(handle_id);
        writers.move_handles_writer.Write(handle_id);
        break;
    }
    default:
        LOG_ERROR(HorizonServices, "Unimplemented control request {}", command);
        break;
    }
}

} // namespace Hydra::Horizon::Services
