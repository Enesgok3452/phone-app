/*
        *                          TEK ENDPOINT KULLANIMI:

        - KONTROL MERKEZ�N� TEKELE D�ND�R�R.       ��NK� DUVARA SIRTINIZI DAYADI�INIZDA GELEN SALDIRI SADECE KAR�I TARAFAN OLACAKTIR.
        - G�VENL�K ARTAR ��NK� B�R SIRA DI�A A�ILAN KAP�Y� KONTROL ETMEK ZORDUR.
        - HATA VE KARMA�A AZALIR.
        - KEY-VALUE �LE B�RL�KTE YAPILDI�INDA YEN� B�R FONKS�YON EKLENMES� KOLAY.
        - B�R �EY DE���ECEK ZAMAN SADECE O DE���ECEK YER �ZER�NDE YAPMAK YETERL� OLACAKTIR.
        - POST �LE JSON BODYS� ���NDE KULLANILIR. POST AZ DA OLSA EKSTRA G�VENL�K SA�LAR.
        - IF ELSE YAPISI KULLANMAK FAZLA METHODDA YARARSIZ OLACAKTIR. ��NK� S�REKL� B�R KONTROL MEKAN�ZMASI HER �EY� TEK TEK KONTROL EDECEK.
        - LAMBDA FONKS�YONU KULLANILDI.
        - LAMBDA FONKS�YONU ASLINDA ANON�M �S�MS�Z B�R FONKS�YONDUR. FONKS�YONU B�R KEY�E ATARIZ.
        - 2. [...] (const crow::json::rvalue &json) { ... } , json parametresi al�r, bu POST iste�inden gelen JSON verisidir.

        ACT�ONFUNC ADINDA B�R JSON ALAN VE CROW::RESPONSE D�ND�REN FONKS�YON T�P� ATADIK.
        STRING B�R KEY VE ACT�ONFUNC�U B�RL�KTE TUTACAK DAHA SONRA ACT�ONDAN GELEN �STE�� E�LE�T�RME YAPMASI ���N ACT�ONMAP OLU�TURDUK.

        IF ELSE YAPILARINI TEK TEK HER LAMBDA FONKS�YONUNDA TUTMAK YER�NE B�R TANE METHOD OLU�TURDUM VE METHODU T�M YERE �A�IRDIM.

        GENEL B�R ACT�ONMAPMAIN ADINDA B�R FONKS�YON OLU�TURDUK BU FONKS�YONU MAIN DE �A�IRACA�IZ. AMA� MAIN��N TEM�Z VE SADE OLMASI.
        MAIN BU ��LERLE U�RA�MAYACAK SADECE SERVER�I BA�LATMAK, PORTU BEL�RLEMEK VE ACT�ONMAP�I �A�IRMAK, �STE�� ALMAK.  SADECE G�R�� NOKTASI OLARAK KULLANMAK �STED�K.

        ACT�ONMAP[""] LAMBDA FONKS�YONUN JSON PARAMETRELER�N� KULLANARAK ATANDI�I YER.
        GEREKL� ��LEM�N FONKS�YONUNU PHONEAPPLICATION::{FONKS�YON} KATMANINDAN �EK�YORUZ.

        */

#include <iostream>
#include <unordered_map>
#include <functional>
#include "application/PhoneApplication.hpp"
#include "infrastructure/Db.hpp"

using ActionFunc = std::function<crow::response(const crow::json::rvalue &)>;
// ActionFunc = Bir JSON alan ve crow::response d�nd�ren fonksiyon tipi.

std::unordered_map<std::string, ActionFunc> actionMap;
// actionMap = String�e (�rne�in "add", "delete") kar��l�k gelen ActionFunc fonksiyonlar�n� tutar.

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
    // crow k�t�phanesindeki json verisini okumak i�in kullan�l�r. referans� json'un orjinalinden al�r.
    // de�i�tirilemezdir.
    // E�er gelen action history ise, �u i�lemi yap; i�lemde JSON verisini parametre olarak kullan.
    actionMap["history"] = [&](const crow::json::rvalue &json) {
        auto resultJson = PhoneApplication::ShowHistory(dbContext, json);
        return crow::response(200, resultJson);
    };
}
