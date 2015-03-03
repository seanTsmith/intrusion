/*++

Copyright (c) 2015 Donald D. Burn

Module Name:

    userapi_ioctl.h

Abstract:

    This file defines the interface for the driver

Environment:

	Either kernel or user mode

--*/
#pragma once
#ifndef __USER_IOCTL_H
#define __USER_IOCTL_H

#define GETVALUE_DEVICE				0x00008760

#define IOCTL_GETVALUE	CTL_CODE( GETVALUE_DEVICE, 0x102, METHOD_NEITHER, FILE_READ_ACCESS )


#endif // __USER_IOCTL_H

