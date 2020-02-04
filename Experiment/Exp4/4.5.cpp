#include <bits/stdc++.h>
#include <fstream>
#include <mpi.h>
using namespace std;
typedef unsigned long L;
#define BLOCK_LOW(my_rank, comm_sz, T) ((my_rank) * (T) / (comm_sz))
#define BLOCK_HIGH(my_rank, comm_sz, T) (BLOCK_LOW(my_rank + 1, comm_sz, T) - 1)
#define BLOCK_SIZE(my_rank, comm_sz, T) (BLOCK_HIGH(my_rank, comm_sz, T) - BLOCK_LOW(my_rank, comm_sz, T) + 1)
bool check(L res[], int len)
{
    for (int i = 0; i < len - 1; i++)
        if (!(res[i] <= res[i + 1]))
            return false;
    return true;
}
struct data
{
    int stindex; // 待归并数组序号
    int index;   // 在数组中的序号
    L stvalue;
    data(int st = 0, int id = 0, L stv = 0) : stindex(st), index(id), stvalue(stv) {}
};

bool operator<(const data &One, const data &Two)
{
    return One.stvalue > Two.stvalue;
}
void merge(L *start[], const int length[], const int number, L newArray[], const int newArrayLength)
{
    priority_queue<data> q;
    // 将每个待归并数组的第一个数加入优先队列
    for (int i = 0; i < number; i++)
        if (length[i] > 0)
            q.push(data(i, 0, start[i][0]));
    int newArrayindex = 0;
    while (!q.empty() && (newArrayindex < newArrayLength))
    {
        // 取最小数据
        data top_data = q.top();
        q.pop();

        // 将拿下的数据加入到结果数组中
        newArray[newArrayindex++] = start[top_data.stindex][top_data.index];

        // 若取出数据不是最后一个，将下一个元素push进优先队列
        if (length[top_data.stindex] > (top_data.index + 1))
        {
            q.push(data(top_data.stindex, top_data.index + 1, start[top_data.stindex][top_data.index + 1]));
        }
    }
}
int main(int argc, char *argv[])
{
    int comm_sz, my_rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int power = strtol(argv[1], NULL, 10);
    L dataLength = pow(2, power); //2^power个数
    ifstream fin("/public/home/shared_dir/psrs_data", ios::binary);

    //计算本进程计算数据块大小，以及读取位置
    L myDataStart = BLOCK_LOW(my_rank, comm_sz, dataLength);
    L myDataLength = BLOCK_SIZE(my_rank, comm_sz, dataLength);
    fin.seekg((myDataStart + 1) * sizeof(L), ios::beg);

    //读取各进程数据
    L *myData = new L[myDataLength];
    for (L i = 0; i < myDataLength; i++)
        fin.read((char *)&myData[i], sizeof(L));
    fin.close();

    //记录时间
    double startTime, endTime;
    MPI_Barrier(MPI_COMM_WORLD);
    startTime = MPI_Wtime();

    //排序本进程数据
    sort(myData, myData + myDataLength);

    //对数据进行等间隔采样即Regular samples，每个进程comm_sz个
    L *regularSamples = new L[comm_sz];
    for (int index = 0; index < comm_sz; index++)
        regularSamples[index] = myData[(index * myDataLength) / comm_sz];

    //将所有Regular Samples发送给0号进程
    L *gatherRegSam;
    if (my_rank == 0)
        gatherRegSam = new L[comm_sz * comm_sz];
    // 参数分别为sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, comm
    MPI_Gather(regularSamples, comm_sz, MPI_UNSIGNED_LONG, gatherRegSam, comm_sz, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    //0号进程执行归并排序，并抽出主元
    L *privots = new L[comm_sz];
    if (my_rank == 0)
    {
        // start用于存储gatherRegSam中各进程RegularSamples开始下标
        L **start = new L *[comm_sz];
        int *length = new int[comm_sz];
        for (int i = 0; i < comm_sz; i++)
        {
            start[i] = &gatherRegSam[i * comm_sz];
            length[i] = comm_sz;
        }

        //归并
        L *sortedGatRegSam = new L[comm_sz * comm_sz];
        merge(start, length, comm_sz, sortedGatRegSam, comm_sz * comm_sz);

        //取出主元
        for (int i = 0; i < comm_sz - 1; i++)
            privots[i] = sortedGatRegSam[(i + 1) * comm_sz];

        delete[] start;
        delete[] length;
        delete[] sortedGatRegSam;
    }

    //广播主元
    MPI_Bcast(privots, comm_sz - 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

    //各进程按主元分段
    int *partStartIndex = new int[comm_sz];
    int *partLength = new int[comm_sz];
    unsigned long dataIndex = 0;
    for (int partIndex = 0; partIndex < comm_sz - 1; partIndex++)
    {
        partStartIndex[partIndex] = dataIndex;
        partLength[partIndex] = 0;

        while ((dataIndex < myDataLength) && (myData[dataIndex] <= privots[partIndex]))
        {
            dataIndex++;
            partLength[partIndex]++;
        }
    }
    partStartIndex[comm_sz - 1] = dataIndex;
    partLength[comm_sz - 1] = myDataLength - dataIndex;

    //全互换(ALLTOALL)
    int *recvRankPartLen = new int[comm_sz];
    MPI_Alltoall(partLength, 1, MPI_INT, recvRankPartLen, 1, MPI_INT, MPI_COMM_WORLD);

    //ALLTOALLV
    int rankPartLenSum = 0;
    int *rankPartStart = new int[comm_sz];
    for (int i = 0; i < comm_sz; i++)
    {
        rankPartStart[i] = rankPartLenSum;
        rankPartLenSum += recvRankPartLen[i];
    }
    // 接收各进程i段的数组
    L *recvPartData = new L[rankPartLenSum];
    MPI_Alltoallv(myData, partLength, partStartIndex, MPI_UNSIGNED_LONG,
                  recvPartData, recvRankPartLen, rankPartStart, MPI_UNSIGNED_LONG, MPI_COMM_WORLD);

    // 归并
    L **mulPartStart = new L *[comm_sz];
    for (int i = 0; i < comm_sz; i++)
        mulPartStart[i] = &recvPartData[rankPartStart[i]];

    // 结果
    L *sortedRes = new L[rankPartLenSum];
    merge(mulPartStart, recvRankPartLen, comm_sz, sortedRes, rankPartLenSum);

    MPI_Barrier(MPI_COMM_WORLD);
    endTime = MPI_Wtime();

    //判断是否已经有序
    cout << "Rank " << my_rank << " sort: " << check(sortedRes, rankPartLenSum) << endl;

    if (my_rank == 0)
    {
        cout << "Processors number :" << comm_sz << " Power:" << power;
        cout << " Time :" << endTime - startTime << endl;
    }
    // 最后处理
    delete[] myData;
    delete[] regularSamples;
    if (my_rank == 0)
        delete[] gatherRegSam;
    delete[] partStartIndex;
    delete[] partLength;
    delete[] recvRankPartLen;
    delete[] rankPartStart;
    delete[] recvPartData;
    delete[] mulPartStart;
    delete[] sortedRes;
    MPI_Finalize();

    return 0;
}

