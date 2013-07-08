#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sqlite3.h"
#include <stdint.h>
#include "heap.h"


#define MAX_VERTEX_NUM 5000
#define INFINITY 		99999
#define NIL				-1

#define STRACE    printf("%s %d\n",__FUNCTION__, __LINE__);
/** 
 * 
 */

typedef struct ArcNode{
	uint64_t adjvex;	//指向adj邻接表的位置
	float weight;
	struct ArcNode *nextarc;
}ArcNode;

typedef struct VNode{
	ArcNode *firstarc;
}VNode,AdjList[MAX_VERTEX_NUM];

typedef struct ALGraph{
	AdjList vertices;
	PQ_TYPE vexnum, arcnum;	//arcnum为边的数量
}ALGraph;


void initALGraph(ALGraph* GPt,PQ_TYPE vn){    //初始化结点
	GPt->arcnum = 0;
	GPt->vexnum = vn;
	PQ_TYPE i;
	for(i=0;i<vn;i++){
		GPt->vertices[i].firstarc = NULL;
	}
}


void insertArc(ALGraph* GPt,PQ_TYPE vhead,PQ_TYPE vtail,DIS_TYPE w){ //增加结点操作
	ArcNode* arcNodePt = malloc(sizeof(ArcNode));
	arcNodePt->nextarc = NULL;
	arcNodePt->adjvex = vtail;
	arcNodePt->weight = w;
  printf("w=%f\n",arcNodePt->weight);
   STRACE
	ArcNode* tailPt = GPt->vertices[vhead].firstarc;
   STRACE
	if(tailPt==NULL){
    STRACE
		GPt->vertices[vhead].firstarc = arcNodePt;
	}else{
		while(tailPt->nextarc!=NULL){
       STRACE
			tailPt = tailPt->nextarc;
		}
       STRACE
  		tailPt->nextarc = arcNodePt;
 	}
	GPt->arcnum ++;
}


void displayGraph(ALGraph G)  //打印结点
{
 ArcNode* arcNodePt;
 PQ_TYPE i=0;
 for(;i<G.vexnum;i++)
 {
  arcNodePt = G.vertices[i].firstarc;
  #ifdef DOUBLE_TYPE
       printf("vertex %d :",i);
  #else
       printf("vertex %d :",i);
  #endif

  while(arcNodePt!=NULL){
    #ifdef DOUBLE_TYPE
        printf("%d->%d,w=%f  ", i, arcNodePt->adjvex,arcNodePt->weight);
    #else
        printf("%d->%d,w=%d  ", i, arcNodePt->adjvex,arcNodePt->weight);
    #endif
   arcNodePt = arcNodePt->nextarc;
  }
  printf("\n");
 }
}

DIS_TYPE getEdgeWeight(ALGraph G,PQ_TYPE vhead,PQ_TYPE vtail)  //求边的权重
{
 ArcNode* arcNodePt = G.vertices[vhead].firstarc;
 while(arcNodePt!=NULL)
 {
  if(arcNodePt->adjvex==vtail)
  {
   return arcNodePt->weight;
  }
  arcNodePt = arcNodePt->nextarc;
 }
 return INFINITY;
}


void initSingleSource(ALGraph G,PQ_TYPE s,DIS_PTR d,PQ_PTR pi) 
{       //1、初始化结点工作
 PQ_TYPE i=0;
 for(;i<G.vexnum;i++)

 {
  d[i] = INFINITY;
  pi[i] = NIL;
 }
 d[s] = 0;
}

void relax(PQ_TYPE u,PQ_TYPE v,ALGraph G,DIS_PTR d,PQ_PTR pi)
{ //4、松弛操作。
        //u是新加入集合S的顶点的标号
 DIS_TYPE w = getEdgeWeight(G,u,v);
 //printf("d[v]=%f,d[u]=%f,w=%f\n", d[v],d[u], w);
 if(d[v]>d[u]+ w){
    d[v] = d[u] + w;
    //printf("d[%d]=%f\n",v, d[v]);
    pi[v] = u;
 }
}

void dijkstra(ALGraph G,PQ_TYPE s, PQ_TYPE t,DIS_PTR d,PQ_TYPE pi[],PQ_TYPE Q[])
{ //Q[]是最小优先队列，Q[1..n]中存放的是图顶点标号,Q[0]中存放堆的大小
 //优先队列中有key的概念，这里key可以从d[]中取得。比如说，Q[2]的大小(key)为 d[ Q[2] ]

 initSingleSource(G,s,d,pi);  //1、初始化结点工作
 
 //2、初始化队列
 Q[0] = G.vexnum;

 pq_init(Q, Q[0], d);
 //Q[1] = s;

 PQ_TYPE u;
 PQ_TYPE v;
 while(Q[0]!=0){
  pq_queue_build_min(Q,d);     //3.1、建立最小堆
  u = pq_dequeue_min_top(Q,d);   //3.2、从最小队列中，抽取最小结点
  ArcNode* arcNodePt = G.vertices[u].firstarc;
  while(arcNodePt!=NULL){
   v = arcNodePt->adjvex;
   relax(u,v,G,d,pi);    //4、松弛操作。
   arcNodePt = arcNodePt->nextarc;
  }
 }

}





