/********************************************************************
 *                  PFS File System DeCompressor                    *
 *                                                                  *
 *  [   Author  ]       -       Andrew Borg                         *
 *  [   Type    ]       -       PFS Extractor                       *
 *  [   Date    ]       -       07.12.2013                          *
 *                                                                  *
 * ******************************************************************
 *                                                                  *
 *  Description                                                     *
 *                                                                  *
 * The PFS File System, is a File System used on some modern        *
 * Embedded Devices.                                                *
 *                                                                  *
 * The Purpose of this application is to extract the files found    *
 * inside the File system.                                          *
 *                                                                  *
 *          Tested on an Intel LITTLE ENDIAN Architecture           *
 *                  Against a Belkin Firmware Image                 *
 *                                                                  *
 * ******************************************************************
 */

#include <stdio.h> 
#include <string.h>
#include <stdint.h>

// Custom Header Files

#include "../Headers/Common.h"

/*
 *  PFS Header Structure
 *  
 *  A PFS Header is Made up of 16 Bytes
 *  
 *       1 -  8     : Signature Bytes PFS/0.9 
 *       9 - 14     : Null Padding AND WTF  
 *      15 - 16     : Number of Entries in Archieve - 16 Bit - Little Endian
 * 
 */ 
 
typedef struct {

    char * Signature[8];

    char * NullPadding [6];

    WORD   Entries;

} PFSHeader;

/*
 *  PFS Entry Structure
 * 
 *  Each PFS File Entry is Made up of 4 Sections Bytes
 *  
 *  Structure of a PFS with a 64 Byte Name Block.
 * 
 *  01 - 64         : File Name and Location - Padded to 64 with NULL Bytes
 *  
 *  65 - 68         : File Timestamp - 32 Bit - Little Endian
 *  
 *  69 - 72         : Offset of the file From the Data Section - 32 Bit - Little Endian
 *  
 *  73 - 76         : The Size of the File - 32 Bit - Little Endian
 * 
 */
 
typedef struct {
    
    char * Filename [64];
    
    DWORD     Timestamp;
    
    DWORD     Offset;
    
    DWORD     Size;
    
} PFSEntry;

short PFSEntrySize = 0;

// External Function Prototypes - Hex Dump Header File

char * DumpHex(char * FileName);

void PartitionExtractor( char * Filename, long StartAddress, int Count, char * OutputFile);

// Internal Function Prototypes

void ShowEntries( char * FileName);

void ExtractEntries( char * FileName);

PFSEntry * CheckFile(char * FileName);


// Global Variables

// The PFSHeader Structure is used to store the PFS Header information
// The Header Information Include the Number of Files inside the Archive, PFS Signature and Some Null Bytes

PFSHeader ArchiveHeader;

int DataSegment = 0;

// The Main Method will check the Passed Arguments and redirect the Applications' Flow Accordingly

int main(int argc, char * argv[])
{
    
    // If the Argument Count is Equal to Three
    
    if (argc == 3)
    {
        // If the Second Parameter is -List Redirect to the Show Entries Method
        
        if (strcmp(argv[1], "-List") == 0)
        {
            ShowEntries(argv[2]);
        }
        
        // If the Second Parameter is -Extract, Redirect to the Extract Entries Method
        
        else if (strcmp(argv[1], "-Extract") == 0)
        {
            ExtractEntries(argv[2]);
        }
        
        printf("\r\n\r\n");
    }   
    
    // If No or an Invalid Option was Entered
    else
    {
        // Print the Application's Syntax
        puts("Syntax");
        
        printf("\t \t %s -List FILE \r\n", argv[0]);
        printf("\t \t %s -Extract FILE \r\n", argv[0]);
    }
    
    return 0;
}

/*
 *  The Show Entries Method will Search the Binary File Header
 *  For Files Inside the PFS File System.
 * 
 *  This Method will only List the Files inside the Archive,
 *  along with their File Offset, Timestamp and Size
 * 
 *  Parameters:
 *              A Char Pointer to the PFS Archive
 * 
 *  Returns : 
 *          VOID 
 */
 
void ShowEntries( char * FileName)
{
    // Create a PFS Entry to Hold the File Information
    
    PFSEntry * Entries = CheckFile(FileName);
    
    int Counter = 0;
  
    // Print the Total Archive Entries Present inside the Archive
    printf(" \t\t\t Total Files in Archive: %d\r\n\r\n", ArchiveHeader.Entries);
    printf("--------------------------------------------------------------------- \r\n\r\n");
    
    // Iterate the PFS Archive and display information for each File inside the Archive
    
    for (Counter = 0; Counter < ArchiveHeader.Entries; Counter++)
    {
        printf("Compressed File %d\r\n", Counter + 1);
        printf(" \t Filename: %50s \r\n", (char *) Entries[Counter].Filename);
        printf(" \t Timestamp: %49u \r\n", Entries[Counter].Timestamp);
        printf(" \t Offset: %52X \r\n", DataSegment + Entries[Counter].Offset);
        printf(" \t Size: %54u \r\n", Entries[Counter].Size);
        
        printf("\r\n");
    }
    
    // Print the Data Segment Location
    
    printf("Data Segment Starts at 0x%X", DataSegment);
    
}

void PartitionExtractor( char * Filename, long StartAddress, int Count, char * OutputFile)
{
    FILE * File = FileOpener(Filename, "rb");
    
    fseek(File, StartAddress, SEEK_SET);
    
    int Counter = 0;
    
    FILE* NewFile = FileOpener(OutputFile, "w");
    
    char Buffer [Count];
     
    while (Counter < Count)
    {
        
        sprintf(Buffer + Counter, "%c", fgetc(File));
        
        Counter ++;
    }
    
    fwrite(Buffer, sizeof(char), sizeof(Buffer), NewFile);
    
}

