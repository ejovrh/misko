#include <avr/io.h>

#include "main.h"																//
#include "misko.h"																//

int main(void)
{
	misko_ctor();

	misko->fooFunc(0xFF);



    while (1)
    {
		;
    }
};