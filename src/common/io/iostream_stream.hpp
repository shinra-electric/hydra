#pragma once

#include <istream>
#include <ostream>

#include "common/io/stream.hpp"

namespace hydra::io {

class IostreamStream : public IStream {
  public:
    IostreamStream(std::iostream& stream_) : stream{stream_} {}

    u64 GetSeek() const override { return static_cast<u64>(stream.tellg()); }
    void SeekTo(u64 seek) override {
        stream.seekg(static_cast<i64>(seek), std::ios::beg);
    }
    void SeekBy(u64 offset) override {
        stream.seekg(static_cast<i64>(offset), std::ios::cur);
    }

    u64 GetSize() const override {
        const auto seek = stream.tellg();
        stream.seekg(0, std::ios::end);
        const auto size = stream.tellg();
        stream.seekg(seek, std::ios::beg);
        return static_cast<u64>(size);
    }

    void Flush() override { stream.flush(); }

    void ReadRaw(std::span<u8> buffer) override {
        stream.read(reinterpret_cast<char*>(buffer.data()),
                    static_cast<i32>(buffer.size()));
    }

    void WriteRaw(std::span<const u8> buffer) override {
        stream.write(reinterpret_cast<const char*>(buffer.data()),
                     static_cast<i32>(buffer.size()));
    }

  private:
    std::iostream& stream;
};

} // namespace hydra::io
