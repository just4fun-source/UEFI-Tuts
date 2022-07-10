// UEFI From Scratch - ThatOSDev ( 2021 - 2022 )
// https://github.com/ThatOSDev/C_UEFI

#ifndef EFILIBS_H
#define EFILIBS_H

#include "ThatCLib/clibs.h"
#include "ThatCLib/stdlib.h"
#include "ThatCLib/string.h"

             // ARGB             
#define ORANGE 0xffffa500
#define CYAN   0xff00ffff
#define RED    0xffff0000
#define GREEN  0xff00ff00
#define BLUE   0xff0000ff
#define GRAY   0xff888888
#define WHITE  0xffffffff
#define BLACK  0xff000000

EFI_HANDLE ImageHandle;
EFI_SYSTEM_TABLE* SystemTable;
EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
EFI_GRAPHICS_OUTPUT_BLT_PIXEL GraphicsColor;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume;
EFI_FILE_PROTOCOL* RootFS;
void* OSBuffer_Handle;

void SetTextPosition(UINT32 Col, UINT32 Row)
{
    // Sets the Column and Row of the text screen cursor position.
    SystemTable->ConOut->SetCursorPosition(SystemTable->ConOut, Col, Row);
}

void ResetScreen()
{
    // This resets the whole console ( A.K.A. your display screen ) interface.
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
}

void ClearScreen()
{
    // This clears the screen buffer, but does not reset it.
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
}

void SetColor(UINTN Attribute)
{
    // We set the color for the text output.
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, Attribute);
}

void Print(CHAR16* str)
{
    // Text Output  ( A.K.A. ConOut is Console Out )
    SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

void HitAnyKey()
{
    // This clears the keyboard buffer.
    SystemTable->ConIn->Reset(SystemTable->ConIn, 1);

    // We setup the struct to take keyboard input.
    EFI_INPUT_KEY Key;

    // In a while loop to see if the keyboard has a key stroke in the buffer.
    while((SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &Key)) == EFI_NOT_READY);
}

void ResetKeyboard()
{
    SystemTable->ConIn->Reset(SystemTable->ConIn, 1);
}

EFI_INPUT_KEY CheckKeystroke;
BOOLEAN GetKey(CHAR16 key)
{
    if(CheckKeystroke.UnicodeChar == key)
    {
        return 1;
    } else {
        return 0;
    }
}

EFI_STATUS CheckKey()
{
    return SystemTable->ConIn->ReadKeyStroke(SystemTable->ConIn, &CheckKeystroke);
}

// This sets the color of the pixels ( Graphics Color )
void SetGraphicsColor(UINT32 color)
{
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL GColor;
    GColor.Reserved = color >> 24;
    GColor.Red      = color >> 16;
    GColor.Green    = color >> 8;
    GColor.Blue     = color;
    GraphicsColor = GColor;
}

// This positions the pixel in the row and column ( X and Y )
void SetPixel(UINT32 xPos, UINT32 yPos)
{
    // TODO : Add in a choice instead of defaulting to EfiBltVideoFill.
    gop->Blt(gop, &GraphicsColor, EfiBltVideoFill, 0, 0, xPos, yPos, 1, 1, 0);
}

// This creates a filled box of pixels.
// NOTE : Call SetGraphicsColor prior to using this function.
void CreateFilledBox(UINT32 xPos, UINT32 yPos, UINT32 w, UINT32 h)
{
    // TODO : Add in a choice instead of defaulting to EfiBltVideoFill.
    gop->Blt(gop, &GraphicsColor, EfiBltVideoFill, 0, 0, xPos, yPos, w, h, 0);
}

typedef struct GRAPHICSBUFFER
{
    void*  BaseAddress;
    UINT64 BufferSize;
    UINT32 ScreenWidth;
    UINT32 ScreenHeight;
    UINT32 PixelsPerScanLine;
} GRAPHICSBUFFER;

GRAPHICSBUFFER         gBuffer;

