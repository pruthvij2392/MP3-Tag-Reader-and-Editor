#include <stdio.h>
#include <string.h>
#include "types.h"
#include "view.h"

/* Function Definitions */

/* Check which Operation is to be Performed
 * Input: Command Line Arguments
 * Output: Operation to be Performed
 * Return Value: p_view, p_edit, p_help or p_unsupported on Invalid options provided by User
 */

OperationType check_operation (char* argv[]){
    if (strcmp (argv [1], "-v") == 0)
        return p_view;
    else if (strcmp (argv [1], "-e") == 0)
        return p_edit;
    else if (strcmp (argv [1], "-help") == 0 || strcmp (argv [1], "-h") == 0)
        return p_help;
    return p_unsupported;
}

/* Check if the Read Command Line Arguments are Valid
 * Input: Command Line Arguments and Structure Pointer
 * Output: Source MP3 file
 * Return Value: p_success or p_failure on Invalid Arguments passed
 */

Status read_and_validate_mp3_file (char* argv[], TagInfo* mp3tagInfo){
    if (argv [2] == NULL){
        printf ("INFO: For Viewing the Tags -> .\\a.exe -v <file_name.mp3>\n");
        return p_failure;
    }
    else{
        mp3tagInfo->fptr_mp3 = fopen (argv [2], "r");	//Open the file in Read Only Mode.
        if (mp3tagInfo->fptr_mp3 != NULL){
            char str[3];
            fread (str, 1, 3, mp3tagInfo->fptr_mp3);    //Read the file Signature. For our MP3 file, it is "ID3".
            if ((strncmp (str, "ID3", 3)) == 0){			//Check if the value read in 'str' is matching with the MP3 file Signature "ID3".
                printf ("     Version ID: v2.3     \n");
                printf ("--------------------------\n");
            }
            else{		//If the file Signature is not "ID3", it is not a '.mp3' file and no further operation can be performed.
                printf ("ERROR: The file Signature is not matching with that of a '.mp3' file.\n");
                printf ("INFO: For Viewing the Tags -> .\\a.exe -v <file_name.mp3>\n");
                return p_failure;
            }
        }
        else{		//If the file does not open, we cannot read and validate the arguments to the Structure. So, return 'p_failure'.
            printf ("ERROR: Unable to Open the %s file.\n", argv [2]);
            printf ("INFO: For Viewing the Tags -> .\\a.exe -v <file_name.mp3>\n");
            return p_failure;
        }

        fclose (mp3tagInfo->fptr_mp3);						//Close the file opened previously.
    }

    //No p-failure returned above, then return p_success.
    return p_success;
}

/* To view all the Frame contents of the MP3 file Tag
 * Input: Command Line Arguments and Structure Pointer
 * Output: All the Frame Contents
 * Return Value: p_success or p_failure
 */

Status view_tag (char* argv[], TagInfo* mp3tagInfo){
    mp3tagInfo->fptr_mp3 = fopen (argv [2], "r");		//Open the file in Read Only Mode.
    fseek (mp3tagInfo->fptr_mp3, 10L, SEEK_SET);		//The first 10 bytes of the '.mp3' file is ID3 Header and need to be skipped.
	
    char* frame_tag [6] = {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"};
    char* imfo [6] = {"Title name", "Artist name", "Album", "Year", "Content type", "Composer"};
    char str[5];
    int size;
    for(int i=0;i<6;i++){
        fread(str,4,1,mp3tagInfo->fptr_mp3);
        if(strncmp(str,frame_tag[i],4)==0){
            printf("Tag : %s -> ",str);
        }
        fread(&size,4,1,mp3tagInfo->fptr_mp3);
        char *ptr=(char *)(&size);
        for(int i=0;i<2;i++){
            char temp=*(ptr+i);
            *(ptr+i)=*(ptr+4-i-1);
            *(ptr+4-i-1)=temp;
        }
        //printf("size : %d\n",size);
        char str[size-1];
        fseek(mp3tagInfo->fptr_mp3,3,SEEK_CUR);
        fread(str,sizeof(str),1,mp3tagInfo->fptr_mp3);
        str[size-1]='\0';
        printf("%-15s : %s\n",imfo[i],str);
    }

    printf ("--------------------------\n");
    fclose (mp3tagInfo->fptr_mp3);				//To close the Source MP3 file.

    //No p-failure returned above, then return p_success.
    return p_success;
}