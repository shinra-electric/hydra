#include "core/horizon/services/codec/hardware_opus_decoder.hpp"

namespace hydra::horizon::services::codec {

DEFINE_SERVICE_COMMAND_TABLE(IHardwareOpusDecoder, 0, DecodeInterleavedOld)

result_t IHardwareOpusDecoder::DecodeInterleavedOld(
    InBuffer<BufferAttr::MapAlias> in_opus_buffer, i32* out_decoded_data_size,
    i32* out_decoded_sample_count,
    OutBuffer<BufferAttr::MapAlias> out_pcm_buffer) {
    return DecodeInterleavedImpl(*in_opus_buffer.reader, out_decoded_data_size,
                                 out_decoded_sample_count,
                                 *out_pcm_buffer.writer);
}

result_t IHardwareOpusDecoder::DecodeInterleavedImpl(
    Reader& in_opus_reader, i32* out_decoded_data_size,
    i32* out_decoded_sample_count, Writer& out_pcm_writer) {
    LOG_FUNC_STUBBED(Services);

    // HACK
    *out_decoded_data_size = 0;
    *out_decoded_sample_count = 0;
    return RESULT_SUCCESS;
}

} // namespace hydra::horizon::services::codec
