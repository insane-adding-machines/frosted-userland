#ifndef _TEST
#define _TEST

#include <stdio.h>
#include <string.h>

#define fail_if(exp, txt)\
	if (exp) { fprintf(stderr, "Failure on %s %s:%d\n", txt, __FILE__, __LINE__); return -1; }

#define test_str(p, txt)\
	if (p) { fail_if((p < 0), txt); fail_if((strlen(txt) != strlen(p)), txt); fail_if((strncmp(txt, p, strlen(txt))), txt); }

#define err_fail_if(exp, txt)\
	if (exp) { fprintf(stderr, "Failure on %s %s:%d\n", txt, __FILE__, __LINE__); perror(txt); return -1; }


#endif