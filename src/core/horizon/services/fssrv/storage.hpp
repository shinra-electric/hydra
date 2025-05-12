#pragma once

#include "core/horizon/filesystem/file_base.hpp"
#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Fssrv {

// TODO: does IStorage always need to be backed by a file?
class IStorage : public ServiceBase {
  public:
    IStorage(Filesystem::FileBase* file_, Filesystem::FileOpenFlags flags);
    ~IStorage() override;

  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

    // Commands
    result_t Read(i64 offset, u64 size,
                  OutBuffer<BufferAttr::MapAlias> out_buffer);
    result_t Write(i64 offset, u64 size,
                   InBuffer<BufferAttr::MapAlias> in_buffer);
    STUB_REQUEST_COMMAND(Flush);
    result_t SetSize(i64 size);
    result_t GetSize(i64* out_size);

  private:
    Filesystem::FileBase* file;
    Filesystem::FileStream stream;
};

} // namespace Hydra::Horizon::Services::Fssrv
