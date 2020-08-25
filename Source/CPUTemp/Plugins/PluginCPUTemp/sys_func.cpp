#include "sys_func.h"

unsigned int len(const char sys[])
{
    unsigned int i = 0;
    char c = sys[i];
    while (c != '\0')
    {
        c = sys[i++];
    }
    return i;
}

unsigned int HexToDec(const char c)
{
    switch (c)
    {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'A':
        return 10;
    case 'B':
        return 11;
    case 'C':
        return 12;
    case 'D':
        return 13;
    case 'E':
        return 14;
    case 'F':
        return 15;
    default:
        return 0;
    }
}

void WriteFromStream(unsigned char* buf, const char* filename, unsigned int size)
{
    FILE* f;
    fopen_s(&f, filename, "wb+");
    if (f)
    {
        fwrite(buf, 1, size, f);
        fclose(f);
    }
}

void HexToChar(const char sys[], const char* filename)
{
    unsigned int size = len(sys);
    unsigned char* buf = new unsigned char[size];
    memset(buf, 0, size);
    unsigned int k = 0;
    char c = 0;
    for (unsigned int i = 0; i <= size; i++)
    {
        if (i % 2 == 0)
        {
            c = HexToDec(sys[i]) * 16;
        }
        else
        {
            c += HexToDec(sys[i]);
            buf[k++] = c;
        }
    }
    WriteFromStream(buf, filename, --k);
    delete[] buf;
}