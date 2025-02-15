#include "horizon/services/hosbinder/hos_binder_driver.hpp"

namespace Hydra::Horizon::Services::HosBinder {

enum class BinderType : i32 {
    Weak = 0,
    Strong = 1,
};

struct AdjustRefcountIn {
    i32 binder_id;
    i32 addval;
    BinderType type;
};

void IHOSBinderDriver::RequestImpl(REQUEST_IMPL_PARAMS) {
    switch (id) {
    case 1: { // AdjustRefcount
        auto in = readers.reader.Read<AdjustRefcountIn>();
        auto& binder = binders[in.binder_id];
        switch (in.type) {
        case BinderType::Weak:
            binder.weak_ref_count += in.addval;
            break;
        case BinderType::Strong:
            binder.strong_ref_count += in.addval;
            break;
        }
        break;
    }
    default:
        LOG_WARNING(HorizonServices, "Unknown request {}", id);
        break;
    }
}

} // namespace Hydra::Horizon::Services::HosBinder
