#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include <dirent.h>

#include"hfInfo.h"

int main(int argc, char *argv[])
{
    int i;
    int64_t *PID[Split];
    Index *uidinfo = (Index *)malloc(sizeof(Index)*Hash*Col); // 存储UID的互粉信息
    for( i = 0 ; i < Split ; i++) //为互粉数据申请存储
    {
        PID[i] =  (int64_t *)malloc(sizeof(int64_t)*(HF_NUM / Split));
    }
    InitialHash(HASH_SIZE,Hash);
    ReadHfFile(PID,uidinfo,argv[1],HASH_SIZE);
//  Traverse(PID,uidinfo);
    printf("run success\n");
    return 0;

}
