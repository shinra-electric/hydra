#pragma once

#include "core/horizon/const.hpp"

namespace hydra::horizon::applets::player_select {

enum class UiMode : u32 {
    UserSelector = 0,
    UserCreator = 1,
    EnsureNetworkServiceAccountAvailable = 2,
    UserIconEditor = 3,
    UserNicknameEditor = 4,
    UserCreatorForStarter = 5,
    NintendoAccountAuthorizationRequestContext = 6,
    IntroduceExternalNetworkServiceAccount = 7,
    IntroduceExternalNetworkServiceAccountForRegistration = 8,    // 6.0.0+
    NintendoAccountNnidLinker = 9,                                // 6.0.0+
    LicenseRequirementsForNetworkService = 10,                    // 6.0.0+
    LicenseRequirementsForNetworkServiceWithUserContextImpl = 11, // 7.0.0+
    UserCreatorForImmediateNaLoginTest = 12,                      // 7.0.0+
    UserQualificationPromoter = 13,                               // 13.0.0+
};

enum class UserSelectionPurpose : u32 {
    LoginAndCreate = 0,
    Login = 1,
    Create = 2,
};

#pragma pack(push, 1)
struct UiSettings {
    UiMode mode;
    u32 _padding_x4;
    uuid_t invalid_uuid_list[8];
    u64 application_id;
    bool is_network_service_account_required;
    bool is_skip_enabled;
    u8 _unknown_x92; // 0x1
    bool is_permitted;
    bool show_skip_button;
    bool additional_select;            // TODO: what is this? Is it bool or u8?
    bool user_creation_button_enabled; // 2.0.0+
    bool _unknown_x97;                 // 6.0.0+
    UserSelectionPurpose purpose;      // 2.0.0+
    u32 _unknown_x9c;
};

struct UiReturnArg {
    u64 result;
    uuid_t selected_user;
};
#pragma pack(pop)

} // namespace hydra::horizon::applets::player_select

ENABLE_ENUM_FORMATTING(
    hydra::horizon::applets::player_select::UiMode, UserSelector,
    "user selector", UserCreator, "user creator",
    EnsureNetworkServiceAccountAvailable,
    "ensure network service account available", UserIconEditor,
    "user icon editor", UserNicknameEditor, "user nickname editor",
    UserCreatorForStarter, "user creator for starter",
    NintendoAccountAuthorizationRequestContext,
    "nintendo account authorization request context",
    IntroduceExternalNetworkServiceAccount,
    "introduce external network service account",
    IntroduceExternalNetworkServiceAccountForRegistration,
    "introduce external network service account for registration",
    NintendoAccountNnidLinker, "nintendo account nnid linker",
    LicenseRequirementsForNetworkService,
    "license requirements for network service",
    LicenseRequirementsForNetworkServiceWithUserContextImpl,
    "license requirements for network service with user context impl",
    UserCreatorForImmediateNaLoginTest,
    "user creator for immediate na login test", UserQualificationPromoter,
    "user qualification promoter")
