#pragma once

#include "core/horizon/filesystem/file_base.hpp"
#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::fssrv {

class IFile : public IService {
  public:
    IFile(filesystem::FileBase* file_, filesystem::FileOpenFlags flags);
    ~IFile() override;

  private:
    result_t RequestImpl(RequestContext& context, u32 id) override;

    // Commands
    result_t Read(aligned<u32, 8> option, i64 offset, u64 size,
                  u64* out_written_size,
                  OutBuffer<BufferAttr::MapAlias> out_buffer);
    result_t Write(aligned<u32, 8> option, i64 offset, u64 size,
                   InBuffer<BufferAttr::MapAlias> in_buffer);
    result_t Flush();
    result_t SetSize(i64 size);
    result_t GetSize(i64* out_size);

  private:
    filesystem::FileBase* file;
    filesystem::FileStream stream;
};

} // namespace hydra::horizon::services::fssrv
