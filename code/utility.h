#ifndef UTILITY_H
#define UTILITY_H

#define BIT(Value) (1 << Value)
#define SET_BIT(BitField, Bit) (BitField |= BIT(Bit))
#define UNSET_BIT(BitField, Bit) (BitField &= ~(BIT(Bit)))
#define GET_BIT(BitField, Bit) ((BitField & BIT(Bit)) >> Bit)

enum input_layout_classification
{
    INPUT_PER_VERTEX_DATA,
    INPUT_PER_INSTANCE_DATA
};

enum input_layout_format
{
    FORMAT_UNKNOWN,
    FORMAT_R32G32B32A32_TYPELESS,
    FORMAT_R32G32B32A32_FLOAT,
    FORMAT_R32G32B32A32_UINT,
    FORMAT_R32G32B32A32_SINT,
    FORMAT_R32G32B32_TYPELESS,
    FORMAT_R32G32B32_FLOAT,
    FORMAT_R32G32B32_UINT,
    FORMAT_R32G32B32_SINT,
    FORMAT_R16G16B16A16_TYPELESS,
    FORMAT_R16G16B16A16_FLOAT,
    FORMAT_R16G16B16A16_UNORM,
    FORMAT_R16G16B16A16_UINT,
    FORMAT_R16G16B16A16_SNORM,
    FORMAT_R16G16B16A16_SINT,
    FORMAT_R32G32_TYPELESS,
    FORMAT_R32G32_FLOAT,
    FORMAT_R32G32_UINT,
    FORMAT_R32G32_SINT,
    FORMAT_R32G8X24_TYPELESS,
    FORMAT_D32_FLOAT_S8X24_UINT,
    FORMAT_R32_FLOAT_X8X24_TYPELESS,
    FORMAT_X32_TYPELESS_G8X24_UINT,
    FORMAT_R10G10B10A2_TYPELESS,
    FORMAT_R10G10B10A2_UNORM,
    FORMAT_R10G10B10A2_UINT,
    FORMAT_R11G11B10_FLOAT,
    FORMAT_R8G8B8A8_TYPELESS,
    FORMAT_R8G8B8A8_UNORM,
    FORMAT_R8G8B8A8_UNORM_SRGB,
    FORMAT_R8G8B8A8_UINT,
    FORMAT_R8G8B8A8_SNORM,
    FORMAT_R8G8B8A8_SINT,
    FORMAT_R16G16_TYPELESS,
    FORMAT_R16G16_FLOAT,
    FORMAT_R16G16_UNORM,
    FORMAT_R16G16_UINT,
    FORMAT_R16G16_SNORM,
    FORMAT_R16G16_SINT,
    FORMAT_R32_TYPELESS,
    FORMAT_D32_FLOAT,
    FORMAT_R32_FLOAT,
    FORMAT_R32_UINT,
    FORMAT_R32_SINT,
    FORMAT_R24G8_TYPELESS,
    FORMAT_D24_UNORM_S8_UINT,
    FORMAT_R24_UNORM_X8_TYPELESS,
    FORMAT_X24_TYPELESS_G8_UINT,
    FORMAT_R8G8_TYPELESS,
    FORMAT_R8G8_UNORM,
    FORMAT_R8G8_UINT,
    FORMAT_R8G8_SNORM,
    FORMAT_R8G8_SINT,
    FORMAT_R16_TYPELESS,
    FORMAT_R16_FLOAT,
    FORMAT_D16_UNORM,
    FORMAT_R16_UNORM,
    FORMAT_R16_UINT,
    FORMAT_R16_SNORM,
    FORMAT_R16_SINT,
    FORMAT_R8_TYPELESS,
    FORMAT_R8_UNORM,
    FORMAT_R8_UINT,
    FORMAT_R8_SNORM,
    FORMAT_R8_SINT,
    FORMAT_A8_UNORM,
    FORMAT_R1_UNORM,
    FORMAT_R9G9B9E5_SHAREDEXP,
    FORMAT_R8G8_B8G8_UNORM,
    FORMAT_G8R8_G8B8_UNORM,
    FORMAT_BC1_TYPELESS,
    FORMAT_BC1_UNORM,
    FORMAT_BC1_UNORM_SRGB,
    FORMAT_BC2_TYPELESS,
    FORMAT_BC2_UNORM,
    FORMAT_BC2_UNORM_SRGB,
    FORMAT_BC3_TYPELESS,
    FORMAT_BC3_UNORM,
    FORMAT_BC3_UNORM_SRGB,
    FORMAT_BC4_TYPELESS,
    FORMAT_BC4_UNORM,
    FORMAT_BC4_SNORM,
    FORMAT_BC5_TYPELESS,
    FORMAT_BC5_UNORM,
    FORMAT_BC5_SNORM,
    FORMAT_B5G6R5_UNORM,
    FORMAT_B5G5R5A1_UNORM,
    FORMAT_B8G8R8A8_UNORM,
    FORMAT_B8G8R8X8_UNORM,
    FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
    FORMAT_B8G8R8A8_TYPELESS,
    FORMAT_B8G8R8A8_UNORM_SRGB,
    FORMAT_B8G8R8X8_TYPELESS,
    FORMAT_B8G8R8X8_UNORM_SRGB,
    FORMAT_BC6H_TYPELESS,
    FORMAT_BC6H_UF16,
    FORMAT_BC6H_SF16,
    FORMAT_BC7_TYPELESS,
    FORMAT_BC7_UNORM,
    FORMAT_BC7_UNORM_SRGB,
    FORMAT_AYUV,
    FORMAT_Y410,
    FORMAT_Y416,
    FORMAT_NV12,
    FORMAT_P010,
    FORMAT_P016,
    FORMAT_420_OPAQUE,
    FORMAT_YUY2,
    FORMAT_Y210,
    FORMAT_Y216,
    FORMAT_NV11,
    FORMAT_AI44,
    FORMAT_IA44,
    FORMAT_P8,
    FORMAT_A8P8,
    FORMAT_B4G4R4A4_UNORM,
    FORMAT_P208,
    FORMAT_V208,
    FORMAT_V408,
    FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE,
    FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE,
    FORMAT_FORCE_UINT
};

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
    r32 FPS;
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
