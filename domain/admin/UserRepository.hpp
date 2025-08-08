#ifndef USERREPOSITORY_HPP
#define USERREPOSITORY_HPP

#include <optional>
#include <string>
#include <stdexcept>
#include "../infrastructure/Db.hpp"
#include "../admin/User.hpp"

namespace UserRepository {
    inline std::optional<User> getUserByUsername(DbContext &dbContext, const std::string &username) {
        try {
            std::string query = "SELECT id, username, password_hash, rol FROM Users WHERE username = ?";
            auto stmt = dbContext.createStatement(query);
            stmt->bind(1, username);

            auto resultSet = stmt->execute();
            auto row = resultSet.row();

            if (row.fetch()) {
                return User(row.getInt(1), row.getString(2), row.getString(3), row.getString(4));
            }
            return std::nullopt;
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("getUserByUsername failed: ") + e.what());
        }
    }

    inline bool saveUserToken(DbContext &dbContext, const std::string &username, const std::string &token) {
        try {
            std::string query = "INSERT INTO UserTokens (username, token) VALUES (?, ?)";
            auto stmt = dbContext.createStatement(query);
            stmt->bind(1, username);
            stmt->bind(2, token);
            stmt->execute();
            return true;
        } catch (const std::exception &e) {
            std::cerr << "[saveUserToken] Exception: " << e.what() << std::endl;
            return false;
        }
    }

    inline bool isTokenValid(DbContext &dbContext, const std::string &token) {
        try {
            std::string query = "SELECT COUNT(*) FROM UserTokens WHERE token = ?";
            auto stmt = dbContext.createStatement(query);
            stmt->bind(1, token);

            auto resultSet = stmt->execute();
            auto row = resultSet.row();
            return (row.fetch() && row.getInt(1) > 0);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("isTokenValid failed: ") + e.what());
        }
    }

    inline std::optional<User> getUserByToken(DbContext &dbContext, const std::string &token) {
        try {
            std::string query =
                    "SELECT u.id, u.username, u.password_hash, u.rol "
                    "FROM Users u "
                    "JOIN UserTokens t ON u.username = t.username "
                    "WHERE t.token = ?";
            auto stmt = dbContext.createStatement(query);
            stmt->bind(1, token);

            auto resultSet = stmt->execute();
            auto row = resultSet.row();

            if (row.fetch()) {
                return User(row.getInt(1), row.getString(2), row.getString(3), row.getString(4));
            }
            return std::nullopt;
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("getUserByToken failed: ") + e.what());
        }
    }

    inline bool clearUserTokens(DbContext &dbContext, const std::string &username) {
        try {
            std::string query = "DELETE FROM UserTokens WHERE username = ?";
            auto stmt = dbContext.createStatement(query);
            stmt->bind(1, username);
            stmt->execute();
            return true;
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("clearUserTokens failed: ") + e.what());
        }
    }
} // namespace UserRepository

#endif // USERREPOSITORY_HPP
