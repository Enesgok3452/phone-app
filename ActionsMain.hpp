#ifndef ACTIONSMAIN_HPP
#define ACTIONSMAIN_HPP

#include <unordered_map>
#include <functional>
#include <string>

#include "application/PhoneApplication.hpp"
#include "infrastructure/Db.hpp"
#include "dto/response/LoginResponse.hpp"
#include "token.hpp"  // Burada token.hpp dosyasýnýn yolu bu olmalý, kendi yapýna göre ayarla.

using ActionFunc = std::function<crow::response(const crow::json::rvalue &)>;

inline crow::response makeResponse(bool success, const std::string &successMsg, const std::string &errorMsg) {
    return success ? crow::response(200, successMsg) : crow::response(400, errorMsg);
}

inline crow::response makeAuthResponse(bool success, const std::string &successMsg, const std::string &errorMsg) {
    return success ? crow::response(200, successMsg) : crow::response(401, errorMsg);
}

// isTokenValid fonksiyonunu application katmanýnda tanýmladýðýmýzý varsayýyoruz.
// Eðer isTokenValid fonksiyonun DbContext ve token string alýyor ve bool döndürüyor.

inline void ActionmapsMain(std::unordered_map<std::string, ActionFunc> &actionMap, DbContext &dbContext) {
    // LOGIN: token kontrolü yok, giriþ yapýlýyor
    actionMap["login"] = [&](const crow::json::rvalue &json) {
        LoginResponse res = PhoneApplication::LoginUser(dbContext, json);
        crow::json::wvalue resJson;
        resJson["message"] = res.message;
        resJson["token"] = res.token;
        return crow::response(res.success ? 200 : 401, resJson);
    };

    // Token kontrolü yapýlacak iþlemler
    auto requireToken = [&](const crow::json::rvalue &json) -> bool {
        if (!json.has("token")) {
            std::cout << "[TOKEN] Token alaný json’da yok\n";
            return false;
        }
        std::string token = json["token"].s();
        if (token.empty()) {
            std::cout << "[TOKEN] Token boþ\n";
            return false;
        }
        if (!PhoneApplication::isTokenValid(dbContext, token)) {
            std::cout << "[TOKEN] Token geçersiz\n";
            return false;
        }
        return true;
    };

    actionMap["add"] = [&](const crow::json::rvalue &json) {
        if (!requireToken(json)) {
            return crow::response(401, "Geçersiz veya eksik token");
        }
        auto res = PhoneApplication::PhoneAdd(dbContext, json);
        return makeResponse(res.has_value(), "Person Successfully Added.", "Invalid Phone Number");
    };

    actionMap["delete"] = [&](const crow::json::rvalue &json) {
        if (!requireToken(json)) {
            return crow::response(401, "Geçersiz veya eksik token");
        }
        auto res = PhoneApplication::DeletePhone(dbContext, json);
        return makeResponse(res.success, "Person Successfully Deleted.", "Not Found Id.");
    };

    actionMap["update"] = [&](const crow::json::rvalue &json) {
        if (!requireToken(json)) {
            return crow::response(401, "Geçersiz veya eksik token");
        }
        auto res = PhoneApplication::PhoneUpdate(dbContext, json);
        return makeResponse(res.success, "Person Successfully Updated.", "Not Found Id.");
    };

    actionMap["list"] = [&](const crow::json::rvalue &json) {
        if (!requireToken(json)) {
            return crow::response(401, "Geçersiz veya eksik token");
        }
        return crow::response(200, PhoneApplication::FilterPhonesJson(dbContext, json));
    };

    actionMap["call"] = [&](const crow::json::rvalue &json) {
        if (!requireToken(json)) {
            return crow::response(401, "Geçersiz veya eksik token");
        }
        auto res = PhoneApplication::MakeCall(dbContext, json);
        return makeResponse(res.success, "Call Started... \nCall Finished.", "Invalid Phone Number");
    };

    actionMap["history"] = [&](const crow::json::rvalue &json) {
        if (!requireToken(json)) {
            return crow::response(401, "Geçersiz veya eksik token");
        }
        auto resultJson = PhoneApplication::ShowHistory(dbContext, json);
        return crow::response(200, resultJson);
    };
}

#endif // ACTIONSMAIN_HPP
