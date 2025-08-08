cmake_minimum_required(VERSION 3.10)

project(PhoneApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include dizinleri
include_directories(
        ${PROJECT_SOURCE_DIR}/asio/include
        ${PROJECT_SOURCE_DIR}/crow/include
        ${PROJECT_SOURCE_DIR}/infrastructure
        ${PROJECT_SOURCE_DIR}/application
        ${PROJECT_SOURCE_DIR}/domain
        ${PROJECT_SOURCE_DIR}/DTO
        ${PROJECT_SOURCE_DIR}
)

# Kaynak dosyalar - proje klas�r yap�s�na g�re d�zenle
set(SOURCES
        main.cpp
        domain/admin/User.hpp
        domain/admin/UserFactory.hpp
        domain/admin/UserRepository.hpp
        domain/admin/UserService.hpp
        domain/phone/wrapper.h
        token.hpp
        # domain ve DTO i�indeki .cpp dosyalar�n� da ekle, varsa
)

add_executable(PhoneApp ${SOURCES})

# Windows k�t�phane linkleri
target_link_libraries(PhoneApp
        odbc32
        ws2_32
        mswsock
)