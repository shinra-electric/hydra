#include "core/horizon/services/ns/const.hpp"

namespace hydra::horizon::services::ns {

const ApplicationTitle&
ApplicationControlProperty::GetApplicationTitle() const {
    // TODO: language
    return titles[0];
}

} // namespace hydra::horizon::services::ns
