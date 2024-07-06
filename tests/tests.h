#pragma once

#include <atomic>
#include <iostream>
#include <format>

#include <spot.h>

static void handle_test_failure(const char* expr, const char* file, int line, std::string_view msg = "")
{
    std::cout << "test failed! (" <<expr << ") not satisfied!\n"
              << "file: " << file << ':' << line << '\n'
              << "msg: " << msg;
}

static std::filesystem::path get_sandbox_path()
{
    return std::filesystem::current_path();
}

static size_t count_matches(const std::filesystem::path& path, const std::string& regex, const spot::search_options& options)
{
    std::atomic<size_t> counter;
    auto increment_counter = [&counter](const spot::match_info& match_info)
    {
        ++counter;
        return true;
    };
    spot::search_directory(path, regex, increment_counter, options);
    return counter;
}

#define VERIFY(expr, ...)                                                                                              \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(expr))                                                                                                   \
        {                                                                                                              \
            handle_test_failure(#expr, __FILE__, __LINE__, ##__VA_ARGS__);                                             \
        }                                                                                                              \
    }                                                                                                                  \
    while (false)
