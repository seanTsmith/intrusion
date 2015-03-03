/*++

Copyright (c) 2015 Donald D. Burn

Module Name:

    UserApi.h

Abstract:

    This file defines the interface for the library to control the driver

Environment:

	User mode only

--*/
#ifndef __USERAPI_H
#define __USERAPI_H

#ifdef __cplusplus
extern "C" {
#endif

extern HRESULT WINAPI GetValue( __out PULONG Val );

#ifdef __cplusplus
}
#endif

#endif // __USERAPI_H
