#ifndef USERAPPLICATION_HPP
#define USERAPPLICATION_HPP

#include <iostream>
#include <string>
#include <optional>
#include "../infrastructure/Db.hpp"
#include "../domain/User/UserService.hpp"
#include "../dto/response/LoginResponse.hpp"
#include "token.hpp"

namespace UserApplication {

    inline bool isTokenValidForUser(DbContext &dbContext, const std::string &token, const std::string &username) {
        return UserService::isTokenValidForUser(dbContext, token, username);
    }

    inline std::optional<LoginResponse> Login(DbContext &dbContext, const crow::json::rvalue &json) {
        std::cout << "[Login] Fonksiyon başladı\n";

        if (!json.has("username") || !json.has("password")) {
            std::cout << "[Login] Eksik parametre\n";
            return std::nullopt;
        }
        std::string username = json["username"].s();
        std::string password = json["password"].s();
        std::cout << "[Login] Kullanıcı adı: " << username << "\n";

        auto userOpt = UserService::getUserByUsername(dbContext, username);
        if (!userOpt.has_value()) {
            std::cout << "[Login] Kullanıcı bulunamadı\n";
            return std::nullopt;
        }

        if (userOpt->password != password) {
            std::cout << "[Login] Şifre uyuşmuyor\n";
            return std::nullopt;
        }

        auto tokenOpt = UserService::getUserTokenByUserId(dbContext, userOpt->id);
        std::string token;

        if (tokenOpt.has_value()) {
            token = tokenOpt->token;
            std::cout << "[Login] Varolan token bulundu\n";
        } else {
            token = generateRandomToken();
            bool saved = UserService::saveUserToken(dbContext, userOpt->id, token);
            if (!saved) {
                std::cout << "[Login] Token kaydedilemedi\n";
                return std::nullopt;
            }
            std::cout << "[Login] Yeni token oluşturuldu ve kaydedildi\n";
        }
        std::cout << "[Login] İşlem başarılı, token: " << token << "\n";

        return LoginResponse{true, token};
    }

} // namespace UserApplication

#endif // US