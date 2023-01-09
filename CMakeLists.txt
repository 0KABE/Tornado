cmake_minimum_required(VERSION 3.18)
project(Tornado)

cmake_policy(SET CMP0091 NEW)

#
# Install dependency by conan
#
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Conan Home Path
if (DEFINED ENV{CONAN_USER_HOME})
    message("Conan Home: $ENV{CONAN_USER_HOME}")
else ()
    message("Conan Home: Default conan home")
endif ()

include(cmake/conan.cmake)
conan_cmake_configure(
        REQUIRES
        gtest/1.10.0
        spdlog/1.9.2
        cli11/2.2.0
        asio/1.24.0

        GENERATORS
        cmake_find_package
)
conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
        BUILD missing
        REMOTE conancenter
        SETTINGS ${settings}
        )

find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)
find_package(CLI11 REQUIRED)
find_package(asio REQUIRED)

add_subdirectory(src)
add_subdirectory(test)