#ifndef UTILITY_H
#define UTILITY_H

u32 StringCount(char *Data)
{
    u32 Result = 0;
    while(Data++ != '\0') ++Result;
    return Result;
}

bool StringCompare(char *A, char *B, int Size)
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
