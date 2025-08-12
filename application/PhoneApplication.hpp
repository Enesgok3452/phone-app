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
#include "../DTO/response/InsertPhoneResponse.hpp"
#include "../DTO/response/UpdateResponse.hpp"
#include "../DTO/response/DeletePhoneRespone.hpp"
#include "../DTO/request/ListPhoneRequest.hpp"
#include "../DTO/response/MakeCallResponse.hpp"
#include "../DTO/request/HistoryRequest.hpp"
#include "UserApplication.h"

namespace PhoneApplication {
    inline std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }


    inline std::optional<InsertResponse> PhoneAdd(DbContext &dbContext, const crow::json::rvalue &json,
                                                  const std::string &token) {
        if (!json.has("username")) {
            return std::nullopt;
        }
        std::string username = json["username"].s();

        if (!UserApplication::isTokenValidForUser(dbContext, token, username)) {
            return std::nullopt;
        }

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

    inline DeleteResponse DeletePhone(DbContext &dbContext, const crow::json::rvalue &json, const std::string &token) {
        DeleteResponse response{};

        if (!json.has("username")) {
            response.success = false;
            return response;
        }
        std::string username = json["username"].s();

        if (!UserApplication::isTokenValidForUser(dbContext, token, username)) {
            response.success = false;
            return response;
        }

        if (!json.has("id")) {
            response.success = false;
            return response;
        }
        int id = json["id"].i();

        bool result = PhoneService::DeleteById(id, dbContext);
        response.success = result;
        return response;
    }

    inline UpdateResponse PhoneUpdate(DbContext &dbContext, const crow::json::rvalue &json, const std::string &token) {
        UpdateResponse response{};

        if (!json.has("username")) {
            response.success = false;
            return response;
        }
        std::string username = json["username"].s();

        if (!UserApplication::isTokenValidForUser(dbContext, token, username)) {
            response.success = false;
            return response;
        }
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

    inline crow::json::wvalue FilterPhonesJson(DbContext &dbContext, const crow::json::rvalue &json,
                                               const std::string &token) {
        crow::json::wvalue emptyResult;

        if (!json.has("username")) {
            return emptyResult;
        }
        std::string username = json["username"].s();

        if (!UserApplication::isTokenValidForUser(dbContext, token, username)) {
            return emptyResult;
        }

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

    inline CallResponse MakeCall(DbContext &dbContext, const crow::json::rvalue &json, const std::string &token) {
        CallResponse response{};
        try {
            if (!json.has("username")) {
                response.success = false;
                return response;
            }
            std::string username = json["username"].s();

            if (!UserApplication::isTokenValidForUser(dbContext, token, username)) {
                response.success = false;
                return response;
            }

            if (!json.has("pnumber")) {
                response.success = false;
                return response;
            }

            std::string pnumber = json["pnumber"].s();

            bool isRegistered = PhoneService::isPhoneExist(pnumber, dbContext);

            if (isRegistered) {
                try {
                    HistoryService::add(pnumber, dbContext);
                } catch (const std::exception &e) {
                    std::cerr << "[MakeCall] HistoryService::add hata: " << e.what() << std::endl;
                    response.success = false;
                    return response;
                }
            }

            response.success = isRegistered;
            return response;
        } catch (const std::exception &ex) {
            std::cerr << "[MakeCall] Genel hata: " << ex.what() << std::endl;
            response.success = false;
            return response;
        }
    }

    inline crow::json::wvalue ShowHistory(DbContext &dbContext, const crow::json::rvalue &json,
                                          const std::string &token) {
        crow::json::wvalue emptyResult;

        if (!json.has("username")) {
            return emptyResult;
        }
        std::string username = json["username"].s();

        if (!UserApplication::isTokenValidForUser(dbContext, token, username)) {
            return emptyResult;
        }

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
} // namespace PhoneApplication

#endif // PHONEAPPLICATION_HPP
