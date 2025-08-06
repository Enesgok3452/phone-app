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
)

# Kaynak dosyalar - proje klasör yapýsýna göre düzenle
set(SOURCES
        main.cpp
        # domain ve DTO içindeki .cpp dosyalarýný da ekle, varsa
)

add_executable(PhoneApp ${SOURCES})

# Windows kütüphane linkleri
target_link_libraries(PhoneApp
        odbc32
        ws2_32
        mswsock
)