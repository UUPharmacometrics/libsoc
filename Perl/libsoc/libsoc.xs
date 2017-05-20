#define PERL_NO_GET_CONTEXT
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"



#include <stdio.h>

void print_hello(void)
{
    printf("Hello, world\n");
}

MODULE = libsoc		PACKAGE = libsoc		

void
print_hello()
