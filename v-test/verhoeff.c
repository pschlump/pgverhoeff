
#include <strings.h>

/* multiplication table */
static int verhoeff_d[][10]	= {
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	{1, 2, 3, 4, 0, 6, 7, 8, 9, 5},
	{2, 3, 4, 0, 1, 7, 8, 9, 5, 6},
	{3, 4, 0, 1, 2, 8, 9, 5, 6, 7},
	{4, 0, 1, 2, 3, 9, 5, 6, 7, 8},
	{5, 9, 8, 7, 6, 0, 4, 3, 2, 1},
	{6, 5, 9, 8, 7, 1, 0, 4, 3, 2},
	{7, 6, 5, 9, 8, 2, 1, 0, 4, 3},
	{8, 7, 6, 5, 9, 3, 2, 1, 0, 4},
	{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
};

/* permutation table */
static int verhoeff_p[][10] = {
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
	{1, 5, 7, 6, 2, 8, 3, 0, 9, 4},
	{5, 8, 0, 3, 7, 9, 6, 1, 4, 2},
	{8, 9, 1, 6, 0, 4, 3, 5, 2, 7},
	{9, 4, 5, 3, 1, 2, 6, 8, 7, 0},
	{4, 2, 8, 6, 5, 7, 3, 9, 0, 1},
	{2, 7, 9, 3, 8, 0, 6, 4, 1, 5},
	{7, 0, 4, 6, 9, 1, 3, 2, 5, 8}
};

/* inverse table */
static int verhoeff_inv[] = {0, 4, 3, 2, 1, 5, 6, 7, 8, 9};

// generate_verhoeff will take a string of digits and return a check digit character.
int generate_verhoeff(const char* num) {
	int c = 0;
	int len = strlen(num);

	for(int i = 0; i < len; i++) {
		c = verhoeff_d[c][verhoeff_p[((i + 1) % 8)][num[len - i - 1] - '0']];
	}

	return verhoeff_inv[c] + '0';
}

// -- Add append_verhoeff_check_digit ( ... )
// append_verhoeff_check_digit will modify the string 'num' to append the check digit.  'num' must be large enough to append a character.
void append_generate_verhoeff(char* num) {
	int len = strlen(num);
	char c = generate_verhoeff(num);
	num[len] = c;
	num[len+1] = '\0';
}

// validate_verhoeff will return true if the check digit is valid for the numeric string.	The check digit must be at the end.
int validate_verhoeff(const char* num) {
	int c = 0;
	int len = strlen(num);

	for (int i = 0; i < len; i++) {
		c = verhoeff_d[c][verhoeff_p[(i % 8)][num[len - i - 1] - '0']];
	}

	return (c == 0);
}

// validate_verhoeff will return true if the check digit is valid for the numeric string.	The check digit must be at the end.
int validate_verhoeff_len(const char* num, int len) {
	int c = 0;
	for (int i = 0; i < len; i++) {
		c = verhoeff_d[c][verhoeff_p[(i % 8)][num[len - i - 1] - '0']];
	}
	return (c == 0);
}


// append_verhoeff_check_char will take a string, 'str' of digits and modify it to have a verhoeff check digit at the end.
// 'str' must have sufficient storage to store the additional character.
void append_verhoeff_check_char(char* str, int *ln) {
	int c = 0, len = *ln;
	for(int i = 0; i < len; i++) {
		c = verhoeff_d[c][verhoeff_p[((i + 1) % 8)][str[len - i - 1] - '0']];
	}
	str[len] verhoeff_inv[c] + '0';
	(*ln)++
}

