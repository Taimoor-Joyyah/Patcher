#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void help();
void create_patch(char *subject_file, char *object_file);
void patch_file(char *subject_file, char *object_file);
unsigned int filesize(FILE *fptr);
char *filename(char *file_name);
char *extension(char *file_name);
unsigned char invert(unsigned char data);

int main(int argc, const char *argv[])
{
    if(argc == 3)
    {
        char exfile[sizeof(argv[2])];
        strcpy(exfile, argv[2]);
        if(!strcmp(extension(exfile), "pd"))
        {
            patch_file(argv[1], argv[2]);
        }
        else
        {
            create_patch(argv[1], argv[2]);
        }
    }
    else
    {
        if(argc == 4)
        {
            if(argv[1][0] == '-')
            {
                switch(argv[1][1])
                {
                case 'h':
                    help();
                    break;
                case 'p':
                    patch_file(argv[2], argv[3]);
                    break;
                case 'c':
                    create_patch(argv[2], argv[3]);
                    break;
                default:
                    printf("Invalid Arguments!!!");
                }
            }
        }
        else
        {
            printf("Invalid Arguments!!!");
        }
    }
    return 0;
}

void help()
{
    printf("This program is for creating patch and for patching files.");
    printf("\nSyntax,\n\tpatcher [mode] [subject_file] [object_file]\nModes\n\t-h\thelp.");
    printf("\n\t-p\tpatching file.\n\t-c\tcreate patch.");
    printf("\nsubject_file is file on which operation is to be done.");
    printf("\nobject_file is file which help operation. it could be a patchdata.");
    printf("\nThe program is created by MTaimoorJoyyah.");
    printf("\nThanks!!!");
}

void create_patch(char *subject_file, char *object_file)
{
    FILE *subf, *objf, *resf; // subject file, object file, result file(patchdata file)
    char *subp = malloc(1), *objp = malloc(1); // subject pointer, object pointer, result pointer
    unsigned int pos, objflen, subflen;
    unsigned char data; // position, object file length
    unsigned char *name, fname[32];
    int cont = 0, difcount = 0; // continuous, difference count
    int curpos, prevpos;
    int i, ntime;
    char signature[] = ".pd.mtj.";
    subf = fopen(subject_file, "rb");
    objf = fopen(object_file, "rb");
    if(!(objf && subf)) // checking input files if existing
    {
        printf("File(s) not found.");
        exit(0);
    }
    objflen = filesize(objf);
    subflen = filesize(subf);
    resf = fopen(".tmp1", "wb");
    printf("Temp file created.");
    for(i = 0; i < 8; i++)
    {
        signature[i] = invert(signature[i]);
    }
    fwrite(signature, 8, 1, resf);
    fwrite(&objflen, 4, 1, resf);
    fwrite(&difcount, 4, 1, resf);

    ntime = time(NULL);
    printf("\nCreating Patch-data...%5.2f%%", 0);
    for(i = 0; (i < objflen) || (cont != 0); i++)
    {
        if(ntime != time(NULL) || (i+1) == objflen)
        {
            ntime = time(NULL);
            printf("\b\b\b\b\b\b%5.2f%%", ((float) (i+1) / (float) objflen) * 100);
        }
        pos = ftell(objf);
        fread(objp, 1, 1, objf);
        fread(subp, 1, 1, subf);
        if(((*subp != *objp) || i >= subflen) && (i < objflen) )
        {
            if(cont == 0)
            {
                fwrite(&pos, 4, 1, resf);
                prevpos = ftell(resf);
                fwrite(&cont, 4, 1, resf);
            }
            data = *objp;
            data = invert(data);
            fwrite(&data, 1, 1, resf);
            cont++;
        }
        else if(cont != 0)
        {
            curpos = ftell(resf);
            fseek(resf, prevpos, SEEK_SET);
            fwrite(&cont, 4, 1, resf);
            fseek(resf, curpos, SEEK_SET);
            difcount++;
            cont = 0;
        }
    }
    fseek(resf, 12, SEEK_SET);
    fwrite(&difcount, 4, 1, resf);
    fclose(subf);
    fclose(objf);
    fclose(resf);
    name = filename(subject_file);
    strcpy(fname, name);
    strcat(fname, ".pd");
    remove(fname);
    rename(".tmp1", fname);
    printf("\nPatch file created.");
}

