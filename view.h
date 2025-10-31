#ifndef VIEW_H
#define VIEW_H

#include "types.h"      //Contains User-defined Data Types.

/*
 * Structure to store information required for fptr
 * Information about Output data is also stored
 */

typedef struct _TagInfo{
    /* Source MP3 file Info */
    FILE* fptr_mp3;

} TagInfo;

/* Printing MP3 Tag details Function Prototype */

/* Check Operation type */
OperationType check_operation (char* argv[]);

/* Perform Validation of the Arguments passed and store in the Structure */
Status read_and_validate_mp3_file (char* argv[], TagInfo* mp3tagInfo);

/* Perform the Viewing Tag details Operation */
Status view_tag (char* argv[], TagInfo* mp3tagInfo);

#endif