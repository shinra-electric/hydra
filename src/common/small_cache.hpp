#pragma once

#include <map>
#include <string>

#include "common/functions.hpp"
#include "common/log.hpp"
#include "common/macros.hpp"
#include "common/type_aliases.hpp"

namespace hydra {

template <typename KeyT, typename T, usize fast_cache_size = 4>
class SmallCache {
  public:
    void Clear() {
        fast_cache.fill({});
        slow_cache.clear();
    }

    T& Find(const KeyT& key) {
        // Fast cache
        for (auto& entry : fast_cache) {
            if (entry.occupied && entry.key == key) {
                return entry.value;
            }
        }

        // Slow cache
        auto it = slow_cache.find(key);
        if (it != slow_cache.end()) {
            return it->second;
        }

        // Insert into fast cache if possible
        for (auto& entry : fast_cache) {
            if (!entry.occupied) {
                entry.occupied = true;
                entry.key = key;
                entry.value = T{};
                return entry.value;
            }
        }

        // Fallback to slow cache
        return slow_cache.emplace(key, T{}).first->second;
    }

  private:
    struct FastCacheEntry {
        bool occupied{false};
        KeyT key;
        T value;
    };

    std::array<FastCacheEntry, fast_cache_size> fast_cache;
    std::map<KeyT, T> slow_cache;

  public:
    // Iterator
    class iterator {
      public:
        using map_iter = typename std::map<KeyT, T>::iterator;

        iterator(SmallCache* cache, usize fast_index)
            : cache(cache), fast_index(fast_index) {
            advance_fast();
        }

        iterator(SmallCache* cache, map_iter slow_it)
            : cache(cache), fast_index(fast_cache_size), slow_it(slow_it) {}

        std::pair<const KeyT&, T&> operator*() const {
            if (fast_index < fast_cache_size) {
                auto& e = cache->fast_cache[fast_index];
                return {e.key, e.value};
            }
            return {slow_it->first, slow_it->second};
        }

        iterator& operator++() {
            if (fast_index < fast_cache_size) {
                ++fast_index;
                advance_fast();
            } else {
                ++slow_it;
            }
            return *this;
        }

        bool operator==(const iterator& other) const {
            return cache == other.cache && fast_index == other.fast_index &&
                   (fast_index < fast_cache_size || slow_it == other.slow_it);
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

      private:
        void advance_fast() {
            while (fast_index < fast_cache_size &&
                   !cache->fast_cache[fast_index].occupied) {
                ++fast_index;
            }

            if (fast_index >= fast_cache_size) {
                slow_it = cache->slow_cache.begin();
            }
        }

        SmallCache* cache;
        usize fast_index;
        map_iter slow_it;
    };

    iterator begin() { return iterator(this, 0); }

    iterator end() { return iterator(this, slow_cache.end()); }
};

} // namespace hydra
