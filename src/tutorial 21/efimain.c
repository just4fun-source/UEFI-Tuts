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

	SetCursorPosition(50, 100);
	SetFontSpacing(2);
	SetFontSize(8);
	
    printf("Welcome to the UEFI From Scratch Tutorials\r\n");

	SetGraphicsColor(GREEN);
	SetFontSize(5);
	
    printf("\r\nHit Any Key to use the printf() function.\r\n");

    HitAnyKey();

	SetGraphicsColor(ORANGE);
	
    printf("Hello Graphical World !!!");
	
	SetGraphicsColor(GRAY);
	SetFontSpacing(1);
	SetFontSize(6);
	SetCursorPosition(100, 300);
	
	printf("Q to quit | R to Reboot");
	
	ResetKeyboard();

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
