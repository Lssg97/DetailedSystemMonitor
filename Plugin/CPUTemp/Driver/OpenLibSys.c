//-----------------------------------------------------------------------------
//     Author : hiyohiyo
//       Mail : hiyohiyo@crystalmark.info
//        Web : http://openlibsys.org/
//    License : The modified BSD license
//
//                     Copyright 2007-2008 OpenLibSys.org. All rights reserved.
//-----------------------------------------------------------------------------

#include "OpenLibSys.h"

//-----------------------------------------------------------------------------
//
// Global
//
//-----------------------------------------------------------------------------

static ULONG refCount;

//-----------------------------------------------------------------------------
//
// Classic NT driver
// DriverEntry / OlsDispatch / Unload
//
//-----------------------------------------------------------------------------


/*
Return Value:
返回值：
	STATUS_SUCCESS if the driver initialized correctly,otherwise an erroror indicating the reason for failure.
	如果驱动程序初始化正确则STATUS_SUCCESS，否则抛出错误原因。
*/
NTSTATUS
DriverEntry(
	IN PDRIVER_OBJECT  DriverObject,
	IN PUNICODE_STRING RegistryPath
	)
{
	NTSTATUS		status;
	UNICODE_STRING  ntDeviceName;
	UNICODE_STRING  win32DeviceName;
	PDEVICE_OBJECT  deviceObject = NULL;
	
	/************************************************************************
	* 函数名称:RtlInitUnicodeString
	* 功能描述:初始化UNICODE_STRING字符串
	* 参数列表:
			DestinationString:要初始化的UNICODE_STRING字符
			SourceString:字符串的内容
	* 返回 值:VOID
	*************************************************************************/
	RtlInitUnicodeString(&ntDeviceName, NT_DEVICE_NAME);

	status = IoCreateDevice(
		DriverObject,					// Our Driver Object(一个指向调用该函数的驱动程序对象.每一个驱动程序在它的DriverEntry过程里接收一个指向它的驱动程序对象)
		0,								// We don't use a device extension(指定驱动程序为设备扩展对象而定义的结构体的大小)
		&ntDeviceName,					// Device name((可选的参数)指向一个以零结尾的包含Unicode字符串的缓冲区,那是这个设备的名称,该字符串必须是一个完整的设备路径名.WDM功能驱动程序和过滤驱动程序它们的设备对象没有名字)
		OLS_TYPE,						// Device type(指定一个由一个系统定义的FILE_DEVICE_XXX常量,表明了这个设备的类型)
		FILE_DEVICE_SECURE_OPEN,		// Device characteristics(指定一个或多个系统定义的常量,连接在一起,提供有关驱动程序的设备其他信息.对于可能的设备特征信息)
		FALSE,							// Not an exclusive device(如果指定设备是独占的,如果是独占的话设置为TRUE，非独占设置为FALSE，大部分驱动程序设置这个值为FALSE)
		&deviceObject );				// Returned ptr to Device Object(一个指向DEVICE_OBJECT结构体指针的指针,这是一个指针的指针,指向的指针用来接收DEVICE_OBJECT结构体的指针)

	if(!NT_SUCCESS(status))
	{
		refCount = (ULONG)-1;
		return status;
	}
	else
	{
		refCount = 0;
	}

	// Initialize the driver object with this driver's entry points.(用这个驱动程序的入口点初始化驱动程序对象。)
	/*
	IO管理器在一个文件或目录创建时,或一个已存在的文件，设备，目录，Volume被打开时,发送此IRP.
	通常在应用层调用CreateFile,或内核层调用:
	IoCreateFile
	IoCreateFileSpecifyDeviceObjectHint
	ZwCreateFile
	ZwOpenFile
	引发此IRP的发送.
	若创建成功，则应用层或内核组件会收到一个此文件对象的句柄.
	*/
	DriverObject->MajorFunction[IRP_MJ_CREATE] = OlsDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = OlsDispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = OlsDispatch;
	DriverObject->DriverUnload = Unload;

	// Initialize a Unicode String containing the Win32 name for our device.(为我们的设备初始化包含Win32名称的Unicode字符串。)
	RtlInitUnicodeString(&win32DeviceName, DOS_DEVICE_NAME);

	// Create a symbolic link between our device name  and the Win32 name(在设备名称和Win32名称之间创建一个符号链接)
	status = IoCreateSymbolicLink(&win32DeviceName, &ntDeviceName);

	if (!NT_SUCCESS(status))
	{
		// Delete everything that this routine has allocated.
		IoDeleteDevice( deviceObject );
	}

	return status;
}