void patch_file(char *subject_file, char *object_file)
{
    FILE *subf, *objf, *resf; // subject file, object file, result file(patchdata file)
    unsigned int pos;
    unsigned char data;
    unsigned char *name, *pname, fname[32], ch;
    unsigned int resflen, fsize; // result file length
    char signature[] = ".pd.mtj.";
    int difcount, cont;
    int i, j;
    int ntime;
    subf = fopen(subject_file, "rb");
    objf = fopen(object_file, "rb");
    if(!(objf && subf)) // checking input files if existing
    {
        printf("file(s) not found.");
        exit(0);
    }
    resf = fopen(".tmp2", "wb");
    printf("Temp file created.");
    for(int i = 0; i < 8; i++)
    {
        fread(&ch, 1, 1, objf);
        if(invert(ch) != signature[i])
        {
            printf("\nInvalid Patch Data File!!!");
            fclose(resf);
            remove(".tmp2");
            exit(0);
        }
    }
    fread(&resflen, 4, 1, objf);
    fread(&difcount, 4, 1, objf);
    if(resflen > filesize(subf))
        fsize = filesize(subf);
    else
        fsize = resflen;
    ntime = time(NULL);
    printf("\nDuplicate Subject File...%5.2f%%", 0);
    for(i = 0; i < fsize; i++)
    {
        if(ntime != time(NULL) || (i+1) == fsize)
        {
            ntime = time(NULL);
            printf("\b\b\b\b\b\b%5.2f%%", ((float) (i+1) / (float) fsize) * 100);
        }
        fread(&data, 1, 1, subf);
        fwrite(&data, 1, 1, resf);
    }
    printf("\nApplying Patch...%5.2f%%", 0);
    for(i = 0; i < difcount; i++)
    {
        if(ntime != time(NULL) || (i+1) == difcount)
        {
            ntime = time(NULL);
            printf("\b\b\b\b\b\b%5.2f%%", ((float) (i+1) / (float) difcount) * 100);
        }
        fread(&pos, 4, 1, objf);
        fread(&cont, 4, 1, objf);
        fseek(resf, pos, SEEK_SET);
        for(j = 0; j < cont; j++)
        {
            fread(&data, 1, 1, objf);
            data = invert(data);
            fwrite(&data, 1, 1, resf);
        }
    }
    fclose(subf);
    fclose(objf);
    fclose(resf);
    name = filename(subject_file);
    pname = strchr(name, '.');
    strcpy(fname, name);
    strcpy(&fname[strcspn(name, ".")], "_patched");
    strcat(fname, pname);
    remove(fname);
    rename(".tmp2", fname);
    printf("\nFile patched.");
}

unsigned int filesize(FILE *fptr)
{
    unsigned int curpos, endpos;
    curpos = ftell(fptr);
    fseek(fptr, 0, SEEK_END);
    endpos = ftell(fptr);
    fseek(fptr, curpos, SEEK_SET);
    return endpos;
}

char *filename(char *file_name)
{
    char *token, *filename;
    token = strtok(file_name, "\\");
    while(token != NULL)
    {
        filename = token;
        token = strtok(NULL, "\\");
    }
    return filename;
}

char *extension(char *file_name)
{
    char *token, *ext;
    token = strtok(file_name, ".");
    while(token != NULL)
    {
        ext = token;
        token = strtok(NULL, ".");
    }
    return ext;
}

unsigned char invert(unsigned char data)
{
    if(data < 128)
        data = data + 128;
    else
        data = data - 128;
    return data;
}
