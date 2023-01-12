set(LIBRARY_NAME Tornado)

add_library(${LIBRARY_NAME}
        base.cc
        args_separator.cc
        )

target_compile_features(${LIBRARY_NAME} PUBLIC cxx_std_20)

target_include_directories(${LIBRARY_NAME}
        PRIVATE ${PROJECT_SOURCE_DIR}/include/tornado
        PUBLIC ${PROJECT_SOURCE_DIR}/include
        )

target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        spdlog::spdlog
        asio::asio
        SRS
        )
