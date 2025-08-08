#ifndef DBCONTEXT_HPP
#define DBCONTEXT_HPP

#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>
#include <memory>
#include "../domain/phone/wrapper.h"  // Statement sınıfını içeren dosya

class DbContext {
public:
    explicit DbContext(const std::string &connStr)
        : connectionString(connStr) {
        initializeEnvironment();
    }

    ~DbContext() {
        cleanup();
    }

    // Bağlantı havuzundan kullanılabilir bağlantı al
    SQLHDBC getConnection() {
        std::lock_guard<std::mutex> lock(mutex);

        for (size_t i = 0; i < pool.size(); ++i) {
            if (!used[i]) {
                used[i] = true;
                return pool[i];
            }
        }

        SQLHDBC newConn = createConnection();
        pool.push_back(newConn);
        used.push_back(true);

        return newConn;
    }

    // Bağlantıyı havuza geri bırakır
    void releaseConnection(SQLHDBC conn) {
        std::lock_guard<std::mutex> lock(mutex);

        for (size_t i = 0; i < pool.size(); ++i) {
            if (pool[i] == conn) {
                used[i] = false;
                return;
            }
        }

        // Havuzda yoksa bağlantıyı kapat
        SQLDisconnect(conn);
        SQLFreeHandle(SQL_HANDLE_DBC, conn);
    }

    // Yeni bir Statement oluşturur ve hazırlar
    std::unique_ptr<Statement> createStatement(const std::string &query) {
        SQLHDBC conn = getConnection();
        auto stmt = std::make_unique<Statement>(conn);
        stmt->prepare(query);
        return stmt;
    }

    // Ham statement handle almak için (gerekirse)
    SQLHSTMT getStatement() {
        SQLHDBC conn = getConnection();
        SQLHSTMT hStmt = nullptr;
        SQLAllocHandle(SQL_HANDLE_STMT, conn, &hStmt);
        return hStmt;
    }

private:
    void initializeEnvironment() {
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
        if (!SQL_SUCCEEDED(ret))
            throw std::runtime_error("ODBC ortamı oluşturulamadı!");

        ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (!SQL_SUCCEEDED(ret))
            throw std::runtime_error("ODBC sürüm bilgisi ayarlanamadı!");
    }

    SQLHDBC createConnection() {
        SQLHDBC conn;
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &conn);
        if (!SQL_SUCCEEDED(ret))
            throw std::runtime_error("ODBC bağlantı handle'ı oluşturulamadı!");

        ret = SQLDriverConnect(conn, NULL, (SQLCHAR *)connectionString.c_str(),
                               SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
        if (!SQL_SUCCEEDED(ret))
            throw std::runtime_error("Veritabanına bağlanılamadı!");

        return conn;
    }

    void cleanup() {
        for (SQLHDBC conn : pool) {
            SQLDisconnect(conn);
            SQLFreeHandle(SQL_HANDLE_DBC, conn);
        }
        pool.clear();
        used.clear();

        if (env) {
            SQLFreeHandle(SQL_HANDLE_ENV, env);
            env = nullptr;
        }
    }

private:
    std::string connectionString;
    SQLHENV env = nullptr;

    std::vector<SQLHDBC> pool;
    std::vector<bool> used;
    std::mutex mutex;
};

#endif // DBCONTEXT_HPP
