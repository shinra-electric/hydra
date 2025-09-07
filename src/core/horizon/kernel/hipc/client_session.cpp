#include "core/horizon/kernel/hipc/client_session.hpp"

#include "core/horizon/kernel/hipc/session.hpp"

namespace hydra::horizon::kernel::hipc {

ClientSession::~ClientSession() { parent->OnClientClose(); }

} // namespace hydra::horizon::kernel::hipc
