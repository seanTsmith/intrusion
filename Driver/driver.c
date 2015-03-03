/*++

Copyright (c) 2015 Donald D. Burn

Module Name:

    driver.c

Abstract:

    The source code for the GetValue driver

Environment:

	Kernel mode only

--*/

#include <ntddk.h>
#include <wdf.h>
#include <UserApi_ioctl.h>

//
// The device extension for the device object
//
typedef struct
{
	PUCHAR				Register;		// The mapped address of the register to read
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

#define PM_BASE					0xFED8035A
#define MILLISECONDS_TO_100NS   (10000)
#define SECOND_TO_MILLISEC      (1000)
#define SECOND_TO_100NS         (SECOND_TO_MILLISEC * MILLISECONDS_TO_100NS)


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME( DEVICE_CONTEXT, GetDeviceContext )

static const UNICODE_STRING NtDeviceName = RTL_CONSTANT_STRING( L"\\Device\\GetValue" );
static const UNICODE_STRING DosDeviceName = RTL_CONSTANT_STRING( L"\\DosDevices\\GetValue" );

/*++

    This event is called when the framework receives IRP_MJ_DEVICE_CONTROL
    requests from the system.

Arguments:

    Queue				- UNUSED: Handle to the framework queue object that is associated
							with the I/O request.
    Request				- Handle to a framework request object.
    OutputBufferLength	- UNUSED: Length of the request's output buffer, 
							if an output buffer is available.
    InputBufferLength	- UNUSED: Length of the request's input buffer, 
							if an input buffer is available.
    IoControlCode		- Driver-defined I/O control code
							(IOCTL) that is associated with the request.

--*/
static EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL EvtIoDeviceControl;
#pragma alloc_text (PAGE, EvtIoDeviceControl)
static VOID EvtIoDeviceControl( IN WDFQUEUE Queue, IN WDFREQUEST Request,
							    IN size_t OutputBufferLength, IN size_t InputBufferLength,
								IN ULONG IoControlCode )
{
    NTSTATUS		status;
	ULONG_PTR		value;
	UCHAR			reg;
	PDEVICE_CONTEXT	deviceContext;

	PAGED_CODE();

    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    KdPrint( ( "GETVALUE: Entering EvtIoDeviceControl\n" ) );
	value = 0;
 	deviceContext = GetDeviceContext( WdfIoQueueGetDevice( Queue ) );
	switch (IoControlCode) 
	{
    case IOCTL_GETVALUE:
		// Since this is a request for a single bit, return the value in the
		// information field for the request completion
        status = STATUS_SUCCESS;
		reg = READ_REGISTER_UCHAR( deviceContext->Register );
		value = (ULONG_PTR)reg;
		reg |= 0x02;

//        WRITE_REGISTER_ULONG( deviceContext->Register, reg );
//        status = KeDelayExecutionThread( KernelMode, FALSE, -1 * MILLISECONDS_TO_100NS );
//        reg &= 0xFD;
//        WRITE_REGISTER_ULONG( deviceContext->Register, reg );
		break;            

	default:
        status = STATUS_INVALID_PARAMETER;
        break;
    }
    KdPrint( ( "GETVALUE: EvtIoDeviceControl: Completing request with status 0x%0x \n", status ) );
    WdfRequestCompleteWithInformation( Request, status, value );
    KdPrint( ( "GETVALUE: EvtIoDeviceControl: Exiting\n" ) );
}    


static EVT_WDF_OBJECT_CONTEXT_CLEANUP EvtCleanupDevice;
#pragma alloc_text (PAGE, EvtCleanupDevice)
static VOID EvtCleanupDevice( IN WDFOBJECT Object )
{
	PDEVICE_CONTEXT	deviceContext;

	PAGED_CODE();

	KdPrint( ( "GETVALUE: Entering EvtCleanupDevice\n" ) );
 	deviceContext = GetDeviceContext( ( (WDFDEVICE) Object ) );

	if ( deviceContext->Register != NULL )
	{
		MmUnmapIoSpace( deviceContext->Register, 1 );
	}
    KdPrint( ( "GETVALUE: EvtCleanupDevice: Exiting\n" ) );
}

/*++

    EvtDeviceAdd is called by the framework in response to AddDevice
    call from the PnP manager. We create and initialize a WDF device object to
    represent a new instance of the device.

Arguments:

    Driver - Handle to a framework driver object created in DriverEntry

    DeviceInit - Pointer to a framework-allocated WDFDEVICE_INIT structure.

Return Value:

    NTSTATUS

--*/
static EVT_WDF_DRIVER_DEVICE_ADD EvtDriverDeviceAdd;
#pragma alloc_text (PAGE, EvtDriverDeviceAdd )
static NTSTATUS EvtDriverDeviceAdd( IN WDFDRIVER Driver, IN PWDFDEVICE_INIT DeviceInit )
{
    NTSTATUS				status;
    WDF_OBJECT_ATTRIBUTES	objAttr;
    WDFDEVICE				device;
    WDF_IO_QUEUE_CONFIG		config;
 	PDEVICE_CONTEXT			deviceContext;
	PHYSICAL_ADDRESS		addr;
   
    PAGED_CODE();

	UNREFERENCED_PARAMETER(Driver);

    KdPrint( ( "GETVALUE: Entering EvtDeviceAdd\n" ) );
	device = NULL;

	// Create the device:
	//
	//	   1.  Setup the object attributes with type DEVICE_CONTEXT for the context structure
	//	   2.  Assign the device name, note: this is fixed so only one instance will work
	//	   3.  Assign a cleanup function so that register mapping may be released
	//	   4.  Create the actual device object

    WDF_OBJECT_ATTRIBUTES_INIT( &objAttr );
    WDF_OBJECT_ATTRIBUTES_SET_CONTEXT_TYPE( &objAttr, DEVICE_CONTEXT );
	objAttr.EvtCleanupCallback = EvtCleanupDevice;
    status = WdfDeviceInitAssignName( DeviceInit, &NtDeviceName );
    if ( !NT_SUCCESS(status) ) 
	{
        KdPrint( ( "GETVALUE: WdfDeviceInitAssignName failed 0x%0x\n", status ) );
    }
	else
	{
		status = WdfDeviceCreate(&DeviceInit, &objAttr, &device);
		if ( !NT_SUCCESS(status)) 
		{
			KdPrint( ( "GETVALUE: WdfDeviceInitialize failed 0x%0x\n", status ) );
		}
	}

	// Setup the pointer to the value you are going to read.  THIS IS NON-STANDARD
	// SINCE THIS NORMALLY WILL BE DONE AT EvtDevicePrepareHardware, but since we
	// are stomping on the value do it here
	if ( NT_SUCCESS( status ) && device != NULL )
	{
		deviceContext = GetDeviceContext( device );
		addr.QuadPart = PM_BASE;
		deviceContext->Register = MmMapIoSpace( addr, 1, MmNonCached);
	}

	// Setup the symbolic link for the device object just created.  Note: again this is
	// a fixed string since only one instance of the device will ever be created
	if ( NT_SUCCESS( status ) && device != NULL )
	{
		status = WdfDeviceCreateSymbolicLink(device, &DosDeviceName);
		if (!NT_SUCCESS(status)) 
		{
			KdPrint( ( "GETVALUE: WdfDeviceCreateSymbolicLink failed 0x%0x\n", status ) );
		}
	}
 
	// Setup the default queue for processing requests.  In this driver the only request
	// supported will be DeviceIoControl's
	if ( NT_SUCCESS( status ) && device != NULL )
	{
		WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&config, WdfIoQueueDispatchSequential);
		config.EvtIoDeviceControl = EvtIoDeviceControl;
		status = WdfIoQueueCreate(device, &config, WDF_NO_OBJECT_ATTRIBUTES, NULL);

		if (!NT_SUCCESS(status)) 
		{
			KdPrint( ( "GETVALUE: WdfIoQueueCreate for default queue failed 0x%0x\n", status ) );
		}
	}
    KdPrint( ( "GETVALUE: EvtDeviceAdd: Exiting with status 0x%0x \n", status ) );
    return status;
}


/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry configures and creates a WDF driver
    object.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
DRIVER_INITIALIZE DriverEntry;
#pragma alloc_text (INIT, DriverEntry)
NTSTATUS DriverEntry( IN PDRIVER_OBJECT  DriverObject,
					  IN PUNICODE_STRING RegistryPath )
{
    NTSTATUS			status;
    WDF_DRIVER_CONFIG	config;

    KdPrint( ( "\nGETVALUE: Driver -- Compiled %s %s\n", __DATE__, __TIME__ ) );

	// Create the drvier object with the routine EvtDeviceAdd as the handler
	// for AddDevice requests

    WDF_DRIVER_CONFIG_INIT(&config, EvtDriverDeviceAdd );
    status = WdfDriverCreate( DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, NULL );
    if ( !NT_SUCCESS( status ) ) 
	{
        KdPrint( ( "GETVALUE: WdfDriverCreate failed with status 0x%0x\n", status ) );
    }

    KdPrint( ( "GETVALUE: DriverEntry: Exiting with status 0x%0x \n", status ) );
    return status;
}



