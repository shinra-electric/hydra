#include "core/horizon/os.hpp"

#include "core/horizon/filesystem/content_archive.hpp"
#include "core/horizon/filesystem/host_file.hpp"
#include "core/horizon/services/account/account_service_for_application.hpp"
#include "core/horizon/services/account/account_service_for_system_service.hpp"
#include "core/horizon/services/am/all_system_applet_proxies_service.hpp"
#include "core/horizon/services/am/apm_manager.hpp"
#include "core/horizon/services/am/application_proxy_service.hpp"
#include "core/horizon/services/aocsrv/add_on_content_manager.hpp"
#include "core/horizon/services/apm/manager_privileged.hpp"
#include "core/horizon/services/audio/audio_out_manager.hpp"
#include "core/horizon/services/audio/audio_renderer_manager.hpp"
#include "core/horizon/services/codec/hardware_opus_decoder_manager.hpp"
#include "core/horizon/services/err/context/writer_for_application.hpp"
#include "core/horizon/services/fatalsrv/service.hpp"
#include "core/horizon/services/friends/service_creator.hpp"
#include "core/horizon/services/fssrv/filesystem_proxy.hpp"
#include "core/horizon/services/hid/hid_debug_server.hpp"
#include "core/horizon/services/hid/hid_server.hpp"
#include "core/horizon/services/hid/hid_system_server.hpp"
#include "core/horizon/services/hosbinder/hos_binder_driver.hpp"
#include "core/horizon/services/lbl/lbl_controller.hpp"
#include "core/horizon/services/lm/log_service.hpp"
#include "core/horizon/services/mii/static_service.hpp"
#include "core/horizon/services/mmnv/request.hpp"
#include "core/horizon/services/nfc/user_manager.hpp"
#include "core/horizon/services/nifm/static_service.hpp"
#include "core/horizon/services/ns/application_manager_interface.hpp"
#include "core/horizon/services/ns/service_getter_interface.hpp"
#include "core/horizon/services/nvdrv/nvdrv_services.hpp"
#include "core/horizon/services/pctl/parental_control_service_factory.hpp"
#include "core/horizon/services/pcv/pcv_service.hpp"
#include "core/horizon/services/pl/sharedresource/platform_shared_resource_manager.hpp"
#include "core/horizon/services/prepo/prepo_service.hpp"
#include "core/horizon/services/psm/psm_server.hpp"
#include "core/horizon/services/settings/settings_server.hpp"
#include "core/horizon/services/settings/system_settings_server.hpp"
#include "core/horizon/services/sm/user_interface.hpp"
#include "core/horizon/services/socket/client.hpp"
#include "core/horizon/services/socket/resolver/resolver.hpp"
#include "core/horizon/services/spl/general_interface.hpp"
#include "core/horizon/services/spl/random_interface.hpp"
#include "core/horizon/services/ssl/sf/ssl_service.hpp"
#include "core/horizon/services/timesrv/static_service.hpp"
#include "core/horizon/services/ts/measurement_server.hpp"
#include "core/horizon/services/visrv/application_root_service.hpp"
#include "core/horizon/services/visrv/manager_root_service.hpp"
#include "core/horizon/services/visrv/system_root_service.hpp"

