/* Common Functions */

#include "../Headers/Common.h"

unsigned long FileSize = 0;

FILE * FileOpener(char * Filename, char * ReadMode)
{
    FILE * File = fopen(Filename, ReadMode);
    
    if (File == NULL)
    {
        puts("File Not Found");
        exit(-1);
    }
    
    if (ferror(File))
    {
        puts("Error Reading File");
        exit(-1);
    }
    
    return File;
}

char * DumpHex(char * FileName)
{
    
    FILE *  File = FileOpener(FileName, "rb");
    
    fseek(File, 0L, SEEK_END);

    FileSize = ftell(File);
    
    fseek(File, 0L, SEEK_SET);
    
    char * HexArray = malloc(FileSize);
        
    int Counter = 0;
    
    while (Counter < FileSize)
    {       
        HexArray[Counter++] = fgetc(File);
    }
    
    
    return HexArray;
}

char * DumpHexWithoutNulls(char * FileName)
{
    
    FILE *  File = FileOpener(FileName, "rb");
    
    fseek(File, 0L, SEEK_END);

    FileSize = ftell(File);
    
    fseek(File, 0L, SEEK_SET);
    
    char * HexArray = malloc(FileSize);
    
    unsigned char Character;
    
    int Counter = 0;
    
    while (Counter < FileSize)
    {
        Character = fgetc(File);
        
        if (Character == '\0')
        {
            Character = 0xFF;
        }
        sprintf(HexArray  + Counter, "%c", Character);
        
        Counter++;
    }
    
    
    return HexArray;
}
