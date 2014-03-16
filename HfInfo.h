#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>
#include <dirent.h>

#define Hash 101 // 用户互粉信息俺照uid哈希打散存储，方便查找
#define Col  700000 // Hash*Col = UID 数目

#define HF_NUM  5000000000// 关系数
#define Split  3  // 将所需要的空间动态分配Split份,防止申请一个太大的不成功

/*
 * @jiawei
 * 与其他版本的不同点是必须经过排序
 * */

int32_t HASH_SIZE[Hash]; // 记录每个"文件块", 已使用空间

int64_t HF_UID[Split]; //count:统计每个中第三维的个数; HASH_SIZE: 统计第二维的个数

typedef struct Index
{
    int64_t pid;
    int64_t index;
    int32_t count;
    int32_t array; //关系在第几个动态分配数组中
}Index;


void Initial(int64_t *PID,int64_t size)
{
    memset(PID,0,sizeof(int64_t)*size);
}
void InitialHash(int32_t *HASH_SIZE, int size)
{
    int i;
    for(i = 0 ; i < size ; ++i)
    {
        HASH_SIZE[i] = 0;
    }
}

int Find(Index *uidinfo,int32_t SIZE[], int64_t lhv)
{
    int pos= -1;
    int hash = lhv % Hash;
    int left = 0, right = SIZE[hash]-1;
    int mid;
    while(left <= right)
    {
        mid =   (( right + left)) >> 1;
        if(uidinfo[hash*Col+ mid].pid  == lhv) // hash*grade+mid*Row=uidinfo[hash][mid][0]
        {
            return mid;
        }
        else if(uidinfo[hash*Col + mid].pid < lhv )
        {
            left = mid + 1;
        }
        else
        {
            right = mid -1;
        }

    }
    return pos;
}

void InsertKey(Index *uidinfo, int64_t key,int64_t index, int32_t count, int32_t hash_size[])
{
    int hash = key % Hash ;
    int mod = key % Split ;
    int len = hash_size[hash]-1;
    while(len >= 0 && uidinfo[hash*Col+len].pid > key )
    {
        uidinfo[hash*Col+len+1].pid=uidinfo[hash*Col+len].pid;
        uidinfo[hash*Col+len+1].index=uidinfo[hash*Col+len].index;
        uidinfo[hash*Col+len+1].count=uidinfo[hash*Col+len].count;
        uidinfo[hash*Col+len+1].array=uidinfo[hash*Col+len].array;
        --len;
    }
    uidinfo[hash*Col+len+1].pid = key;
    uidinfo[hash*Col+len+1].index = index;
    uidinfo[hash*Col+len+1].count = count;
    uidinfo[hash*Col+len+1].array = mod;
    hash_size[hash]++;
}

void Traverse(int64_t *PID[],Index *uidinfo)
{
    printf("Traverse begining\n");
    int i,j,k;
    int mod;
    for(i = 0; i < Hash ; i++)
    {
        for(j = 0; j < HASH_SIZE[i] ; j++)
        {
            printf("%lld 's hf info:\n",uidinfo[i*Col +j].pid);
            mod = uidinfo[i*Col +j].pid % Split;
            for(k = 0; k < uidinfo[i*Col +j].count ; k++)
            {
//              printf("%d\t",uidinfo[i][j][k]);
                printf("%lld\t",PID[mod][uidinfo[i*Col +j].index +k]);
            }
            printf("\n");
        }
    }
    printf("Traverse endding\n");
}

void ReadHfFile(int64_t *PID[],Index *uidinfo, char *infile,int32_t hash_size[])
{
    FILE *fp;
    if((fp=fopen(infile,"r"))==NULL)
    { //文本只读方式重新打开文件
        printf("cannot open file\n");
        return ;
    }
    int64_t key=0, value=0;
    int64_t index[Split]= {0}, pre = 0, count = 0;
    int32_t mod = 0, hash;
    while(fscanf(fp,"%lld\t%lld",&key,&value) != EOF)
    {
        if(key == value) continue;
        mod = key % Split ;
        hash = key % Hash;
        if( pre != key && pre != 0)
        {
            InsertKey(uidinfo,pre,index[pre%Split]-count,count,hash_size);
            count = 0;
        }
        PID[mod][index[mod]] = value;
        ++index[mod];
        ++count;
//      ++hash_size[hash];
        pre = key;
    }
    InsertKey(uidinfo,pre,index[pre%Split]-count,count,hash_size);
}

