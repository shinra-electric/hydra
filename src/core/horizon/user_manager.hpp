#pragma once

#include "core/horizon/const.hpp"

#define USER_MANAGER_INSTANCE horizon::UserManager::GetInstance()

namespace hydra::horizon {

constexpr uuid_t INVALID_USER_ID = 0x0;

class User {
    friend class UserManager;

    public:
    bool EditedSince(u64 timestamp) const {
        return last_edit_timestamp > timestamp;
    }

    // Getters
    u64 GetLastEditTimestamp() const {
        return last_edit_timestamp;
    }

    const std::string& GetNickname() const {
        return nickname;
    }

    // Setters
    void SetNickname(const std::string& nickname_) {
        nickname = nickname_;
        NotifyEdit();
    }

    private:
    u64 last_edit_timestamp;
    std::string nickname;
    // TODO: more

    User(const std::string& nickname_) : nickname{nickname_} {
        NotifyEdit();
    }

    // Helpers
    void NotifyEdit() {
        last_edit_timestamp = get_absolute_time();
    }
};

class UserManager {
  public:
    static UserManager& GetInstance();

    UserManager();
    ~UserManager();

    uuid_t Create();

    void Flush();

    const uuid_t* Begin() {
        return &users.begin()->first;
    }

    const uuid_t* End() {
        return &users.end()->first;
    }

    // Getters
    User& Get(uuid_t user_id) {
        return GetPair(user_id).first;
    }

    bool Exists(uuid_t user_id) const {
        return users.contains(user_id);
    }

    usize GetCount() const {
        return users.size();
    }

  private:
    std::map<uuid_t, std::pair<User, u64>> users;

    // Helpers
    std::pair<User, u64>& GetPair(uuid_t user_id) {
        auto it = users.find(user_id);
        ASSERT(it != users.end(), Horizon, "Invalid user 0x{:08x}", user_id);

        return it->second;
    }

    void Serialize(uuid_t user_id);
    void Deserialize(uuid_t user_id);
};

} // namespace hydra::horizon
