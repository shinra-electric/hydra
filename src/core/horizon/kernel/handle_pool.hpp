#pragma once

#include "core/horizon/kernel/auto_object.hpp"

namespace hydra::horizon::kernel {

template <typename Pool>
class HandlePool {
  public:
    ~HandlePool() { CleanUp(); }

    void CleanUp() {
        for (u32 i = 0; i < pool.GetCapacity(); i++) {
            if (!pool.IsFree(i))
                pool.Get(i)->Release(); // Decrement ref count
        }
    }

    handle_id_t AddNoRetain(AutoObject* obj) {
        if (!obj)
            return INVALID_HANDLE_ID;

        u32 index = pool.AllocateForIndex();
        pool.GetRef(index) = obj;
        return IndexToHandleID(index);
    }

    handle_id_t Add(AutoObject* obj) {
        if (!obj)
            return INVALID_HANDLE_ID;

        obj->Retain(); // Increment ref count
        return AddNoRetain(obj);
    }

    void Free(handle_id_t handle_id) {
        // TODO: allow invalid handles
        ASSERT_DEBUG(handle_id != INVALID_HANDLE_ID, Kernel,
                     "Invalid handle ID");
        u32 index = HandleIDToIndex(handle_id);
        pool.Get(index)->Release(); // Decrement ref count
        pool.Free(index);
    }

    AutoObject* Get(handle_id_t handle_id) const {
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

template <usize size>
class StaticHandlePool : public HandlePool<StaticPool<AutoObject*, size>> {};

class DynamicHandlePool : public HandlePool<DynamicPool<AutoObject*>> {};

} // namespace hydra::horizon::kernel
