#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <random>

inline std::string generateToken() {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string token;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, sizeof(alphanum) - 2);

    for (int i = 0; i < 32; ++i) {
        token += alphanum[dist(gen)];
    }
    return token;
}

#endif // TOKEN_HPP