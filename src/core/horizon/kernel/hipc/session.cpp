#include "core/horizon/kernel/hipc/session.hpp"

#include "core/horizon/kernel/hipc/client_session.hpp"
#include "core/horizon/kernel/hipc/cmif.hpp"
#include "core/horizon/kernel/process.hpp"

namespace hydra::horizon::kernel::hipc {

Session::Session(ServerSession* server_side_, ClientSession* client_side_,
                 const std::string_view debug_name)
    : AutoObject(debug_name), server_side{server_side_}, client_side{
                                                             client_side_} {
    client_side->SetParent(this);
}

} // namespace hydra::horizon::kernel::hipc