// This creates a filled box of pixels using the GRAPHICSBUFFER.
// NOTE : Call SetGraphicsColor prior to using this function.
void CreateBufferFilledBox(UINT32 xPos, UINT32 yPos, UINT32 w, UINT32 h)
{
    // This functions puts a color filled box on the screen
    UINT32 ByteOffset = 4;
    if(xPos < 0){xPos = 0;}
    if(yPos < 0){yPos = 0;}
    if(w < 1){w = 1;}
    if(h < 1){h = 1;}
    UINT32 x;
    UINT32 y      = yPos;
    UINT32 width  = (xPos + w) * ByteOffset;
    UINT32 height = yPos + h;

    for(y = yPos; y <= height; y++)
    {
        for(x = xPos * ByteOffset; x <= width; x+=ByteOffset)
        {
            *(UINT32*)(x + (y * gBuffer.PixelsPerScanLine * ByteOffset) + gBuffer.BaseAddress) = *(UINT32*)&GraphicsColor;
        }
    }
}

void Delay(UINTN d)
{
    // The Stall function is set is microseconds. So you have to convert
    // from microseconds ( µs ) to milliseconds ( ms ). EXAMPLE : 1ms = 1000µs.
    // The microsecond is the smallest unit of time measurement the EFI provides.
    SystemTable->BootServices->Stall(d * 1000);
}

void Delay1()
{
    // The Stall function is set as microseconds. We stall 1 microsecond.
    SystemTable->BootServices->Stall(1);
}

void COLD_REBOOT()
{
    // Hardware Reboot
    SystemTable->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, 0);
}

void WARM_REBOOT()
{
    // Software reboot
    SystemTable->RuntimeServices->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, 0);
}

void SHUTDOWN()
{
    // Shuts off the computer
    // NOTE : This does not work in VirtualBox.
    // WORKS in QEMU !!!
    SystemTable->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, 0);
}

void InitializeFILESYSTEM()
{
    EFI_STATUS Status;
    // To load a file, you must have a file system. EFI takes advantage of the FAT32 file system.
    SetColor(EFI_BROWN);
    Print(L"LoadedImage ... ");
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
    Status = SystemTable->BootServices->HandleProtocol(ImageHandle, &EFI_LOADED_IMAGE_PROTOCOL_GUID, (void**)&LoadedImage);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
    
    SetColor(EFI_BROWN);
    Print(L"DevicePath ... ");
    EFI_DEVICE_PATH_PROTOCOL *DevicePath;
    Status = SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_DEVICE_PATH_PROTOCOL_GUID, (void**)&DevicePath);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
    
    SetColor(EFI_BROWN);
    Print(L"Volume ... ");
    Status = SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID, (void**)&Volume);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
	
    SetColor(EFI_BROWN);
    Print(L"RootFS ... ");
    Status = Volume->OpenVolume(Volume, &RootFS);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
}

