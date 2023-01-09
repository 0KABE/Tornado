set(LIB_NAME Tornado)

add_library(${LIB_NAME}
        base.cc
        )

target_compile_features(${LIB_NAME} PUBLIC cxx_std_20)

target_include_directories(${LIB_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/include/tornado
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        )

target_link_libraries(${LIB_NAME}
        PUBLIC
        spdlog::spdlog
        asio::asio
        )
