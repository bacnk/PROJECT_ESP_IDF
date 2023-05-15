#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "Utilities.h"
uint32_t getNumberFromString(uint16_t BeginAddress, char *Buffer)
{
   
    uint16_t i = 0;
    i = BeginAddress;
    uint32_t hexNumber = 0;

    // Skip any leading white space
    while (isspace(Buffer[i]))
    {
        i++;
    }

    // Skip "0x" prefix if present
    if (Buffer[i] == '0' && (Buffer[i + 1] == 'x' || Buffer[i + 1] == 'X'))
    {
        i += 2;
    }

    // Parse the hex number
    while (isxdigit(Buffer[i]))
    {
        char c = tolower(Buffer[i]);
        uint8_t nibble = (c >= 'a' && c <= 'f') ? (c - 'a' + 10) : (c - '0');
        hexNumber = (hexNumber << 4) | nibble;
        i++;
    }

    return hexNumber;
}