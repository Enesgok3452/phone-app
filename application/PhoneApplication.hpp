#ifndef PHONEAPPLICATION_HPP
#define PHONEAPPLICATION_HPP

#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <optional>

#include "../infrastructure/Db.hpp"
#include "../domain/phone/PhoneService.hpp"
#include "../domain/phone/Phone.hpp"
#include "../domain/History/HistoryService.hpp"
#include "../domain/History/History.hpp"
#include "../domain/History/HistoryFactory.hpp"

#include "../DTO/request/InsertPhoneRequest.hpp"
#include "../DTO/response/InsertPhoneResponse.hpp"
#include "../DTO/request/UpdateRequest.hpp"
#include "../DTO/response/UpdateResponse.hpp"
#include "../DTO/request/DeletePhoneRequest.hpp"
#include "../DTO/response/DeletePhoneRespone.hpp"
#include "../DTO/request/ListPhoneRequest.hpp"
#include "../DTO/response/ListPhoneResponse.hpp"
#include "../DTO/request/MakeCallPhoneRequest.hpp"
#include "../DTO/response/MakeCallResponse.hpp"
#include "../DTO/response/HistoryResponse.hpp"
#include "../DTO/request/HistoryRequest.hpp"

#include "../domain/admin/UserService.hpp"
#include "../dto/request/LoginRequest.hpp"
#include "../dto/response/LoginResponse.hpp"
#include "token.hpp"

namespace PhoneApplication {
    inline std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    inline bool isTokenValid(DbContext &dbContext, const std::string &token) {
        auto userOpt = UserService::getUserByToken(dbContext, token);
        return userOpt.has_value(); // Token varsa geçerli kabul et
    }

    // Eğer kullanıcı bilgisine ihtiyacın varsa
    inline std::optional<User> getUserByToken(DbContext &dbContext, const std::string &token) {
        return UserService::getUserByToken(dbContext, token);
    }

    inline bool authenticateUser(DbContext &dbContext, const std::string &username, const std::string &inputpassword) {
        auto userOpt = UserService::getUserByUsername(dbContext, username);
        if (!userOpt.has_value()) {
            return false;
        }
        User user = userOpt.value();
        // Not: user.getPassword() veritabanındaki kayıt (düz metin veya hash). Eğer hash saklıyorsan burada aynı hash fonksiyonunu uygulamalısın.
        return user.getPassword() == inputpassword;
    }

    inline LoginResponse LoginUser(DbContext &dbContext, const crow::json::rvalue &json) {
        LoginResponse res;
        if (!json.has("username") || !json.has("password")) {
            res.success = false;
            res.message = "Eksik parametre: username veya password";
            return res;
        }

        std::string username = json["username"].s();
        std::string password = json["password"].s();

        bool authSuccess = authenticateUser(dbContext, username, password);
        res.success = authSuccess;

        if (authSuccess) {
            try {
                UserService::clearUserTokens(dbContext, username); // Eski tokenları temizle
                std::string token = generateToken();

                // Yeni token'ı kaydet
                bool saveSuccess = UserService::saveUserToken(dbContext, username, token);
                if (!saveSuccess) {
                    res.success = false;
                    res.message = "Token kaydedilemedi!";
                    return res;
                }

                res.message = "Giriş başarılı";
                res.token = token;
            }
            catch (const std::exception &ex) {
                res.success = false;
                res.message = std::string("Veritabanı hatası: ") + ex.what();
            }
        } else {
            res.message = "Yetkilendirme hatası: Kullanıcı adı veya şifre yanlış.";
        }

        return res;
    }



    inline std::optional<InsertResponse> PhoneAdd(DbContext &dbContext, const crow::json::rvalue &json) {
        if (!json.has("name") || !json.has("sname") || !json.has("pnumber")) {
            return std::nullopt;
        }

        auto name = json["name"].s();
        auto sname = json["sname"].s();
        auto pnumber = json["pnumber"].s();

        if (!PhoneService::isValidPhoneNumber(pnumber)) {
            return std::nullopt;
        }

        PhoneService::AddPhone(name, sname, pnumber, dbContext);

        return InsertResponse{name, sname};
    }

    inline DeleteResponse DeletePhone(DbContext &dbContext, const crow::json::rvalue &json) {
        DeleteResponse response;
        if (!json.has("id")) {
            response.success = false;
            return response;
        }
        int id = json["id"].i();

        bool result = PhoneService::DeleteById(id, dbContext);
        response.success = result;
        return response;
    }

    inline UpdateResponse PhoneUpdate(DbContext &dbContext, const crow::json::rvalue &json) {
        UpdateResponse response;
        if (!json.has("id") || !json.has("name") || !json.has("sname") || !json.has("pnumber")) {
            response.success = false;
            return response;
        }

        int id = json["id"].i();
        auto name = json["name"].s();
        auto sname = json["sname"].s();
        auto pnumber = json["pnumber"].s();

        bool success = PhoneService::UpdatePhone(dbContext, id, name, sname, pnumber);
        response.success = success;

        if (success) {
            response.name = name;
            response.sname = sname;
        }

        return response;
    }

    inline crow::json::wvalue FilterPhonesJson(DbContext &dbContext, const crow::json::rvalue &json) {
        GetPhoneListRequest request;
        request.filterName = json.has("filterName") ? std::string(json["filterName"].s()) : "";

        auto filteredPhones = PhoneService::FilterPhonesByName(dbContext, request.filterName);

        crow::json::wvalue resultJson;
        crow::json::wvalue phones_array = crow::json::wvalue::list();

        int index = 0;
        for (const auto &phone: filteredPhones) {
            crow::json::wvalue item;
            item["id"] = phone.getId().value_or(0);
            item["name"] = phone.getName();
            item["sname"] = phone.getSname();
            item["pnumber"] = phone.getPnumber();
            phones_array[index++] = std::move(item);
        }

        resultJson["phones"] = std::move(phones_array);
        return resultJson;
    }

    inline CallResponse MakeCall(DbContext &dbContext, const crow::json::rvalue &json) {
        CallResponse response;

        if (!json.has("pnumber")) {
            response.success = false;
            return response;
        }

        std::string pnumber = json["pnumber"].s();

        bool isRegistered = PhoneService::isPhoneExist(pnumber, dbContext);

        if (isRegistered) {
            HistoryService::add(pnumber, dbContext);
        }

        response.success = isRegistered;
        return response;
    }

    inline crow::json::wvalue ShowHistory(DbContext &dbContext, const crow::json::rvalue &json) {
        HistoryRequest request;
        request.filterName = json.has("filterName") ? std::string(json["filterName"].s()) : "";

        auto histories = HistoryService::GetAll(dbContext, request.filterName);

        crow::json::wvalue histories_array = crow::json::wvalue::list();

        int index = 0;
        for (const auto &h: histories) {
            crow::json::wvalue item;
            item["callerName"] = h.getcallerName();
            item["dialedName"] = h.getdialedName();
            item["dialedTime"] = h.getdialedTime();
            histories_array[index++] = std::move(item);
        }

        crow::json::wvalue resultJson;
        resultJson["histories"] = std::move(histories_array);

        return resultJson;
    }

    inline std::optional<Phone> FindPhonesById(DbContext &dbContext, int id) {
        return PhoneService::findPhoneById(id, dbContext);
    }
} // namespace PhoneApplication

#endif // PHONEAPPLICATION_HPP
