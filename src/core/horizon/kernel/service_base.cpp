#include "core/horizon/kernel/service_base.hpp"

#include "core/horizon/kernel/cmif.hpp"

namespace hydra::horizon::kernel {

void ServiceBase::Request(RequestContext& context) {
    auto cmif_in = context.readers.reader.Read<cmif::InHeader>();
    ASSERT_DEBUG(cmif_in.magic == cmif::IN_HEADER_MAGIC, Kernel,
                 "Invalid CMIF in magic 0x{:08x}", cmif_in.magic);

    auto result = cmif::write_out_header(context.writers.writer);
    *result = RequestImpl(context, cmif_in.command_id);
}

void ServiceBase::TipcRequest(RequestContext& context, const u32 command_id) {
    auto result = context.writers.writer.WritePtr<result_t>();
    *result = RequestImpl(context, command_id);
}

} // namespace hydra::horizon::kernel
