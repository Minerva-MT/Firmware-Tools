/********************************************************************
 *                  PFS File System Packer                          *
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
 * The Purpose of this application is to Pack individual binaries   *
 * into a PFS File System.                                          *
 *                                                                  *
 *          Tested on an Intel LITTLE ENDIAN Architecture           *
 *                  Against a Belkin Firmware Image                 *
 *                                                                  *
 * ******************************************************************
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "../Headers/Common.h"

#define NAME_BLOCK 64
#define MAX_FILES 1000
#define PATH_MAX 4096

// A Structure used to store the Header of the PFS Image
// A more Detailed Description can be found in the Unpacker 

typedef struct 
{
    char Signature [8];

    char NullPadding [4];

    WORD UnknownField;

    WORD EntryCount;

} PFSHeader;

// A Structure used to store each file's attributes including the equivalent Hex Dump
// A more Detailed Description can be found in the Unpacker 

typedef struct
{
    char FileName[NAME_BLOCK];
    
    DWORD Timestamp;
    
    DWORD Offset;
    
    DWORD Size;
    
    char * HexEquivalent;
    
} PFSEntry;


////////////////////////////////////////////////////////////////////////////////

// External Function Prototypes

char * DumpHex (char * FileName);

////////////////////////////////////////////////////////////////////////////////

// Internal Function Prototypes

void GatherFiles(char * ParentFolder, char RecursiveScan);

PFSHeader PopulateArchiveHeader(WORD Entries);

void WriteBinary(PFSEntry * Packer, char * OutputFile);

PFSEntry * PackFiles();

////////////////////////////////////////////////////////////////////////////////

// Static Variable Used to hold The ammount of files being Packed

static int TotalFiles;

// Static Variable Used to Hold The Filenames of the files being Packed

static char * FileNames[MAX_FILES] = {0};


void main ( int argc, char * argv[] )
{
    // If the argument count is greater then Four, display the Application's Syntax
    
    if ( argc > 4 )
    {
        printf("%s Syntax Usage \r\n", argv[0]);
        printf("\t %s <Directory to Pack> <Output FileName> \r\n", argv[0]);
    }
    
    // If the argument count is equal to three, perform a Non-Recursive Packing
    
    else if ( argc == 3 )
    {
        // Display File Population Type
        
        printf ("Non - Recursive Packing - ");
        
        // Populate the FileNames Array with the Files found in the Selected Folder
        
        GatherFiles(argv[1], 0);
        
        // Store the Packed Files inside an array of the PFSEntry Structure
        
        PFSEntry * PackedFiles = PackFiles();
        
        // Write the Binary File to the Client's Computer
        
        WriteBinary(PackedFiles, argv[2]);
    }
    
    // If the argument count is equal to four, perform a Recursive Packing
    
    else if ( argc == 4)
    {
        if ( strcmp( argv[1], "-R" ) == 0)
        {
            printf ("Recursive Packing - ");
        
            // Populate the FileNames Array with Files found in the Selected Folder and it's subfolders
            
            GatherFiles(argv[2], 1);
            
            // Store the Packed Files inside an array of the PFSEntry Structure


            PFSEntry * PackedFiles = PackFiles();

            // Write the Binary File to the Client's Computer

            WriteBinary(PackedFiles, argv[3]);
        }
    }
    
    // If none of the Conditions are met, Display the application's syntax
    
    else
    {
        printf("%s Syntax Usage \r\n", argv[0]);
        printf("\t %s <Directory to Pack> <Output FileName> \r\n", argv[0]);
    }
}


/*
 *  The Gather Files Method will search a Given folder 
 *  for regular files.
 * 
 *  Once executed, this method will populate the Globally declared 
 *  Filenames Array with the relative path of the files found
 * 
 *  Parameters:
 *          A Char Array with the Folder Being Searched
 *          A Char Indicating weather a Recursive scan should be performed ( 1 ) or not ( 0 )
 * 
 *  Returns:
 *          VOID
 * 
 */
 
