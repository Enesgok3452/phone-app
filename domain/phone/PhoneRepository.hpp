#ifndef PHONEREPOSITORY_HPP
#define PHONEREPOSITORY_HPP

#include <optional>
#include <vector>
#include <string>
#include <algorithm>
#include "Phone.hpp"
#include "PhoneFactory.hpp"
#include "../infrastructure/Db.hpp"
#include "Wrapper.h" // Wrapper içindeki Statement ve ResultSet sınıfları

namespace PhoneRepository {
    inline std::vector<Phone> fetchAll(ResultSet &rs) {
        std::vector<Phone> results;

        while (rs.fetch()) {
            auto row = rs.row();
            int id = row.getInt(1);
            std::string name = row.getString(2);
            std::string surname = row.getString(3);
            std::string phone = row.getString(4);
            results.emplace_back(PhoneFactory::generateFromDb(id, name.c_str(), surname.c_str(), phone.c_str()));
        }
        return results;
    }

    inline void addPhone(const Phone &p, DbContext &dbContext) {
        Statement stmt(dbContext.getConnection());
        stmt.prepare("INSERT INTO Directory (name, surname, phonenumber) VALUES (?, ?, ?)");
        stmt.bind(1, p.getName());
        stmt.bind(2, p.getSname());
        stmt.bind(3, p.getPnumber());
        stmt.execute();
    }

    inline void deletePhoneById(int id, DbContext &dbContext) {
        Statement stmt(dbContext.getConnection());
        stmt.prepare("DELETE FROM Directory WHERE id = ?");
        stmt.bind(1, id);
        stmt.execute();
    }

    inline void updatePhone(const Phone &p, DbContext &dbContext) {
        Statement stmt(dbContext.getConnection());
        stmt.prepare("UPDATE Directory SET name = ?, surname = ?, phonenumber = ? WHERE id = ?");
        stmt.bind(1, p.getName());
        stmt.bind(2, p.getSname());
        stmt.bind(3, p.getPnumber());
        stmt.bind(4, p.getId().value());
        stmt.execute();
    }

    inline std::optional<Phone> findPhoneById(int id, DbContext &dbContext) {
        Statement stmt(dbContext.getConnection());
        stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory WHERE id = ?");
        stmt.bind(1, id);

        ResultSet rs = stmt.execute();

        if (rs.fetch()) {
            auto row = rs.row();
            int fetchedId = row.getInt(1);
            std::string name = row.getString(2);
            std::string surname = row.getString(3);
            std::string phone = row.getString(4);

            return PhoneFactory::generateFromDb(fetchedId, name.c_str(), surname.c_str(), phone.c_str());
        }

        return std::nullopt;
    }

    inline std::optional<std::vector<Phone>>findAllPhones(DbContext &dbContext) {
        Statement stmt(dbContext.getConnection());
        stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory");
        stmt.execute();

        ResultSet rs(stmt.getStmt());
        std::vector<Phone> phones;

        while (rs.fetch()) {
            auto row = rs.row();
            int id = row.getInt(1);
            std::string name = row.getString(2);
            std::string surname = row.getString(3);
            std::string phone = row.getString(4);

            phones.emplace_back(PhoneFactory::generateFromDb(id, name.c_str(), surname.c_str(), phone.c_str()));

        }

        if (phones.empty()) {
            return std::nullopt;
        }

        return phones;
    }


    inline bool isPhoneRegistered(const std::string &phonenumber, DbContext &dbContext) {
        Statement stmt(dbContext.getConnection());
        stmt.prepare("SELECT 1 FROM Directory WHERE phonenumber = ?");
        stmt.bind(1, phonenumber);
        auto result = stmt.execute();
        auto row = result.row();
        return row.fetch(); // Satır varsa telefon kayıtlı demektir
    }

    inline std::vector<Phone> FilterPhonesByName(const std::string &filterName, DbContext &dbContext) {
        Statement stmt(dbContext.getConnection());

        if (!filterName.empty()) {
            stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory WHERE LOWER(name) LIKE ? "
                "ORDER BY name COLLATE Turkish_CI_AS, surname COLLATE Turkish_CI_AS");
            std::string pattern = filterName + "%";
            std::transform(pattern.begin(), pattern.end(), pattern.begin(), ::tolower);
            stmt.bind(1, pattern);
        } else {
            stmt.prepare("SELECT id, name, surname, phonenumber FROM Directory "
                "ORDER BY name COLLATE Turkish_CI_AS, surname COLLATE Turkish_CI_AS");
        }

        stmt.execute();

        ResultSet rs(stmt.getStmt());
        return fetchAll(rs);
    }
}

#endif // PHONEREPOSITORY_HPP
