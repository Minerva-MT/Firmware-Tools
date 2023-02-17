/********************************************************************
 *                  Serial Connector for Belkin Routers             *
 *                                                                  *
 *  [   Author  ]       -       Andrew Borg                         *
 *  [   Type    ]       -       Serial Port Connector               *
 *  [   Date    ]       -       19.01.2013                          *
 *                                                                  *
 * ******************************************************************
 *                                                                  *
 *  Description                                                     *
 *                                                                  *
 * Many IOT Devices include a UART Connector, which can be          *
 * used to review debug messages, flash the firmware and for        *
 * other tasks.                                                     *
 *                                                                  *
 * The Purpose of this application is to allow the user to connect  *
 * to the Serial interface of the Router using a UART Cable.        *       
 *                                                                  *
 * Tested on a Belkin Router                                        *
 * ******************************************************************
 */
 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "../Headers/Common.h"

#define STDOUT 1

/* Function Prototypes */

FILE * FileOpener(char * Filename, char * ReadMode);

// The Below Methods are used to Print the Syntax of the Application and the baudrates allowed

void PrintHelp();
void PrintBaud();

// This Method does all the Serial Configuration and Connection

int ConnectSerial(char * PortName, speed_t BaudRate, char * LogFile);

// This Method Converts the User's Chosen Baudrate (String) to SPEED_T

speed_t ReturnBaud(int BaudRate);

// The Main Method will check the Passed Arguments and redirect the Applications' Flow Accordingly

int main (int argc, char ** argv)
{
    if (argc == 3)
    {       
        speed_t BaudRate = ReturnBaud(atoi(argv[2]));
        
        ConnectSerial(argv[1], BaudRate, "");
    }
    else if (argc == 2)
    {
        if (strcmp(argv[1], "-List") == 0)
        {
            PrintBaud();
            exit(0);
        }
        else
        {
            PrintHelp(argv[0]);
        }
    }
    else if (argc == 5)
    {
        if (strcmp(argv[1], "-Write") == 0)
        {
                
            speed_t BaudRate = ReturnBaud(atoi(argv[4]));
            
            
            ConnectSerial(argv[3], BaudRate, argv[2]);
        }
    }
    else
    {
        PrintHelp(argv[0]);
    }
    
}


// Prints the Syntax Help

void PrintHelp(char * ProgramName)
{
    puts("Serial Port Terminal \r\n");
        
        puts("Sytax : ");
        
            printf("\t %s <Serial Port> <Baud Rate> \r\n\r\n", ProgramName);
            printf("\t %s -Write LOGFILE <Serial Port> <Baud Rate> \r\n\r\n", ProgramName);
        
    puts("Available Options:");
        
        printf("\t -List : List Available Buad Rates \r\n\r\n");
        printf("\t -Write LOGFILE : Write Serial Output to Logfile \r\n\r\n");
            
    exit(0);
    
}

// Prints the Allowed Baud Rates

void PrintBaud()
{
    printf("%s", "\r\nAvailable Baud Rates \r\n");
    
    printf("%10s \r\n", "0");
    printf("%10s \r\n", "50");
    printf("%10s \r\n", "75");
    printf("%10s \r\n", "110");
    printf("%10s \r\n", "134");
    printf("%10s \r\n", "150");
    printf("%10s \r\n", "200");
    printf("%10s \r\n", "300");
    printf("%10s \r\n", "600");
    printf("%10s \r\n", "1200");
    printf("%10s \r\n", "1800");
    printf("%10s \r\n", "2400");
    printf("%10s \r\n", "4800");
    printf("%10s \r\n", "9600");
    printf("%10s \r\n", "19200");
    printf("%10s \r\n", "38400");
    printf("%10s \r\n", "57600");
    printf("%10s \r\n", "115200");
    printf("%10s \r\n", "230400");
}

