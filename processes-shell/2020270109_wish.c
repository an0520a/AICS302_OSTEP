#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <malloc.h>
#include <memory.h>
#include <fcntl.h>
#include "2020270109_strVector.h"
#include "2020270109_strVector.c"

void shellError();
bool wishExit(strVector* args, bool outputFileFlag, const char* outputFileName);
bool wishCd(strVector* args, bool outputFileFlag, const char* outputFileName);
bool wishPath(strVector* args, bool outputFileFlag, const char* outputFileName, strVector* pathVec);
char* strcatWithDup(const char* _front, const char* _back);

int main(const int argc, const char* const *argv)
{
    strVector *inputVec = strVector_new();
    strVector *args = strVector_new();
    strVector *pathVec = strVector_new();
    (*pathVec).push_back(pathVec, strdup("/bin"));

    if(argc > 2)
    {
        shellError();
        exit(1);
    }

    if(argc == 2)
    {
        int fd = open(argv[1], O_RDONLY);
        if(fd == -1)
        {
            shellError();
            exit(1);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    while(true)
    {
        char* inputStr = (char *)NULL;
        char* tmpStr= (char *)NULL;
        char* tmpStrPtr = (char *)NULL;
        char* tmpArg = (char *)NULL;
        bool errorFlag = false;
        bool waitFlag = false;
        size_t tmpLen = 0;
        (*inputVec).clear(inputVec);

        if(argc != 2) printf("wish> ");
        if(getline(&inputStr, &tmpLen, stdin) == -1) exit(0); // 입력

        tmpLen = strlen(inputStr);
        if(inputStr[tmpLen - 1] == '\n') inputStr[tmpLen - 1] = '\0'; // 맨 끝 개행문자 제거

        tmpStr = strdup(inputStr);
        tmpStrPtr = tmpStr;
        while(tmpArg = strsep(&tmpStrPtr, " \t&>")) // 문자열 파싱
        {
            if(*tmpArg != '\0'                                                     ) (*inputVec).push_back(inputVec, strdup(tmpArg));
            if(tmpStrPtr != (char *)NULL && inputStr[tmpStrPtr - 1 - tmpStr] == '&') (*inputVec).push_back(inputVec, strdup("&"));
            if(tmpStrPtr != (char *)NULL && inputStr[tmpStrPtr - 1 - tmpStr] == '>') (*inputVec).push_back(inputVec, strdup(">"));
        }
        free(inputStr);
        free(tmpStr);

        size_t inputVecSize = (*inputVec).size(inputVec);
        for (size_t i = 0; i < inputVecSize; i++) // 명령 실행 부분
        {
            (*args).clear(args);
            bool outputFileFlag = false;
            char* outputFileName = (char *)NULL;
            (*args).push_back(args, (*inputVec).at(inputVec)[i]);
            size_t j;

            if(!strcmp((*args).at(args)[0], "&")) continue;         // 만약 &가 첫 시작이라면, 명령어가 없다는 뜻임. 따라서 continue
            if(!strcmp((*args).at(args)[0], ">")) errorFlag = true; // 만약 실행파일이 존재하지 않고 redirection만 존재한다면 오류

            for (j = i + 1; j < inputVecSize; j++)
            {
                if     (!strcmp((*inputVec).at(inputVec)[j], "&")                   ) break;
                else if(!strcmp((*inputVec).at(inputVec)[j], ">") && !outputFileFlag) outputFileFlag = true;
                else if(outputFileFlag)
                {
                    if   (outputFileName != (char *)NULL) { errorFlag = true; break; }
                    else                                  outputFileName = (*inputVec).at(inputVec)[j];
                }
                else (*args).push_back(args, (*inputVec).at(inputVec)[j]);
            }

            if(outputFileFlag && !outputFileName) errorFlag = true;
            if(errorFlag) break;

            (*args).push_back(args, (char *)NULL); // 여기까지가 cmd랑 인자 설정하는 부분

            if     (!strcmp((*args).at(args)[0], "exit")) errorFlag = wishExit(args, outputFileFlag, outputFileName);
            else if(!strcmp((*args).at(args)[0], "cd"  )) errorFlag = wishCd(args, outputFileFlag, outputFileName);
            else if(!strcmp((*args).at(args)[0], "path")) errorFlag = wishPath(args, outputFileFlag, outputFileName, pathVec);
            else
            {
                errorFlag = true;
                size_t pathVecSize = (*pathVec).size(pathVec);

                for (size_t i = 0; i < pathVecSize; i++)
                {
                    char* tmp = strcatWithDup((*pathVec).at(pathVec)[i], "/");
                    char* cmd = strcatWithDup(tmp, (*args).at(args)[0]);
                    free(tmp);

                    if(!access(cmd, X_OK))
                    {
                        errorFlag = false;

                        int rc = fork();

                        if      (rc < 0)  errorFlag = true;
                        else if (rc == 0)
                        {
                            if(outputFileFlag)
                            {
                                int fd = open(outputFileName, O_CREAT | O_TRUNC | O_WRONLY);
                                dup2(fd, STDOUT_FILENO);
                                dup2(fd, STDERR_FILENO);
                                close(fd);
                            }
                            execv(cmd, (*args).at(args));
                        }
                        else waitFlag = true;

                        free(cmd);
                        break;
                    }

                    free(cmd);
                }
            } // 여기까지가 명령어 실행 부분

            if(errorFlag) break;
            i = j;
        }

        if(waitFlag) while(wait(NULL) > 0);

        if(errorFlag) shellError();

        size_t vec_size = (*inputVec).size(inputVec);
        for (size_t i = 0; i < vec_size; i++) free((*inputVec).at(inputVec)[i]);
        (*inputVec).clear(inputVec);
    }

    strVector_delete(inputVec);
    strVector_delete(args);
    strVector_delete(pathVec);
}

void shellError()
{
    const char const *error_message = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

bool wishExit(strVector* args, bool outputFileFlag, const char* outputFileName)
{
    if((*args).size(args) == 2 && outputFileFlag == false) { exit(0); return 0; }
    else return true;
}

bool wishCd(strVector* args, bool outputFileFlag, const char* outputFileName)
{
    if((*args).size(args) == 3 && outputFileFlag == false)
    {
        int flag = 0;
        flag = chdir((*args).at(args)[1]);
        return (flag)? 1 : 0;
    }
    else return true;
}

bool wishPath(strVector* args, bool outputFileFlag, const char* outputFileName, strVector* pathVec)
{
    if(outputFileFlag == false)
    {
        size_t vecSize = (*pathVec).size(pathVec);
        for (size_t i = 0; i < vecSize; i++) free((*pathVec).at(pathVec)[i]);
        (*pathVec).clear(pathVec);

        vecSize = (*args).size(args);
        for (size_t i = 1; i < vecSize - 1; i++) (*pathVec).push_back(pathVec, strdup((*args).at(args)[i]));

        return 0;
    }
    else return true;
}

char* strcatWithDup(const char* _front, const char* _back)
{
    size_t frontSize = strlen(_front);
    size_t backSize = strlen(_back);

    char* newStr = (char *)malloc(frontSize + backSize + 1);
    strcpy(newStr, _front), strcpy(newStr + frontSize, _back);

    return newStr;
}