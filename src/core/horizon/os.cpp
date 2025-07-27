#include "core/horizon/os.hpp"

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

SINGLETON_DEFINE_GET_INSTANCE(OS, Horizon)

OS::OS(audio::ICore& audio_core_, ui::HandlerBase& ui_handler_)
    : audio_core{audio_core_}, ui_handler{ui_handler_} {
    SINGLETON_SET_INSTANCE(OS, Horizon);

    // Services
    sm_server.RegisterServiceToPort<std::string>(
        kernel.GetServiceManager(), "sm:", new services::sm::IUserInterface());
    sm_server.Start();

#define REGISTER_SERVICE_CASE(server_name, service, name)                      \
    server_name##_server.RegisterServiceToPort(service_manager, name##_u64,    \
                                               new services::service());
#define REGISTER_SERVICE(server_name, service, ...)                            \
    FOR_EACH_2_1(REGISTER_SERVICE_CASE, server_name, service, __VA_ARGS__)

    // HID
    REGISTER_SERVICE(hid, hid::IHidServer, "hid");
    REGISTER_SERVICE(hid, hid::IHidDebugServer, "hid:dbg");
    REGISTER_SERVICE(hid, hid::IHidSystemServer, "hid:sys");
    hid_server.Start();

    // AM
    REGISTER_SERVICE(am, am::IApmManager, "apm", "apm:am");
    REGISTER_SERVICE(am, am::IApplicationProxyService, "appletOE");
    REGISTER_SERVICE(am, am::IAllSystemAppletProxiesService, "appletAE");
    am_server.Start();

    // NS
    REGISTER_SERVICE(ns, ns::IApplicationManagerInterface, "ns:am");
    REGISTER_SERVICE(ns, ns::IServiceGetterInterface, "ns:am2", "ns:ec",
                     "ns:rid", "ns:rt", "ns:web", "ns:ro", "ns:sweb");
    REGISTER_SERVICE(ns, aocsrv::IAddOnContentManager, "aoc:u");
    REGISTER_SERVICE(ns, account::IAccountServiceForApplication, "acc:u0");
    REGISTER_SERVICE(ns, account::IAccountServiceForSystemService, "acc:u1");
    ns_server.Start();

    // PPC
    REGISTER_SERVICE(ppc, apm::IManagerPrivileged, "apm:p");
    ppc_server.Start();

    // Glue
    REGISTER_SERVICE(glue, timesrv::IStaticService, "time:u", "time:a",
                     "time:s");
    REGISTER_SERVICE(glue, pl::shared_resource::IPlatformSharedResourceManager,
                     "pl:u");
    REGISTER_SERVICE(glue, err::context::IWriterForApplication, "ectx:aw");
    glue_server.Start();

    // Audio
    REGISTER_SERVICE(audio, audio::IAudioOutManager, "audout:u");
    REGISTER_SERVICE(audio, audio::IAudioRendererManager, "audren:u");
    REGISTER_SERVICE(audio, codec::IHardwareOpusDecoderManager, "hwopus");
    audio_server.Start();

    // Bcat
    REGISTER_SERVICE(bcat, prepo::IPrepoService, "prepo:a", "prepo:a2",
                     "prepo:m", "prepo:u", "prepo:s");
    bcat_server.Start();

    // PCV
    REGISTER_SERVICE(pcv, pcv::IPcvService, "pcv");
    pcv_server.Start();

    // Socket
    REGISTER_SERVICE(socket, socket::IClient, "bsd:u", "bsd:s", "bsd:a");
    REGISTER_SERVICE(socket, socket::Resolver::IResolver, "sfdnsres");
    socket_server.Start();

    // Capsrv
    REGISTER_SERVICE(capsrv, mmnv::IRequest, "mm:u");
    capsrv_server.Start();

    // VI
    REGISTER_SERVICE(vi, visrv::IApplicationRootService, "vi:u");
    REGISTER_SERVICE(vi, visrv::ISystemRootService, "vi:s");
    REGISTER_SERVICE(vi, visrv::IManagerRootService, "vi:m");
    REGISTER_SERVICE(vi, lbl::ILblController, "lbl");
    vi_server.Start();

    // Nvnflinger
    REGISTER_SERVICE(nvnflinger, hosbinder::IHOSBinderDriver, "dispdrv");
    nvnflinger_server.Start();

    // PTM
    REGISTER_SERVICE(ptm, psm::IPsmServer, "psm");
    REGISTER_SERVICE(ptm, ts::IMeasurementServer, "ts");
    ptm_server.Start();

    // Fatal
    REGISTER_SERVICE(fatal, fatalsrv::IService, "fatal:u");
    fatal_server.Start();

    // Friends
    REGISTER_SERVICE(friends, friends::IServiceCreator, "friend:u", "friend:v",
                     "friend:m", "friend:s", "friend:a");
    friends_server.Start();

    // FS
    REGISTER_SERVICE(fs, fssrv::IFileSystemProxy, "fsp-srv");
    fs_server.Start();

    // NFC
    REGISTER_SERVICE(nfc, nfc::IUserManager, "nfp:user");
    nfc_server.Start();

    // Nifm
    REGISTER_SERVICE(nifm, nifm::IStaticService, "nifm:a", "nifm:s", "nifm:u");
    nifm_server.Start();

    // Nvservices
    REGISTER_SERVICE(nvservices, nvdrv::INvDrvServices, "nvdrv", "nvdrv:a",
                     "nvdrv:s", "nvdrv:t");
    nvservices_server.Start();

    // Pctl
    REGISTER_SERVICE(pctl, pctl::IParentalControlServiceFactory, "pctl:s",
                     "pctl:r", "pctl:a", "pctl");
    pctl_server.Start();

    // Settings
    REGISTER_SERVICE(settings, settings::ISettingsServer, "set");
    REGISTER_SERVICE(settings, settings::ISystemSettingsServer, "set:sys");
    settings_server.Start();

    // Ssl
    REGISTER_SERVICE(ssl, ssl::sf::ISslService, "ssl");
    ssl_server.Start();

    // Spl
    REGISTER_SERVICE(spl, spl::IRandomInterface, "csrng");
    REGISTER_SERVICE(spl, spl::IGeneralInterface, "spl:");
    spl_server.Start();

    // Unknown
    REGISTER_SERVICE(unknown, lm::ILogService, "lm");
    REGISTER_SERVICE(unknown, mii::IStaticService, "mii:u", "mii:e");
    unknown_server.Start();
}

OS::~OS() { SINGLETON_UNSET_INSTANCE(); }

} // namespace hydra::horizon
