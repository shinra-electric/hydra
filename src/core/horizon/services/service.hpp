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
    virtual ~IService();

    void HandleRequest(Server& server, kernel::Process* caller_process,
                       uptr ptr);

    void AddService(RequestContext& context, IService* service);
    IService* GetService(RequestContext& context, handle_id_t handle_id);

  protected:
    virtual result_t RequestImpl(RequestContext& context, u32 id) = 0;
    virtual usize GetPointerBufferSize() { return 0; }

    u32 AddSubservice(IService* service) {
        if (!service)
            return INVALID_HANDLE_ID;

        u32 index = parent->subservice_pool->AllocateForIndex();
        parent->subservice_pool->GetRef(index) = service;
        return IndexToObjectID(index);
    }

    void FreeSubservice(handle_id_t handle_id) {
        u32 index = ObjectIDToIndex(handle_id);
        // TODO: should it be deleted?
        // delete parent->subservice_pool->Get(index);
        parent->subservice_pool->Free(index);
    }

    IService* GetSubservice(handle_id_t handle_id) const {
        return parent->subservice_pool->Get(ObjectIDToIndex(handle_id));
    }

  private:
    // Domain
    bool is_domain{false};
    IService* parent{this};
    DynamicPool<IService*>* subservice_pool{nullptr};

    void Close();
    void Request(RequestContext& context);
    void CmifRequest(RequestContext& context);
    void Control(Server& server, kernel::Process* caller_process,
                 kernel::hipc::Readers& readers,
                 kernel::hipc::Writers& writers);
    void Clone(Server& server, kernel::Process* caller_process,
               kernel::hipc::Writers& writers);
    void TipcRequest(RequestContext& context, const u32 command_id);

    // Helpers
    static u32 ObjectIDToIndex(handle_id_t handle_id) {
        ASSERT_DEBUG(handle_id != INVALID_HANDLE_ID, Kernel,
                     "Invalid handle ID");
        return handle_id - 1;
    }

    static u32 IndexToObjectID(u32 index) { return index + 1; }

  public:
    GETTER(is_domain, IsDomain);
};

} // namespace hydra::horizon::services
