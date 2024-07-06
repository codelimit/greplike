#include <algorithm>
#include <filesystem>
#include <iostream>
#include <syncstream>

#include <spot.h>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cout << "Invalid number of arguments\n"
                  << "Usage: spot.exe <pattern> <search_dir>\n";
        return EXIT_FAILURE;
    }

    const std::vector<std::string> args(argv, argv + argc);
    const std::string& regex = args[1];
    const std::filesystem::path path(args[2]);
    if (!std::filesystem::exists(path))
    {
        std::cout << path << ": no such file or directory";
        return EXIT_FAILURE;
    }

    auto non_ascii = [](const char c) { return static_cast<unsigned char>(c) > 127; };

    auto on_match = [&](const spot::match_info& match_info)
    {
        if (std::ranges::any_of(match_info.line, non_ascii))
        {
            std::osyncstream(std::cout) << match_info.file_path << ": binary file matches\n";
            return false;
        }

        // clang-format off
        std::osyncstream(std::cout) << match_info.file_path << ':'
                                       << match_info.line_number << ' '
                                       << match_info.line << '\n';
        // clang-format off
        return true;
    };

    try
    {
        spot::search_options options;
        // playing with case sensetivity
        // options.case_sensetive = true;
        spot::search_directory(path, regex, on_match, options);
    }
    catch (std::exception& e)
    {
        std::cout << "Critical error: " << e.what();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
