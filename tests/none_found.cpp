#include "tests.h"

int none_found(int argc, char** const argv)
{
    size_t match_count = count_matches(get_sandbox_path(), "off", spot::search_options());
    VERIFY(match_count == 0, std::format("unexpected match count! expected 0, got {}", match_count));

    return 0;
}