void GatherFiles(char * ParentFolder, char RecursiveScan)
{
    // DIR Pointer, Defined inside the Dirent Header File
    
    DIR * Directory;
    
    // Open the Directory and store it's content inside the Directory structure
    
    Directory = opendir(ParentFolder);
    
    // Dirent Pointer, Defined inside the Dirent Header File
    
    struct dirent * Entry;
    
    // A Char Array of maximum size NAME_BLOCK to store the file found
    
    char FullPath[NAME_BLOCK];
    
    // If Directory is NULL
    
    if (!Directory)
    {
        // Print Error message and Exit
        
        puts("Cannot Open Folder \n");
        
        exit(EXIT_FAILURE);
    }
    
    while (1)
    {
        // Read Entry inside the Directory
        
        Entry = readdir(Directory);
        
        if (!Entry)
        {
            break;
        }
        
        // Skit the Entry if the retrieved entry Name is either .. ( Top Folder ) or . ( Current Folder ) 
        
        if ( strcmp(Entry -> d_name, "..") == 0 || strcmp(Entry -> d_name, ".") == 0 )
        {
            continue;
        }
        
        // Write the Relative Path of the Entry inside the FullPath char Array
        
        strcpy(FullPath, ParentFolder);
        strcat(FullPath, "/");
        strcpy(FullPath, Entry -> d_name);
        
        // If the Entry is a Directory
        
        if ( ( Entry -> d_type & DT_DIR ))
        {
            // If a recursive scan is selected
            
            if (RecursiveScan == 1)
            {
                int PathLength = 0;
                
                char Path [NAME_BLOCK];
                
                // Concatenate the Relative path and store it's character length inside the Path Length Variable
                
                PathLength = snprintf(Path, PATH_MAX, "%s/%s", ParentFolder, Entry -> d_name);
                
                // If the Length is greater then the acceptable path length
                
                if (PathLength > NAME_BLOCK)
                {
                    // Print Error Message and skip the directory
                    
                    printf("Ommiting Directory %s. Path too Long \r\n", Entry -> d_name);
                    
                    continue;
                }
                
                
                printf ("Recurring to Folder %s \r\n", Entry -> d_name);
                
                // Recure Folder
                
                GatherFiles(Path, 1);
            }
        }
        
        // If the Entry is not a Folder
        else
        {
            // Allocate Memory for the Entry ( Full Path Length + NULL Byte)
            
            FileNames[TotalFiles] = malloc( strlen( FullPath ) + 1 );
            
            // Copy the Path inside the array
                        
            strcpy( FileNames[TotalFiles], FullPath );
            
            // Increment Total Files Counter declared globally
            
            TotalFiles ++;
        }
    }
    
    // Close the Directory
    
    closedir(Directory);
    
}

/*
 *  The Populate Header Method will populate the Header for the 
 *  PFS Image.
 * 
 * 
 *  Parameters:
 *          An 16 Bits (2 Bytes) Unsigned integer with the Number of Entries ( Files ) being Packed
 * 
 *  Returns:
 *          PFSHeader Structure
 * 
 */

PFSHeader PopulateArchiveHeader(WORD Entries)
{
    PFSHeader ArchiveHeader;
    
    ArchiveHeader.Signature[0] = 0x50; // P
    ArchiveHeader.Signature[1] = 0x46; // F
    ArchiveHeader.Signature[2] = 0x53; // S
    ArchiveHeader.Signature[3] = 0x2F; // / 
    ArchiveHeader.Signature[4] = 0x30; // 0
    ArchiveHeader.Signature[5] = 0x2E; // .
    ArchiveHeader.Signature[6] = 0x39; // 9
    ArchiveHeader.Signature[7] = 0x00; // /0
    
    // Null Seperator
    
    ArchiveHeader.NullPadding[0] = 0x00;
    ArchiveHeader.NullPadding[1] = 0x00;
    ArchiveHeader.NullPadding[2] = 0x00;
    ArchiveHeader.NullPadding[3] = 0x00;
    
    // Unknown Field
    
    ArchiveHeader.UnknownField = 0; // For the Sake of Diffing -
    
    // Entry Count
    
    ArchiveHeader.EntryCount = TotalFiles;
    
    return ArchiveHeader;
    
}

/*
 *  The Pack Files Method will Iterate the Filenames Array which is populated by the
 *  Gather Files Method and generate information about each File.
 *  
 *  This method retrieved information about the File's Size, Timestamp 
 *  and Hex Dump directly from the File.
 * 
 *  The Offset property is calculated inside this method. This property is needed
 *  to tell the Unpacker where the File resides inside the final Binary.
 * 
 *  The DumpHex Method defined in the Hex Dump Header File is used in this method.
 * 
 *  Parameters:
 *          None
 *  
 *  Returns:
 *          PFSEntry Structure Pointer
 * 
 */
 
