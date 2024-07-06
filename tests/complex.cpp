#include "tests.h"

int complex(int argc, char** const argv)
{
    size_t match_count = count_matches(get_sandbox_path(), "n.$", spot::search_options());
    VERIFY(match_count == 3, std::format("unexpected match count! expected 3, got {}", match_count));

    // all lines
    match_count = count_matches(get_sandbox_path(), "", spot::search_options());
    VERIFY(match_count == 13, std::format("unexpected match count! expected 13, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "^t.*at", spot::search_options());
    VERIFY(match_count == 1, std::format("unexpected match count! expected 1, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "^b.*s", spot::search_options());
    VERIFY(match_count == 3, std::format("unexpected match count! expected 3, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "n...l.*w", spot::search_options());
    VERIFY(match_count == 1, std::format("unexpected match count! expected 1, got {}", match_count));

    return 0;
}
