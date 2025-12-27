#pragma once

#include "common/io/stream.hpp"

namespace hydra::io {

struct SparseStreamEntry {
    IStream* stream;
    range<u64> range;
};

class SparseStream : public IStream {
  public:
    // Entries must be sorted by offset
    SparseStream(const std::vector<SparseStreamEntry>& entries_, u64 size_)
        : entries{std::move(entries_)}, size{size_} {}

    u64 GetSeek() const override { return seek; }
    void SeekTo(u64 seek) override { seek = seek; }
    void SeekBy(u64 offset) override { seek += offset; }

    u64 GetSize() const override { return size; }

    void Flush() override {
        // TODO: only flush modified entries
        for (auto entry : entries)
            entry.stream->Flush();
    }

    void ReadRaw(std::span<u8> buffer) override {
        while (!buffer.empty()) {
            ASSERT_DEBUG(seek <= size, Common, "Seek out of bounds");

            const auto entry = GetEntry(seek);
            const auto max_read_size = std::min(
                entry.range.end - seek, static_cast<u64>(buffer.size()));
            if (entry.stream)
                entry.stream->ReadRaw(buffer.subspan(0, max_read_size));
            else
                std::fill(buffer.begin(), buffer.begin() + max_read_size, 0);
            seek += max_read_size;
            buffer = buffer.subspan(max_read_size);
        }
    }

    void WriteRaw(std::span<const u8> buffer) override {
        while (!buffer.empty()) {
            ASSERT_DEBUG(seek <= size, Common, "Seek out of bounds");

            const auto entry = GetEntry(seek);
            const auto max_write_size = std::min(
                entry.range.end - seek, static_cast<u64>(buffer.size()));
            if (entry.stream)
                entry.stream->WriteRaw(buffer.subspan(0, max_write_size));
            seek += max_write_size;
            buffer = buffer.subspan(max_write_size);
        }
    }

  protected:
    std::vector<SparseStreamEntry> entries;

  private:
    u64 size;

    u64 seek{0};
    std::optional<SparseStreamEntry> cached_entry{std::nullopt};

    // Helpers
    SparseStreamEntry GetEntry(u64 offset) {
        // First, check if the entry has been cached
        if (cached_entry.has_value()) {
            const auto entry = cached_entry.value();
            if (entry.range.Contains(offset))
                return entry;
        }

        // Find the entry that contains the offset
        auto it =
            std::lower_bound(entries.begin(), entries.end(), offset,
                             [](const SparseStreamEntry& entry, u64 offset) {
                                 return entry.range.begin < offset;
                             });

        // If the offset is beyond the last entry, return an empty entry
        if (it == entries.end())
            return {.stream = nullptr, .range = {offset, size - offset}};

        // Check if entry is past the range
        if (!it->range.Contains(offset)) {
            auto next_it = std::next(it);
            if (next_it == entries.end())
                return {.stream = nullptr, .range = {offset, size - offset}};

            return {.stream = nullptr, .range = {offset, next_it->range.begin}};
        }

        // Cache the entry and return it
        cached_entry = *it;
        return *it;
    }
};

class OwnedSparseStream : public SparseStream {
  public:
    using SparseStream::SparseStream;

    ~OwnedSparseStream() override {
        for (auto entry : entries)
            delete entry.stream;
    }
};

} // namespace hydra::io
