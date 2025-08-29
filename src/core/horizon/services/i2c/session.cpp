#include "core/horizon/services/i2c/session.hpp"

namespace hydra::horizon::services::i2c {

DEFINE_SERVICE_COMMAND_TABLE(ISession, 10, Send, 11, Receive, 12,
                             ExecuteCommandList)

result_t ISession::Send(TransactionOption transaction_option,
                        InBuffer<BufferAttr::AutoSelect> in_data_buffer) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Transaction option: {}", transaction_option);

    return RESULT_SUCCESS;
}

result_t ISession::Receive(TransactionOption transaction_option,
                           OutBuffer<BufferAttr::AutoSelect> out_data_buffer) {
    LOG_FUNC_STUBBED(Services);

    LOG_DEBUG(Services, "Transaction option: {}", transaction_option);

    return RESULT_SUCCESS;
}

result_t ISession::ExecuteCommandList(
    InBuffer<BufferAttr::HipcPointer> in_command_buffer,
    OutBuffer<BufferAttr::AutoSelect> out_receive_buffer) {
    LOG_FUNC_STUBBED(Services);
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::i2c