namespace hydra::horizon {

namespace {

template <typename Key>
void RegisterServiceToPort(services::Server* server,
                           kernel::hipc::ServiceManager<Key>& service_manager,
                           const Key& port_name, services::IService* service) {
    // Register the server as the server for this service
    service->SetServer(server);

    // Debug
    std::string debug_name;
    if constexpr (std::is_same_v<Key, std::string>)
        debug_name = port_name;
    else
        debug_name = u64_to_str(port_name);

    // Session
    auto server_session = new kernel::hipc::ServerSession(
        service, fmt::format("\"{}\" server session", debug_name));
    auto client_session = new kernel::hipc::ClientSession(
        fmt::format("\"{}\" client session", debug_name));
    new kernel::hipc::Session(server_session, client_session,
                              fmt::format("\"{}\" session", debug_name));

    // Register server side
    if (server)
        server->RegisterSession(server_session);

    // Register client side
    service_manager.RegisterPort(port_name, client_session);
}

} // namespace

SINGLETON_DEFINE_GET_INSTANCE(OS, Horizon)

OS::OS(audio::ICore& audio_core_, ui::HandlerBase& ui_handler_)
    : audio_core{audio_core_}, ui_handler{ui_handler_} {
    SINGLETON_SET_INSTANCE(OS, Horizon);

    // Display
    display_driver.CreateDisplay();

    // Firmware
    std::map<u64, std::string> firmware_titles_map = {
        {0x010000000000080E, "TimeZoneBinary"},
        {0x0100000000000810, "FontNintendoExtension"},
        {0x0100000000000811, "FontStandard"},
        {0x0100000000000812, "FontKorean"},
        {0x0100000000000813, "FontChineseTraditional"},
        {0x0100000000000814, "FontChineseSimple"},
    };

    const auto& firmware_path = CONFIG_INSTANCE.GetFirmwarePath().Get();
    if (std::filesystem::exists(firmware_path)) {
        // Iterate over the directory
        for (const auto& entry :
             std::filesystem::directory_iterator(firmware_path)) {
            auto file =
                new horizon::filesystem::HostFile(entry.path().string());
            horizon::filesystem::ContentArchive content_archive(file);
            // TODO: find a better way to handle this
            if (content_archive.GetContentType() ==
                horizon::filesystem::ContentArchiveContentType::Meta)
                continue;

            auto it = firmware_titles_map.find(content_archive.GetTitleID());
            if (it == firmware_titles_map.end())
                continue;

            auto res = FILESYSTEM_INSTANCE.AddEntry(
                fmt::format(FS_FIRMWARE_PATH "/{}", it->second), file, true);
            ASSERT(res == horizon::filesystem::FsResult::Success, Other,
                   "Failed to add firmware entry: {}", res);
        }
    } else {
        LOG_ERROR(Other, "Firmware path does not exist");
    }

    // Services

    // Only some services have dedicated servers so as to avoid creating
    // unnecessary processes and threads
    // TODO: adjust the number of servers based on host CPU core count

    // SM
    RegisterServiceToPort<std::string>(
        nullptr, kernel.GetServiceManager(),
        "sm:", new services::sm::IUserInterface());

#define REGISTER_SERVICE_CASE(server, service, name)                           \
    RegisterServiceToPort(server, service_manager, name##_u64,                 \
                          new services::service());
#define REGISTER_SERVICE_IMPL(server, service, ...)                            \
    FOR_EACH_2_1(REGISTER_SERVICE_CASE, server, service, __VA_ARGS__)

#define REGISTER_SERVICE(service, ...)                                         \
    REGISTER_SERVICE_IMPL(nullptr, service, __VA_ARGS__)
#define REGISTER_SERVICE_TO_SERVER(server_name, service, ...)                  \
    REGISTER_SERVICE_IMPL(&server_name##_server, service, __VA_ARGS__)

    // HID
    REGISTER_SERVICE(hid::IHidServer, "hid");
    REGISTER_SERVICE(hid::IHidDebugServer, "hid:dbg");
    REGISTER_SERVICE(hid::IHidSystemServer, "hid:sys");

    // AM
    REGISTER_SERVICE(am::IApmManager, "apm", "apm:am");
    REGISTER_SERVICE(am::IApplicationProxyService, "appletOE");
    REGISTER_SERVICE(am::IAllSystemAppletProxiesService, "appletAE");

    // NS
    REGISTER_SERVICE(ns::IApplicationManagerInterface, "ns:am");
    REGISTER_SERVICE(ns::IServiceGetterInterface, "ns:am2", "ns:ec", "ns:rid",
                     "ns:rt", "ns:web", "ns:ro", "ns:sweb");
    REGISTER_SERVICE(aocsrv::IAddOnContentManager, "aoc:u");
    REGISTER_SERVICE(account::IAccountServiceForApplication, "acc:u0");
    REGISTER_SERVICE(account::IAccountServiceForSystemService, "acc:u1");

    // PPC
    REGISTER_SERVICE(apm::IManagerPrivileged, "apm:p");

    // Glue
    REGISTER_SERVICE(timesrv::IStaticService, "time:u", "time:a", "time:s");
    REGISTER_SERVICE(pl::shared_resource::IPlatformSharedResourceManager,
                     "pl:u");
    REGISTER_SERVICE(err::context::IWriterForApplication, "ectx:aw");

    // Audio
    REGISTER_SERVICE(audio::IAudioOutManager, "audout:u");
    REGISTER_SERVICE(audio::IAudioRendererManager, "audren:u");
    REGISTER_SERVICE(codec::IHardwareOpusDecoderManager, "hwopus");

    // Bcat
    REGISTER_SERVICE(prepo::IPrepoService, "prepo:a", "prepo:a2", "prepo:m",
                     "prepo:u", "prepo:s");

    // PCV
    REGISTER_SERVICE(pcv::IPcvService, "pcv");

    // Socket
    REGISTER_SERVICE(socket::IClient, "bsd:u", "bsd:s", "bsd:a");
    REGISTER_SERVICE(socket::Resolver::IResolver, "sfdnsres");

    // Capsrv
    REGISTER_SERVICE(mmnv::IRequest, "mm:u");

    // VI
    REGISTER_SERVICE(visrv::IApplicationRootService, "vi:u");
    REGISTER_SERVICE(visrv::ISystemRootService, "vi:s");
    REGISTER_SERVICE(visrv::IManagerRootService, "vi:m");
    REGISTER_SERVICE(lbl::ILblController, "lbl");

    // Nvnflinger
    REGISTER_SERVICE(hosbinder::IHOSBinderDriver, "dispdrv");

    // PTM
    REGISTER_SERVICE(psm::IPsmServer, "psm");
    REGISTER_SERVICE(ts::IMeasurementServer, "ts");

    // Fatal
    REGISTER_SERVICE(fatalsrv::IService, "fatal:u");

    // Friends
    REGISTER_SERVICE(friends::IServiceCreator, "friend:u", "friend:v",
                     "friend:m", "friend:s", "friend:a");

    // FS
    REGISTER_SERVICE(fssrv::IFileSystemProxy, "fsp-srv");

    // NFC
    REGISTER_SERVICE(nfc::IUserManager, "nfp:user");

    // Nifm
    REGISTER_SERVICE(nifm::IStaticService, "nifm:a", "nifm:s", "nifm:u");

    // Nvservices
    REGISTER_SERVICE_TO_SERVER(nvservices, nvdrv::INvDrvServices, "nvdrv",
                               "nvdrv:a", "nvdrv:s", "nvdrv:t");
    nvservices_server.Start();

    // Pctl
    REGISTER_SERVICE(pctl::IParentalControlServiceFactory, "pctl:s", "pctl:r",
                     "pctl:a", "pctl");

    // Settings
    REGISTER_SERVICE(settings::ISettingsServer, "set");
    REGISTER_SERVICE(settings::ISystemSettingsServer, "set:sys");

    // Ssl
    REGISTER_SERVICE(ssl::sf::ISslService, "ssl");

    // Spl
    REGISTER_SERVICE(spl::IRandomInterface, "csrng");
    REGISTER_SERVICE(spl::IGeneralInterface, "spl:");

    // Unknown
    REGISTER_SERVICE(lm::ILogService, "lm");
    REGISTER_SERVICE(mii::IStaticService, "mii:u", "mii:e");
}

OS::~OS() { SINGLETON_UNSET_INSTANCE(); }

} // namespace hydra::horizon
