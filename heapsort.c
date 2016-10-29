堆排序
堆排序是一种完全二叉树，满足：k(i) <= k(2i+1)  && k(i) <= k(2i+2)  (0<=i<=n/i)
分为：大根堆、小根堆
k(i)相当于二叉树的非叶子节点，k(2i+1)是左子节点， k(2i+2)是右子节点。
堆元素调整可以这样理解：父节点值分别于两个子节点值进行比较，若右子节点值最大（假设为大根堆），则与父节点调整位置，
                    然后右子节点作为父节点继续与子节点比较，如此一直到无子节点。
因为父子节点满足i,2i+1,2i+2关系，所以只需要调整前一半元素即可；又由于在这一半元素中，若从前端开始则后面元素的调整对前面已经调整的结果有影响，
所以从后向前元素依次调整。

demo:
#include <stdio.h>
//array是待调整的堆数组，i是待调整的数组元素的位置，nlength是数组的长度
//本函数功能是：根据数组array构建大根堆
void HeapAdjustBig(int array[],int i,int nLength)
{
    int nChild;
    int nTemp;
    for(;2*i+1<nLength;i=nChild)
    {
        //子结点的位置=2*（父结点位置）+1
        nChild=2*i+1;
        //得到子结点中较大的结点
        if(nChild<nLength-1&&array[nChild+1]>array[nChild])++nChild;
        //如果较大的子结点大于父结点那么把较大的子结点往上移动，替换它的父结点
        if(array[i]<array[nChild])
        {
            nTemp=array[i];
            array[i]=array[nChild];
            array[nChild]=nTemp;
        }
        else break; //否则退出循环
    }
}

void HeapAdjustSmall(int array[],int i,int nLength)
{
    int nChild;
    int nTemp;
    for(;2*i+1<nLength;i=nChild)
    {
        //子结点的位置=2*（父结点位置）+1
        nChild=2*i+1;
        //得到子结点中较小的结点
        if(nChild<nLength-1&&array[nChild+1]<array[nChild])++nChild;
        //如果较小的子结点小于父结点那么把较小的子结点往上移动，替换它的父结点
        if(array[i]>array[nChild])
        {
            nTemp=array[i];
            array[i]=array[nChild];
            array[nChild]=nTemp;
        }
        else break; //否则退出循环
    }
}

//堆排序算法
void HeapSort(int array[],int length)
{
    int i;
    //调整序列的前半部分元素，调整完之后第一个元素是序列的最大的元素
    //length/2-1是最后一个非叶节点，此处"/"为整除
    for(i=length/2-1;i>=0;--i)
        HeapAdjustSmall(array,i,length);
}
int main()
{
    int i;
    //int num[]={10,15,56,25,30,70};
    int num[]={1,2,3,4,5,6,7,8,9,10};
    HeapSort(num,sizeof(num)/sizeof(int));
    for(i=0;i<sizeof(num)/sizeof(int);i++)
    {
        printf("%d ",num[i]);
    }
    printf("\nok\n");
    return 0;
}

参考：
1. 堆排序及其分析
http://www.cnblogs.com/zabery/archive/2011/07/26/2117103.html
2.堆排序缺点何在？
https://www.zhihu.com/question/20842649
3.白话经典算法系列之七 堆与堆排序
http://blog.csdn.net/morewindows/article/details/6709644/
4. 百度百科
