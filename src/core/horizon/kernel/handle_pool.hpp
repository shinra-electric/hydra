#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

template <typename T, typename Pool>
class HandlePool {
    static_assert(std::is_convertible_v<T*, AutoObject*>,
                  "Type does not inherit from AutoObject");

  public:
    ~HandlePool() { CleanUp(); }

    void CleanUp() {
        for (u32 i = 0; i < pool.GetCapacity(); i++) {
            if (!pool.IsFree(i))
                pool.Get(i)->Release(); // Decrement ref count
        }
    }

    handle_id_t Add(T* handle) {
        if (!handle)
            return INVALID_HANDLE_ID;

        handle->Retain(); // Increment ref count
        u32 index = pool.AllocateForIndex();
        pool.GetRef(index) = handle;
        return IndexToHandleID(index);
    }

    void Free(handle_id_t handle_id) {
        // TODO: allow invalid handles
        ASSERT_DEBUG(handle_id != INVALID_HANDLE_ID, Kernel,
                     "Invalid handle ID");
        u32 index = HandleIDToIndex(handle_id);
        pool.Get(index)->Release(); // Decrement ref count
        pool.Free(index);
    }

    T* Get(handle_id_t handle_id) const {
        if (handle_id == INVALID_HANDLE_ID)
            return nullptr;

        return pool.GetOrDefault(HandleIDToIndex(handle_id));
    }

  private:
    Pool pool;

    // Helpers
    static u32 HandleIDToIndex(handle_id_t handle_id) { return handle_id - 1; }

    static handle_id_t IndexToHandleID(u32 index) { return index + 1; }
};

template <typename T, usize size>
class StaticHandlePool : public HandlePool<T, StaticPool<T*, size>> {};

template <typename T>
class DynamicHandlePool : public HandlePool<T, DynamicPool<T*>> {};

} // namespace hydra::horizon::kernel
