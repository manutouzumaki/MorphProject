#ifndef UTILITY_H
#define UTILITY_H

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
