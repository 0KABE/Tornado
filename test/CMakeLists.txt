set(LIB_NAME Tornado_UTest)

file(GLOB_RECURSE ${LIB_NAME}_SRCS *.cc)

add_executable(${LIB_NAME}
        main.cc
        )

target_link_libraries(
        ${LIB_NAME}
        PUBLIC
        Tornado
        GTest::gtest
        spdlog::spdlog
        CLI11::CLI11
)

target_include_directories(${LIB_NAME}
        PRIVATE
        ${PROJECT_SOURCE_DIR}/src
        )

target_compile_options(${LIB_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_sources(${LIB_NAME}
        PRIVATE ${${LIB_NAME}_SRCS})

target_compile_features(${LIB_NAME} PUBLIC cxx_std_20)

target_compile_definitions(${LIB_NAME} PRIVATE
        UT_BIN_NAME=\"${LIB_NAME}\"
        UT_BIN_PATH=\"${CMAKE_CURRENT_BINARY_DIR}\"
        )

include(GoogleTest)
enable_testing()
gtest_discover_tests(${LIB_NAME})