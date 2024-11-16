#include <assert.h>
#include "main.h" // Include your header file

void print_msg(const char *str)
{
	time_t now;
	struct tm *tbd;
	char buffer[26];

	now = time(NULL);	// Get UNIX-style time since epoc
	tbd = localtime(&now);	// Convert to broken down time in result
	strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tbd);	// format time in buffer
	printf("%s: %s\n", buffer, str);	// print buffer and string
}

int test_option_init()
{
	user_data *udp = g_malloc(sizeof(user_data));
	const char *opt_file = "/home/dhugh/.config/ddup.cfg";

	// Test 1 - if opt file not there should get all default values
	// - Force the file to be missing if there
	remove(opt_file);

	option_init(udp);

	if (udp->opt_include_empty == FALSE ||
		udp->opt_include_directory == FALSE ||
		udp->opt_include_duplicate == FALSE ||
		udp->opt_include_unique == FALSE) { 
		print_msg("Test 1 failed - default values not set correctly");
		g_free(udp);
		return 0;
	}	
	else {
		print_msg("Test 1 passed");
	}
	// Test 2 - write and read back tftf
	// Check values read from file (if there)
	unsigned char tftf[4] = {TRUE, FALSE, TRUE, FALSE};
	FILE *fp = fopen("/home/dhugh/.config/ddup.cfg", "wb+");
	for (int i = 0; i < 4; i++)
	{
		fwrite(&tftf[i], sizeof(unsigned char), 1, fp);
	}
	fclose(fp);

	option_init(udp);

	if (udp->opt_include_empty == TRUE &&
		udp->opt_include_directory == FALSE &&
		udp->opt_include_duplicate == TRUE &&
		udp->opt_include_unique == FALSE) {
		print_msg("Test 2 passed");
	}	
	else {
		print_msg("Test 2 failed - tftf values are not set correctly");
		g_free(udp);
		return 0;
	}	
	// Test 3 - write and read back ftft
	// Check values read from file (if there)
	unsigned char ftft[4] = {FALSE, TRUE, FALSE, TRUE};
	fp = fopen("/home/dhugh/.config/ddup.cfg", "wb+");
	// Write the sequence
	for (int i = 0; i < 4; i++)
	{
		fwrite(&ftft[i], sizeof(unsigned char), 1, fp);
	}
	fclose(fp);

	option_init(udp);

	if (udp->opt_include_empty == FALSE &&
		udp->opt_include_directory == TRUE &&
		udp->opt_include_duplicate == FALSE &&
		udp->opt_include_unique == TRUE) {
		print_msg("Test 3 passed");
	}	
	else {
		print_msg("Test 3 failed - ftft values are not set correctly");
		g_free(udp);
		return 0;
	}
	g_free(udp);
	return 1;
}

int main()
{
	int result = test_option_init();	
	if (result == 1) {	
		printf("All tests passed.\n");
		return 1;
	}	
	else {	
		printf("Test failed.\n");
		return 0;
	}	
}
