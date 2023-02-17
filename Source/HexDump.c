/********************************************************************
 *                  Generic File Operations                         *
 *                                                                  *
 *  [   Author  ]       -       Andrew Borg                         *
 *  [   Type    ]       -       Generic                             *
 *  [   Date    ]       -       07.12.2013                          *
 *                                                                  *
 * ******************************************************************
 *                                                                  *
 *  Description                                                     *
 *                                                                  *
 * This Application implements some of the basic utilities          *
 * used during reverse engineering                                  *
 *                                                                  *
 * This Application include a Hex Dumper, String Extractor and      *
 * a Partition detector.                                            *
 *                                                                  *
 * -----------------------------------------------------------------*
 *                      The Hex Dumper                              *
 * -----------------------------------------------------------------*
 *                                                                  *
 *          The Purpose of the Hex Dumper is to output the          *
 *          Hex Equivalent of a file or binary                      *
 *                                                                  *
 *          - Arguments :   A Character Array Filled with the       *
 *                          File's equivalent Hex Codes             *
 *                                                                  *
 *          - Returns   :   VOID ( None )                           *
 *                                                                  *
 * -----------------------------------------------------------------*
 *                      The String Extractor                        *
 * -----------------------------------------------------------------*
 *                                                                  *
 *          The String Extractor will Output Strings present        *
 *          inside the File's Binary.                               *
 *          Strings can give an Idea weather or not a Binary is     *
 *          Encrypted or Compressed. Also strings can give hints    *
 *          about what a File might be or do.                       *
 *                                                                  *
 *          - Arguments :   A Character Array Filled with the       *
 *                          File's equivalent Hex Codes             *
 *                                                                  *
 *          - Returns   :   VOID ( None )                           *
 *                                                                  *
 *          - Note      :   A String is displayed if it contains    *
 *                          at least Ten Printable Characters       *
 *                                                                  *
 * -----------------------------------------------------------------*
 *                      The Partition Detector                      *
 * -----------------------------------------------------------------*
 *          The Partition Detector will search through the          *
 *          Files Hex Digits and try to identify different          *
 *          Partitions.                                             *
 *                                                                  *
 *          The Partition Detecor takes advantage of padding /      *
 *          allignment which may be implmented inside the           *
 *          File.                                                   *
 *                                                                  *
 *          - Arguments :   A Character Array Filled with the       *
 *                          File's equivalent Hex Codes             *
 *                                                                  *
 *          - Returns   :   VOID ( None )                           *
 *                                                                  *
 *          - Note      :   A Partition is identified if atleast    *
 *                          100 NULL (FF) Characters are repeated   *
 *                          in succession                           *
 *                                                                  *
 * ******************************************************************/
 

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../Headers/Common.h"

// The Maximum Hex Digits Displayed Per Line - Used in the Format Hex Method

#define MAX_HEX_PER_LINE 10

// The Minimum Length of the String to be Considered as a Valid String

#define MINIMUM_STRING_LENGTH 10

// The Minimum ammount of null Bytes to consider a partition split

#define MINIMUM_NULL 20

// External Function, Found in the HexDump Header File

char * DumpHex(char * FileName);

// Internal Function Prototyes

void FormatHex(char * HexDump);
void StringExtractor(char * HexDump);
void PartitionDetector(char * HexDump);
void ExtractFromHex(int Start, int Count, char * FileName, char * HexDump);

// The Main Method will check the Passed Arguments and redirect the Flow Accordingly

