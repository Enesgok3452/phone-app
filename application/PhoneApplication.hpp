/*
- GETCURRENTDATET�ME (
S�STEM SAAT�N� ALIP STD::CHRONO VE STD::PUT_TIME KULLANARAK BUNU    Y-A-G-SAAT-DAK�KA-SAN�YE FORMATINA �EV�R�R.
STd::STR�NGSTREAM BUNU STR�NG E �EV�R�R.)

-   PHONEADD (
BU FONKS�YON JSON VER�S�NDEN �S�M SOY�S�M NUMARA ALIR. TELEFON NUMARASI GE�ERL�YSE VER�TABANINA KAYDEDER E�ER VE INSERTRESPONSE D�NER.
E�ER GE�ERS�ZSE BOL� (NULLOPT)  D�NER. 
VAL�DASYON YAPILDI = GE�ERL� M� GE�ERS�Z M� D�YE KONTROL ETME ��LEM�.)

DELETEPHONE (
JSON VER�S�NDEN ID Y� ALIR VE SERV�CE ARACILI�IYLA VER�TABANINDAN S�LER. ��LEM BA�ARILI MI BA�ARISIZ MI D�YE DELETERESPONSE NESNES� D�ND�R�R. )

NEDEN COUT OLARAK YAZMADIK ? = Web uygulamalar�nda istemciye veri g�ndermek i�in response kullan�l�r, 
konsola yazmak (cout) sadece geli�tiricinin kendi ekran� i�indir ve istemciye ula�maz.

PHONEUPDATE (
Bu fonksiyon, JSON�dan al�nan id, name, sname ve pnumber bilgileriyle veritaban�ndaki kayd� g�nceller; G�NCELLEME ��LEM�N�N SONUCUNU 
SUCCESS DE���KEN�NDE TUTAR , EE�ER G�NCELLEME BA�ARILIYSA UPDATERESPONSE NESNES�NE NAME , SNAME B�LG�LER� ATANIR. VE BA�ARI DURUMUNDA
SUCCESS = TRUE NESNES�NDE TUTULUR. FONSK�YON KULLANILDI�INDA G�NCELLEME ��LEM� YAPILDIYSA B�LG�LER UPDATERESPONSE OLARAK KAR�I TARAFA MESAJ D�NER.
)
*/




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

namespace PhoneApplication
{
    inline std::string getCurrentDateTime()
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
//register fonksiyonunda map'e kaydedecek ve key value su olacak. valuesu zaten lambda fonksiyonlar� olacak.
//run ad�nda bir �al��t�rmaa fonksiyonu olacak. map i �a��rd���nda gelecek.
    inline std::optional<InsertResponse> PhoneAdd(DbContext &dbContext, const crow::json::rvalue &json)
    {
        auto name = json["name"].s();
        auto sname = json["sname"].s();
        auto pnumber = json["pnumber"].s();

        if (!PhoneService::isValidPhoneNumber(pnumber))
        {
            return std::nullopt;
        }

        PhoneService::AddPhone(name, sname, pnumber, dbContext);

        return InsertResponse{name, sname};
    }

    inline DeleteResponse DeletePhone(DbContext &dbContext, const crow::json::rvalue &json)
    {
        int id = json["id"].i();

        DeleteResponse response;
        bool result = PhoneService::DeleteById(id, dbContext);
        response.success = result;
        return response;
    }

    inline UpdateResponse PhoneUpdate(DbContext &dbContext, const crow::json::rvalue &json)
    {
        int id = json["id"].i();
        auto name = json["name"].s();
        auto sname = json["sname"].s();
        auto pnumber = json["pnumber"].s();

        UpdateResponse response;
        bool success = PhoneService::UpdatePhone(dbContext, id, name, sname, pnumber);
        response.success = success;

        if (success)
        {
            response.name = name;
            response.sname = sname;
        }

        return response;
    }
    //crow json değilde dto nesnesi dön. crow onu çeviirebiliyor mu çeviremiyor mu bir kontrol et.
    //
    inline crow::json::wvalue FilterPhonesJson(DbContext &dbContext, const crow::json::rvalue &json)
    {
        GetPhoneListRequest request;
        request.filterName = json.has("filterName") ? std::string(json["filterName"].s()) : "";

        auto filteredPhones = PhoneService::FilterPhonesByName(dbContext, request.filterName);

        crow::json::wvalue resultJson;
        crow::json::wvalue phones_array = crow::json::wvalue::list();

        int index = 0;
        for (const auto &phone : filteredPhones)
        {
            crow::json::wvalue item;
            item["id"] = phone.getId().value_or(0);
            item["name"] = phone.getName();
            item["sname"] = phone.getSname();
            item["pnumber"] = phone.getPnumber();
            phones_array[index++] = std::move(item); // push_back yerine indeks ile atama
        }

        resultJson["phones"] = std::move(phones_array);
        return resultJson;
    }

    inline CallResponse MakeCall(DbContext &dbContext, const crow::json::rvalue &json)
    {
        CallResponse response;

        if (!json.has("pnumber"))
        {
            response.success = false;
            return response;
        }

        std::string pnumber = json["pnumber"].s();

        bool isRegistered = PhoneService::isPhoneExist(pnumber, dbContext);

        if (isRegistered)
        {
            HistoryService::add(pnumber, dbContext);
        }

        response.success = isRegistered;
        return response;
    }
    // Aynı filterphone işlemini burada da yap.

    inline crow::json::wvalue ShowHistory(DbContext &dbContext, const crow::json::rvalue &json)
    {
        HistoryRequest request;
        request.filterName = json.has("filterName") ? std::string(json["filterName"].s()) : "";

        auto histories = HistoryService::GetAll(dbContext, request.filterName);

        crow::json::wvalue histories_array = crow::json::wvalue::list();

        int index = 0;
        for (const auto &h : histories)
        {
            crow::json::wvalue item;
            item["callerName"] = h.getcallerName();
            item["dialedName"] = h.getdialedName();
            item["dialedTime"] = h.getdialedTime();
            histories_array[index++] = std::move(item); // push_back yerine indeks ile atama
        }

        crow::json::wvalue resultJson;
        resultJson["histories"] = std::move(histories_array);

        return resultJson;
    }

    inline std::optional<Phone> FindPhonesById(DbContext &dbContext, int id)
    {
        return PhoneService::findPhoneById(id, dbContext);
    }

}

#endif
