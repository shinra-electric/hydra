#pragma once

#include "core/horizon/services/service_base.hpp"

namespace Hydra::Horizon::Services::Fssrv {

// TODO: does IStorage always need to be backed by a file?
class IStorage : public ServiceBase {
  public:
    DEFINE_SERVICE_VIRTUAL_FUNCTIONS(IStorage)

    IStorage(Filesystem::FileBase* file_);
    ~IStorage() override;

  protected:
    void RequestImpl(REQUEST_IMPL_PARAMS) override;

    // Commands
    virtual void Read(REQUEST_COMMAND_PARAMS);
    virtual void Write(REQUEST_COMMAND_PARAMS);
    STUB_REQUEST_COMMAND(SetSize);
    void GetSize(REQUEST_COMMAND_PARAMS);

    // Helpers
    void ReadImpl(u8* ptr, i64 offset, usize& size);
    void WriteImpl(u8* ptr, i64 offset, usize size);

  private:
    Filesystem::FileBase* file;
};

} // namespace Hydra::Horizon::Services::Fssrv