int main(int argc, char **argv)
{
    
    // Check User Input and Redirect Accordingly
    
    // If the Number of Arguments is Equal to Three, Check for Valid Arguments
    
    if (argc == 3)
    {
        char * Hex = DumpHex(argv[2]);

        // If the First Argument is -Hex Redirect To the Format Hex Method
        
        if (strcmp(argv[1], "-Hex") == 0)
            FormatHex(Hex);
        
        // If the First Argument is -Strings Redirect To the String Extractor Method    
        
        else if (strcmp(argv[1], "-Strings") == 0)
            StringExtractor(Hex);
        
        // If the First Argument is -Partitions Redirect to the Partition Detector Method
        
        else if (strcmp(argv[1], "-Partitions") == 0)
            PartitionDetector(Hex);
        
        
        printf("\r\n\r\n");
    }
    
    // If the Number of Arguments is Equal to 6, Check for Valid Arguments
    
    else if (argc == 6)
    {
        char * Hex = DumpHex(argv[5]);
        
        // If the Second Argument is -Extract, Redirect Flow to the Extract From Hex Method
        
         if(strcmp(argv[1], "-Extract") == 0)
            ExtractFromHex(atoi(argv[2]), atoi(argv[3]), argv[4], Hex);
    }
    
    // If the Number of Arguments is not Equal to 6 or 3, Show the Applications' Syntax
    
    else
    {
        puts("Syntax : \r\n");
        printf("\t %s -Hex FILE \r\n", argv[0]);
        printf("\t %s -Strings FILE \r\n", argv[0]);
        printf("\t %s -Partitions FILE \r\n\r\n", argv[0]);
        printf("\t %s -Extract Start BytesToExtract OutputName FILE \r\n\r\n", argv[0]);
    }
    
    
    
    
    return 0;
}

/*
 *  The Format Hex Method will output a File's equivalent Hex Representation
 *  
 *  This Method uses the MAX_HEX_PER_LINE Pre-Processor Tag to align the
 *  Hex Digits to a certain Ammount Per Line.
 * 
 *  Parameters:
 *          A Char Array with the File's Equivalent Hex Dump
 * 
 *  Returns:
 *          VOID
 */
 
void FormatHex(char * HexDump)
{
    int Counter = 0;

    // Iterate the Whole File
    
    while (Counter < FileSize)
    {
        // If The Hex Digits Per Line is equal to MAX_HEX_PER_LINE
        
        if (Counter % MAX_HEX_PER_LINE == 0)
        {
            // Skip Line
            
            printf("\r\n");
            
            // Print the File Offset
            printf("Offset 0x%-20.5x", Counter);
            
        }
        
        // Print the Hex Equivalent
                
        printf(" %2.2X ", (unsigned char)HexDump[Counter]);
        
        // Increment Counter
        
        Counter++;
        
    }
}

/*
 *  The String Extractor Method will search the Binary file
 *  for potential strings inside the File.
 * 
 *  This Method Makes use of the MINIMUM_STRING_LENGTH Pre-Processor tag,
 *  to seperate Garbage Characters from Actual Strings
 * 
 *  Parameter:
 *          A Char Array with the File's Equivalent Hex Dump
 * 
 *  Returns:
 *          VOID
 * 
 */
 
void StringExtractor(char * HexDump)
{
        int Counter = 0;
        
        int EmptyLine = 1;
        
        char CharArray[50];
        
        int CharCounter = 0;
        
        // While Counter is Less Then File Size Bytes
        
        while (Counter < FileSize)
        {
            // Store the Equivalent Hex Dump inside a Four Byte Char Variable
            
            char Character = HexDump[Counter];
            
            // If the Character is Printable
            
            if (isprint(Character))
            {
                // Check if the Character have space inside the Char Array
                // The + 1 denotes the NULL Byte
                
                if (CharCounter + 1 != 50)
                {
                    // Store the Character inside the Char Array
                    
                    CharArray[CharCounter++] = Character;
                    
                    // Set the Empty Line Variable to Zero
                    
                    EmptyLine = 0;
                }
                else
                {
                    // Put A Null Byte inside the Char Array, to Prevent a Memory Leak
                    // The NULL Byte is Inputted inside the Character Array, as a Null Terminator for the String
                    
                    CharArray[CharCounter] = '\0';
                    
                    // Print hte Character Array
                    
                    printf("%s", CharArray);
                    
                    // Set the Char Counter to Zero
                    CharCounter = 0;
                }
            }
            
            // Else : The Character is Not Printable
            
            else if (EmptyLine == 0) 
            {               
                // Insert a NULL Byte inside the Char Array
                CharArray[CharCounter] = '\0';
                
                // If The Total Char Array Count is Bigger then the MINIMUM_STRING_LENGTH
                
                if (strlen(CharArray) > MINIMUM_STRING_LENGTH)
                {
                    // Print the Char Array
                    
                    printf("%s", CharArray);
                    
                    // Print a New Line
                    
                    printf("\r\n");
                }
                
                EmptyLine = 1;
                
                CharCounter = 0;
                
            }   
            
            Counter ++;
        }
}

