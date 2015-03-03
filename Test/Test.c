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
		printf( "FAILED with %x\n", res );
	}
	else
	{
		printf( "VALUE=%x\n", val );
	}
    exit(0);
}


