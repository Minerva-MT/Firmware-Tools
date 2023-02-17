/********************************************************************
 *                  Common Header File                              *
 *                                                                  *
 *  [   Author  ]       -       Andrew Borg                         *
 *  [   Type    ]       -       Firmware Analysis                   *
 *  [   Date    ]       -       02.01.2014                          *
 *                                                                  *
 * ******************************************************************
 *                                                                  *
 *  Description                                                     *
 *                                                                  *
 * The Purpose of this Header file is to Include the Hex Dump       *
 * functionality and File Opener to other Functions.                *
 *                                                                  *
 * ******************************************************************
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Sizes.h"

extern unsigned long FileSize;

FILE * FileOpener(char * Filename, char * ReadMode);
char * DumpHex(char * FileName);
char * DumpHexWithoutNulls(char * FileName);

