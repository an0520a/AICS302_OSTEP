#include <stdio.h>

#define MAX_STR_LEN 1024

int main(const int argc, const char* const *argv) // const를 안쓰는 이유? 레거시와 호완성?
{
    char str[MAX_STR_LEN] = { '\0' };

    for (int i = 1; i < argc; i++)
    {
        FILE* fp;

        if(!(fp = fopen(argv[i], "r")))
        {
            printf("wcat: cannot open file\n");
            return 1;
        }
        
        while(!feof(fp))
        {
            fgets(str, MAX_STR_LEN, fp);
            printf("%s", str);
            *str = '\0';
        }

        fclose(fp);
    }

    return 0;
}