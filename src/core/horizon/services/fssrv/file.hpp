#pragma once

#include "core/horizon/filesystem/file_base.hpp"
#include "core/horizon/services/const.hpp"

namespace Hydra::Horizon::Services::Fssrv {

class IFile : public ServiceBase {
  public:
    IFile(Filesystem::FileBase* file_, Filesystem::FileOpenFlags flags);
    ~IFile() override;

  private:
    result_t RequestImpl(RequestContext& context, u32 id) override;

    // Commands
    result_t Read(u32 option, u32 _pad, i64 offset, u64 size,
                  u64* out_written_size,
                  OutBuffer<BufferAttr::MapAlias> out_buffer);
    result_t Write(u32 option, u32 _pad, i64 offset, u64 size,
                   InBuffer<BufferAttr::MapAlias> in_buffer);
    STUB_REQUEST_COMMAND(Flush);
    result_t SetSize(i64 size);
    result_t GetSize(i64* out_size);

  private:
    Filesystem::FileBase* file;
    Filesystem::FileStream stream;
};

} // namespace Hydra::Horizon::Services::Fssrv
