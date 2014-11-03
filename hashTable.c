/*
*  linwanggm@gmail.com
*  2014.11.03
*/
#include <stdio.h>
#include <malloc.h>
#include <error.h>
#include <string.h>
#include <stdlib.h>

//数据类型
#define dataType int

#ifndef bool
typedef int bool;
#endif

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#define mod 4

//定义节点
typedef struct NODE
{
    dataType data;
    struct NODE *next;
}node;

//定义hash表
typedef struct HASHTABLE
{
    node *modArray[mod];
}hashTable;

//创建哈希表
hashTable *initHashTable(void)
{
    hashTable *pHashTable = (hashTable *)malloc(sizeof(hashTable));
	if(NULL == pHashTable)
	{
	    perror("malloc hashTable errror.\n");
		return NULL;
	}
    memset(pHashTable,0,sizeof(hashTable));
	return pHashTable;
}

//查找数据
node *searchDataInHashTable(hashTable *pHashTable, const dataType data)
{
    //判断pHashTable不为NULL，hash表已经初始化
    if(NULL == pHashTable)
	{
	    return NULL;
	}
	//
	node *pNode;
	if(NULL == (pNode=pHashTable->modArray[data%mod]))
	{
	    return NULL;
	}
	while(pNode)
	{
	    if(data==pNode->data)
		{  
		    return pNode;
                    
		}
	    pNode=pNode->next;
	}
    //pNode->next=NULL仍未查找到
	return NULL;

}


//hash表插入数据
bool insertDataIntoHashTable(hashTable *pHashTable, const dataType data)
{
    if(NULL == pHashTable)
	{
	    return false;
	}
	node *pNode;
	if(NULL == (pNode=pHashTable->modArray[data%mod]))
    {
	    if(NULL == (pNode=(node *)malloc(sizeof(node))))
		{
		    perror("malloc node error.\n");
			return false;
		}
		memset(pNode,0,sizeof(node));
		pNode->data=data;
		pHashTable->modArray[data%mod]=pNode;
		return true;
	}	
    
	//若要插入数据已经在hash表中存在
	if(NULL != searchDataInHashTable(pHashTable,data))
	{
	    fprintf(stderr,"the data has already in hash table.\n");
		return true;
	}
	//data在hash表中不存在且不为链表首部
	node *tempNode;
	tempNode=pHashTable->modArray[data%mod];
	while(tempNode->next)
	{
	    tempNode=tempNode->next;
	}
	pNode=(node *)malloc(sizeof(node));
	if(NULL == pNode)
	{
	    perror("malloc node error.");
		fprintf(stderr,"malloc node error at line : %d\n", __LINE__);
		return false;
	}
	memset(pNode,0,sizeof(node));
	pNode->data=data;
	pNode->next=NULL;
	tempNode->next=pNode;
	return true;
}

//hash表删除数据
bool deleteDataFromHashTable(hashTable *pHashTable, const dataType data)
{
    if(NULL == pHashTable || NULL == pHashTable->modArray[data%mod])
	{
	    return false;
	}
	//hash表中若查找不到该数据
	node *pNode;
	if(false == (pNode=searchDataInHashTable(pHashTable,data)))
	{
	    return false;
	}
	//若pNode在首部
	//if(pNode == pHashTable->modArray[data%mod])
	if(pNode == pHashTable->modArray[data%mod])
	{
	    pHashTable->modArray[data%mod]=pHashTable->modArray[data%mod]->next;
	    goto final;
	}
	
	//data在非首部
	node *tempNode=pHashTable->modArray[data%mod];
	while(pNode != tempNode->next)
	{
	    tempNode=tempNode->next;
	}
	tempNode->next=pNode->next;
	
final:
	free(pNode);
	pNode=NULL;
	return true;
}

//删除hash表
bool destoryHashTable(hashTable *pHashTable)
{

    if(NULL == pHashTable)
        return false;
    int i=0;
    for(i=0;i<mod;++i)
    {
        while(NULL != pHashTable->modArray[i])
        {  
            dataType temp=pHashTable->modArray[i]->data;
            deleteDataFromHashTable(pHashTable,temp);
        }

    }
 
    return true;
}

//显示hash table内容
bool showHashTable(hashTable *pHashTable)
{

    if(NULL == pHashTable)
        return false;
    int i=0;
    for(i=0;i<mod;++i)
    {
	    node *pNode;
		pNode=pHashTable->modArray[i];
        while(NULL != pNode)
        {  
            dataType temp=pNode->data;
            fprintf(stdout," %d ",temp);
			pNode=pNode->next;
        }
        fprintf(stdout,"\n");

    }
 
    return true;
}



int main()
{
    hashTable *pHashTable = initHashTable();
    if (NULL == pHashTable)
    {
	     fprintf(stderr,"create hashTable error.\n");
	     exit(1);
    }
    int intArray[]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
    int i=0;
	
    //test insert 
    for(i=0;i<sizeof(intArray)/sizeof(int);++i)
    {
	     if(false == (insertDataIntoHashTable(pHashTable,intArray[i])))
       {
          fprintf(stderr,"insert into hash table error.\n");
	        exit(1);
	      }
        else
        {
          fprintf(stdout,"insert %d\n",intArray[i]);
        }
    }
	
    //test search
    node *pNode;
    if(NULL == (pNode=searchDataInHashTable(pHashTable,12)))
    {
	      fprintf(stderr,"search from hash table error.\n");
        exit(1);
    }
    else
    {
	      printf("search : %d\n",pNode->data);
    }
	
    //test delete
    if(false == deleteDataFromHashTable(pHashTable,12))
    {
	      fprintf(stderr,"delete from hash table error.\n");
	      exit(1);
    }
    else
    {
        fprintf(stdout,"delete 12\n"); 
    }

    //show hash table
    if(false == showHashTable(pHashTable))
    {
        fprintf(stderr,"show hash table error.\n");
    }
    else
    {
       fprintf(stdout,"show hash table ok.\n");
    }

    //drop hash table
    if(false == destoryHashTable(pHashTable))
    {
	      fprintf(stderr,"drop hash table error.\n");
      	exit(1);
    }
    else
    {
        fprintf(stdout,"drop hash table ok.\n");
    }
	
}

