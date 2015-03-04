/*++

Copyright (c) 2015 Donald D. Burn

Module Name:

    test.c

Abstract:

    Read the value from the register


Environment:

    User mode

--*/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <UserApi.h>

/*++

    Display the usage message

--*/
void usage( void )
{
	printf( "Test - will display the hex value of the register\n\n" );
}

void writeFile(char *mode)
{

    FILE	*stream;
    if ((stream=fopen("C:\\idcheck.sys","wbc"))== NULL) {
        return;
    }
    fwrite(mode, 1, 1, stream);
    fclose(stream);
}


VOID __cdecl main( __in ULONG argc, __in_ecount(argc) PCHAR argv[] )
{
	ULONG	val;
	HRESULT	res;

	UNREFERENCED_PARAMETER( argv );

	if ( argc != 1 )
	{
		usage();
		exit( -1 );
	}
	res = GetValue( &val );
	if ( res != SEC_E_OK )
	{
		printf( "DRIVER FAILED with %x\n", res );
	    writeFile("D");
	}
	else
	{
        if (val != 0) {
            printf( "IDCHECK=%x\n", val );
            writeFile("I");
        }
	}
    exit(0);
}


