#include "horizon/services/hosbinder/hos_binder_driver.hpp"

#include "horizon/cmif.hpp"

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

void IHOSBinderDriver::Request(Readers& readers, Writers& writers,
                               std::function<void(ServiceBase*)> add_service) {
    auto cmif_in = readers.reader.Read<Cmif::InHeader>();

    Result* res = Cmif::write_out_header(writers.writer);

    switch (cmif_in.command_id) {
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
        LOG_WARNING(HorizonServices, "Unknown request {}", cmif_in.command_id);
        break;
    }

    *res = RESULT_SUCCESS;
}

} // namespace Hydra::Horizon::Services::HosBinder
