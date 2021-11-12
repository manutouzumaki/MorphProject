#ifndef UTILITY_H
#define UTILITY_H

#define BIT(Value) (1 << Value)
#define GET_BIT(BitField, Bit) ((BitField & BIT(Bit)) >> Bit)

struct button
{
    bool IsDown;
    bool WasDown; 
};

#define NUMB_OF_BUTTONS 7

union buttons
{
    struct
    {
        button Up;
        button Down;
        button Left;
        button Right;
        button Start;
        button Back;
        button Debug;
    };
    button Buttons[NUMB_OF_BUTTONS];
};

#define NUMB_OF_MOUSE_BUTTONS 3

union mouse_buttons
{
    struct
    {
        button Left;
        button Middle;
        button Right;
    };
    button Buttons[NUMB_OF_MOUSE_BUTTONS];
};

struct input
{
    // Gameplay Buttons
    buttons *Buttons;

    // Debug and editor Buttons
    i32 MouseX, MouseY;
    mouse_buttons *MouseButtons;
};

#pragma pack(push, 1)
struct bitmap_header
{
	u16 FileType;        /* File type, always 4D42h ("BM") */
	u32 FileSize;        /* Size of the file in bytes */
	u16 Reserved1;       /* Always 0 */
	u16 Reserved2;       /* Always 0 */
	u32 BitmapOffset;    /* Starting position of image data in bytes */

	u32 Size;            /* Size of this header in bytes */
	i32 Width;           /* Image width in pixels */
	i32 Height;          /* Image height in pixels */
	u16 Planes;          /* Number of color planes */
	u16 BitsPerPixel;    /* Number of bits per pixel */
    u32 Compression;     /* Compression methods used */
	u32 SizeOfBitmap;    /* Size of bitmap in bytes */
	i32 HorzResolution;  /* Horizontal resolution in pixels per meter */
	i32 VertResolution;  /* Vertical resolution in pixels per meter */
	u32 ColorsUsed;      /* Number of colors in the image */
	u32 ColorsImportant; /* Minimum number of important colors */
};
#pragma pack(pop)

// this is a opaque struct define in directx.h and opengl.h
//struct texture;

struct bitmap
{
    void *Pixels;
    size_t Size;
    i32 Width;
    i32 Height;
    i32 BitsPerPixel;
    u32 *ColorPalette;
};

struct texture_info
{
    i32 Width;
    i32 Height;
    void *Pixels;
};

void *ReadEntireFile(char *FileName, size_t *FileSizePtr, arena *Arena)
{
    HANDLE FileHandle =  CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, 0,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if(FileHandle == INVALID_HANDLE_VALUE)
    {
        OutputDebugString("ERROR::OPENING::FILE\n");
        return 0;
    }
    LARGE_INTEGER FileSize;
    GetFileSizeEx(FileHandle, &FileSize);
    void *FileBuffer = PushData(Arena, FileSize.QuadPart);
    if(FileSizePtr) *FileSizePtr = (size_t)FileSize.QuadPart;
    if(!ReadFile(FileHandle, FileBuffer, (DWORD)FileSize.QuadPart, 0, 0))
    {
        OutputDebugString("ERROR::READING::FILE\n");
        return 0;
    }
    return FileBuffer;
}

bool WriteEntireFile(char* FileName, u32 MemorySize, void* Memory)
{
    bool Result = false;
    HANDLE FileHandle = CreateFileA(FileName,
                                    GENERIC_WRITE, 0,
                                    0, CREATE_ALWAYS, 0, 0);
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten;
        if(WriteFile(FileHandle, Memory, MemorySize, &BytesWritten, 0))
        {
            // NOTE: File Read Successfully.
            Result = (BytesWritten == MemorySize);
        }
        else
        {
            OutputDebugString("Error Written the FILE\n");
        }
        CloseHandle(FileHandle);
    }
    else
    {
        OutputDebugString("Error Opening FILE\n");
    }
    return(Result);
}

bitmap LoadBMP(char *FileName, arena *Arena)
{
    bitmap Result = {};   
    void *BMPFile = ReadEntireFile(FileName, &Result.Size, Arena);
    if(!BMPFile)
    {
        OutputDebugString("ERROR::LOADING::BMP\n");
        return Result;
    }
    bitmap_header *BMPHeader = (bitmap_header *)BMPFile;
    Result.Width = BMPHeader->Width;
    Result.Height = BMPHeader->Height;
    Result.BitsPerPixel = BMPHeader->BitsPerPixel;
    if(Result.BitsPerPixel == 8)
    {
        Result.ColorPalette = (u32 *)((u8 *)BMPFile + sizeof(bitmap_header));
        int StopHere = 0;
    }
    Result.Pixels = (void *)((u8 *)BMPFile + BMPHeader->BitmapOffset);
    return Result;
}

i32 StringLength(char * String)
{
    i32 Count = 0;
    while(*String++)
    {
        ++Count;
    }
    return Count;
}

bool StringCompare(char *A, char *B, i32 Size)
{
    bool Result = true;
    for(i32 Index = 0;
        Index < Size;
        ++Index)
    {
        if(*A++ != *B++)
        {
            Result = false;
            break;
        } 
    }
    return Result;
}

#endif
