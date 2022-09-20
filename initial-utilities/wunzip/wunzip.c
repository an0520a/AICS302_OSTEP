#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <stdbool.h>

int main(const int argc, const char* const *argv)
{
    if(argc < 2)
    {
        printf("wunzip: file1 [file2 ...]\n");
        return 1;
    }

    char str[1024];
    *str = '\0';

    for (int i = 1; i < argc; i++)
    {
        FILE* fp = (FILE *)NULL;

        if(!(fp = fopen(argv[i], "rb")))
        {
            printf("wunzip: cannot open file\n");
            return 1;
        }

        while(true)
        {
            int n;
            char ch;

            if(!fread(&n, sizeof(int), 1, fp)) break;
            if(!fread(&ch, sizeof(char), 1, fp)) break;

            while(n)
            {
                if(n >= 1024)
                {
                    memset(str, (int)ch, 1023);
                    str[1024] = '\0';
                    n -= 1023;
                }
                else
                {
                    memset(str, (int)ch, n);
                    str[n] = '\0';
                    n = 0;
                }

                printf("%s", str);
            }

            *str = '\0';
        }
    }
}