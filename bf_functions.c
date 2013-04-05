#include <stdio.h>
#include "bf_functions.h"

int bf_getchar()
{
	return getchar();
}

void bf_putchar()
{
	char c = 0;
	asm("mov byte ptr [ebp-0x09], cl");
	putchar(c);
}

