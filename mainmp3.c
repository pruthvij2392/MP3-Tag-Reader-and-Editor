/*DOCUMENTATION
NAME:pruthvi jyoti
BATCH : 24018_018
DATE:26/10/2024
DES:MP3_TAG_READER_AND_EDITOR
*/
#include <stdio.h>
#include "types.h"
#include "view.h"
#include "edit.h"

int main(int argc, char* argv[])
{
    TagInfo mp3tagInfo;
    TagData mp3tagData;

    if (argc<2){	
        printf ("ERROR: Incorrect format of Command Line Arguments.\n");
        printf ("INFO: Use .\\a.exe/a.out -help or -h for Help menu.\n");
    }
    else{
        OperationType ret = check_operation (argv);	//To check which Operation needs to be performed based on the Option passed in Command Line.

        if (ret==p_view){	
            printf ("--------------------------\nMP3 Tag Reader and Editor\n--------------------------\n");
            // validate all the arguments passed in Command Line
            if (read_and_validate_mp3_file (argv, &mp3tagInfo) == p_success){	//If validation is successful, proceed with the Viewing process.
                //To execute the Viewing process of the MP3 Tag.
                if (view_tag (argv, &mp3tagInfo) == p_success){		
                    printf ("INFO: Done.\n");
                    printf ("--------------------------\n");
                }
            }
        }
        else if(ret==p_edit){			
            printf ("MP3 Tag Reader and Editor:\n");
            printf ("--------------------------\n");
            //To validate all the arguments passed in Command Line.
            if (read_and_validate_mp3_file_args(argv,&mp3tagData) == p_success){	//If validation is successful, proceed with the Editing process.
                //To execute the Editing process of the MP3 Tag.
                if (edit_tag(argv,&mp3tagData) == p_success){	
                    printf("INFO: Done.\n--------------------------\n");
                    //Call the view_tag function to display the Updated content.
                }
            }
        }
        else if(ret==p_help){			
            printf ("INFO: Help Menu for Tag Reader and Editor:\n");
            printf ("For Viewing the Tags -> .\\a.exe/a.out -v <file_name.mp3>\n");
            printf ("For Editing the Tags -> .\\a.exe/a.out -e <modifier> \"New_Value\" <file_name.mp3>\n");
            printf ("INFO: Modifier Functions:\n");
            printf ("-t\t Title Tag\n-a\t Artist Tag\n-A\t Album Tag\n-y\t Year Tag\n-d\t Content Type Tag\n-c\t Comments Tag\n");
        }
        else if(ret==p_unsupported){	
            printf ("ERROR: Unsupported Operation.\n");
            printf ("INFO: Use \\a.exe -help or -h for Help menu.\n");
        }
    }
    return 0;
}
