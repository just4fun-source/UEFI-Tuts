// UEFI From Scratch - ThatOSDev ( 2021 - 2022 )
// https://github.com/ThatOSDev/C_UEFI

#ifndef EFILIBS_H
#define EFILIBS_H

#define NULL (void*)0

EFI_HANDLE ImageHandle;
EFI_SYSTEM_TABLE* SystemTable;
EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume;
EFI_FILE_PROTOCOL* RootFS;
void* OSBuffer_Handle;
UINT64* LoaderFileSize;

unsigned long long strlen(const char* str)
{
	const char* strCount = str;

	while (*strCount++);
	return strCount - str - 1;
}

void SetTextPosition(UINT32 Col, UINT32 Row)
{
    SystemTable->ConOut->SetCursorPosition(SystemTable->ConOut, Col, Row);
}

void ResetScreen()
{
    SystemTable->ConOut->Reset(SystemTable->ConOut, 1);
}

void ClearScreen()
{
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
}

void SetColor(UINTN Attribute)
{
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut, Attribute);
}

void Print(CHAR16* str)
{
    SystemTable->ConOut->OutputString(SystemTable->ConOut, str);
}

typedef struct BLOCKINFO
{
    unsigned long long*    BaseAddress;
    unsigned long long     BufferSize;
    unsigned int           ScreenWidth;
    unsigned int           ScreenHeight;
    unsigned int           PixelsPerScanLine;
	unsigned long long*    LoaderFileSize;
	EFI_MEMORY_DESCRIPTOR* MMap;
	unsigned long long     MMapSize;
	unsigned long long     MMapDescriptorSize;
} __attribute__((__packed__)) BLOCKINFO;

BLOCKINFO bi;

void InitializeFILESYSTEM()
{
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
	EFI_DEVICE_PATH_PROTOCOL *DevicePath;
	
    if((SystemTable->BootServices->HandleProtocol(ImageHandle, &EFI_LOADED_IMAGE_PROTOCOL_GUID, (void**)&LoadedImage)) == EFI_SUCCESS)
	{
		if((SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_DEVICE_PATH_PROTOCOL_GUID, (void**)&DevicePath)) == EFI_SUCCESS)
		{
			if((SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID, (void**)&Volume)) == EFI_SUCCESS)
			{
				if((Volume->OpenVolume(Volume, &RootFS)) != EFI_SUCCESS)
				{
					Print(L"Loading Root File System FAILED!");
				}
			} else {
		        Print(L"Volume Handle FAILED!");
	        }
		} else {
		    Print(L"DevicePath FAILED!");
	    }
	} else {
		Print(L"LoadedImage FAILED!");
	}
}

EFI_FILE_PROTOCOL* getFile(CHAR16* FileName)
{
    // This opens a file from the EFI FAT32 file system volume.
    // It loads from root, so you must supply full path if the file is not in the root.
    // Example : "somefolder//myfile"  <--- Notice the double forward slash.
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    if((RootFS->Open(RootFS, &FileHandle, FileName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0)) == EFI_NOT_FOUND)
	{
		SetColor(EFI_CYAN);
		Print(L"\r\nWARNING : Unable to find File.\r\n\r\n");
	}
    
    return FileHandle;
}

EFI_FILE_PROTOCOL* getDir(CHAR16* DirName)
{
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    if((RootFS->Open(RootFS, &FileHandle, DirName, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY)) == EFI_NOT_FOUND)
	{
		SetColor(EFI_CYAN);
		Print(L"\r\nWARNING : Unable to find Directory.\r\n\r\n");
	}
    
    return FileHandle;
}

void closeFile(EFI_FILE_PROTOCOL* FileHandle)
{
    if((FileHandle->Close(FileHandle)) != EFI_SUCCESS)
	{
        SetColor(EFI_BROWN);
        Print(L"Closing File Handle FAILED\r\n");
	}
}

void closeDir(EFI_FILE_PROTOCOL* FileHandle)
{
    if((FileHandle->Close(FileHandle)) != EFI_SUCCESS)
	{
		SetColor(EFI_BROWN);
		Print(L"Closing Directory Handle FAILED\r\n");
	}
}

