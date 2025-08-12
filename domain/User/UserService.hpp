#ifndef USERSERVICE_HPP
#define USERSERVICE_HPP

#include <optional>
#include <string>
#include "../infrastructure/Db.hpp"
#include "../domain/User/User.hpp"
#include "../domain/User/UserRepository.hpp"

namespace UserService {
    inline std::optional<User> getUserByUsername(DbContext &dbContext, const std::string &username) {
        try {
            return UserRepository::findByUsername(dbContext, username);
        } catch (const std::exception &e) {
            std::cerr << "getUserByUsername hata: " << e.what() << std::endl;
            return std::nullopt;
        }
    }

    inline std::optional<UserToken> getUserTokenByUserId(DbContext &dbContext, int userId) {
        return UserRepository::getTokenByUsername(dbContext, userId);
    }

    inline bool saveUserToken(DbContext &dbContext, int userId, const std::string &token) {
        return UserRepository::generateToken(dbContext, userId, token);
    }

    inline bool isTokenValid(DbContext &dbContext, const std::string &token) {
        return UserRepository::isTokenValid(dbContext, token);
    }

    inline bool isTokenValidForUser(DbContext &dbContext, const std::string &token, const std::string &username) {
        return UserRepository::isTokenValidForUser(dbContext, token, username);
    }


    // Aşağıdaki fonksiyonları repository tarafında tanımladıysan ekleyebilirsin:
    // inline bool isTokenValid(DbContext& dbContext, const std::string& token);
    // inline std::optional<User> getUserByToken(DbContext& dbContext, const std::string& token);
    // inline bool clearUserTokens(DbContext& dbContext, const std::string& username);
}

#endif // USERSERVICE_HPP
