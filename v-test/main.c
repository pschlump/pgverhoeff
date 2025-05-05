
#include <stdio.h>
#include <strings.h>
#include "verhoeff.h"

// int generate_verhoeff(const char* num) ;
// int validate_verhoeff(const char*	num) ;

int main ( int argc, char *argv[]) {
	char buf[10];
	strcpy ( buf, "12345");
	char c = generate_verhoeff ( buf );
	buf [5] = c;
	buf [6] = '\0';
	fprintf ( stdout, "Is Valid: %s\n", (validate_verhoeff(buf) ? "Yes" : "No" ));
	fprintf ( stdout, "data check >%s< matches >%s<, %s\n", "123451", buf, ( strcmp ( "123451", buf ) == 0 ) ? "PASS" :  "Failed" );
}
