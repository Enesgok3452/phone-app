#ifndef ACTIONSMAIN_HPP
#define ACTIONSMAIN_HPP

#include <unordered_map>
#include <functional>
#include <string>
#include <iostream>

#include "application/PhoneApplication.hpp"
#include "infrastructure/Db.hpp"
#include "dto/response/LoginResponse.hpp"
#include "token.hpp"

using ActionFunc = std::function<crow::response(const crow::json::rvalue &, DbContext &)>;

inline crow::response makeResponse(bool success, const std::string &successMsg, const std::string &errorMsg) {
    return success ? crow::response(200, successMsg) : crow::response(400, errorMsg);
}

inline crow::response makeAuthResponse(bool success, const std::string &successMsg, const std::string &errorMsg) {
    return success ? crow::response(200, successMsg) : crow::response(401, errorMsg);
}

inline void ActionmapsMain(std::unordered_map<std::string, ActionFunc> &actionMap) {
    actionMap["login"] = [&](const crow::json::rvalue &json, DbContext &dbContext) {
        auto loginOpt = UserApplication::Login(dbContext, json);
        if (!loginOpt.has_value()) {
            crow::json::wvalue errorJson;
            errorJson["message"] = "Invalid username or password";
            return crow::response(401, errorJson);
        }
        const auto &loginRes = loginOpt.value();

        crow::json::wvalue resJson;
        resJson["success"] = loginRes.success;
        resJson["token"] = loginRes.token; // burasý token’ý json’a koyuyor

        return crow::response(200, resJson);
    };


    auto requireToken = [&](const crow::json::rvalue &json, DbContext &dbContext) -> bool {
        if (!json.has("token")) {
            std::cout << "[TOKEN] Token alaný json’da yok\n";
            return false;
        }
        if (!json.has("username")) {
            std::cout << "[TOKEN] Username alaný json’da yok\n";
            return false;
        }

        std::string token = json["token"].s();
        std::string username = json["username"].s();

        if (token.empty()) {
            std::cout << "[TOKEN] Token boþ\n";
            return false;
        }

        if (username.empty()) {
            std::cout << "[TOKEN] Username boþ\n";
            return false;
        }

        if (!UserApplication::isTokenValidForUser(dbContext, token, username)) {
            std::cout << "[TOKEN] Token geçersiz\n";
            return false;
        }
        return true;
    };

    actionMap["add"] = [&](const crow::json::rvalue &json, DbContext &dbContext) {
        try {
            if (!requireToken(json, dbContext)) {
                return crow::response(401, "Geçersiz veya eksik token");
            }
            auto resOpt = PhoneApplication::PhoneAdd(dbContext, json, json["token"].s());
            if (!resOpt.has_value()) {
                return crow::response(400, "Eksik veya hatalý parametre");
            }
            crow::json::wvalue resJson;
            resJson["name"] = resOpt->name;
            resJson["sname"] = resOpt->sname;
            return crow::response(200, resJson);
        } catch (const std::exception &ex) {
            std::cerr << "[add] Exception: " << ex.what() << std::endl;
            return crow::response(500, "Sunucu hatasý add");
        }
        catch (...) {
            std::cerr << "[add] Bilinmeyen hata!" << std::endl;
            return crow::response(500, "Sunucu hatasý add");
        }
    };

    actionMap["delete"] = [&](const crow::json::rvalue &json, DbContext &dbContext) {
        try {
            if (!requireToken(json, dbContext)) {
                return crow::response(401, "Geçersiz veya eksik token");
            }
            auto res = PhoneApplication::DeletePhone(dbContext, json, json["token"].s());
            if (!res.success) {
                return crow::response(400, "Silme iþlemi baþarýsýz");
            }
            return crow::response(200, "Baþarýyla silindi");
        } catch (const std::exception &ex) {
            std::cerr << "[delete] Exception: " << ex.what() << std::endl;
            return crow::response(500, "Sunucu hatasý delete");
        }
        catch (...) {
            std::cerr << "[delete] Bilinmeyen hata!" << std::endl;
            return crow::response(500, "Sunucu hatasý delete");
        }
    };

    actionMap["update"] = [&](const crow::json::rvalue &json, DbContext &dbContext) {
        try {
            if (!requireToken(json, dbContext)) {
                return crow::response(401, "Geçersiz veya eksik token");
            }
            auto res = PhoneApplication::PhoneUpdate(dbContext, json, json["token"].s());
            if (!res.success) {
                return crow::response(400, "Güncelleme iþlemi baþarýsýz");
            }
            crow::json::wvalue resJson;
            resJson["name"] = res.name;
            resJson["sname"] = res.sname;
            return crow::response(200, resJson);
        } catch (const std::exception &ex) {
            std::cerr << "[update] Exception: " << ex.what() << std::endl;
            return crow::response(500, "Sunucu hatasý update");
        }
        catch (...) {
            std::cerr << "[update] Bilinmeyen hata!" << std::endl;
            return crow::response(500, "Sunucu hatasý update");
        }
    };

    actionMap["list"] = [&](const crow::json::rvalue &json, DbContext &dbContext) {
        try {
            if (!requireToken(json, dbContext)) {
                return crow::response(401, "Geçersiz veya eksik token");
            }
            auto resultJson = PhoneApplication::FilterPhonesJson(dbContext, json, json["token"].s());
            return crow::response(200, resultJson);
        } catch (const std::exception &ex) {
            std::cerr << "[list] Exception: " << ex.what() << std::endl;
            return crow::response(500, "Sunucu hatasý list");
        }
        catch (...) {
            std::cerr << "[list] Bilinmeyen hata!" << std::endl;
            return crow::response(500, "Sunucu hatasý list");
        }
    };

    actionMap["call"] = [&](const crow::json::rvalue &json, DbContext &dbContext) {
        try {
            if (!requireToken(json, dbContext)) {
                return crow::response(401, "Geçersiz veya eksik token");
            }
            auto res = PhoneApplication::MakeCall(dbContext, json, json["token"].s());
            return res.success
                       ? crow::response(200, "Call started and finished")
                       : crow::response(400, "Invalid phone number");
        } catch (const std::exception &ex) {
            std::cerr << "[call] Exception: " << ex.what() << std::endl;
            return crow::response(500, "Sunucu hatasý call");
        }
        catch (...) {
            std::cerr << "[call] Bilinmeyen hata!" << std::endl;
            return crow::response(500, "Sunucu hatasý call");
        }
    };

    actionMap["history"] = [&](const crow::json::rvalue &json, DbContext &dbContext) {
        try {
            if (!requireToken(json, dbContext)) {
                return crow::response(401, "Geçersiz veya eksik token");
            }
            auto resultJson = PhoneApplication::ShowHistory(dbContext, json, json["token"].s());
            return crow::response(200, resultJson);
        } catch (const std::exception &ex) {
            std::cerr << "[history] Exception: " << ex.what() << std::endl;
            return crow::response(500, "Sunucu hatasý history");
        }
        catch (...) {
            std::cerr << "[history] Bilinmeyen hata!" << std::endl;
            return crow::response(500, "Sunucu hatasý history");
        }
    };
}

#endif // ACTIONSMAIN_HPP
