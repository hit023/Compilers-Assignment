#include <stdlib.h>
#include <stdio.h>
#include "tokens.h"

char* read_file(char* fname)
{
    char* buffer = NULL;
    int program_size,read_size;
    FILE* f = fopen(fname,"r");
    if(f)
    {
        fseek(f,0,SEEK_END);
        program_size = ftell(f);
        fseek(f,0L,SEEK_SET);
        buffer = (char*)malloc(sizeof(char) * (program_size+1));
        read_size = fread(buffer,sizeof(char),program_size,f);
        buffer[program_size] = '\0';
        if(program_size != read_size)
        {
            free(buffer);
            buffer = 0;
            printf("Error reading the file.\n");
        }
        fclose(f);
    }
    return buffer;
}

int main(int argc, char *argv[])
{
    char* program = read_file(argv[1]);
    getTokens(program);
    printTokens();
    return 0;
}
