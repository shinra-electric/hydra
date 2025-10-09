#pragma once

#include "core/horizon/services/account/const.hpp"

namespace hydra::horizon::services::account::internal {

constexpr uuid_t INVALID_USER_ID = 0x0;

struct Avatar {
    std::string bg_path;
    std::string char_path;
};

class User {
    friend class UserManager;

  public:
    User(const ProfileBase& base_, const UserData& data_, const Avatar& avatar_)
        : base{base_}, data{data_}, avatar{avatar_} {}
    User(const std::string_view nickname, const Avatar& avatar_) {
        // TODO: don't use the setters?
        SetNickname(nickname);
        SetAvatar(avatar_);
    }

    bool EditedSince(u64 timestamp) const {
        return base.last_edit_timestamp > timestamp;
    }

    u64 GetLastEditTimestamp() const { return base.last_edit_timestamp; }

    // Nickname
    std::string_view GetNickname() const { return base.nickname; }

    void SetNickname(const std::string_view nickname) {
        ASSERT(nickname.size() < NICKNAME_SIZE, Services,
               "Nickname size ({}) too big", nickname.size());
        std::memcpy(base.nickname, nickname.data(), nickname.size());
        base.nickname[nickname.size()] = '\0';
        NotifyEdit();
    }

    // Avatar
    void SetAvatar(const Avatar& avatar_) {
        avatar = avatar_;
        NotifyEdit();
    }

  private:
    ProfileBase base;
    UserData data;
    Avatar avatar;

    // Helpers
    void NotifyEdit() { base.last_edit_timestamp = get_absolute_time(); }

  public:
    CONST_REF_GETTER(base, GetBase);
    CONST_REF_GETTER(data, GetData);
    CONST_REF_GETTER(avatar, GetAvatar);
};

} // namespace hydra::horizon::services::account::internal
