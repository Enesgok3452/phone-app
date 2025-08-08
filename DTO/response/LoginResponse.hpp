#ifndef LOGIN_RESPONSE_HPP
#define LOGIN_RESPONSE_HPP

#include <string>

struct LoginResponse {
    bool success = false;
    std::string message;  // opsiyonel, açıklama mesajı da tutabiliriz
    std::string token;
};

#endif // LOGIN_RESPONSE_HPP
