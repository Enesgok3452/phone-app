#ifndef PHONESERVICE_HPP
#define PHONESERVICE_HPP

#include <algorithm>
#include <string>
#include <vector>
#include <optional>
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

#include "../infrastructure/Db.hpp"
#include "../domain/phone/Phone.hpp"
#include "../domain/phone/PhoneFactory.hpp"
#include "../domain/phone/PhoneRepository.hpp"
#include "../domain/History/HistoryService.hpp"  // HistoryService �a�r�s� i�in

namespace PhoneService {
    inline bool isValidPhoneNumber(const std::string &phone) {
        size_t len = phone.length();
        if (!(len == 3 || len == 5 || len == 7 || len == 11))
            return false;

        return std::all_of(phone.begin(), phone.end(), [](char c) {
            return std::isdigit(static_cast<unsigned char>(c));
        });
    }

    inline void AddPhone(const std::string &name, const std::string &sname, const std::string &pnumber,
                         DbContext &dbContext) {
        Phone p = PhoneFactory::generatePhone(name, sname, pnumber);
        PhoneRepository::addPhone(p, dbContext);
    }

    inline bool DeleteById(int id, DbContext &dbContext) {
        auto maybePhone = PhoneRepository::findPhoneById(id, dbContext);
        if (maybePhone.has_value()) {
            PhoneRepository::deletePhoneById(id, dbContext);
            return true;
        } else {
            return false;
        }
    }

    inline bool UpdatePhone(DbContext &dbContext, const int &id, const std::string &name, const std::string &sname,
                            const std::string &pnumber) {
        auto maybePhone = PhoneRepository::findPhoneById(id, dbContext);

        if (!maybePhone.has_value()) {
            return false;
        }

        if (!isValidPhoneNumber(pnumber)) {
            return false;
        }

        Phone updated = PhoneFactory::generatePhone(name, sname, pnumber);
        updated.setId(id);

        PhoneRepository::updatePhone(updated, dbContext);
        return true;
    }

    inline std::vector<Phone> FilterPhonesByName(DbContext &dbContext, const std::string &filterName) {
        return PhoneRepository::FilterPhonesByName(filterName, dbContext);
    }

    inline bool isPhoneExist(const std::string &pnumber, DbContext &dbContext) {
        return PhoneRepository::isPhoneRegistered(pnumber, dbContext);
    }

    inline std::optional<Phone> findPhoneById(int id, DbContext &dbContext) {
        return PhoneRepository::findPhoneById(id, dbContext);
    }

    // Arama i�lemi: sadece numara varsa HistoryService �zerinden ekle
    inline bool MakeCall(const std::string &pnumber, DbContext &dbContext) {
        bool exists = isPhoneExist(pnumber, dbContext);
        if (exists) {
            HistoryService::add(pnumber, dbContext); // HistoryService::add fonksiyonunun signature's� bu olmal�
            return true;
        }
        return false;
    }
}

#endif // PHONESERVICE_HPP
