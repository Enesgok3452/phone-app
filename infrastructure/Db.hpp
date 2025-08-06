#ifndef DBCONTEXT_HPP
#define DBCONTEXT_HPP

#include <iostream>
#include <vector>
#include <mutex>
#include <string>
#include <sql.h>
#include <sqlext.h>
#include <stdexcept>

class DbContext
{
public:
    SQLHSTMT getStatement()
    {
        SQLHDBC conn = getConnection(); // Baðlantýyý al
        SQLHSTMT hStmt;
        SQLAllocHandle(SQL_HANDLE_STMT, conn, &hStmt);
        return hStmt;
    }

    // Artýk sade ce connection string alýyor
    DbContext(const std::string &connStr)
        : connectionString(connStr)
    {
        initializeEnvironment();
    }

    ~DbContext()
    {
        cleanup();
    }

    SQLHDBC getConnection()
    {
        std::lock_guard<std::mutex> lock(mutex);

        for (size_t i = 0; i < pool.size(); ++i)
        {
            if (!used[i])
            {
                used[i] = true;

                return pool[i];
            }
        }

        // Yeni baðlantý oluþtur (sýnýrsýz havuz)
        SQLHDBC newConn = createConnection();
        pool.push_back(newConn);
        used.push_back(true);

        return newConn;
    }

    void releaseConnection(SQLHDBC conn)
    {
        std::lock_guard<std::mutex> lock(mutex);

        for (size_t i = 0; i < pool.size(); ++i)
        {
            if (pool[i] == conn)
            {
                used[i] = false;

                return;
            }
        }

        // Havuzda yoksa kapat
        SQLDisconnect(conn);
        SQLFreeHandle(SQL_HANDLE_DBC, conn);
    }

private:
    void initializeEnvironment()
    {
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
        if (!SQL_SUCCEEDED(ret))
            throw std::runtime_error("ODBC ortamý oluþturulamadý!");

        ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
        if (!SQL_SUCCEEDED(ret))
            throw std::runtime_error("ODBC sürüm bilgisi ayarlanamadý!");
    }

    SQLHDBC createConnection()
    {
        SQLHDBC conn;
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &conn);
        if (!SQL_SUCCEEDED(ret))
            throw std::runtime_error("ODBC baðlantý handle'ý oluþturulamadý!");

        ret = SQLDriverConnect(conn, NULL, (SQLCHAR *)connectionString.c_str(),
                               SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
        if (!SQL_SUCCEEDED(ret))
            throw std::runtime_error("Veritabanýna baðlanýlamadý!");

        return conn;
    }

    void cleanup()
    {
        for (SQLHDBC conn : pool)
        {
            SQLDisconnect(conn);
            SQLFreeHandle(SQL_HANDLE_DBC, conn);
        }
        pool.clear();
        used.clear();

        if (env)
        {
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
