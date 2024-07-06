#pragma once

#include <string_view>
#include <memory>

namespace spot
{
    class search_engine
    {
    public:
        virtual ~search_engine() = default;
        [[nodiscard]] virtual std::string_view::const_iterator search(const std::string_view& str) const = 0;
    };

    std::unique_ptr<search_engine> make_search_engine(const std::string& regex, bool case_sensitive);
}