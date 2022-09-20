#include <stdio.h>

#define MAX_INPUT_LEN 1024

int main(const int argc, const char* const *argv)
{
    if(argc < 2)
    {
        printf("wzip: file1 [file2 ...]\n");
        return 1;
    }

    char input[MAX_INPUT_LEN];
    const char* p = input;
    char ch;
    unsigned int count = 0;

    for (int i = 1; i < argc; i++)
    {
        FILE* fpr = (FILE *)NULL;
        FILE* fpw = (FILE *)NULL;

        if(!(fpr = fopen(argv[i], "r")))
        {
            printf("wgrep: cannot open file\n");
            return 1;
        }
        
        while (!feof(fpr))
        {
            *input = '\0';

            fgets(input, MAX_INPUT_LEN, fpr);

            p = input;

            if(*p != ch && count)
            {
                fwrite(&count, sizeof(unsigned int), 1, stdout);
                fwrite(&ch   , sizeof(char)        , 1, stdout);
                count = 0;
            }

            ch = *p;

            while (*p != '\0')
            {
                if(*p == ch) count++, p++;
                else
                {
                    fwrite(&count, sizeof(unsigned int), 1, stdout);
                    fwrite(&ch   , sizeof(char)        , 1, stdout);

                    ch = *p, count = 1;
                    p++;
                }
            }

        }

        fclose(fpr);
    }

    if(count)
    {
        fwrite(&count, sizeof(unsigned int), 1, stdout);
        fwrite(&ch   , sizeof(char)        , 1, stdout);
    }
}