/*
 *  The Partition Detector Method will search a Binary File 
 *  for potential Partition Allignment, which may be used to
 *  seperate Partitions inside the Binary File
 * 
 *  Parameters :
 *              A Char Array with the Binary's equivalent Hex Code
 * 
 *  Returns :
 *              VOID
 * 
 */
void PartitionDetector(char * HexDump)
{
    
    // Set Environment
    
        int Counter = 0;
        
        int NullStart = 0;
        int NullBytes = 0;
        
        int DataOffset = 0;
        
        int PartitionCount = 0;
    
    // While Counter is less then FileSize iterate Hex Dump
        
        while (Counter < FileSize)
        {
    
            // If the Hex Code is Equivalent to FF
            
            if (((unsigned char)HexDump[Counter]) == 255)
            {
                // Increment Null Bytes
                                
                NullBytes++;
                
                // If NullStart is equal to 0
                
                if (NullStart == 0)
                {
                    // Copy the Counter Value inside the NullStart Variable
                    // This is used to Hold the Starting Offset of the Null Padding
                
                    NullStart = Counter;
                }
            }
            
            // Else : The Hex Character is not FF
            else
            {
                // If There was more then 100 NULL Bytes
                
                if (NullBytes > MINIMUM_NULL)
                {
                    // Print Partition Details
                    
                    printf("\t\t\t Partition %d \r\n\r\n", ++PartitionCount);
                    
                    // Print the Data Partition Starting and Ending Offset
                    
                    printf("Data Found From Offset 0x%-6X Till 0x%-6X (%d Bytes) \r\n", DataOffset, Counter - 1, (Counter - 1) - DataOffset);
                    
                    // Print the Null Partition Starting and Enging Offset
                    
                    //printf("Null Padding Found From Offset 0x%-6X Till 0x%-6X (%d Bytes) \r\n", NullStart, Counter, Counter - NullStart);
                    
                    puts("-----------------------------------------------------------------------------------------");
                    
                    
                    // Set the Data Offset Variable to the Value of Counter
                    
                    DataOffset = Counter;
                                        
                }
                
                NullBytes = 0;
                NullStart = 0;
            }
            Counter ++;
        }
        
        
        // Print the Partition Summary
        
        // Check if Partitions were found and display a Message accordingly
        
        if (PartitionCount > 0)
        {
            // If Partitions were Found, Print how many
            
            printf("%d Possible Partitions Found ", PartitionCount);
        }
        else
        {
            // If not Partitions were found, Print Message
            puts("No Partitions were Found ");
        }
}


void ExtractFromHex(int Start, int Count, char * FileName, char * HexDump)
{
    
    
    if (Count == -1)
    {
        Count = FileSize - Start;
    }
    
    printf("Extracting %d Bytes ... \r\n", Count);
    
    int Counter = 0;
    
    FILE * File = fopen(FileName, "w");
    
    while (Counter < Count)
    {           
        fputc(HexDump[Start + Counter], File);
        
        Counter ++;
    }
    
    
    printf("Done. Extracted Partition. Saved to %s \r\n", FileName);
    
    fclose(File);
}
