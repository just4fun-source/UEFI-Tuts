// UEFI From Scratch - ThatOSDev ( 2021 - 2022 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "efilibs.h"

void itoa(unsigned long int n, unsigned short int* buffer, int basenumber)
{
	unsigned long int hold;
	int i, j;
	hold = n;
	i = 0;
	
	do{
		hold = n % basenumber;
		buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
	} while(n /= basenumber);
	buffer[i--] = 0;
	
	for(j = 0; j < i; j++, i--)
	{
		hold = buffer[j];
		buffer[j] = buffer[i];
		buffer[i] = hold;
	}
}

EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE *ST)
{
    ImageHandle = IH;
    SystemTable = ST;
	
	InitializeSystem();
	
	SetColor(EFI_WHITE);
	
    Print(L"Loading ThatOS64 ...\r\n\r\n");
    
	readFile(u"ThatOS64\\loader.bin");


	UINT8* loader = (UINT8*)OSBuffer_Handle;
	
	bi.MagicNumber = 31337;
	
	// TODO : Setup and grab address of memory MAP
	
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
	
    int (*KernelBinFile)(int, BLOCKINFO*) = ((__attribute__((ms_abi)) int (*)(int, BLOCKINFO*) ) &loader[262]);
    int g = KernelBinFile(0, &bi);
    
	SetTextPosition(70, 24);
    UINT16 tmp[8];
    itoa(g, tmp, 10);
    SetColor(EFI_LIGHTMAGENTA);
    Print(tmp);

    SetTextPosition(5, 24);
    SetColor(EFI_YELLOW);
	Print(L"Q to quit | R to Reboot");

    while(1)
    {
		Delay1();

		if(CheckKey() == EFI_SUCCESS)
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
    return EFI_SUCCESS;
}
