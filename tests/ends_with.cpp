#include "tests.h"

int ends_with(int argc, char** const argv)
{
    size_t match_count = count_matches(get_sandbox_path(), "e$", spot::search_options());
    VERIFY(match_count == 1, std::format("unexpected match count! expected 1, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "n$", spot::search_options());
    VERIFY(match_count == 0, std::format("unexpected match count! expected 0, got {}", match_count));

    return 0;
}
