#include "core/horizon/services/settings/settings_server.hpp"

#include "core/horizon/const.hpp"

namespace Hydra::Horizon::Services::Settings {

DEFINE_SERVICE_COMMAND_TABLE(ISettingsServer, 0, GetLanguageCode, 1,
                             GetAvailableLanguageCodes, 3,
                             GetAvailableLanguageCodeCount)

void ISettingsServer::GetLanguageCode(REQUEST_COMMAND_PARAMS) {
    // TODO: make this configurable
    writers.writer.Write(LanguageCode::AmericanEnglish);
}

void ISettingsServer::GetAvailableLanguageCodes(REQUEST_COMMAND_PARAMS) {
    writers.recv_list_writers[0].Write(available_languages,
                                       sizeof_array(available_languages));
    writers.writer.Write<i32>(sizeof_array(available_languages));
}

void ISettingsServer::GetAvailableLanguageCodeCount(REQUEST_COMMAND_PARAMS) {
    writers.writer.Write<i32>(sizeof_array(available_languages));
}

} // namespace Hydra::Horizon::Services::Settings
