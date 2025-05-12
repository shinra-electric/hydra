#include "core/horizon/kernel/service_base.hpp"

#include "core/horizon/kernel/cmif.hpp"

namespace Hydra::Horizon::Kernel {

void ServiceBase::Request(RequestContext& context) {
    auto cmif_in = context.readers.reader.Read<Cmif::InHeader>();

    result_t* result = Cmif::write_out_header(context.writers.writer);

    *result = RequestImpl(context, cmif_in.command_id);
}

} // namespace Hydra::Horizon::Kernel
