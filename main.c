#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int blacklistAdd(char *filepath, char *line);
int blacklistRemove(char *filepath, char *line);

int main(int argc, char **argv)
{
    int status;
    char *filepath = "C:\\Windows\\System32\\drivers\\etc\\hosts-test";
    if(argc < 2) {
        printf("Output help\n");
        return 0;
    }
    char url[256] = "127.0.0.1 ";
    char opt = argv[1][1];
    switch(opt) {
        case 'a':
            strcat(url, argv[2]);
            status = blacklistAdd(filepath, url);
            if(status == 0) {
                printf("%s added to blacklist.\n", url);
            } else {
                printf("File error.\n");
            }
            break;
        case 'r':
            strcat(url, argv[2]);
            status = blacklistRemove(filepath, url);
            if(status == 0) {
                printf("%s removed from blacklist.\n", url);
            } else {
                printf("File error.\n");
            }
            break;
        case 'h':
            printf("Output help\n");
            break;
        default:
            printf("Output help\n");
            break;
    }
    return 0;
}

/*______________________________________________________

Params: String in - input buffer containing original file contents
Params: String out - output buffer containing modified file contents
Return: int - size of the output buffer
________________________________________________________*/

size_t deleteLine(char *in, char *out, char *line)
{
    size_t outpos = 0;
    char *tokens = strtok(in, "\n");
    while (tokens)
    {
        size_t len = strlen(tokens);

        // strip CR from Windows CRLF
        if (len > 0 && tokens[len - 1] == '\r')
        {
            tokens[len - 1] = '\0';
            len--;
        }
        // copy contents of in to out as long as the token does not match the function line arg
        if (strcmp(tokens, line) != 0)
        {
            memcpy(out + outpos, tokens, len);
            outpos += len;
            out[outpos] = '\n';
            outpos++;
        }
        tokens = strtok(NULL, "\n"); // move to the next token
    }
    return outpos;
}

/*______________________________________________________

Params: String filepath - path to windows hostfile
Params: String line - site to blacklist. Format "127.0.0.1 URL_to_be_blacklisted"
Return: Int - execution status. 0 = success, -1, file open error, -2, write error
Description: Appends a line to the end of the windows host file that should be a site to blacklist

TODO: Check if link exists first
________________________________________________________*/

int blacklistAdd(char *filepath, char *line)
{
    FILE *fptr = fopen(filepath, "a");
    if (!fptr)
        return -1;
    if (fputs(line, fptr) == EOF || fputs("\n", fptr) == EOF)
    {
        fclose(fptr);
        return -2;
    }

    fclose(fptr);
    return 0;
}

/*______________________________________________________

Params: String filepath - path to file to modify
Params: String line - line to remove from file
Return: Int - execution status. 0 = success, -1 = file open error
Description: Removes a line from a file
________________________________________________________*/

int blacklistRemove(char *filepath, char *line)
{
    FILE *fptr = fopen(filepath, "r");
    if (!fptr)
        return -1;
    // move to the end of the file
    fseek(fptr, 0, SEEK_END);
    // get the size of the file contents
    long size = ftell(fptr);
    // resets the file pointer to start of file
    rewind(fptr);
    // create an input buffer and read the file into it
    char *in = malloc(size + 1);
    size_t nread = fread(in, 1, size, fptr);
    in[nread] = '\0';
    fclose(fptr);

    // copy the input to the output skipping lines matching line
    char *out = malloc(size + 1);
    size_t outpos = deleteLine(in, out, line);
    // write the output to file, overwriting the original
    fptr = fopen(filepath, "w");
    if (!fptr)
    {
        free(in);
        free(out);
        return -1;
    }

    fwrite(out, 1, outpos, fptr);
    fclose(fptr);

    free(in);
    free(out);

    return 0;
}

// TODO: Output blacklist