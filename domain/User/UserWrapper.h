#ifndef WRAPPER_H
#define WRAPPER_H

#include <sql.h>
#include <sqlext.h>
#include <string>
#include <stdexcept>
#include <iostream>

// Satır verisini temsil eden basit sınıf
class Row {
    SQLHSTMT stmt_;

public:
    explicit Row(SQLHSTMT stmt) : stmt_(stmt) {
    }

    int getInt(int col) {
        SQLINTEGER val;
        SQLLEN ind;
        SQLRETURN ret = SQLGetData(stmt_, col, SQL_C_SLONG, &val, 0, &ind);
        if (!SQL_SUCCEEDED(ret) || ind == SQL_NULL_DATA) throw std::runtime_error("Invalid int data");
        return val;
    }

    std::string getString(int col) {
        char buffer[512];
        SQLLEN ind;
        SQLRETURN ret = SQLGetData(stmt_, col, SQL_C_CHAR, buffer, sizeof(buffer), &ind);
        if (!SQL_SUCCEEDED(ret) || ind == SQL_NULL_DATA) return "";
        return std::string(buffer);
    }
};

// ResultSet sınıfı: sorgu sonucu üzerinde iterasyon sağlar
class ResultSet {
    SQLHSTMT stmt_;

public:
    explicit ResultSet(SQLHSTMT stmt) : stmt_(stmt) {
    }

    bool fetch() {
        SQLRETURN ret = SQLFetch(stmt_);
        if (ret == SQL_NO_DATA) return false;
        if (!SQL_SUCCEEDED(ret)) throw std::runtime_error("Fetch failed");
        return true;
    }

    Row row() {
        return Row(stmt_);
    }
};

// Statement sınıfı: sorgu hazırlama, parametre bağlama ve çalıştırma
class Statement {
    SQLHSTMT stmt_;
    SQLHDBC dbc_;

public:
    explicit Statement(SQLHDBC dbc) : dbc_(dbc), stmt_(nullptr) {
        SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc_, &stmt_);
        if (!SQL_SUCCEEDED(ret)) throw std::runtime_error("Failed to allocate statement handle");
    }

    ~Statement() {
        if (stmt_ != nullptr) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
        }
    }

    void prepare(const std::string &query) {
        SQLRETURN ret = SQLPrepare(stmt_, (SQLCHAR *) query.c_str(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret)) throw std::runtime_error("Prepare failed");
    }

    void bind(int paramIndex, int val) {
        SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT) paramIndex, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0,
                                         0, &val, 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) throw std::runtime_error("Bind int param failed");
    }

    void bind(int paramIndex, const std::string &val) {
        SQLRETURN ret = SQLBindParameter(stmt_, (SQLUSMALLINT) paramIndex, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                                         val.size(), 0, (SQLPOINTER) val.c_str(), 0, nullptr);
        if (!SQL_SUCCEEDED(ret)) throw std::runtime_error("Bind string param failed");
    }

    ResultSet execute() {
        SQLRETURN ret = SQLExecute(stmt_);
        if (!SQL_SUCCEEDED(ret)) throw std::runtime_error("Execute failed");
        return ResultSet(stmt_);
    }

    SQLHSTMT getStmt() {
        return stmt_;
    }
};

#endif // WRAPPER_H
