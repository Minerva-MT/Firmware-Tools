/********************************************************************
 *                  File Merger                                     *
 *                                                                  *
 *  [   Author  ]       -       Andrew Borg                         *
 *  [   Type    ]       -       Firmware Analysis                   *
 *  [   Date    ]       -       02.01.2014                          *
 *                                                                  *
 * ******************************************************************
 *                                                                  *
 *  Description                                                     *
 *                                                                  *
 *  This Application will take multiple files as arguments          *
 *  and merge them as a single binary                               *
 *                                                                  *
 * ******************************************************************/
 
#include <string.h>
#include <stdlib.h>

#include "../Headers/Common.h"

////////////////////////////////////////////////////////////////////////////////

// External Function Prototypes

FILE * FileOpener(char * Filename, char * ReadMode);

char * DumpHex(char * FileName);

////////////////////////////////////////////////////////////////////////////////

// Internal Function Prototypes

void MergeFiles(char * Files[], int FileCount, char * OutputFile);

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char * argv[])
{
    // If the Argument Count is greater then Two
    
    if (argc > 2)
    {       
        int Counter ;
        
        // A Character Array to hold the Files being Merged
        
        char * Files[argc - 2];
        
        // Get the Passed Filenames and store then in the Files Array
        
        for ( Counter = 1; Counter < argc - 1; Counter ++)
        {
            Files[Counter - 1] = argv[Counter];
        }       
        
        MergeFiles(Files, argc - 2, argv[Counter]);
    }
    else
    {
        printf("Syntax : \r\n");
        printf("\t %s <Files to Pack ...> <Output> \r\n", argv[0]); 
    }
}

/*  The Merge Files method takes Two or More Binary Files and concatenates 
 *  them into one Binary File.
 * 
 *  Parameters:
 *          Files[]     : Char Array of the Files Being Merged
 *          File Count  : Integer with the Number of Files being Merged
 *          Output File : Char Array with the Name of the Output File
 * 
 *  Returns : 
 *          VOID
 */
 
void MergeFiles(char * Files[], int FileCount, char * OutputFile)
{
    // Create a writing handle using the Output File Variable
    
    FILE * File = FileOpener(OutputFile, "w");
    
    int Counter;
    
    printf("Output is going to be saved to : %s ", OutputFile);
    
    puts("Proceed ? [Y/N]");
    
    // Get User Confirmation
    
    int UserInput = getchar();
    
    // If the User does Not Confirm
    
    if (UserInput != 'Y')
    {
        // Print Message and Exit
        
        puts("Exiting ... ");
        
        exit(EXIT_FAILURE);
    }
    
    // Iterate the Files Array
    
    for (Counter = 0; Counter < FileCount; Counter ++)
    {
        // Get the File's Equivalent Hex Dump
        
        char * Hex = DumpHex(Files[Counter]);
        
        // Write the HexDump inside the New Output File
        
        fwrite(Hex, FileSize, 1, File);
    }
    
    
    // Close the Output File
    
    fclose(File);

    printf("Merged File saved as : %s \r\n", OutputFile);
    
}
