/*++

Copyright (c) 2015  Don Burn

Module Name:

        userAPI.c

Abstract:

        Simple API for accessing value

Environment:

        User Space Only

Notes:


Revision History:


Authors:

--*/


#include "windows.h"
#pragma warning( disable : 4201 28719 )
#include "stdio.h"
#include "stdlib.h"
#include <devioctl.h>
#include "UserApi_ioctl.h"
#include "UserApi.h"


/*++

Routine Description:

    This routine is the entry for the DLL.  The function
	checks for the process attach and sets up the device
	and for the process detack to shutdown the device

Arguments:

    Handle		- unused handle to the module
	Reason		- reason for the call
	Reserved	- reserved unused flag
Return Value:

    TRUE if successful

--*/
BOOL WINAPI DllMain( __in HINSTANCE Handle, __in DWORD Reason,
					 __in LPVOID Reserved )
{
	BOOL	retVal;

	UNREFERENCED_PARAMETER( Handle );
	UNREFERENCED_PARAMETER( Reserved );

	retVal = TRUE;
	switch ( Reason )
	{
	case DLL_PROCESS_ATTACH:
		break;

	case DLL_PROCESS_DETACH:
		break;

	default:
		break;
	}
	return retVal;
}


/*++

    Get the value

Arguments:

    Val - Pointer to the variable to recieve the value

Return Value:

    HRESULT

--*/
HRESULT WINAPI GetValue(  __out PULONG Val )
{
	HRESULT		res;
	HANDLE		h;

	res = SEC_E_OK;
	h = CreateFile("\\\\.\\GETVALUE", GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if ( h == INVALID_HANDLE_VALUE )
	{
		res = HRESULT_FROM_WIN32( GetLastError() );
	}
	else
	{
		if ( !DeviceIoControl( h, (DWORD) IOCTL_GETVALUE, NULL, 0, NULL, 0, Val, NULL ) )
		{
			res = HRESULT_FROM_WIN32( GetLastError() );
		}
		CloseHandle( h );
	}
	return res;
}

