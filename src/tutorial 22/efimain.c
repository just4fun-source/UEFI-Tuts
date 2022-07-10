// UEFI From Scratch - ThatOSDev ( 2021 - 2022 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "ErrorCodes.h"
#include "tosdfont.h"
#include "efilibs.h"

EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE *ST)
{
    ImageHandle = IH;
    SystemTable = ST;
    
    ResetScreen();
	
    InitializeGOP();
	
	InitializeFILESYSTEM();
    
	SetCursorPosition(50, 10);
	SetFontSpacing(2);
	SetFontSize(8);
	
    printf("Welcome to the UEFI From Scratch Tutorials\r\n");
    
	readFile(u"ThatOS64\\loader.bin");
	
	SetGraphicsColor(ORANGE);
	SetCursorPosition(190, 230);
	SetFontSpacing(2);
	SetFontSize(7);
	
	printf("Jumping to the loader.");
	
	UINT8* loader = (UINT8*)OSBuffer_Handle;
	
	int j = 3456;
	/*
    UINTN                  MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR  *MemoryMap;
    UINTN                  MapKey;
    UINTN                  DescriptorSize;
    UINT32                 DescriptorVersion;
    
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    MemoryMapSize += 2 * DescriptorSize;
    SystemTable->BootServices->AllocatePool(2, MemoryMapSize, (void **)&MemoryMap);
    SystemTable->BootServices->GetMemoryMap(&MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
    SystemTable->BootServices->ExitBootServices(ImageHandle, MapKey);
	*/
    int (*KernelBinFile)(int, char*) = ((__attribute__((ms_abi)) int (*)(int, char*) ) &loader[262]);
    int g = KernelBinFile(j, 0);
    
    UINT16 tmp[8];
    itoa(g, tmp, 10);
    Print(L"\r\n\r\nThe returned number is : ");
    SetColor(EFI_LIGHTMAGENTA);
    Print(tmp);

	SetGraphicsColor(GRAY);
	SetFontSpacing(1);
	SetFontSize(6);
	SetCursorPosition(100, 500);
	
	printf("Q to quit | R to Reboot");

    while(1)
    {
		Delay1();
		
		EFI_STATUS Status = CheckKey();
		if(Status == EFI_SUCCESS)
		{
			if(GetKey('q') == 1)
			{
				SHUTDOWN();
				break;
			}
			if(GetKey('r') == 1)
			{
				COLD_REBOOT();
				break;
			}
		}

	}

    // We should not make it to this point.
    COLD_REBOOT();

    // We should not make it to this point.
    return EFI_SUCCESS;
}
