#ifndef USERSERVICE_HPP
#define USERSERVICE_HPP

#include <optional>
#include <string>
#include "../infrastructure/Db.hpp"
#include "../admin/User.hpp"
#include "../domain/admin/UserRepository.hpp"

namespace UserService {
    inline std::optional<User> getUserByUsername(DbContext &dbContext, const std::string &username) {
        return UserRepository::getUserByUsername(dbContext, username);
    }

    inline bool saveUserToken(DbContext &dbContext, const std::string &username, const std::string &token) {
        return UserRepository::saveUserToken(dbContext, username, token);
    }

    inline bool isTokenValid(DbContext &dbContext, const std::string &token) {
        return UserRepository::isTokenValid(dbContext, token);
    }

    inline std::optional<User> getUserByToken(DbContext &dbContext, const std::string &token) {
        return UserRepository::getUserByToken(dbContext, token);
    }

    inline bool clearUserTokens(DbContext &dbContext, const std::string &username) {
        return UserRepository::clearUserTokens(dbContext, username);
    }
} // namespace UserService

#endif // USERSERVICE_HPP