EFI_FILE_PROTOCOL* getFile(CHAR16* FileName)
{
    // This opens a file from the EFI FAT32 file system volume.
    // It loads from root, so you must supply full path if the file is not in the root.
    // Example : "somefolder//myfile"  <--- Notice the double forward slash.
    SetColor(EFI_BROWN);
    Print(L"Getting File Handle ... ");
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    EFI_STATUS Status = RootFS->Open(RootFS, &FileHandle, FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
    SetColor(EFI_CYAN);
	if(Status == EFI_NOT_FOUND)
	{
		Print(L"\r\nWARNING : Unable to find File.\r\n\r\n");
	}
    
    return FileHandle;
}

EFI_FILE_PROTOCOL* getDir(CHAR16* DirName)
{
    SetColor(EFI_BROWN);
    Print(L"Opening Directory Handle ... ");
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    EFI_STATUS Status = RootFS->Open(RootFS, &FileHandle, DirName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY);
    SetColor(EFI_CYAN);
	if(Status == EFI_NOT_FOUND)
	{
		Print(L"\r\nWARNING : Unable to find Directory.\r\n\r\n");
	}
    
    return FileHandle;
}

void closeFile(EFI_FILE_PROTOCOL* FileHandle)
{
    // This closes the file.
    SetColor(EFI_BROWN);
    Print(L"Closing File Handle ... ");
    EFI_STATUS Status = FileHandle->Close(FileHandle);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
}

void closeDir(EFI_FILE_PROTOCOL* FileHandle)
{
    SetColor(EFI_BROWN);
    Print(L"Closing Directory Handle ... ");
    EFI_STATUS Status = FileHandle->Close(FileHandle);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
}

EFI_FILE_PROTOCOL* createFile(CHAR16* FileName)
{
    SetColor(EFI_BROWN);
    Print(L"Creating File Handle ... ");
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    EFI_STATUS Status = RootFS->Open(RootFS, &FileHandle, FileName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
    SetColor(EFI_CYAN);
	if(Status == EFI_NOT_FOUND)
	{
		Print(L"\r\nWARNING : Unable to create file. Please check your path.\r\n\r\n");
	}
    
    return FileHandle;
}

UINTN* GetFileSize (EFI_FILE_PROTOCOL* FileName)
{
    UINTN* Size = 0;
    FileName->SetPosition(FileName, 0xFFFFFFFFFFFFFFFFULL);
    FileName->GetPosition(FileName, Size);
	FileName->SetPosition(FileName, 0);
    return Size;
}

void readFile(CHAR16* FileName)
{
	// We get the file size, allocate memory for it,
    // read the file into the buffer, then we close the file.
    EFI_FILE_PROTOCOL* FileHandle = getFile(FileName);
    if(FileHandle != NULL)
    {
		UINT64* FileSize = (UINT64*)GetFileSize(FileHandle);
		
        SetColor(EFI_BROWN);
        Print(L"Allocating Pool ... ");
        EFI_STATUS Status = SystemTable->BootServices->AllocatePool(EfiLoaderData, *FileSize, (void**)&OSBuffer_Handle);
        SetColor(EFI_CYAN);
        Print(CheckStandardEFIError(Status));
    
        SetColor(EFI_BROWN);
        Print(L"Reading File ... ");
        Status = FileHandle->Read(FileHandle, FileSize, OSBuffer_Handle);
        SetColor(EFI_CYAN);
        Print(CheckStandardEFIError(Status));

        closeFile(FileHandle);
    }
}

void removeDir(CHAR16* dirName)
{
	EFI_FILE_PROTOCOL* FileHandle = getDir(dirName);
	if(FileHandle != NULL)
	{
		SetColor(EFI_BROWN);
		Print(L"Deleting Directory ... ");
		EFI_STATUS Status = FileHandle->Delete(FileHandle);
		SetColor(EFI_CYAN);
		Print(CheckStandardEFIError(Status));
	}
}

void makeDir(CHAR16* dirName)
{
    SetColor(EFI_BROWN);
    Print(L"Creating Directory Handle ... ");
    EFI_FILE_PROTOCOL* FileHandle = NULL;
	Print(L"Creating Directory ... ");
    EFI_STATUS Status = RootFS->Open(RootFS, &FileHandle, dirName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY);
    SetColor(EFI_CYAN);
    Print(CheckStandardEFIError(Status));
	closeDir(FileHandle);
}

void deleteFile(CHAR16* FileName)
{
	EFI_FILE_PROTOCOL* FileHandle = getFile(FileName);
	if(FileHandle != NULL)
	{
		SetColor(EFI_BROWN);
		Print(L"Deleting File ... ");
		EFI_STATUS Status = FileHandle->Delete(FileHandle);
		SetColor(EFI_CYAN);
		Print(CheckStandardEFIError(Status));
	}
}

void WriteToFile(char* buf, CHAR16* FileName)
{
	UINT64 fileSize = strlen(buf);
	EFI_FILE_PROTOCOL* writefilehandle = createFile(FileName);
	if(writefilehandle != NULL)
	{
        SetColor(EFI_BROWN);
		Print(L"Writing to File ... ");
		EFI_STATUS Status = writefilehandle->Write(writefilehandle, &fileSize, buf);
		SetColor(EFI_CYAN);
		Print(CheckStandardEFIError(Status));

		closeFile(writefilehandle);
	}
}

typedef struct PIXELPOSITIONS
{
    UINT32 PixelxPos;
    UINT32 PixelyPos;
} PIXELPOSITIONS;

PIXELPOSITIONS* pixelpos;

// Font / printf()
UINT32 FontSpacing;
void SetFontSpacing(UINT32 fspacing)
{
	if(fspacing < 0) { fspacing = 0; }
	FontSpacing = fspacing;
}

UINT32 CurrentFontSize;
void SetFontSize(UINT32 fs)
{
	if(fs < 4) { fs = 4; }
	CurrentFontSize = (UINT32)((fs / 4));
}

void GetCharacter(UINT32 character, UINT32 xPos, UINT32 yPos)
{
	pixelpos->PixelxPos = xPos;
	pixelpos->PixelyPos = yPos;
	UINT32 SelectASCII  = character;

	UINT32 mcX = pixelpos->PixelxPos;
	UINT32 mcY = pixelpos->PixelyPos;
	UINT32 pPos = 0;
	UINT32 sPos = 0;
	UINT32 FontSize = CurrentFontSize;

	for(UINT32 t = (SelectASCII * 128); t < ((SelectASCII * 128) + 128); t++)
	{
		if(pPos > 7)
		{
			pPos = 0;
			mcY += FontSize;
			mcX = pixelpos->PixelxPos;
			sPos++;
			if(sPos > 15)
			{
				sPos = 0;
				mcY += FontSize;
			}
		}
		UINT32 ASCIIColor = asciifont[t];
		switch(ASCIIColor)
		{
			case 0:
			{
				break;
			}
			case 1:
			{
				CreateFilledBox(mcX, mcY, FontSize, FontSize);
				break;
			}
		}
		mcX += FontSize;
		pPos++;
	}
}

typedef struct CPOS
{
	UINT32 cmX;
	UINT32 cmY;
} CPOS;

CPOS* cursorPos;

void SetCursorPosition(UINT32 x, UINT32 y)
{
	cursorPos->cmX = x;
	cursorPos->cmY = y;
	if(cursorPos->cmX < 0){cursorPos->cmX = 0;}
	if(cursorPos->cmY < 0){cursorPos->cmY = 0;}
}

void printf(void* str) // Our Custom printf function.
{
	for (char *i = str; *i; i++)
	{
		if(*i == 10)
		{
			cursorPos->cmY += 10 + ((CurrentFontSize * 5) + FontSpacing);
		} else if(*i == 13)
		{
			cursorPos->cmX = 0;
		} else {
			GetCharacter(*i, cursorPos->cmX, cursorPos->cmY);
		    cursorPos->cmX += ((CurrentFontSize * 7) + FontSpacing);
		}
    }
}

GRAPHICSBUFFER* InitializeGOP()
{
    // We initialize the Graphics Output Protocol.
    // This is used instead of the VGA interface.
    EFI_STATUS Status = SystemTable->BootServices->LocateProtocol(&EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, 0, (void**)&gop);
    if(Status != EFI_SUCCESS)
    {
        return NULL;
    }
	
    gBuffer.BaseAddress        = (void*)gop->Mode->FrameBufferBase;
    gBuffer.BufferSize         = gop->Mode->FrameBufferSize;
    gBuffer.ScreenWidth        = gop->Mode->Info->HorizontalResolution;
    gBuffer.ScreenHeight       = gop->Mode->Info->VerticalResolution;
    gBuffer.PixelsPerScanLine  = gop->Mode->Info->PixelsPerScanLine;
    
	SetGraphicsColor(WHITE);
	SetCursorPosition(0, 0);
	SetFontSpacing(0);
	SetFontSize(4);
    
	return &gBuffer;
}

#endif // EFILIBS_H