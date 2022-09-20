#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(const int argc, const char* const *argv)
{
    int bufferLen = 128;
    size_t tmp;
    char **buf = (char **)calloc(bufferLen, sizeof(char *));
    char **bufP = buf, **bufE = buf + bufferLen;
    FILE* fpr = (FILE *)NULL;

    if(argc > 3) fprintf(stderr, "usage: reverse <input> <output>\n"), exit(1);
    if(argc == 3)
    {
        struct stat readStat;
        struct stat writeStat;

        if(stat(argv[1], &readStat))  fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]), exit(1);
        if(!stat(argv[2], &writeStat) && readStat.st_ino == writeStat.st_ino) fprintf(stderr, "reverse: input and output file must differ\n"), exit(1);
    }

    if(argc == 1)
    {
        while(true)
        {
            tmp = 0;
            if(getline(bufP, &tmp, stdin) == -1) break;
            if(tmp != 0) bufP++;
            if(bufP == bufE)
            {
                if(!(buf = realloc(buf, 2 * bufferLen * sizeof(char *)))) fprintf(stderr, "reverse: malloc failed\n"), exit(1);

                memset(buf + bufferLen, 0x00, bufferLen * sizeof(char *));

                bufP = buf + bufferLen;
                bufE = bufP + bufferLen;
                bufferLen = bufferLen * 2;
            }
        }
    }
    else
    {
        if(!(fpr = fopen(argv[1], "r"))) fprintf(stderr, "reverse: cannot open file '%s'\n", argv[1]), exit(1);
        while(true)
        {
            tmp = 0;
            if(getline(bufP, &tmp, fpr) == -1) break;
            if(tmp != 0) bufP++;
            if(bufP == bufE)
            {
                if(!(buf = realloc(buf, 2 * bufferLen * sizeof(char *)))) fprintf(stderr, "reverse: malloc failed\n"), exit(1);

                memset(buf + bufferLen, 0x00, bufferLen * sizeof(char *));

                bufP = buf + bufferLen;
                bufE = bufP + bufferLen;
                bufferLen = bufferLen * 2;
            }
        }

        fclose(fpr);
    }

    bufP--;

    if(argc < 3)
    {
        do
        {
            fprintf(stdout, "%s", *bufP);
            free(*bufP);
        } while (bufP-- != buf);
    }
    else
    {
        FILE* fpw = (FILE *)NULL;
        if(!(fpw = fopen(argv[2], "w"))) fprintf(stderr, "reverse: cannot make file '%s'\n", argv[2]), exit(1);

        do
        {
            fprintf(fpw, "%s", *bufP);
            free(*bufP);
        } while (bufP-- != buf);

        fclose(fpw);
    }

    
    free(buf);
}

// 방법 1) 재귀
// 장점 : 구현하기 쉬움
// 단점 : 꼬리재귀형식으로 구현하긴 어렵고, 꼬리재귀 아니면 낭비되는 리소스 존재. 재귀가 너무 깊게 들어가면 에러남.
// 비고 : 빠를지 느릴지 예상이 안 감. 재귀를 활용하면 단일 배열에서 (문장1\n문장2\n문장3\n문장4\n문장5...) 
//        형식으로 구현할 수 있음. 그렇게 되면 fprintf를 반복적으로 호출하지 않고, 단 한번만 호출하면 되서 오버헤드가 적어짐
//        또, 배열을 순차적으로 사용하기 때문에 캐시 활용성도 좋음. 근데 재귀형식이라 오버헤드가 있음. 그러니까 속도는 몰?루

// 방법 2) 스택
// 장점 : 그럭저럭 빠름
// 단점 : 이건 C고, C는 스택을 제공해주지 않음. 직접 구현해야함

// 방법 3) fpot_t를 통한 위치 저장
// 장점 : 메모리 사용량이 매우 적음
// 단점 : 파일 포인터를 움직이는 작업은 오버헤드가 큼. 따라서 가장 느릴것