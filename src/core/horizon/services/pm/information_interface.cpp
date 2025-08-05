#include "core/horizon/services/pm/information_interface.hpp"

#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::services::pm {

DEFINE_SERVICE_COMMAND_TABLE(IInformationInterface, 0, GetProgramId)

result_t IInformationInterface::GetProgramId(kernel::Process* process,
                                             u64* out_program_id) {
    *out_program_id = process->GetTitleID();
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::pm
