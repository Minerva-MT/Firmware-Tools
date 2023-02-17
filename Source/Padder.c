/********************************************************************
 *                  Partition Padder                                *
 *                                                                  *
 *  [   Author  ]       -       Andrew Borg                         *
 *  [   Type    ]       -       Firmware Analysis                   *
 *  [   Date    ]       -       02.01.2014                          *
 *                                                                  *
 * ******************************************************************
 *                                                                  *
 *  Description                                                     *
 *                                                                  *
 *  This application takes an input file and adds padding to        *
 *  enlarge the file to a specified size.                           *       
 *                                                                  *
 * ******************************************************************/
 
#include <string.h>
#include <stdlib.h>

#include "../Headers/Common.h"

FILE * FileOpener(char * Filename, char * ReadMode);
char * DumpHex(char * FileName);
void PadFile(char * Filename, int PartitionSize, char * OutputFile);    

int main(int argc, char * argv[])
{
    if (argc == 4)
    {
        printf("Padding File %s \r\n", argv[1]);
        PadFile(argv[1], atoi(argv[2]), argv[3]);
    }
    else
    {
        puts("Syntax");
        printf("%s <INPUT FILE> <Partition Size> <Output File> \r\n", argv[0]);
    }
}

void PadFile(char * Filename, int PartitionSize, char * OutputFile)
{
    
    FILE * File = FileOpener(OutputFile, "w");
    
    int Counter;
    
    char * Hex = DumpHex(Filename);
        
    if (FileSize > PartitionSize)
    {
        printf("The File size is already bigger then the specified Partition Size ( %lu Bytes ) \r\n", FileSize);
        exit(-1);
    }
    else if (PartitionSize > FileSize)
    {
        printf("Padding File To %d \r\n", PartitionSize);
        
        PartitionSize -= FileSize;
        
        char * Hex = DumpHex(Filename);
        
        fwrite(Hex, FileSize, 1, File);
        
        char NullChar = 0xFF;
        
        while (PartitionSize > 12)
        {
            fputc(NullChar, File);
            PartitionSize --;
        }
        
        // Write the Length of the Partition
        
        fwrite(&FileSize, 4, 1, File);
        
        char BelkinSignature [4];
        
        BelkinSignature[0] = 0x78; 
        BelkinSignature[1] = 0x56; 
        BelkinSignature[2] = 0x34; 
        BelkinSignature[3] = 0x12; 
        
        
        
        
        fwrite(BelkinSignature, sizeof(BelkinSignature), 1, File);
        
        char BypassCRC[4];
        
        BypassCRC[0] = 0xFF; 
        BypassCRC[1] = 0xFF; 
        BypassCRC[2] = 0xFF; 
        BypassCRC[3] = 0xFF; 

            
        fwrite(BypassCRC, sizeof(BypassCRC), 1, File);
        
        
    }
}
