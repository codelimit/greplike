#include "tests.h"

int star(int argc, char** const argv)
{
    size_t match_count = count_matches(get_sandbox_path(), "bet*er", spot::search_options());
    VERIFY(match_count == 1, std::format("unexpected match count! expected 1, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "ol*o", spot::search_options());
    VERIFY(match_count == 2, std::format("unexpected match count! expected 2, got {}", match_count));

    return 0;
}
