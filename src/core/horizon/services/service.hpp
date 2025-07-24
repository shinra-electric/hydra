#pragma once

#include "core/horizon/kernel/hipc/const.hpp"

namespace hydra::horizon::kernel {
class Process;
}

namespace hydra::horizon::services {

using result_t = kernel::result_t;

class Server;
class IService;

struct RequestContext {
    Server& server;
    kernel::Process* process;
    kernel::hipc::Readers& readers;
    kernel::hipc::Writers& writers;
};

class IService {
  public:
    virtual ~IService() {}

    void HandleRequest(Server& server, kernel::Process* caller_process,
                       uptr ptr);

    void AddService(RequestContext& context, IService* service);
    IService* GetService(RequestContext& context, handle_id_t handle_id);

  protected:
    virtual result_t RequestImpl(RequestContext& context, u32 id) = 0;
    virtual usize GetPointerBufferSize() { return 0; }

  private:
    // Domain
    bool is_domain{false};
    DynamicPool<IService*> subservice_pool;

    void Close();
    void Request(RequestContext& context);
    void CmifRequest(RequestContext& context);
    void Control(Server& server, kernel::Process* caller_process,
                 kernel::hipc::Readers& readers,
                 kernel::hipc::Writers& writers);
    void Clone(Server& server, kernel::Process* caller_process,
               kernel::hipc::Writers& writers);
    void TipcRequest(RequestContext& context, const u32 command_id);
};

} // namespace hydra::horizon::services
