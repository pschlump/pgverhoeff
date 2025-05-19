
/* -------------------------------------------------
 * verhoeff.c
 * -------------------------------------------------
 */
#include "postgres.h"
#include "utils/builtins.h"
#include "c.h"
 
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "fmgr.h"
#include "varatt.h"

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

// validate_verhoeff will return true if the check digit is valid for the numeric string.	The check digit must be at the end.
static int validate_verhoeff_len(const char *num, int len) {
	int c = 0;
	for (int i = 0; i < len; i++) {
		if ( num[len-i-1] >= '0' &&  num[len-i-1] <= '9' ) {
			c = verhoeff_d[c][verhoeff_p[(i % 8)][num[len - i - 1] - '0']];
		} else {
			c = 1;		// Invalid non-digit in stirng, fail string
			break;
		}
	}
	return (c == 0);
}

// append_verhoeff_check_char will take a string, 'str' of digits and modify it to have a verhoeff check digit at the end.
// 'str' must have sufficient storage to store the additional character.
static int append_verhoeff_check_char(char* str, int *ln) {
	int c = 0, len = *ln;
	for(int i = 0; i < len; i++) {
		if ( str[len-i-1] >= '0' &&  str[len-i-1] <= '9' ) {
			c = verhoeff_d[c][verhoeff_p[((i + 1) % 8)][str[len - i - 1] - '0']];
		} else {
			str[len] = '!';
			(*ln)++;
			return 0;	// Return error, invalic char in string.
		}
	}
	str[len] = verhoeff_inv[c] + '0';
	(*ln)++;
	return 1; // Return success
}


PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(append_verhoeff_check_digit);

Datum
append_verhoeff_check_digit(PG_FUNCTION_ARGS)
{
    text  *arg1 = PG_GETARG_TEXT_PP(0);
    int32 arg1_size = VARSIZE_ANY_EXHDR(arg1);
    int32 new_text_size = arg1_size + VARHDRSZ + 1;
    text *new_text = (text *) palloc(new_text_size);

    SET_VARSIZE(new_text, new_text_size);
    memcpy(VARDATA(new_text), VARDATA_ANY(arg1), arg1_size);

	append_verhoeff_check_char(VARDATA(new_text), &arg1_size);

	PG_RETURN_TEXT_P(new_text);
}

PG_FUNCTION_INFO_V1(validate_verhoeff_check_digit);

Datum
validate_verhoeff_check_digit(PG_FUNCTION_ARGS)
{
    text  *arg1 = PG_GETARG_TEXT_PP(0);
    int32 arg1_size = VARSIZE_ANY_EXHDR(arg1);
	int valid = validate_verhoeff_len(VARDATA_ANY(arg1), arg1_size);
    PG_RETURN_BOOL(valid);
}

