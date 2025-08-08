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

        crow::SimpleApp app; // Crow uygulamas�n� ba�lat�r (web server gibi �al���r)

        std::string connStr = "Driver={ODBC Driver 17 for SQL Server};Server=ENESGOK;Database=rehberuygulamasi;Trusted_Connection=yes;";
        DbContext dbContext(connStr); // Veritaban� ba�lant�s� kurulur

        // ActionFunc: JSON al�p crow::response d�nd�ren fonksiyon tipi
        // actionMap: "add", "delete" gibi string action'lara kar��l�k gelen i�lemleri tutar
        std::unordered_map<std::string, std::function<crow::response(const crow::json::rvalue &)>> actionMap;
        ActionmapsMain(actionMap, dbContext); // actionMap'leri y�kler

        // Tek bir endpoint tan�mlan�r ("/")
        CROW_ROUTE(app, "/").methods(crow::HTTPMethod::Post)(
            [&actionMap](const crow::request &req) -> crow::response
            {
                std::string action;

                // 1. �ncelikle header'da 'action' var m� kontrol et
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
                        return crow::response(400, "Ge�ersiz veya Eksik 'action' alan�");
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

        app.port(18080).multithreaded().run(); // Sunucuyu ba�lat

        return 0;
    }
