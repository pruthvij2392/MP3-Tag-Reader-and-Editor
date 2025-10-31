#include <stdio.h>
#include <string.h>
#include "types.h"
#include "edit.h"

Status read_and_validate_mp3_file_args(char* argv[],TagData* mp3tagData){
    if (argv[2]==NULL){
        printf ("INFO: For Editing the Tags -> .\\a.out -e <modifier> \"New_Value\" <file_name.mp3>\n");
        printf ("INFO: Modifier Functions:\n");
        printf ("-t\t Title Tag\n-a\t Artist Tag\n-A\t Album Tag\n-y\t Year Tag\n-d\t Content Type Tag\n-c\t Comments Tag\n");
        return p_failure;
    }
    else{
        if ((strncmp(argv[2],"-t",2)==0)){
            strncpy(mp3tagData->frame_Id,"TIT2",5);
        }
        else if((strncmp(argv[2],"-a",2)==0)){
            strncpy(mp3tagData->frame_Id,"TPE1",5);
        }
        else if((strncmp(argv[2],"-A",2)==0)){
            strncpy(mp3tagData->frame_Id,"TALB",5);
        }
        else if ((strncmp(argv[2],"-y",2)==0)){
            strncpy(mp3tagData->frame_Id,"TYER",5);
        }
        else if ((strncmp(argv[2],"-d",2)==0)){
            strncpy(mp3tagData->frame_Id,"TCON",5);
        }
        else if ((strncmp(argv[2],"-c",2)==0)){
            strncpy(mp3tagData->frame_Id,"COMM",5);
        }
        else{
            printf ("ERROR: Unsupported Modifier.\n");
            printf ("INFO: Modifier Functions:\n");
            printf ("-t\tModify Title Tag\n-A\tModify Artist Tag\n-a\tModify Album Tag\n-y\tModify Year Tag\n-G\tModify Content Type Tag\n-c\tModify Comments Tag\n");
            return p_failure;
        }
        printf ("Frame ID to change: %s.\n",mp3tagData->frame_Id);
    }

    if (argv[3]==NULL){
        printf ("ERROR: New_Value to be updated on the Frame ID %s is Empty.\n", mp3tagData->frame_Id);
        printf ("INFO: For Editing the Tags -> .\\a.out -e <modifier> \"New_Value\" <file_name.mp3>\n");
        return p_failure;
    }
    else{
        mp3tagData->frame_Id_size = strlen(argv[3])+1;
        if (mp3tagData->frame_Id_size<100)
        {
            printf ("Length of the Data in CLA = %d.\n", mp3tagData->frame_Id_size);
            strncpy (mp3tagData->frame_Id_value, argv [3], mp3tagData->frame_Id_size);
            printf ("Updated Value of Frame ID to %s.\n", mp3tagData->frame_Id_value);
        }
        else
        {
            printf ("ERROR: Length of the Data is too Long!.\n");
            return p_failure;
        }
    }

    if (argv[4]==NULL){
        printf("INFO: For Editing the Tags -> .\\a.out -e <modifier> \"New_Value\" <file_name.mp3>\n");
        return p_failure;
    }
    else{
        mp3tagData->fptr_mp3 = fopen(argv[4],"r");
        if (mp3tagData->fptr_mp3 != NULL){
            char str[3];
            fread(str,1,3,mp3tagData->fptr_mp3);
            if ((strncmp (str, "ID3", 3)) == 0){
                printf ("     Version ID: v2.3     \n");
                printf ("--------------------------\n");
            }
            else{
                printf ("ERROR: The file Signature is not matching with that of a '.mp3' file.\n");
                printf ("INFO: For Editing the Tags -> .//a.out -e <modifier> \"New_Value\" <file_name.mp3>\n");
                return p_failure;
            }
        }
        else{
            printf ("ERROR: Unable to Open the %s file.\n",argv[4]);
            printf ("INFO: For Editing the Tags -> .//a.out -e <modifier> \"New_Value\" <file_name.mp3>\n");
            return p_failure;
        }
        fclose (mp3tagData->fptr_mp3);
    }
    return p_success;
}

