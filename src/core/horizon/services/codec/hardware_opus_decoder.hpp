#pragma once

#include "core/horizon/services/const.hpp"

namespace hydra::horizon::services::codec {

class IHardwareOpusDecoder : public IService {
  protected:
    result_t RequestImpl(RequestContext& context, u32 id) override;

  private:
    // Commands
    result_t
    DecodeInterleavedOld(InBuffer<BufferAttr::MapAlias> in_opus_buffer,
                         i32* out_decoded_data_size,
                         i32* out_decoded_sample_count,
                         OutBuffer<BufferAttr::MapAlias> out_pcm_buffer);

    // Impl
    result_t DecodeInterleavedImpl(Reader& in_opus_reader,
                                   i32* out_decoded_data_size,
                                   i32* out_decoded_sample_count,
                                   Writer& out_pcm_writer);
};

} // namespace hydra::horizon::services::codec
