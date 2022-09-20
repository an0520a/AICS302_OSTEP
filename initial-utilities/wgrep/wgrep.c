#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

typedef struct _entry
{
    bool isFind;
    char* ptr;
} entry;

void createPi(const char* const search);
entry kmp(const char* const src, const char* const search);
size_t* pi;


int main(const int argc, const char* const *argv)
{
    if(argc == 1)
    {
        printf("wgrep: searchterm [file ...]\n");
        return 1;
    }

    size_t searchLen = strlen(argv[1]);
    size_t maxInputLen = (1024 > searchLen * 10)? 1024 : (searchLen * 10);
    char* const input = (char * const)malloc(maxInputLen * sizeof(char));
    *input = '\0';
    createPi(argv[1]);
    
    if(argc == 2) // stdin은 파일포인터가 아니라 fgetpos와 같은 함수가 안됨
    {
        while (!feof(stdin))
        {
            *input = '\0';
            fgets(input, maxInputLen, stdin);

            if(kmp(input, argv[1]).isFind == true)
            {
                printf("%s", input);
            }
        }
    }
    else if(argc == 3)
    {
        for (int i = 2; i < argc; i++)
        {
            FILE* fp = (FILE *)NULL;

            if(!(fp = fopen(argv[i], "r")))
            {
                printf("wgrep: cannot open file\n");
                return 1;
            }

            while (!feof(fp)) // 다음을 한 줄 씩 반복한다.
            {
                fpos_t pos;
                fgetpos(fp, &pos);
                char* preInput = input;
                *input = '\0';

                do
                {
                    fgets(preInput, maxInputLen - (preInput - input), fp);
                    if(*preInput == '\0') break;
                    entry tmp = kmp(input, argv[1]);


                    if(tmp.isFind == true) // 만약 포함되어 있으면 한줄을 출력하고 종료한다
                    {                  
                        fsetpos(fp, &pos);
                        char ch;

                        do
                        {
                            *input = '\0';
                            fgets(input, maxInputLen, fp);
                            printf("%s", input);
                        } while((ch = input[strlen(input) - 1]) != '\n' && ch != '\0');
                        break;
                    }
                    else // 아니라면, 마지막으로 탐색한 부분의 다음에서 탐색할 수 있도록 strcpy와 preInput 설정
                    {
                        strcpy(input, tmp.ptr);
                        preInput = input + strlen(tmp.ptr);
                    }
                } while (*input != '\0');
            }
            fclose(fp);
        }
    }

    free(pi);
    free(input);
}

void createPi(const char* const search)
{
    size_t searchLen = strlen(search);
    size_t j = 0;
    pi = (size_t *)malloc(searchLen * sizeof(size_t));
    pi[0] = 0;

    for (size_t i = 1; i < searchLen; i++)
    {
        while(j > 0 && search[i] != search[j]) j = pi[j - 1];
        if(search[i] == search[j]) pi[i] = j + 1, j++;
    }
}

entry kmp(const char* const src, const char* const search)
{
    size_t srcLen = strlen(src), searchLen = strlen(search);
    size_t j = 0;
    entry returnVal = { false, (char *)NULL };

    for (size_t i = 0; i < srcLen; i++)
    {
        while(j > 0 && src[i] != search[j]) j = pi[j - 1];
        if(src[i] == search[j])
        {
            if(j == searchLen - 1)
            {
                returnVal.isFind = true;
                return returnVal;
            }
            else                   j++;
        }
    }

    returnVal.ptr = (char *)src + srcLen - j;
    return returnVal;
}

// kmp 원형 코드
//
// size_t* createPi(const char* const search)
// {
//     size_t searchLen = strlen(search);
//     size_t j = 0;
//     size_t* pi = (size_t *)malloc(searchLen * sizeof(size_t));
//     pi[0] = 0;

//     for (size_t i = 1; i < searchLen; i++)
//     {
//         while(j > 0 && search[i] != search[j]) j = pi[j - 1];
//         if(search[i] == search[j]) pi[i] = j + 1, j++;
//     }

//     return pi;
// }

// std::vector<size_t> kmp(const char* const src, const char* const search)
// {
//     size_t srcLen = strlen(src), searchLen = strlen(search);
//     size_t* pi = createPi(search);
//     size_t j = 0;
//     std::vector<size_t> result;

//     for (size_t i = 0; i < srcLen; i++)
//     {
//         while(j > 0 && src[i] != search[j]) j = pi[j - 1];
//         if(src[i] == search[j])
//         {
//             if(j == searchLen - 1) result.push_back(i - j), j = pi[j];
//             else                   j++;
//         }
//     }

//     free(pi);

//     return result;
// }