Status edit_tag (char* argv[], TagData* mp3tagData){
    mp3tagData->fptr_mp3 = fopen (argv [4], "r");       // opening song.mp3 in read mode
    mp3tagData->fptr_temp = fopen ("temp.mp3", "w");    // opening temp.mp3 in write mode

    fseek (mp3tagData->fptr_mp3,0,SEEK_SET);     // setting both pointers at 0th position
    fseek (mp3tagData->fptr_temp,0,SEEK_SET);

    char str [10];
    fread (str,1,10,mp3tagData->fptr_mp3);  //read 10 bytes header of the song.mp3
    fwrite (str,1,10,mp3tagData->fptr_temp);

    char* frame_tag [6] = {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"};
    for (int i=0;i<6;i++){
        fread (str,1,4,mp3tagData->fptr_mp3);
        fwrite (str,1,4,mp3tagData->fptr_temp);
        if ((strncmp(mp3tagData->frame_Id,frame_tag [i], 4)) == 0){
            if (copy_data (mp3tagData, 1) == p_failure){
                printf ("ERROR: Unable to copy the updated Data from Source to temp '.mp3' file.\n");
                return p_failure;
            }
        }
        else{
            if (copy_data (mp3tagData, 0) == p_failure){
                printf ("ERROR: Unable to copy the remaining Data from Source to temp '.mp3' file.\n");
                return p_failure;
            }
        }
    }

    if (copy_remaining_data (mp3tagData) == p_failure){
        printf ("ERROR: Unable to copy the remaining Data from Source to temp '.mp3' file.\n");
        return p_failure;
    }

	fclose (mp3tagData->fptr_mp3);
	fclose (mp3tagData->fptr_temp);

	mp3tagData->fptr_mp3 = fopen (argv [4], "w");   // opening main mp3 file in write mode 
	mp3tagData->fptr_temp = fopen ("temp.mp3", "r");     // opening main mp3 file in read mode 

    if (copy_back_to_src (mp3tagData) == p_failure){
        printf ("ERROR: Unable to copy the remaining Data from temp.mp3 to Source '.mp3' file.\n");
        return p_failure;
    }

    //No p-failure returned above, then return p_success.
    return p_success;
}

Status copy_data (TagData* mp3tagData, char flag){
    if(flag==1){
        uint old_size,new_size;
        fread(&old_size,4,1,mp3tagData->fptr_mp3);
        char* ptr = (char*) &old_size;
        for(int i=0;i<2;i++){     //Reversal of the Bytes after reading to get the correct value of the 'Size'.
            char temp=*(ptr+i);
            *(ptr+i)=*(ptr+4-i-1);
            *(ptr+4-i-1)=temp;
        }
        printf("Old Tag Data Size = %d.\n", old_size);

        new_size = mp3tagData->frame_Id_size;
        ptr = (char*) &mp3tagData->frame_Id_size;
        for(int i=0;i<2;i++){     //Reversal of the Bytes after reading to get the correct value of the 'Size'.
            char temp=*(ptr+i);
            *(ptr+i)=*(ptr+4-i-1);
            *(ptr+4-i-1)=temp;
        }
        printf("New Tag Data Size = %X.\n", mp3tagData->frame_Id_size);
        fwrite(&mp3tagData->frame_Id_size,4,1,mp3tagData->fptr_temp);

        char f[3];
        fread(f,1,3,mp3tagData->fptr_mp3);
        fwrite(f,1,3,mp3tagData->fptr_temp);

        fwrite(mp3tagData->frame_Id_value,1,(new_size-1),mp3tagData->fptr_temp);   // writing my new string  

        fseek(mp3tagData->fptr_mp3,(long)(old_size-1),SEEK_CUR);  // moving main mp3 file pointer to old size-1 bytes 
        printf("New Data = %s.\n",mp3tagData->frame_Id_value);
    }
    else{
        uint size;
        fread(&size,4,1,mp3tagData->fptr_mp3);
        fwrite(&size,4,1,mp3tagData->fptr_temp);
        char* ptr = (char*) &size;
        for(int i=0;i<2;i++){     //Reversal of the Bytes after reading to get the correct value of the 'Size'.
            char temp=*(ptr+i);
            *(ptr+i)=*(ptr+4-i-1);
            *(ptr+4-i-1)=temp;
        }
        printf("Size of the Frame ID content = %d.\n", size);
        char f[3];
        fread(f,1,3,mp3tagData->fptr_mp3);
        fwrite(f,1,3,mp3tagData->fptr_temp);

        char str[size-1];
        fread(str,1,(size-1),mp3tagData->fptr_mp3);
        str[size-1] ='\0';
        printf("Frame ID content = %s.\n",str);
        fwrite(str,1,(size-1),mp3tagData->fptr_temp);
    }
    printf ("MP3 position = %lu.\n",ftell(mp3tagData->fptr_mp3));
    printf ("Temp position = %lu.\n",ftell(mp3tagData->fptr_temp));
    //No p-failure returned above, then return p_success.
    return p_success;
}

Status copy_remaining_data (TagData* mp3tagData){
    printf("Copy Remaining Data:\n");
    char ch;
    printf("Temp position = %lu.\n",ftell(mp3tagData->fptr_temp));
    printf("MP3 position = %lu.\n",ftell(mp3tagData->fptr_mp3));

    while(fread(&ch,1,1,mp3tagData->fptr_mp3)){
        fwrite(&ch,1,1,mp3tagData->fptr_temp);
    }

    printf("MP3 position = %lu.\n",ftell(mp3tagData->fptr_mp3));
    printf("Temp position = %lu.\n",ftell(mp3tagData->fptr_temp));

     //No p-failure returned above, then return p_success.
     return p_success;
}

Status copy_back_to_src (TagData* mp3tagData){
	printf("Copy Data Back to Source:\n");
	fseek(mp3tagData->fptr_mp3,0,SEEK_SET);
	fseek(mp3tagData->fptr_temp,0,SEEK_SET);
	char ch;

    while (fread(&ch,1,1,mp3tagData->fptr_temp)){
        fwrite(&ch,1,1,mp3tagData->fptr_mp3);
    }
    printf("MP3 position = %lu.\n", ftell (mp3tagData->fptr_mp3));
    printf("Temp position = %lu.\n", ftell (mp3tagData->fptr_temp));

	//Update the new File size in the Header of sample.mp3.
    return p_success;
}