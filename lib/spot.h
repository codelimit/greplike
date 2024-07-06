#pragma once

#include <filesystem>
#include <functional>

namespace spot
{
    struct search_options
    {
        bool case_sensetive = false;
        bool all_occurrances = false;
    };

    struct match_info
    {
        std::string file_path;
        size_t line_number = 0;
        std::string_view line;
        std::string_view::iterator pos;
    };

    /// @brief callback used in grep functions
    /// @param info custon read-only struct to get info about current match
    /// @return true to continue iterating over current file (if more than one matches were found),
    /// false - to stop iterating over  file and go to the next one
    using callback = std::function<bool(const match_info& info)>;

    /// @brief Perform a recursive directory search and apply a regular expression match on each file found.
    /// @param directory The path to the directory to search in.
    /// @param regex The regular expression pattern to match.
    /// @param on_match The callback function to call when a match is found.
    /// @param options The flags to customize search behaviour
    /// @warning search is performed in multiple threds, therefore, on_match callback is called
    /// from different threads. Take care to ensure thread-safety when accessing shared resources or modifying shared
    /// state in the *callback function.
    /// @note The `grep_file` function is used internally to match the regular expression on each file.
    void search_directory(const std::filesystem::path& directory, const std::string& regex, const callback& on_match,
                          const search_options& options);

    /// @brief Searches for a regular expression pattern in a file and applies a callback function on each match.
    /// @param path The path to the file to be searched.
    /// @param regex The regular expression pattern to match.
    /// @param on_match The callback function to call when a match is found.
    /// @param options The flags to customize search behaviour
    void search_file(const std::filesystem::path& path, const std::string& regex, const callback& on_match,
                     const search_options& options);
} // namespace spot
