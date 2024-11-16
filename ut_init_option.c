#include <assert.h>
#include "main.h" // Include your header file

void test_option_init() {
    user_data udp;

    init_option(&udp);

    // Check default values
    assert(udp.opt_include_empty == TRUE);
    assert(udp.opt_include_directory == TRUE);
    assert(udp.opt_include_duplicate == TRUE);
    assert(udp.opt_include_unique == TRUE);

    // Clean up
    g_free(udp.opt_name);
}

int main() {
    test_option_init();
    printf("All tests passed.\n");
    return 0;
}
