#include "tests.h"

int starts_with(int argc, char** const argv)
{
    size_t match_count = count_matches(get_sandbox_path(), "^b", spot::search_options());
    VERIFY(match_count == 3, std::format("unexpected match count! expected 3, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "^t", spot::search_options());
    VERIFY(match_count == 4, std::format("unexpected match count! expected 4, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "^z", spot::search_options());
    VERIFY(match_count == 0, std::format("unexpected match count! expected 0, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "^better", spot::search_options());
    VERIFY(match_count == 1, std::format("unexpected match count! expected 1, got {}", match_count));

    return 0;
}
