
#include <stdio.h>
#include <strings.h>
#include "verhoeff.h"

// int generate_verhoeff(const char* num) ;
// int validate_verhoeff(const char*	num) ;

int main ( int argc, char *argv[]) {
	char buf[5];
	strcpy ( buf, "123");
	char c = generate_verhoeff ( "123" );
	buf [3] = c;
	buf [4] = '\0';
	fprintf ( stdout, "Is Valid: %s\n", (validate_verhoeff(buf) ? "Yes" : "No" ));
}
