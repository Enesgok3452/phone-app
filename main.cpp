#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <optional>

#include "asio/include/asio.hpp"
#include "crow/include/crow/crow_all.h"

#include "../infrastructure/Db.hpp"
#include "../application/PhoneApplication.hpp"
#include "ActionsMain.hpp"

#include <locale.h>

int main()
{
    setlocale(LC_ALL, "Turkish");

    crow::SimpleApp app; // Crow uygulamasýný baþlatýr (web server gibi çalýþýr)

    std::string connStr = "Driver={ODBC Driver 17 for SQL Server};Server=ENESGOK;Database=rehberuygulamasi;Trusted_Connection=yes;";
    DbContext dbContext(connStr); // Veritabaný baðlantýsý kurulur

    // ActionFunc: JSON alýp crow::response döndüren fonksiyon tipi
    // actionMap: "add", "delete" gibi string action'lara karþýlýk gelen iþlemleri tutar
    std::unordered_map<std::string, std::function<crow::response(const crow::json::rvalue &)>> actionMap;
    ActionmapsMain(actionMap, dbContext); // actionMap'leri yükler

    // Tek bir endpoint tanýmlanýr ("/")
    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::Post)(
        [&actionMap](const crow::request &req) -> crow::response
        {
            std::string action;

            // 1. Öncelikle header'da 'action' var mý kontrol et
            auto headerIt = req.headers.find("action");
            if (headerIt != req.headers.end())
            {
                action = headerIt->second;
            }
            else
            {
                // 2. Header'da yoksa body'den json olarak al
                const auto json = crow::json::load(req.body);
                if (!json || !json.has("action"))
                {
                    return crow::response(400, "Geçersiz veya Eksik 'action' alaný");
                }
                action = json["action"].s();
            }

            try
            {
                return actionMap.at(action)(crow::json::load(req.body));
            }
            catch (const std::out_of_range &)
            {
                return crow::response(400, "Bilinmeyen action");
            }
        });

    app.port(18080).multithreaded().run(); // Sunucuyu baþlat

    return 0;
}
