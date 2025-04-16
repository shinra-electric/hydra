#include "core/horizon/services/service_base.hpp"

#include "core/horizon/cmif.hpp"
#include "core/horizon/const.hpp"
#include "core/horizon/kernel.hpp"
#include "core/hw/tegra_x1/cpu/mmu_base.hpp"

namespace Hydra::Horizon::Services {

void ServiceBase::Request(REQUEST_PARAMS) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* result = Cmif::write_out_header(writers.writer);
    *result = RESULT_SUCCESS;

    RequestImpl(readers, writers, add_service, *result, cmif_in.command_id);
}

} // namespace Hydra::Horizon::Services
