#pragma once

namespace hydra::horizon::kernel {

template <typename T>
class StrongRef {
    static_assert(std::is_base_of<AutoObject, T>::value,
                  "T must derive from AutoObject");

  public:
    StrongRef(T* obj_) : obj{obj_} { obj->Retain(); }

    template <typename... Args>
    StrongRef(Args&&... args) : obj{new T(std::forward<Args>(args)...)} {}

    StrongRef(const StrongRef& other) : obj{other.obj} { obj->Retain(); }

    ~StrongRef() { obj->Release(); }

    StrongRef& operator=(const StrongRef& other) = delete;

    T* operator T*() const { return obj; }

    T* operator->() const { return obj; }

    T* GetRetained() const {
        obj->Retain();
        return obj;
    }

  private:
    T* obj;

  public:
    GETTER(obj, Get);
};

} // namespace hydra::horizon::kernel
