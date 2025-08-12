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

int main() {
    setlocale(LC_ALL, "Turkish");

    crow::SimpleApp app;


    std::unordered_map<std::string, std::function<crow::response(const crow::json::rvalue &, DbContext &)> > actionMap;

    try {
        ActionmapsMain(actionMap);
    } catch (const std::exception &ex) {
        std::cerr << "[main] ActionmapsMain hata: " << ex.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "[main] ActionmapsMain bilinmeyen hata" << std::endl;
        return 1;
    }

    CROW_ROUTE(app, "/").methods(crow::HTTPMethod::Post)(
        [&actionMap](const crow::request &req) -> crow::response {
            try {
                auto jsonBodyR = crow::json::load(req.body);
                if (!jsonBodyR)
                    return crow::response(400, "Geçersiz JSON");

                std::string action;
                std::string token;
                std::string username; // Burada username değişkeni tanımlandı

                // action, token ve username header'dan veya body'den alınır
                auto headerActionIt = req.headers.find("action");
                if (headerActionIt != req.headers.end())
                    action = headerActionIt->second;
                if (action.empty() && jsonBodyR.has("action"))
                    action = jsonBodyR["action"].s();

                auto headerTokenIt = req.headers.find("token");
                if (headerTokenIt != req.headers.end())
                    token = headerTokenIt->second;
                if (token.empty() && jsonBodyR.has("token"))
                    token = jsonBodyR["token"].s();

                auto headerUsernameIt = req.headers.find("username"); // username header'dan kontrol
                if (headerUsernameIt != req.headers.end())
                    username = headerUsernameIt->second;
                if (username.empty() && jsonBodyR.has("username")) // yoksa body'den al
                    username = jsonBodyR["username"].s();

                if (action.empty())
                    return crow::response(400, "Geçersiz veya Eksik 'action' alanı");

                std::string connStr =
                        "Driver={ODBC Driver 17 for SQL Server};Server=ENESGOK;Database=rehberuygulamasi;Trusted_Connection=yes;";
                DbContext dbContext(connStr);


                // login işlemi değilse token ve username kontrolü yap
                if (action != "login") {
                    std::cerr << "[DEBUG] Gelen token: '" << token << "'\n";
                    std::cerr << "[DEBUG] Gelen username: '" << username << "'\n";

                    if (token.empty() || username.empty() || !UserApplication::isTokenValidForUser(
                            dbContext, token, username)) {
                        std::cerr << "[DEBUG] Token doğrulama başarısız!\n";
                        return crow::response(401, "Geçersiz veya eksik token veya kullanıcı");
                    }
                }

                return actionMap.at(action)(jsonBodyR, dbContext);
            } catch (const std::out_of_range &) {
                std::cerr << "[main] Bilinmeyen action: " << std::endl;
                return crow::response(400, "Bilinmeyen action");
            }
            catch (const std::exception &ex) {
                std::cerr << "[main] Exception: " << ex.what() << std::endl;
                return crow::response(500, "Sunucu hatası");
            }
            catch (...) {
                std::cerr << "[main] Bilinmeyen hata!" << std::endl;
                return crow::response(500, "Sunucu hatası");
            }
        });

    app.port(18080).multithreaded().run();

    return 0;
}
