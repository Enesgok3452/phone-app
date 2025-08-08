#ifndef WRAPPER_HPP
#define WRAPPER_HPP


#include <sqlext.h>
#include <stdexcept>
#include <string>

// Tek bir satırdaki veri için sarıcı (row wrapper)
class ResultRow {
    SQLHSTMT stmt;

public:
    explicit ResultRow(SQLHSTMT s) : stmt(s) {
    }

    // Bir sonraki satıra geçer, veri varsa true döner
    bool fetch() {
        SQLRETURN ret = SQLFetch(stmt);
        return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
    }

    // Sütun indeksine göre int değer alır
    int getInt(int colIndex) const {
        int value = 0;
        SQLLEN indicator = 0;
        if (SQLGetData(stmt, colIndex, SQL_C_SLONG, &value, sizeof(value), &indicator) != SQL_SUCCESS)
            throw std::runtime_error("SQLGetData failed for int");
        if (indicator == SQL_NULL_DATA)
            throw std::runtime_error("NULL int value");
        return value;
    }

    // Sütun indeksine göre string değer alır
    std::string getString(int colIndex) const {
        char buffer[512] = {0};
        SQLLEN indicator = 0;
        if (SQLGetData(stmt, colIndex, SQL_C_CHAR, buffer, sizeof(buffer), &indicator) != SQL_SUCCESS)
            throw std::runtime_error("SQLGetData failed for string");
        if (indicator == SQL_NULL_DATA)
            return "";
        return std::string(buffer);
    }
};

// Tüm sonuç kümesini sarmalar
class ResultSet {
    SQLHSTMT stmt;

public:
    explicit ResultSet(SQLHSTMT s) : stmt(s) {
    }

    // ResultRow nesnesi oluşturur (aktif satır)
    ResultRow row() {
        return ResultRow(stmt);
    }

    // ResultSet kendi içinde fetch yapmamalı, fetch işini ResultRow yapar.
    // Bu yüzden bu fetch fonksiyonunu kaldırdım veya kullanımı önerilmez.

    bool fetch() {
        return SQLFetch(stmt) == SQL_SUCCESS || SQLFetch(stmt) == SQL_SUCCESS_WITH_INFO;
    }
};

// SQL sorgusu için sarmalayıcı
class Statement {
    SQLHSTMT stmt = nullptr;
    SQLHDBC conn = nullptr;

    void check(SQLRETURN ret, const char *msg) {
        if (!SQL_SUCCEEDED(ret)) {
            throw std::runtime_error(msg);
        }
    }

public:
    explicit Statement(SQLHDBC connection) : conn(connection) {
        check(SQLAllocHandle(SQL_HANDLE_STMT, conn, &stmt), "SQLAllocHandle failed");
    }

    ~Statement() {
        if (stmt) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            stmt = nullptr;
        }
    }

    void prepare(const std::string &sql) {
        check(SQLPrepare(stmt, (SQLCHAR *) sql.c_str(), SQL_NTS), "SQLPrepare failed");
    }

    void bind(int index, const std::string &val) {
        check(SQLBindParameter(stmt, index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
                               (SQLULEN) val.size(), 0, (SQLPOINTER) val.c_str(), 0, nullptr),
              "SQLBindParameter failed for string");
    }

    void bind(int index, int val) {
        check(SQLBindParameter(stmt, index, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
                               0, 0, (SQLPOINTER) &val, 0, nullptr), "SQLBindParameter failed for int");
    }

    // Sorguyu çalıştırır ve ResultSet döner
    ResultSet execute() {
        check(SQLExecute(stmt), "SQLExecute failed");
        return ResultSet(stmt);
    }

    SQLHSTMT getStmt() const {
        return stmt;
    }
};

#endif // WRAPPER_HPP