void printRoute(int i, PQ_PTR pi){

}



int main(int argc, char*argv[]){
  clock_t starttime, endtime;
  double totaltime;
  starttime = clock();
  ALGraph G;
  ALGraph* GPt = &G;
  initALGraph(GPt,1000);
 #if 0
 initALGraph(GPt,6);

 insertArc(GPt,0,1,7);
 
 insertArc(GPt,0,2,9);
 insertArc(GPt,0,5,14);
 insertArc(GPt,1,2,10);
 insertArc(GPt,1,3,15);
 insertArc(GPt,2,5,2);
 insertArc(GPt,2,3,10);
 insertArc(GPt,5,4,9);
 insertArc(GPt,3,4,6);
 insertArc(GPt,4,3,6);
#endif 
 #if 1
   sqlite3 *db;
    char *zErrMsg = 0;
    int rc, ret = 0, j,i, row_count = 0;
    printf("sqlite3 info:\n libversion:%s\n souceid:%s\n vesion_num:%d\n",sqlite3_libversion(), sqlite3_sourceid(),
      sqlite3_libversion_number());
  sqlite3_stmt *pStmt = NULL;
  
    if( argc < 3 ){
      fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
      return(1);
    }
    rc = sqlite3_open(argv[1], &db);
    if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return(1);
    }

    rc = sqlite3_prepare(db, argv[2], strlen(argv[2]), &pStmt, NULL);
    printf("rc =%d\n", rc);
  if(rc != SQLITE_OK){
    fprintf(stderr, "SQL error1: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
    return 1;
  }
  
  printf("pStmt=%p\n",pStmt);
  ret = sqlite3_column_count(pStmt);
  printf("BEFORE STEP col count= %d, rc = %d\n", ret, rc);
   printf("pStmt22=%p\n",pStmt);
  i = 0;
  //´òÓ¡colÃû³Æ
  while(i < ret){
    printf("%20s \t %d \t %20s \t %d\n",
      sqlite3_column_name(pStmt, i),
      sqlite3_column_type(pStmt, i),
      sqlite3_column_decltype(pStmt, i),
      sqlite3_column_bytes(pStmt, i));
    i++;
  }
  printf("pStmt23=%p\n",pStmt);
 

 
  printf("\n------ ------ ------ ------ ------------ ------ ------ ------\n");
  printf("pStmt2=%p\n",pStmt);
  rc = sqlite3_step(pStmt);
   printf("pStmt3=%p\n",pStmt);
    STRACE
  if(rc != SQLITE_ROW){
          fprintf(stderr, "SQL error2: %s,rc =%d\n", zErrMsg, rc);
          sqlite3_free(zErrMsg);
    return 1;
  }
  STRACE
  //¿ªÊ¼¶ÁÈ¡Ã¿Ò»ÐÐ
  while(rc == SQLITE_ROW){
    //STRACE
    i = 0;row_count++;
    // for(j=0;j< ret;j++){
    //   printf("%s ", sqlite3_column_text(pStmt,j));
    // }
    printf("%f -> %f w:%f\n", sqlite3_column_double(pStmt,3), sqlite3_column_double(pStmt,4), sqlite3_column_double(pStmt,5));
    insertArc(GPt,sqlite3_column_int(pStmt,3), sqlite3_column_int(pStmt,4), sqlite3_column_double(pStmt,5));
    #if 0
    printf("%d, %s, %s, %s\n",
      sqlite3_column_int(pStmt, 0),
      sqlite3_column_text(pStmt, 1),
      sqlite3_column_text(pStmt, 2),
      sqlite3_column_text(pStmt, 3));
    #endif
    rc = sqlite3_step(pStmt);
  }
  STRACE
  printf("row_count=%d\n ", row_count);
  sqlite3_finalize(pStmt);

    sqlite3_close(db);
#endif
 G.vexnum = row_count;
 printf("显示出此构造的图:\n");
 //displayGraph(G);
 printf("\n");
 
 DIS_TYPE d[MAX_VERTEX_NUM];
 PQ_TYPE pi[MAX_VERTEX_NUM];
 PQ_TYPE Q[MAX_VERTEX_NUM+1]; 
        //Q[]的第一个元素只保存堆的大小，不保存元素。所以定义长度时+1

dijkstra(G,1, 580,d,pi,Q);
STRACE
    i= atoi(argv[3]);
    printf("从源点1到点%d的最短路径信息:\n",i);
    printf("长度为%f\n",d[i]);
    printf("路径为: %d ", i);
    for(j=i;j>1;){
     printf("%d ",pi[j]);
     j=pi[j];
    }
    printf("\n");

 // for(i=1;i<G.vexnum;i++){
  
 //  printf("从源点1到点%d的最短路径信息:\n",i);
 //  printf("长度为%d\n",d[i]);
 //  printf("路径为: %d ", i);
 //  for(j=i;j>0;){
 //  	printf("%d ",pi[j]);
 //  	j=pi[j];
 //  }
 
 //}
  endtime = clock();
  totaltime = (double)( (endtime - starttime)/(double)CLOCKS_PER_SEC );
  printf("total time cost: %f\n", totaltime);
 return 0; 
}
