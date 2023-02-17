/********************************************************************
 *                  Binary Searcher                                 *
 *                                                                  *
 *  [   Author  ]       -       Andrew Borg                         *
 *  [   Type    ]       -       Firmware Analysis                   *
 *  [   Date    ]       -       02.01.2014                          *
 *                                                                  *
 * ******************************************************************
 *                                                                  *
 *  Description                                                     *
 *                                                                  *
 *  This Application will Search a Given Binary File                *
 *  and try to identify the Content inside it.                      *
 *                                                                  *
 *  The Signatures are Stored inside an SQLITE 3 Database File      *
 *  along with a Name of the File and a brief Description           *
 *                                                                  *
 * -----------------------------------------------------------------*
 *                      The Binary Searcher                         *
 * -----------------------------------------------------------------*
 *                                                                  *
 *  - Parameters:                                                   *
 *              char * FileName                                     *
 *                  - The FileName of the Binary to Analyse         *   
 *  - Returns:                                                      *
 *              VOID                                                *
 *                                                                  *
 *  - Dependencies:                                                 *
 *              - GetSignatures(char * DatabaseName)                *
 *              - DumpHex (char * FileName)                         *
 *                                                                  *
 * -----------------------------------------------------------------*
 *                      The Get Signatures Method                   *
 * -----------------------------------------------------------------*
 *  This Method will retrieve all the Signature information         *
 *  Stored inside the Database.                                     *
 *                                                                  *
 *  - Parameters:                                                   *
 *              char * DatabaseName                                 *
 *                  - The FileName of the Database File to use      *
 *  - Returns:                                                      *
 *              An Array of type SignatureRow Structure             *
 *                                                                  *
 *  - Dependencies:                                                 *
 *              - SQLITE Libraries                                  *
 * -----------------------------------------------------------------*
 *                      The SQLITE Database                         *
 * -----------------------------------------------------------------*
 *                                                                  *
 *  The Database Holds all Information about the file Signatures    *
 *  that can be retrieved.                                          *
 *                                                                  *
 *  - Database Schema:                                              *
 *              Name : Varchar(45)                                  *
 *              Description : Varchar(100)                          *
 *              Signature : BLOB                                    *
 *                                                                  *
 *          CREATE TABLE Signatures                                 *
 *                      (                                           *
 *                          Name varchar(45),                       *
 *                          Description varchar(100),               *
 *                          Signature BLOB                          *
 *                      );                                          *
 *                                                                  *
 *  - Inserting Custom Signatures                                   *
 *                                                                  *
 *      INSERT INTO Signatures VALUES                               *
 *                                  (                               *
 *                                      '<Name>',                   *
 *                                      '<Description>',            *
 *                                      X'<HEX Signatures>'         *
 *                                  );                              *
 ********************************************************************/
 
#include <stdio.h>
#include <stdlib.h>

// The SQLITE 3 Library is needed Because we are using an SQLITE Database File

#include <sqlite3.h>
#include <string.h>

#include "../Headers/Common.h"

#define DATABASE "Database.DB"


// Structure For the Signature Table

typedef struct
{
    char Name [45];
    char Description [100];
    unsigned char Signature[100];
    
} SignatureRow;

// Function Prototype for the Get Signatures Method

SignatureRow * GetSignatures(char * DatabaseName);

// Function Prototype for the Signature Search Method

void SignatureSearch(char * FileName);

int SignatureCount;

// The Main Method for the Application

// The Main Method will check for the Passed Arguments and redirect the Execution Flow Accordingly

int main(int argc, char * argv[])
{
    // If The Total Number of Arguments is not Equal to Two, show the Syntax
    
    if (argc != 2)
    {
        puts("Syntax: \r\n");
        printf("\t\t %s FILE \r\n", argv[0]);
    }
    // Else Redurect to the Signature Search Method
    
    else 
    {
        puts("Binary Searcher \r\n\r\n");
        SignatureSearch(argv[1]);
    }
}

// This Method will Search the Binary File for Signatures which may reveal contents inside the File
// The Signatures are Stored inside the SQLITE Database and are Retrieved via the Get Signatures Method