int ConnectSerial(char * PortName, speed_t BaudRate, char * LogFile)
{
    
            
            
    if (strcmp(LogFile, "") != 0)
    {
        printf("Output is Going to Be Written to %s\r\n", LogFile);
    }
    else
    {
        puts("Output is not Going to be Logged");
    } 
    
    puts("Press CTRL-C To Close the Connection");

    /* Structures to Hold the Settings for 
     
            - Standard Output and the Serial Port Configurations
            - Backup Standard Output
            
    */
        
    struct termios OldSettings, Terminal;
    
    int FileDescriptor;
    
    unsigned char Character = 'X';
    
    // Get The Standard Output's Old Settings and Store then in Old Settings

    tcgetattr(STDOUT, &OldSettings);

    // Allocate Memory for the New Settings Structure
    
    memset(&Terminal, 0, sizeof(Terminal));

    /*
      Configure the Standard Output
      
        - The Below Configurations are Found inside the Termios Header File
            + Input Modes           - C_IFLAG
            + Output Modes          - C_OFLAG
            + Control Modes         - C_CFLAG
            + Local Modes           - C_LFLAG
            + Special Characters    - C_CC
        
    */
    
    // Raw Input Mode

    Terminal.c_iflag    = 0;
    
    // Raw Output Mode
    
    Terminal.c_oflag    = 0;
    
    // Raw Control Mode
    
    Terminal.c_cflag    = 0;
    
    // Raw Local Mode
    
    Terminal.c_lflag    = 0;
    
    // Minimum Characters for Non-Canonical Read
    
    Terminal.c_cc[VMIN]     = 1;
    
    // Time out in Deci Seconds - For Non-Canonical Read
    
    Terminal.c_cc[VTIME] = 0;
    
    // Apply the settings for the Standard Output Immediately

    tcsetattr(STDOUT, TCSANOW, &Terminal);

    
    /*  Configure the Standard Input
            - Set The File Status Flags (F_SETFL)
            - Set No Blocking   */
        
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

    // Allocate Memory for the Serial Port 
    
    memset(&Terminal, 0, sizeof(Terminal));
    
    // Set the Serial Port Configurations - Check Above for Details
    
    Terminal.c_iflag        =   0;
    Terminal.c_oflag        =   0;
    
    /* Set the Control Flags TO:
            + Eight Character Size
            + Enable Read
            + Ignore Modem Control Lines    */
            
    Terminal.c_cflag        =   CS8|CREAD|CLOCAL;
    
    Terminal.c_lflag        =   0;
    
    Terminal.c_cc[VMIN]     =   1;
    
    Terminal.c_cc[VTIME]    =   5;
    
    //Open the Serial Port and Store it's File Descriptor inside the Variable FileDescriptor

    FileDescriptor = open(PortName, O_RDWR | O_NONBLOCK);

    // Set the Baudrate of the Serial Port to the One Specified by the User ( Both for the Input (cfsetispeed) and Output (cfsetospeed)
    
    cfsetospeed(&Terminal, BaudRate);
    cfsetispeed(&Terminal, BaudRate);
    
    // Apply the Settings - TCSANOW Apply the Settings Immediately

    tcsetattr(FileDescriptor, TCSANOW, &Terminal);
    
    FILE * OutputFile;

    if (strcmp(LogFile, "") != 0)
    {
        OutputFile = FileOpener(LogFile, "w");
    }
    // Keep Connected with the Serial Port until the CTRL + C Sequence Button is Pressed
        
    while (1)
    {
        // Read the Serial Port and if New Data is Available, Display It
        
        if (read(FileDescriptor, &Character, 1) > 0)
        {
            write(STDOUT_FILENO, &Character, 1);
            
            if (strcmp(LogFile, "") != 0)
            {               
                fputc(Character, OutputFile);
            }
            
        }
        
        // Read the Console and if New Input is Available, Send it to the Serial Port
        
        if (read(STDIN_FILENO, &Character, 1) > 0)
        {
            if (Character != 0x03)
            {
                write(FileDescriptor, &Character, 1);
            }
            else
            {
                break;
            }
        }
            
    }
    
    // Close the Serial Port
    
    close(FileDescriptor);
    
    // Apply the Old Settings Immediately (TCSANOW)
    
    tcsetattr(STDOUT, TCSANOW, &OldSettings);
    
    puts("Exiting");
    
    return 1;
    
}

speed_t ReturnBaud(int BaudRate)
{
    switch(BaudRate)
    {
            case 0:
                return B0;
            
            case 50:
                return B50;
            
            case 75:
                return B75;
         
            case 110:
                return B110;
            
            case 134:
                return B134;
            
            case 150:
                return B150;
            
            case 200:
                return B200;
            
            case 300:
                return B300;
            
            case 600:
                return B600;
            
            case 1200:
                return B1200;
            
            case 1800:
                return B1800;
            
            case 2400:
                return B2400;
            
            case 4800:
                return B4800;
            
            case 9600:
                return B9600;
            
            case 19200:
                return B19200;
            
            case 38400:
                return B38400;
            
            case 57600:
                return B57600;
            
            case 115200:
                return B115200;
            
            case 230400:
                return B230400;
                
            // If an Incorrect Baudrate is Enterred Exit the Application
            default:
                exit(-1);
    }
}