EFI_FILE_PROTOCOL* createFile(CHAR16* FileName)
{
    EFI_FILE_PROTOCOL* FileHandle = NULL;
    if((RootFS->Open(RootFS, &FileHandle, FileName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0)) == EFI_NOT_FOUND)
	{
		SetColor(EFI_CYAN);
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

unsigned int ENTRY_POINT;

void readFile(CHAR16* FileName)
{
	// We get the file size, allocate memory for it,
    // read the file into the buffer, then we close the file.
    EFI_FILE_PROTOCOL* FileHandle = getFile(FileName);
    if(FileHandle != NULL)
    {
		UINT64* FileSize = (UINT64*)GetFileSize(FileHandle);
		if((SystemTable->BootServices->AllocatePool(EfiLoaderData, *FileSize, (void**)&OSBuffer_Handle)) != EFI_SUCCESS)
		{
			SetColor(EFI_BROWN);
			Print(L"Allocating Pool FAILED\r\n");
		}
		
		FileHandle->SetPosition(FileHandle, 0);

        if((FileHandle->Read(FileHandle, FileSize, OSBuffer_Handle)) != EFI_SUCCESS)
		{
			SetColor(EFI_BROWN);
			Print(L"Reading File FAILED\r\n");
		}
		
		LoaderFileSize = FileSize;

		SetColor(EFI_LIGHTCYAN);    
		Print(L"\r\nDynamic File Signature\r\n");
		SetColor(EFI_LIGHTRED);    
		UINT8* test1 = (UINT8*)OSBuffer_Handle;
		
		// 86 64    <---- BIN
		// 45 4C 46 <---- ELF
		
		UINT8 p1,p2,p3,p4;
		p1 = *test1;
		test1+=1;
		p2 = *test1;
		test1+=1;
		p3 = *test1;
		test1+=1;
		p4 = *test1;

		if(p1 == 100 && p2 == 134)
		{
			Print(L"BINARY - 8664 Signature\r\n");
			SetColor(EFI_WHITE);
			test1+=37;
			p1 = *test1;
			test1+=1;
			p2 = *test1;
			test1+=1;
			p3 = *test1;
			test1+=1;
			p4 = *test1;

				UINT16 s[2];
				itoa(p1, s, 16);
				Print(s);
				Print(L"  ");
				
				itoa(p2, s, 16);
				Print(s);
				Print(L"  ");
				
				itoa(p3, s, 16);
				Print(s);
				Print(L"  ");
				
				itoa(p4, s, 16);
				Print(s);
				SetColor(EFI_BROWN); 
				Print(L"  \r\n\r\nENTRY POINT : ");
				SetColor(EFI_GREEN); 
				
                ENTRY_POINT = (p4 << 24) | (p3 << 16) | (p2 << 8) | p1 ;
				
				UINT16 s2[5];
				itoa(ENTRY_POINT, s2, 10);
				Print(s2);
				Print(L"  ");
		}
		else if(p2 == 69 && p3 == 76 && p4 == 70)
		{
			Print(L"ELF - 45 4C 46 Signature\r\n");
			Print(L"Add your own code + the ELF Header file to make this work.");
		}
		
        closeFile(FileHandle);
    }
}

void removeDir(CHAR16* dirName)
{
	EFI_FILE_PROTOCOL* FileHandle = getDir(dirName);
	if(FileHandle != NULL)
	{
		if((FileHandle->Delete(FileHandle)) != EFI_SUCCESS)
		{
			SetColor(EFI_BROWN);
		    Print(L"Deleting Directory FAILED\r\n");
		}
	}
}

void makeDir(CHAR16* dirName)
{
    EFI_FILE_PROTOCOL* FileHandle = NULL;
	
    if((RootFS->Open(RootFS, &FileHandle, dirName, EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, EFI_FILE_DIRECTORY)) != EFI_SUCCESS)
	{
		SetColor(EFI_BROWN);
		Print(L"Creating Directory FAILED\r\n");
	}
	closeDir(FileHandle);
}

void deleteFile(CHAR16* FileName)
{
	EFI_FILE_PROTOCOL* FileHandle = getFile(FileName);
	if(FileHandle != NULL)
	{
		if((FileHandle->Delete(FileHandle)) != EFI_SUCCESS)
		SetColor(EFI_BROWN);
		Print(L"Deleting File FAILED\r\n");
	}
}

void WriteToFile(char* buf, CHAR16* FileName)
{
	UINT64 fileSize = strlen(buf);
	EFI_FILE_PROTOCOL* writefilehandle = createFile(FileName);
	if(writefilehandle != NULL)
	{
		if((writefilehandle->Write(writefilehandle, &fileSize, buf)) != EFI_SUCCESS)
		{
			SetColor(EFI_BROWN);
			Print(L"Writing to File FAILED\r\n");
		}
		closeFile(writefilehandle);
	}
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

void InitializeGOP()
{
    // We initialize the Graphics Output Protocol.
    // This is used instead of the VGA interface.
    SystemTable->BootServices->LocateProtocol(&EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID, 0, (void**)&gop);
	
	UINT32 NewNativeMode = 9000000;
	UINTN  GOPSizeOfInfo      = gop->Mode->SizeOfInfo;
	UINT32 NativeMode         = gop->Mode->Mode;
	UINT32 MaxResolutionModes = gop->Mode->MaxMode;
	
	SetColor(EFI_WHITE);
	UINT16 tmp[4];
    itoa(NativeMode, tmp, 10);
    Print(L"Current Native Mode : ");
    SetColor(EFI_YELLOW);
    Print(tmp);
	
	SetColor(EFI_WHITE);
	itoa(gop->Mode->Info->HorizontalResolution, tmp, 10);
    Print(L"    WIDTH : ");
    SetColor(EFI_LIGHTMAGENTA);
    Print(tmp);
	
	SetColor(EFI_WHITE);
	itoa(gop->Mode->Info->VerticalResolution, tmp, 10);
    Print(L"    HEIGHT : ");
    SetColor(EFI_LIGHTMAGENTA);
    Print(tmp);
	
	SetColor(EFI_BROWN);
	Print(L"\r\nChecking Graphic Modes ... \r\n");
	SetColor(EFI_CYAN);
	EFI_STATUS Status;
	for (UINT32 i = 0; i < MaxResolutionModes; i++)
	{
		EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
        Status = gop->QueryMode(gop, i, &GOPSizeOfInfo, &info);
		if(Status == EFI_SUCCESS)
		{
			SetColor(EFI_WHITE);
			itoa(i, tmp, 10);
			Print(L"Current Native Mode : ");
			SetColor(EFI_YELLOW);
			Print(tmp);
			
			SetColor(EFI_WHITE);
			itoa(info->HorizontalResolution, tmp, 10);
			Print(L"    WIDTH : ");
			SetColor(EFI_LIGHTMAGENTA);
			Print(tmp);
			
			SetColor(EFI_WHITE);
			itoa(info->VerticalResolution, tmp, 10);
			Print(L"    HEIGHT : ");
			SetColor(EFI_LIGHTMAGENTA);
			Print(tmp);
			Print(L"\r\n");
			
			if((info->HorizontalResolution == 1920) && (info->VerticalResolution == 1080))
			{
				NewNativeMode = i;
			}
		}
	}
	if(NewNativeMode == 9000000)
	{
		SetColor(EFI_LIGHTRED);
		Print(L"WARNING : Unable to find 1920 x 1080 MODE !\r\nUsing Default GOP.");
		bi.BaseAddress        = (void*)gop->Mode->FrameBufferBase;
		bi.BufferSize         = gop->Mode->FrameBufferSize;
		bi.ScreenWidth        = gop->Mode->Info->HorizontalResolution;
		bi.ScreenHeight       = gop->Mode->Info->VerticalResolution;
		bi.PixelsPerScanLine  = gop->Mode->Info->PixelsPerScanLine;
	} else {
		SetColor(EFI_GREEN);
		Print(L"Hit Any Key to Change Resolution to 1920 x 1080\r\n");
		HitAnyKey();
		Status = gop->SetMode(gop, NewNativeMode);
		if(Status == EFI_SUCCESS)
		{
			bi.BaseAddress        = (void*)gop->Mode->FrameBufferBase;
			bi.BufferSize         = gop->Mode->FrameBufferSize;
			bi.ScreenWidth        = gop->Mode->Info->HorizontalResolution;
			bi.ScreenHeight       = gop->Mode->Info->VerticalResolution;
			bi.PixelsPerScanLine  = gop->Mode->Info->PixelsPerScanLine;
			
			ResetScreen();
			SetColor(EFI_YELLOW);
			Print(L"CONGRATS ! You are now using a screen resolution of ");
			
			itoa(bi.ScreenWidth, tmp, 10);
			SetColor(EFI_LIGHTMAGENTA);
			Print(tmp);
			
			SetColor(EFI_WHITE);
			Print(L" x ");
			
			itoa(bi.ScreenHeight, tmp, 10);
			SetColor(EFI_LIGHTMAGENTA);
			Print(tmp);
			
			SetColor(EFI_LIGHTCYAN);
			Print(L"\r\nCURRENT MODE : ");
			itoa(NewNativeMode, tmp, 10);
			SetColor(EFI_CYAN);
			Print(tmp);
		} else {
			SetColor(EFI_LIGHTRED);
			Print(L"WARNING : Unable to find 1920 x 1080 MODE !\r\nUsing Default GOP.");
			bi.BaseAddress        = (void*)gop->Mode->FrameBufferBase;
			bi.BufferSize         = gop->Mode->FrameBufferSize;
			bi.ScreenWidth        = gop->Mode->Info->HorizontalResolution;
			bi.ScreenHeight       = gop->Mode->Info->VerticalResolution;
			bi.PixelsPerScanLine  = gop->Mode->Info->PixelsPerScanLine;
		}
	}
	
	Print(L"\r\n");
}

void InitializeSystem()
{
    ResetScreen();
    InitializeGOP();
	InitializeFILESYSTEM();
}

#endif // EFILIBS_H