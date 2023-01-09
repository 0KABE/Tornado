//
// Created by Chen, WenTao on 2023/1/10.
//
#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <set>
#include <vector>

struct CLIOptions {
    std::string log_level;
};

void InitCLI(CLI::App &app, CLIOptions &cli_options) {
    std::set<std::string> levels;
    {
        std::set<spdlog::string_view_t> spdlog_levels(SPDLOG_LEVEL_NAMES);
        std::transform(
                spdlog_levels.begin(), spdlog_levels.end(), std::inserter(levels, levels.begin()),
                [](spdlog::string_view_t level) { return std::string(level.data(), level.size()); });
    }

    app.add_option(
                    "-l,--log", cli_options.log_level,
                    fmt::format("Set log level in [{}]",
                                fmt::join(std::vector<spdlog::string_view_t>(SPDLOG_LEVEL_NAMES), ", ")))
            ->type_name("LEVEL")
            ->check(CLI::IsMember(levels))
            ->default_val("info");
}

int main(int argc, char **argv) {
    CLI::App app;
    CLIOptions cli_options;

    testing::InitGoogleTest(&argc, argv);

    InitCLI(app, cli_options);
    CLI11_PARSE(app, argc, argv)

    spdlog::set_level(spdlog::level::from_str(cli_options.log_level));
    spdlog::set_pattern("[%T.%e] [%L] [%t] %v");

    return RUN_ALL_TESTS();
}