PFSEntry * PackFiles()
{
    // Variable used to Calculate the File's Offset
    
    int OffsetCounter = 0;
    
    // Variable used to loob the Filenames Array
    
    int PackingCounter = 0;
    
    // PFSEntry Structure Pointer. 
    // The Pointer is allocated Memory according to the number of Files being Packed
    
    PFSEntry * Packer = malloc (sizeof(PFSEntry) * TotalFiles );
    
    // Display Packing Information
    
    printf("Total Files to Pack : %d \r\n\r\n", TotalFiles);
    
    // If Memory Allocation Fails
    
    if (Packer == NULL)
    {
        // Print Error Message and Exit
        
        puts("Error Allocating Memory \r\n");
        exit(EXIT_FAILURE);
    }
    
    // Temorary PFSEntry to store temporary File Information
    
    PFSEntry Packed;
    
    for (PackingCounter = 0; PackingCounter < TotalFiles; PackingCounter ++)
    {
        // Display Packing Information
        
        printf("Packing File %d of %d - %s \r\n", PackingCounter + 1, TotalFiles, FileNames[PackingCounter] );
        
        // Open The File, Retrieve it's Hex dump and store it inside the HexEquivalent Property of the PFSEntry Structure
        
        Packed.HexEquivalent = DumpHex( FileNames[PackingCounter] );
        
        //Store the File's Name inside the Filename Property of the PFSEntry Structure
        
        strncpy(Packed.FileName, FileNames[PackingCounter], NAME_BLOCK);
        
        //Store the File's Offset inside the Offset Property of the PFSEntry Structure
        
        Packed.Offset = OffsetCounter;
        
        // Add the File's Size to the current offset, to point to the next file
        
        OffsetCounter += FileSize;
        
        // Store the File's Size inside the Size Property of the PFSEntry Structure
        
        Packed.Size = FileSize;
        
        // Store the Timestamp inside the Timestamp Property of the PFSEntry Structure
        
        Packed.Timestamp = 1;
        
        // Copy the Temporary PFSEntry Variable to the PFSEntry Array
        
        Packer[PackingCounter] = Packed;
        
    }
    
    // Return a Pointer to the PFSEntry Structure Array
    
    return Packer;
}


/*
 *  The Write Binary Method will Iterate the PFSEntry Array and write 
 *  the final PFSImage inside the Client's Computer
 * 
 *  This Method First writes the PFSHeader to the File and then iterates
 *  each packed file found inside the PFSEntry Array, writing each file 
 *  in each iteration
 * 
 *  The File Opener Method defined in the Hex Dump Header File is used in this method.
 * 
 *  Parameters:
 *          PFSEntry Array Pointer to the Packed Files
 *          char Array with the Output Binary Name
 *  
 *  Returns:
 *          VOID
 * 
 */
 
void WriteBinary(PFSEntry * Packer, char * OutputFile)
{
    // Display basic Writing Information
    
    printf("\r\nWriting Output to : %s\r\n", OutputFile);
    
    // Open a Writing File Handle using the Output File Variable as the File Name
    
    FILE * File = FileOpener(OutputFile, "w");
    
    // Generate the PFSHeader
    
    PFSHeader ArchiveHeader = PopulateArchiveHeader(TotalFiles);
    
    // Write the PFSHeader inside the File
    
    fwrite(ArchiveHeader.Signature, sizeof(ArchiveHeader.Signature), 1, File);
    fwrite(ArchiveHeader.NullPadding, sizeof(ArchiveHeader.NullPadding), 1, File);
    fwrite(&ArchiveHeader.UnknownField, sizeof(ArchiveHeader.UnknownField), 1, File);
    fwrite(&ArchiveHeader.EntryCount, sizeof(ArchiveHeader.EntryCount), 1, File);
    
    int Counter;
    
    // Iterate the Packed Files
    
    // PFSEntry Bytes
        // [ 64 Bytes - Filename ] [ 4 Bytes - Timestamp ] [ 4 Bytes - Offset ] [ 4 Bytes - Size ]
        
    for (Counter = 0; Counter < TotalFiles; Counter ++)
    {
        // Write the Name of the Packed File.
        
        fwrite(Packer[Counter].FileName, NAME_BLOCK, 1, File);
        
        // Write the Timestamp of the Packed File.
        
        fwrite(&Packer[Counter].Timestamp, sizeof(DWORD), 1, File);
        
        // Write the Data Offset of the Packed File.
        
        fwrite(&Packer[Counter].Offset, sizeof(DWORD), 1, File);
        
        // Write the Size of the Packed File.
        
        fwrite(&Packer[Counter].Size, sizeof(DWORD), 1, File);
    }
    
    // Re-Iterate the Array Writing the Hex Equivalent of each file
    
    for (Counter = 0; Counter < TotalFiles; Counter ++)
    {
        fwrite(Packer[Counter].HexEquivalent, Packer[Counter].Size, 1, File);
    }
    
}
