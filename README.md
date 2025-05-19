# Implement Postgres Extension in C for the Verhoeff Checksum

Postgres offers powerful extensibility features that allow developers to enhance its functionality. 

This guide will walk you through creating a basic C extension for Postgres that implements the Verhoeff checksum algorythm. 

The Verhoeff algorithm is a checksum for error detection developed by Dutch mathematician Jacobus Verhoeff. 
It is the first decimal check digit algorithm which detects all single-digit errors, transposition errors involving two adjacent digits.

In 1985 H. Peter Gurmm formally proved that the algorythm worked.

## Required Files

To build a C extension for Postgres, you'll need:

1. **Control file** - Specifies extension metadata and loading instructions
2. **SQL file** - Defines the interface between Postgres and your C code
3. **C source file** - Contains the implementation code
4. **Makefile** - Handles compilation and installation

Additionally, two helpful files:
- **setup.sql** - Installs the extension
- **test3.sql** - Verifies the extension works correctly

## Prerequisites

- Postgres installed locally
- C compiler (gcc in this example)
- Basic understanding of C programming

## The Control File

Create a file named `verhoeff.control`:

```
comment = 'C Extension to compute and validate Verhoeff checksums'
default_version = '1.0'
module_pathname = '$libdir/verhoeff'
relocatable = true
```

This file defines:
- A description of the extension
- The current version number
- Where Postgres will find the compiled module
- Whether the extension can be relocated across schemas

## The SQL Interface File

Create `verhoeff--1.0.sql` (matching the version specified in the control file):

```sql
-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION verhoeff" to load this file. \quit

-- convet_to_title will compute a Verhoeff checksum and return a new string with this check digit appened.
CREATE FUNCTION validate_verhoeff(inp text) RETURNS boolean 
AS 'MODULE_PATHNAME', 'validate_verhoeff_check_digit'
LANGUAGE c STRICT VOLATILE;

-- convet_to_title will return true of the string has an accurate Verhoeff check digit at th end.
CREATE FUNCTION generate_verhoeff(inp text) RETURNS text 
AS 'MODULE_PATHNAME', 'append_verhoeff_check_digit'
LANGUAGE c STRICT VOLATILE;

```

This SQL file creates a function that:
- Takes text as input and returns text
- Links to the C implementation
- Specifies that the function is implemented in C

## The C Implementation

Create `verhoeff.c`:

```c

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
```

### Important C Programming Considerations

When writing C extensions for Postgres, remember these key points:

1. **String Handling Differences**
   - C uses null-terminated strings
   - Postgres uses length-prefixed string formats
   - Don't use standard C string functions directly on Postgres strings
   - Converting between formats requires careful memory management

2. **Memory Management**
   - Always use `palloc()`/`pfree()` instead of `malloc()`/`free()`
   - Postgres has its own optimized memory allocation system

## The Makefile

Create a `Makefile`:

```make
#
EXTENSION = verhoeff
PGFILEDESC = "C Extension to compute and validate Verhoeff checksums"
MODULE_big = verhoeff
DATA = verhoeff--1.0.sql
OBJS = verhoeff.o 

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)

```

This Makefile:
- Describes your extension
- Specifies the extension name
- Defines the module to compile
- Lists the SQL and object files
- Uses Postgres's build system for compilation

Before using the Makefile, verify:
1. GNU `make` is installed (`brew install make` on macOS)
2. Postgres is properly configured (`pg_config --version`)

## Building and Installing

1. **Compile the code**:
   ```bash
   $ make
   ```

2. **Install the extension**:
   ```bash
   $ make install
   ```
   Note: On some systems, you may need elevated permissions for this step.

3. **Create a setup file** (`setup.sql`):
   ```sql
   DROP EXTENSION IF EXISTS verhoeff;
   CREATE EXTENSION IF NOT EXISTS verhoeff;
   ```

4. **Install the extension in your database**:
   ```bash
   $ psql -f setup.sql
   ```

## Testing the Extension

Test with a simple query:

```sql
SELECT generate_verhoeff('12345');

SELECT validate_verhoeff('123451');

```

Expected output:
```

philip=# SELECT generate_verhoeff('12345');
 generate_verhoeff
-------------------
 123451
(1 row)

philip=# SELECT validate_verhoeff('123451');
 validate_verhoeff
-------------------
 t
(1 row)

```

For comprehensive testing, create a `test3.sql` file:

```sql

-- A full block of tests that results in 'pass'/'fail' 

DO $$
DECLARE
 	l_txt text;
 	l_ok boolean;
 	n_err int;
	l_msg text;
BEGIN

 	n_err = 0;

	SELECT generate_verhoeff('12345')
		into l_txt;
	if l_txt != '123451' then
		n_err = n_err + 1;
	end if;

	SELECT validate_verhoeff('123451')
		into l_ok;
	if not l_ok then
		n_err = n_err + 1;
	end if;

	SELECT validate_verhoeff('123452')
		into l_ok;
	if l_ok then
		n_err = n_err + 1;
	end if;

	SELECT generate_verhoeff('12a45')
		into l_txt;
	if l_txt != '12a45!' then
		n_err = n_err + 1;
	end if;

	SELECT validate_verhoeff('12a452')
		into l_ok;
	if l_ok then
		n_err = n_err + 1;
	end if;

	if n_err = 0 then
		RAISE NOTICE 'PASS';
	else 
		RAISE NOTICE 'FAIL';
	end if;

END;
$$ LANGUAGE plpgsql;


```

Run the test:
```bash
$ psql -f test.sql
```

## Source Code

The complete code for this extension is available on GitHub at [https://github.com/pschlump/pgverhoeff](https://github.com/pschlump/pgverhoeff).

