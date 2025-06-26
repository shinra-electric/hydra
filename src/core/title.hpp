#pragma once

namespace hydra {

enum class TitleID : u64 {
    // Applets
    OverlayApplet = 0x010000000000100c,
    SystemAppletMenu = 0x0100000000001000,
    SystemApplication = 0x0100000000001012,
    LibraryAppletAuth = 0x0100000000001001,
    LibraryAppletCabinet = 0x0100000000001002,
    LibraryAppletController = 0x0100000000001003,
    LibraryAppletDataErase = 0x0100000000001004,
    LibraryAppletError = 0x0100000000001005,
    LibraryAppletNetConnect = 0x0100000000001006,
    LibraryAppletPlayerSelect = 0x0100000000001007,
    LibraryAppletSwkbd = 0x0100000000001008,
    LibraryAppletMiiEdit = 0x0100000000001009,
    LibraryAppletWeb = 0x010000000000100a,
    LibraryAppletShop = 0x010000000000100b,
    LibraryAppletPhotoViewer = 0x010000000000100d,
    LibraryAppletSet = 0x010000000000100e,
    LibraryAppletOfflineWeb = 0x010000000000100f,
    LibraryAppletLoginShare = 0x0100000000001010,
    LibraryAppletWifiWebAuth = 0x0100000000001011,
    LibraryAppletMyPage = 0x0100000000001013,
    LibraryAppletGift = 0x010000000000101a,
    LibraryAppletUserMigration = 0x010000000000101c,
    LibraryAppletPreomiaSys = 0x010000000000101d,
    LibraryAppletStory = 0x0100000000001020,
    LibraryAppletPreomiaUsr = 0x010070000e3c0000,
    LibraryAppletPreomiaUsrDummy = 0x010086000e49c000,
    LibraryAppletSample = 0x0100000000001038,
    LibraryAppletPromoteQualification = LibraryAppletPlayerSelect,
    // TODO: more
};

}
