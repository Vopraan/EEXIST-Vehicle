#ifndef STRING_H
#define STRING_H

#include <stdlib.h>
#include <string.h>

#define LENGTH 512

class String
{

private:

    char* Data = NULL;

    int DataLength = 0;

public:

    char* toChar();

    void empty();

    int dataLength();

    bool isValid();

    String();
    String(const char* NewString);
    String(int Size);
    ~String();

};

#endif