/* 
 *  The Extract Entries Method will Extract All Files inside the PFS Archive
 * 
 *  Parameters:
 *              A Char Pointer to the PFS Archive
 * 
 *  Returns:
 *          Void
 */
void ExtractEntries( char * FileName)
{
    // 
    PFSEntry * Entries = CheckFile(FileName);
    
    int Counter = 0;
    
    // Iterate the PFS Entries.
    
    for (Counter = 0; Counter < ArchiveHeader.Entries; Counter++)
    {
        // Show Debug Information for Each File inside the Archive
        
        printf("Extracting %s Size %d \r\n", (char *)Entries[Counter].Filename, Entries[Counter].Size);
        
        // Redirect Execution Flow to the Partition Detector Method found inside the Hex Dump Header File
        
        PartitionExtractor(FileName, DataSegment + Entries[Counter].Offset, Entries[Counter].Size, (char *) Entries[Counter].Filename);
    }
}

/*
 *  The Check File Method will Check a File for a Valid PFS Archive.
 *  If Found, this method will also iterate the PFS Archive for all the Files Present inside the Archive
 * 
 *  Parameters: 
 *              A Char Pointer to the PFS Archive
 *  Returns:
 *              A PFS Entry Structure Array with all the File information 
 */
PFSEntry * CheckFile(char * FileName)
{
    // Get a Hex Dump of the PFS Archive
    
    char * PFS = DumpHex(FileName);
    
    // Copy the First Eight Bytes (PFS Signature )of the Archive inside the Archive Header Structure
    
    memcpy(ArchiveHeader.Signature, PFS, 8 * sizeof(char));
    
    // Check if the File is a Valid PFS Archive
    
    if (strncmp((const char *)ArchiveHeader.Signature, "PFS", 3))
    {
        puts("Invalid PFS File");
        exit (-1);
    }
    
    // If Valid, Copy the Next Six Bytes inside the Archive Header Structure
    
    memcpy(ArchiveHeader.NullPadding, PFS + 9 , 6 * sizeof(char));
    
    // Copy the Last Four Bytes ( The Number of Entries ) of the Header inside the Header Structoue
    
    memcpy(&ArchiveHeader.Entries, PFS + 14, sizeof(WORD));
    
    
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    
    char NameBlockChecker[128];
    
    memcpy(&NameBlockChecker, PFS + 16, 128);
    
    int NameLength = 0;
    int NullPadding = 0;
    
    // Iterate The Name Field to get the Byte Size allocated to the Name Field
    // The Below Loop is required because the Name Byte Size is not standard between PFS Images
    
    for (NameLength = 0; NameLength < 128; NameLength++)
    {
        if (NameBlockChecker[NameLength] == '\0' && NullPadding == 0)
        {
            NullPadding = 1;
        }
        else if(NullPadding == 1 && NameBlockChecker[NameLength] != 00)
        {
            break;
        }
        
    }
    
    // The Total PFS Entry Size is : The Size of the Name Length + Offset + Timestamp + Size        
    
    PFSEntrySize = NameLength + 4 + 4 + 4;
    
    // Print all the Information Gathered
    
    printf("--------------------------------------------------------------------- \r\n\r\n");
    
    printf("\t\t\t Valid %s File Found \r\n", (char *)ArchiveHeader.Signature);
    
    printf("\t\t\t   Entry Size %d Bytes \r\n\r\n", PFSEntrySize);

    
    // Start Gathering File Information Present inside the PFS Archive
    
    int Counter = 0;
    
    // Allocate Memory to Hold all the Files Information inside the Archive
    
    PFSEntry * ArchiveEntries = malloc(ArchiveHeader.Entries * sizeof(PFSEntry));
    
    // Temp Structure to Hold the Current PFS File Entry
    PFSEntry Temp;
    
    // Loop All Entries inside the Archive
    
    while (Counter < ArchiveHeader.Entries)
    {
        
            // Copy the File Name inside the Name Field of the PFS Entry Structure
            
            memcpy(Temp.Filename, PFS + ( 16 + (Counter * PFSEntrySize )), 40 * sizeof(char));
            
            // Copy the File's Timestamp inside the Timestamp Field of the PFS Entry Structure
            
            memcpy(&Temp.Timestamp, PFS + 16 + NameLength + (Counter * PFSEntrySize ), sizeof(DWORD));
            
            // Copy the File's Offset inside the Offset Field of the PFS Entry Structure
            
            memcpy(&Temp.Offset, PFS + 16 + NameLength + 4 + (Counter * PFSEntrySize ), sizeof(DWORD));
            
            // Copy the File's Size inside the Sixe Field of the PFS Entry Structure            
            
            memcpy(&Temp.Size, PFS + 16 + NameLength + 4 + 4 + (Counter * PFSEntrySize ), sizeof(DWORD));
            
            // Copy the Current PFS Entry to the Global Array
            
            ArchiveEntries[Counter ++] = Temp;
            
                
    }
    
    // Once all PFS Entries are Iterated, Store the Data Segment Offset of the Archive
    // The Data Segment is the location where all the File's data is stored
    
    DataSegment = 16 + (Counter * PFSEntrySize );
    
    // Return an Array of type PFSEntry, with all the File Information inside the PFS Archive
    
    return ArchiveEntries;
}

