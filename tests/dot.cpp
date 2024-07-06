#include "tests.h"

int dot(int argc, char** const argv)
{
    size_t match_count = count_matches(get_sandbox_path(), "ho..s", spot::search_options());
    VERIFY(match_count == 2, std::format("unexpected match count! expected 2, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "t..t", spot::search_options());
    VERIFY(match_count == 6, std::format("unexpected match count! expected 6, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "n.......t", spot::search_options());
    VERIFY(match_count == 2, std::format("unexpected match count! expected 2, got {}", match_count));

    return 0;
}
