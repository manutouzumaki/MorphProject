#ifndef UTILITY_H
#define UTILITY_H

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