void SignatureSearch(char * FileName)
{
    
    // Get the Hex Equivalent of the Binary File
        // The Dump Hex Without Nulls Method is found inside the HexDump.h Header File
        // The Main Difference of the Method and the Stock Hex Dump Method is, that 
        // this method remove any NULL Byte for the sake of using generic string comparism functions
        
    unsigned char * HexDump = DumpHexWithoutNulls(FileName);
    
    
    int Counter = 0;
        
    // Retrieve the Signatures from the Database File
    
    SignatureRow  * Signatures = GetSignatures(DATABASE);
    
    int SignatureByteCounter = 0;
    
    int FilesFound;
    
    // Loop the Signatures and Search the Hex Dump
    
    while (Counter < SignatureCount)
    {
        FilesFound = 0;     
        
        char * Pointer = HexDump;
        
        // Search the whole hexdump, for Multiple Matching Patterns
        
        while ( ( Pointer = strstr( Pointer, Signatures[Counter].Signature)) != NULL ) 
        {
            // Get the Offset of the Found Signature
            
            int Offset = strlen(HexDump)  - strlen(Pointer);
            
            // Print The Offset, along with a brief description of the found File
            
            printf("%s was Found at Offset 0x%X \r\n", Signatures[Counter].Description, Offset);
            
            // Increment the File Found Variable
            
            FilesFound ++;
            
            // Increment the Hex Pointer, to Prevent String, Loop Lockup
            Pointer++;
            
        }
        
        // If any file was Found, print Summary
        
        if (FilesFound > 0)
            printf("\t%d %s Partitions Found ! \r\n\r\n", FilesFound, Signatures[Counter].Name);
        
        // Free the Pointer to Prevent Memory Leaks
        
        free(Pointer);
        
        // Increment Counter, to Search for the next Signature
        
        Counter ++;
        
    }
}

// This Method will retrieve all the information inside the SQLITE Database.
    // Information related to the Signature Files are stored inside an SQLITE Database.
    // The Database should be placed inside the Application's Directory and named Database.DB
    
// This Method will Return an Arraylist of type SignatureRow Structure

SignatureRow * GetSignatures(char * DatabaseName)
{
    sqlite3 * Connection;
    sqlite3_stmt * Result;
    
    const char * End;
    
    // Open the Database
    
    int Error = sqlite3_open(DatabaseName, &Connection);
    
    // If Error Occurs, Print Message and Exit the Application
    
    if (Error)
    {
        puts("Cannot Find Signature File");
        exit(-1);
    }
    
    // Get the Total Row Count to Assign sufficiant memory to the Array List
    
    Error = sqlite3_prepare_v2(Connection, "SELECT COUNT(*) FROM Signatures", 100, &Result, &End);
    
    // If Error Occurs, Print Message and Exit the Application
    
    if (Error != SQLITE_OK)
    {
        puts("Error Getting Signatures");
        exit(-1);
    }
    
    // Get the Row Count from the Previous Transaction
    
    while (sqlite3_step(Result) == SQLITE_ROW)
    {
        SignatureCount = sqlite3_column_int(Result, 0);
    }
    
    // Retrieve the Signatures inside the Database
    
    Error = sqlite3_prepare_v2(Connection, "SELECT Name, Description, Signature, Length(Signature) FROM Signatures", 100, &Result, &End);
    
    // If Error Occurs, Print message and Exit the Application
    
    if (Error != SQLITE_OK)
    {
        puts("Error Getting Signatures");
        exit(-1);
    }

    // Create a SignatureRow Array and assign Sufficiant Memory to it
    
    SignatureRow * Signatures = malloc(sizeof(SignatureRow) * 100); 
    
    SignatureRow Temp;
    
    int Counter = 0;
    
    // Loop the Row Counts from the Previous Transaction
        
    while (sqlite3_step(Result) == SQLITE_ROW)
    {
        // Copy the Signature name to the Name Property of the Signature Row Structure
        
        memcpy(Temp.Name, sqlite3_column_text(Result, 0), 45);
        
        // Copy the Signature Description inside the Description Property of the Signature Row Structure
        
        memcpy(Temp.Description, sqlite3_column_text(Result, 1), 100);
        
        // Copy the Actual Signature inside the Signatutre Property of the Signature Row Structure
        
        memcpy(Temp.Signature, sqlite3_column_text(Result, 2), 20);         

        
        int SignatureSize = sqlite3_column_int(Result,3);               
        
        unsigned char TempSignature[20];
        
        printf("Getting Signature for: %s \r\n", Temp.Name);
        
        int ByteCounter = 0;
        
        // Iterate the Signature and Replace any NULL Byte with 0xFF
                    // As mentioned earlier this is carried out
                    // to use generic string comparism functions
                    // which stop scanning Strings when a NULL Character is found
                    
        while (ByteCounter < SignatureSize)
        {
            char T = Temp.Signature[ByteCounter];
            
            // If The Character is a NULL Byte
            if (T == '\0')
            {
                // Replace it with 0xFF
                
                T = 0xFF;
            }
            
            TempSignature[ByteCounter ++] = T;
        }
    
        // Save the Newly modified Signature
        
        memcpy(Temp.Signature, TempSignature, SignatureSize);   
        
        // Copy the Temp Signature row inside the Array
        
        Signatures[Counter ++] = Temp;
        
        
    }
    
    printf("----------------------------------------");
    
    printf("\r\n\r\n");
    
    
    // Return all the Signatures Retrieved from the database
    return Signatures;
    
}
