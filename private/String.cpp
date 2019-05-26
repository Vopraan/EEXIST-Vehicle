#include "../public/String.h"

#include "stdio.h"

String::String()
{
    DataLength = LENGTH;
    Data = (char*)malloc(sizeof(char) * DataLength);
}

String::String(const char* NewString)
{
    DataLength = (strlen(NewString) + 1);
    Data = (char*)malloc(sizeof(char) * DataLength);

    strcpy(Data, NewString);
}

String::String(int Size)
{
    DataLength = Size;
    Data = (char*)malloc(sizeof(char) * DataLength);
}

char* String::toChar()
{
    return Data;
}

int String::dataLength()
{
    return DataLength;
}

bool String::isValid()
{

}

void String::empty()
{
    if(Data != NULL) { free(Data); }
    Data = NULL;
}

String::~String()
{
    if(Data != NULL) { free(Data); }
}