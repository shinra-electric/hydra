#include "core/horizon/kernel/service_base.hpp"

#include "core/horizon/kernel/cmif.hpp"

namespace Hydra::Horizon::Kernel {

void ServiceBase::Request(REQUEST_PARAMS) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* result = Cmif::write_out_header(writers.writer);
    *result = RESULT_SUCCESS;

    RequestImpl(readers, writers, add_service, *result, cmif_in.command_id);
}

} // namespace Hydra::Horizon::Kernel
