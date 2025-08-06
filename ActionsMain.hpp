/*
        *                          TEK ENDPOINT KULLANIMI:

        - KONTROL MERKEZÝNÝ TEKELE DÖNDÜRÜR.       ÇÜNKÜ DUVARA SIRTINIZI DAYADIÐINIZDA GELEN SALDIRI SADECE KARÞI TARAFAN OLACAKTIR.
        - GÜVENLÝK ARTAR ÇÜNKÜ BÝR SIRA DIÞA AÇILAN KAPÝYÝ KONTROL ETMEK ZORDUR.
        - HATA VE KARMAÞA AZALIR.
        - KEY-VALUE ÝLE BÝRLÝKTE YAPILDIÐINDA YENÝ BÝR FONKSÝYON EKLENMESÝ KOLAY.
        - BÝR ÞEY DEÐÝÞECEK ZAMAN SADECE O DEÐÝÞECEK YER ÜZERÝNDE YAPMAK YETERLÝ OLACAKTIR.
        - POST ÝLE JSON BODYSÝ ÝÇÝNDE KULLANILIR. POST AZ DA OLSA EKSTRA GÜVENLÝK SAÐLAR.
        - IF ELSE YAPISI KULLANMAK FAZLA METHODDA YARARSIZ OLACAKTIR. ÇÜNKÜ SÜREKLÝ BÝR KONTROL MEKANÝZMASI HER ÞEYÝ TEK TEK KONTROL EDECEK.
        - LAMBDA FONKSÝYONU KULLANILDI.
        - LAMBDA FONKSÝYONU ASLINDA ANONÝM ÝSÝMSÝZ BÝR FONKSÝYONDUR. FONKSÝYONU BÝR KEY’E ATARIZ.
        - 2. [...] (const crow::json::rvalue &json) { ... } , json parametresi alýr, bu POST isteðinden gelen JSON verisidir.

        ACTÝONFUNC ADINDA BÝR JSON ALAN VE CROW::RESPONSE DÖNDÜREN FONKSÝYON TÝPÝ ATADIK.
        STRING BÝR KEY VE ACTÝONFUNC’U BÝRLÝKTE TUTACAK DAHA SONRA ACTÝONDAN GELEN ÝSTEÐÝ EÞLEÞTÝRME YAPMASI ÝÇÝN ACTÝONMAP OLUÞTURDUK.

        IF ELSE YAPILARINI TEK TEK HER LAMBDA FONKSÝYONUNDA TUTMAK YERÝNE BÝR TANE METHOD OLUÞTURDUM VE METHODU TÜM YERE ÇAÐIRDIM.

        GENEL BÝR ACTÝONMAPMAIN ADINDA BÝR FONKSÝYON OLUÞTURDUK BU FONKSÝYONU MAIN DE ÇAÐIRACAÐIZ. AMAÇ MAIN’ÝN TEMÝZ VE SADE OLMASI.
        MAIN BU ÝÞLERLE UÐRAÞMAYACAK SADECE SERVER’I BAÞLATMAK, PORTU BELÝRLEMEK VE ACTÝONMAP’I ÇAÐIRMAK, ÝSTEÐÝ ALMAK.  SADECE GÝRÝÞ NOKTASI OLARAK KULLANMAK ÝSTEDÝK.

        ACTÝONMAP[""] LAMBDA FONKSÝYONUN JSON PARAMETRELERÝNÝ KULLANARAK ATANDIÐI YER.
        GEREKLÝ ÝÞLEMÝN FONKSÝYONUNU PHONEAPPLICATION::{FONKSÝYON} KATMANINDAN ÇEKÝYORUZ.

        */

#include <iostream>
#include <unordered_map>
#include <functional>
#include "application/PhoneApplication.hpp"
#include "infrastructure/Db.hpp"

using ActionFunc = std::function<crow::response(const crow::json::rvalue &)>;
// ActionFunc = Bir JSON alan ve crow::response döndüren fonksiyon tipi.

std::unordered_map<std::string, ActionFunc> actionMap;
// actionMap = String’e (örneðin "add", "delete") karþýlýk gelen ActionFunc fonksiyonlarýný tutar.

crow::response responseControl(bool success, const std::string &successMsg, const std::string &errorMsg) {
    return success ? crow::response(200, successMsg) : crow::response(400, errorMsg);
}

inline void ActionmapsMain(
    std::unordered_map<std::string, std::function<crow::response(const crow::json::rvalue &)> > & actionMap,
    DbContext & dbContext) {
    actionMap["add"] = [&](const crow::json::rvalue &json) {
        auto res = PhoneApplication::PhoneAdd(dbContext, json);

        return responseControl(res.has_value(), "Person Successfully Added.", "Invalid Phone Number");
    };

    // DELETE
    actionMap["delete"] = [&](const crow::json::rvalue &json) {
        auto res = PhoneApplication::DeletePhone(dbContext, json);
        return responseControl(res.success, "Person Successfully Deleted.", "Not Found Id.");
    };

    // UPDATE
    actionMap["update"] = [&](const crow::json::rvalue &json) {
        auto res = PhoneApplication::PhoneUpdate(dbContext, json);
        return responseControl(res.success, "Person Successfully Updated.", "Not Found Id.");
    };

    // LIST
    actionMap["list"] = [&](const crow::json::rvalue &json) {
        return crow::response(200, PhoneApplication::FilterPhonesJson(dbContext, json));
    };

    // CALL
    actionMap["call"] = [&](const crow::json::rvalue &json) {
        auto res = PhoneApplication::MakeCall(dbContext, json);

        return responseControl(res.success, "Call Started... \nCall Finished.", "Invalid Phone Number");
    };

    // HISTORY
    // crow kütüphanesindeki json verisini okumak için kullanýlýr. referansý json'un orjinalinden alýr.
    // deðiþtirilemezdir.
    // Eðer gelen action history ise, þu iþlemi yap; iþlemde JSON verisini parametre olarak kullan.
    actionMap["history"] = [&](const crow::json::rvalue &json) {
        auto resultJson = PhoneApplication::ShowHistory(dbContext, json);
        return crow::response(200, resultJson);
    };
}
