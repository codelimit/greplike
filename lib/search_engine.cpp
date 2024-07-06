#include "search_engine.h"

#include <iostream>
#include <mutex>
#include <regex>

namespace spot
{
    class std_regex_search_engine final : public search_engine
    {
    public:
        explicit std_regex_search_engine(std::regex r) : regex(std::move(r)) {}
        ~std_regex_search_engine() override = default;
        [[nodiscard]] std::string_view::const_iterator search(const std::string_view& str) const override
        {
            std::match_results<std::string_view::iterator> match;
            if (std::regex_search(str.begin(), str.end(), match, regex))
            {
                return str.begin() + match.position();
            }
            return str.end();
        }

        std::regex regex;
    };

    // https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html
    class rob_pike_search_engine final : public search_engine
    {
    public:
        explicit rob_pike_search_engine(std::string regex) : regex(std::move(regex)) {}
        ~rob_pike_search_engine() override = default;
        [[nodiscard]] std::string_view::const_iterator search(const std::string_view& str) const override
        {
            if (const int pos = match(regex.c_str(), str.data()); pos != -1)
            {
                return str.begin() + pos;
            }
            return str.end();
        }

    private:
        static int match(const char* regexp, const char* text)
        {
            if (regexp[0] == '^')
            {
                return matchhere(regexp + 1, text) == true ? 0 : -1;
            }

            int pos = 0;
            do
            {
                if (matchhere(regexp, text))
                {
                    return pos;
                }
                ++pos;
            }
            while (*text++ != '\0');
            return -1;
        }

        static bool matchhere(const char* regexp, const char* text)
        {
            if (regexp[0] == '\0')
                return true;
            if (regexp[1] == '*')
                return matchstar(regexp[0], regexp + 2, text);
            if (regexp[0] == '$' && regexp[1] == '\0')
                return *text == '\0';
            if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
                return matchhere(regexp + 1, text + 1);
            return false;
        }

        static bool matchstar(int c, const char* regexp, const char* text)
        {
            do
            {
                if (matchhere(regexp, text))
                    return true;
            }
            while (*text != '\0' && (*text++ == c || c == '.'));
            return false;
        }

        std::string regex;
    };

    std::unique_ptr<search_engine> make_search_engine(const std::string& regex, bool case_sensetive)
    {
        static std::once_flag flag;
        constexpr bool experimental_engine = false;
        if constexpr (experimental_engine)
        {
            std::call_once(
                    flag, []()
                    { std::cout << "warning: using experimental search engnie, which ignores case_sensetive option"; });
            return std::make_unique<rob_pike_search_engine>(regex);
        }

        const std::regex::flag_type flags = case_sensetive ? std::regex::basic : std::regex::icase;
        return std::make_unique<std_regex_search_engine>(std::regex(regex, flags));
    }
} // namespace spot
