// UEFI From Scratch - ThatOSDev ( 2021 - 2022 )
// https://github.com/ThatOSDev/C_UEFI

#include "efi.h"
#include "efilibs.h"

EFI_STATUS efi_main(EFI_HANDLE IH, EFI_SYSTEM_TABLE *ST)
{
    ImageHandle = IH;
    SystemTable = ST;
	
	InitializeSystem();
	
	SetColor(EFI_WHITE);
	
    Print(L"You may now shutdown computer\r\n\r\n");
    
   while(1){__asm__ ("hlt");}
	
    // We should not make it to this point.
    return EFI_SUCCESS;
}
