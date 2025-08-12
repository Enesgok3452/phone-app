#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <random>
#include <string>

    inline std::string generateRandomToken(int length = 16) {
    constexpr char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string token;
    token.resize(length);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);

    for (int i = 0; i < length; i++) {
        token[i] = charset[dist(gen)];
    }
    return token;
}


#endif // TOKEN_HPP
