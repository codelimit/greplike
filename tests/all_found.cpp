#include "tests.h"

int all_found(int argc, char** const argv)
{
    size_t match_count = count_matches(get_sandbox_path(), "word", spot::search_options());
    VERIFY(match_count == 3, std::format("unexpected match count! expected 3, got {}", match_count));

    match_count = count_matches(get_sandbox_path(), "ou", spot::search_options());
    VERIFY(match_count == 6, std::format("unexpected match count! expected 6, got {}", match_count));

    spot::search_options alt_options;
    alt_options.case_sensetive = true;
    match_count = count_matches(get_sandbox_path(), "word", alt_options);
    VERIFY(match_count == 1, std::format("unexpected match count! expected 1, got {}", match_count));
    return 0;
}