/*
Routine Description(函数说明):
	This routine is the dispatch handler for the driver.  It is responsible for processing the IRPs.
	此函数是驱动程序的调度处理程序。它负责处理IRP。
Arguments(参数):
	pDO - Pointer to device object.
	pDO-指向设备对象的指针。
	pIrp - Pointer to the current IRP.
	pIrp-指向当前IRP的指针。
Return Value(返回值):
	STATUS_SUCCESS if the IRP was processed successfully, otherwise an erroror indicating the reason for failure.
	如果IRP处理成功，返回STATUS_SUCCESS，否则返回错误原因
*/
NTSTATUS
OlsDispatch(
	IN	PDEVICE_OBJECT pDO,
	IN	PIRP pIrp
	)
{
	PIO_STACK_LOCATION pIrpStack;	//IPR堆栈
	NTSTATUS status;
	//int index;

	//  Initialize the irp info field.(初始化irp信息字段。)
	//	This is used to return the number of bytes transfered.(这用于返回传输的字节数。)
	pIrp->IoStatus.Information = 0;
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	//  Set default return status(设置默认返回状态)
	status = STATUS_NOT_IMPLEMENTED;

	//	Dispatch based on major fcn code.(基于主要fcn代码的调度。)
	switch(pIrpStack->MajorFunction)
	{
		case IRP_MJ_CREATE:
			if(refCount != (ULONG)-1) refCount++;
			status = STATUS_SUCCESS;
			break;
		case IRP_MJ_CLOSE:
			if(refCount != (ULONG)-1) refCount--;
			status = STATUS_SUCCESS;
			break;

		case IRP_MJ_DEVICE_CONTROL:
			//  Dispatch on IOCTL
			switch(pIrpStack->Parameters.DeviceIoControl.IoControlCode)
			{
			case IOCTL_OLS_GET_DRIVER_VERSION:
				*(PULONG)pIrp->AssociatedIrp.SystemBuffer = OLS_DRIVER_VERSION;
				pIrp->IoStatus.Information = 4;
				status = STATUS_SUCCESS;
				break;

			case IOCTL_OLS_GET_REFCOUNT:
				*(PULONG)pIrp->AssociatedIrp.SystemBuffer = refCount;
				pIrp->IoStatus.Information = sizeof(refCount);
				status = STATUS_SUCCESS;
				break;

			case IOCTL_OLS_READ_MSR:
				status = ReadMsr(
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;
			case IOCTL_OLS_WRITE_MSR:
				status = WriteMsr(
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;
			case IOCTL_OLS_READ_PMC:
				status = ReadPmc(
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;
			case IOCTL_OLS_HALT:
				__halt();
				status = STATUS_SUCCESS;
				break;

			case IOCTL_OLS_READ_IO_PORT:
			case IOCTL_OLS_READ_IO_PORT_BYTE:
			case IOCTL_OLS_READ_IO_PORT_WORD:
			case IOCTL_OLS_READ_IO_PORT_DWORD:
				status = ReadIoPort(
					pIrpStack->Parameters.DeviceIoControl.IoControlCode,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;
			case IOCTL_OLS_WRITE_IO_PORT:
			case IOCTL_OLS_WRITE_IO_PORT_BYTE:
			case IOCTL_OLS_WRITE_IO_PORT_WORD:
			case IOCTL_OLS_WRITE_IO_PORT_DWORD:
				status = WriteIoPort(
					pIrpStack->Parameters.DeviceIoControl.IoControlCode,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;

			case IOCTL_OLS_READ_PCI_CONFIG:
				status = ReadPciConfig(
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;
			case IOCTL_OLS_WRITE_PCI_CONFIG:
				status = WritePciConfig(
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;

			case IOCTL_OLS_READ_MEMORY:
				status = ReadMemory(
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;
			case IOCTL_OLS_WRITE_MEMORY:
				status = WriteMemory(
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
					pIrp->AssociatedIrp.SystemBuffer,
					pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
					(ULONG*)&pIrp->IoStatus.Information
					);
				break;
			}
			break;
	}

	// We're done with I/O request.  Record the status of the I/O action.
	// I/O请求已完成。记录I/O操作的状态。
	pIrp->IoStatus.Status = status;

	// Don't boost priority when returning since this took little time.
	// 返回时不要提高优先级，因为这只需要很少的时间。
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;
}

VOID
Unload(
	IN PDRIVER_OBJECT DriverObject
	)
/*++

Routine Description:

	This routine is called by the I/O system to unload the driver.

	Any resources previously allocated must be freed.

Arguments:

	DriverObject - a pointer to the object that represents our driver.

Return Value:

	None
--*/

{
	PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
	UNICODE_STRING win32NameString;

	PAGED_CODE();

	// Create counted string version of our Win32 device name.
	RtlInitUnicodeString(&win32NameString, DOS_DEVICE_NAME);

	// Delete the link from our device name to a name in the Win32 namespace.
	IoDeleteSymbolicLink(&win32NameString);

	if(deviceObject != NULL) IoDeleteDevice(deviceObject);
}

//-----------------------------------------------------------------------------
//
// CPU
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadMsr(	void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	__try
	{
		ULONGLONG data = __readmsr(*(ULONG*)lpInBuffer);
		memcpy((PULONG)lpOutBuffer, &data, 8);
		*lpBytesReturned = 8;
		return STATUS_SUCCESS;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		*lpBytesReturned = 0;
		return STATUS_UNSUCCESSFUL;
	}
}

NTSTATUS
WriteMsr(	void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	__try
	{
		OLS_WRITE_MSR_INPUT* param;
		param = (OLS_WRITE_MSR_INPUT*)lpInBuffer;

		__writemsr(param->Register, param->Value.QuadPart);
		*lpBytesReturned = 0;
		return STATUS_SUCCESS;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		*lpBytesReturned = 0;
		return STATUS_UNSUCCESSFUL;
	}
}

NTSTATUS
ReadPmc(	void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	__try
	{
		ULONGLONG data = __readpmc(*(ULONG*)lpInBuffer);
		memcpy((PULONG)lpOutBuffer, &data, 8);
		*lpBytesReturned = 8;
		return STATUS_SUCCESS;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		*lpBytesReturned = 0;
		return STATUS_UNSUCCESSFUL;
	}
}

//-----------------------------------------------------------------------------
//
// IO Port
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadIoPort( ULONG	ioControlCode,
			void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	ULONG nPort = *(ULONG*)lpInBuffer;

	switch(ioControlCode)
	{
		case IOCTL_OLS_READ_IO_PORT_BYTE:
			*(PUCHAR)lpOutBuffer = READ_PORT_UCHAR((PUCHAR)(ULONG_PTR)nPort);
			break;
		case IOCTL_OLS_READ_IO_PORT_WORD:
			*(PUSHORT)lpOutBuffer = READ_PORT_USHORT((PUSHORT)(ULONG_PTR)nPort);
			break;
		case IOCTL_OLS_READ_IO_PORT_DWORD:
			*(PULONG)lpOutBuffer = READ_PORT_ULONG((PULONG)(ULONG_PTR)nPort);
			break;
		default:
			*lpBytesReturned = 0;
			return STATUS_INVALID_PARAMETER;
			break;
	}
	
	*lpBytesReturned = nInBufferSize;
	return STATUS_SUCCESS;
}

NTSTATUS
WriteIoPort(ULONG	ioControlCode,
			void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	ULONG nPort;
	OLS_WRITE_IO_PORT_INPUT* param;
	
	param = (OLS_WRITE_IO_PORT_INPUT*)lpInBuffer;
	nPort = param->PortNumber;

	switch(ioControlCode)
	{

		case IOCTL_OLS_WRITE_IO_PORT_BYTE:
			WRITE_PORT_UCHAR((PUCHAR)(ULONG_PTR)nPort, param->CharData);
			break;
		case IOCTL_OLS_WRITE_IO_PORT_WORD:
			WRITE_PORT_USHORT((PUSHORT)(ULONG_PTR)nPort, param->ShortData);
			break;
		case IOCTL_OLS_WRITE_IO_PORT_DWORD:
			WRITE_PORT_ULONG((PULONG)(ULONG_PTR)nPort, param->LongData);
			break;
		default:
			return STATUS_INVALID_PARAMETER;
			break;
	}

	return STATUS_SUCCESS;
}

//-----------------------------------------------------------------------------
//
// PCI
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadPciConfig(	void	*lpInBuffer, 
				ULONG	nInBufferSize, 
				void	*lpOutBuffer, 
				ULONG	nOutBufferSize, 
				ULONG	*lpBytesReturned)
{
	OLS_READ_PCI_CONFIG_INPUT *param;
	NTSTATUS status;

	if(nInBufferSize != sizeof(OLS_READ_PCI_CONFIG_INPUT))
	{
		return STATUS_INVALID_PARAMETER;
	}
	param = (OLS_READ_PCI_CONFIG_INPUT *)lpInBuffer;

	status = pciConfigRead(param->PciAddress, param->PciOffset, lpOutBuffer, nOutBufferSize);

	if(status == STATUS_SUCCESS)
	{
		*lpBytesReturned = nOutBufferSize;
	}
	else
	{
		*lpBytesReturned = 0;
	}

	return status;
}

NTSTATUS
WritePciConfig(	void *lpInBuffer, 
				ULONG nInBufferSize, 
				void *lpOutBuffer, 
				ULONG nOutBufferSize, 
				ULONG *lpBytesReturned)
{
	OLS_WRITE_PCI_CONFIG_INPUT *param;
	ULONG writeSize;
	//NTSTATUS status;

	if(nInBufferSize < offsetof(OLS_WRITE_PCI_CONFIG_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	param = (OLS_WRITE_PCI_CONFIG_INPUT *)lpInBuffer;
	writeSize = nInBufferSize - offsetof(OLS_WRITE_PCI_CONFIG_INPUT, Data);
	
	*lpBytesReturned = 0;

	return pciConfigWrite(param->PciAddress, param->PciOffset,
							&param->Data, writeSize);

}

//-----------------------------------------------------------------------------
//
// Support Function
//
//-----------------------------------------------------------------------------

NTSTATUS pciConfigRead(ULONG pciAddress, ULONG offset, void *data, int length)
{
	PCI_SLOT_NUMBER slot;
	int error;
	ULONG busNumber;

	busNumber = PciGetBus(pciAddress);
	slot.u.AsULONG = 0;
	slot.u.bits.DeviceNumber = PciGetDev(pciAddress);
	slot.u.bits.FunctionNumber = PciGetFunc(pciAddress);
	error = HalGetBusDataByOffset(PCIConfiguration, busNumber, slot.u.AsULONG, data, offset, length);

	if(error == 0)
	{
		return OLS_ERROR_PCI_BUS_NOT_EXIST;
	}
	else if(length != 2 && error == 2)
	{
		return OLS_ERROR_PCI_NO_DEVICE;
	}
	else if(length != error)
	{
		return OLS_ERROR_PCI_READ_CONFIG;
	}

	return STATUS_SUCCESS;
}

NTSTATUS pciConfigWrite(ULONG pciAddress, ULONG offset, void *data, int length)
{
	PCI_SLOT_NUMBER slot;
	int error;
	ULONG busNumber;

	busNumber = PciGetBus(pciAddress);

	slot.u.AsULONG = 0;
	slot.u.bits.DeviceNumber = PciGetDev(pciAddress);
	slot.u.bits.FunctionNumber = PciGetFunc(pciAddress);
	error = HalSetBusDataByOffset(PCIConfiguration, busNumber, slot.u.AsULONG,
									data, offset, length);

	if(error != length)
	{
		return OLS_ERROR_PCI_WRITE_CONFIG;
	}

	return STATUS_SUCCESS;
}


//-----------------------------------------------------------------------------
//
// Physical Memory
//
//-----------------------------------------------------------------------------

NTSTATUS
ReadMemory(	void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
	OLS_READ_MEMORY_INPUT *param;
	ULONG	size;
	PHYSICAL_ADDRESS address;
	PVOID	maped;
	BOOLEAN	error;

	if(nInBufferSize != sizeof(OLS_READ_MEMORY_INPUT))
	{
		return STATUS_INVALID_PARAMETER;
	}

	param = (OLS_READ_MEMORY_INPUT *)lpInBuffer;
	size = param->UnitSize * param->Count;

	if(nOutBufferSize < size)
	{
		return STATUS_INVALID_PARAMETER;
	}

	address.QuadPart = param->Address.QuadPart;

#ifndef _PHYSICAL_MEMORY_SUPPORT

	if(0x000C0000 > address.QuadPart 
	|| (address.QuadPart + size - 1) > 0x000FFFFF)
	{
		return STATUS_INVALID_PARAMETER;
	}

#endif

	maped = MmMapIoSpace(address, size, FALSE);

	error = FALSE;
	switch(param->UnitSize){
		case 1:
			READ_REGISTER_BUFFER_UCHAR(maped, lpOutBuffer, param->Count);
			break;
		case 2:
			READ_REGISTER_BUFFER_USHORT(maped, lpOutBuffer, param->Count);
			break;
		case 4:
			READ_REGISTER_BUFFER_ULONG(maped, lpOutBuffer, param->Count);
			break;
		default:
			error = TRUE;
			break;
	}

	MmUnmapIoSpace(maped, size);

	if(error)
	{
		return STATUS_INVALID_PARAMETER;
	}

	*lpBytesReturned = nOutBufferSize;

	return STATUS_SUCCESS;
}

NTSTATUS
WriteMemory(void	*lpInBuffer, 
			ULONG	nInBufferSize, 
			void	*lpOutBuffer, 
			ULONG	nOutBufferSize, 
			ULONG	*lpBytesReturned)
{
#ifdef _PHYSICAL_MEMORY_SUPPORT

	OLS_WRITE_MEMORY_INPUT *param;
	ULONG size;
	PHYSICAL_ADDRESS address;
	PVOID	maped;
	BOOLEAN	error;

	if(nInBufferSize < offsetof(OLS_WRITE_MEMORY_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	param = (OLS_WRITE_MEMORY_INPUT *)lpInBuffer;

	size = param->UnitSize * param->Count;
	if (nInBufferSize < size + offsetof(OLS_WRITE_MEMORY_INPUT, Data))
	{
		return STATUS_INVALID_PARAMETER;
	}

	address.QuadPart = param->Address.QuadPart;

	maped = MmMapIoSpace(address, size, FALSE);

	error = FALSE;
	switch(param->UnitSize){
		case 1:
			WRITE_REGISTER_BUFFER_UCHAR(maped, 
										(UCHAR*)&param->Data, param->Count);
			break;
		case 2:
			WRITE_REGISTER_BUFFER_USHORT(maped,
										(USHORT*)&param->Data, param->Count);
			break;
		case 4:
			WRITE_REGISTER_BUFFER_ULONG(maped,
										(ULONG*)&param->Data, param->Count);
			break;
		default:
			error = TRUE;
			break;
	}

	MmUnmapIoSpace(maped, size);

	if(error)
	{
		return STATUS_INVALID_PARAMETER;
	}

	*lpBytesReturned = 0;

	return STATUS_SUCCESS;

#else

	*lpBytesReturned = 0;
	
	return STATUS_INVALID_PARAMETER;

#endif
}
