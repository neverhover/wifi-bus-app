#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_VERTEX_NUM 20
#define INFINITY 		99999
#define NIL				-1


#define STRACE	printf("%s %d\n", __FUNCTION__ ,__LINE__);
/** 
 * 
 */

typedef struct ArcNode{
	int adjvex;	//指向adj邻接表的位置
	int weight;
	struct ArcNode *nextarc;
}ArcNode;

typedef struct VNode{
	ArcNode *firstarc;
}VNode,AdjList[MAX_VERTEX_NUM];

typedef struct ALGraph{
	AdjList vertices;
	int vexnum, arcnum;	//arcnum为边的数量
}ALGraph;


void initALGraph(ALGraph* GPt,int vn){    //初始化结点
	GPt->arcnum = 0;
	GPt->vexnum = vn;
	int i;
	for(i=0;i<vn;i++){
		GPt->vertices[i].firstarc = NULL;
	}
}


void insertArc(ALGraph* GPt,int vhead,int vtail,int w){ //增加结点操作
	ArcNode* arcNodePt = malloc(sizeof(ArcNode));
	arcNodePt->nextarc = NULL;
	arcNodePt->adjvex = vtail;
	arcNodePt->weight = w;
 
	ArcNode* tailPt = GPt->vertices[vhead].firstarc;
	if(tailPt==NULL){
		GPt->vertices[vhead].firstarc = arcNodePt;
	}else{
		while(tailPt->nextarc!=NULL){
			tailPt = tailPt->nextarc;
		}
  		tailPt->nextarc = arcNodePt;
 	}
	GPt->arcnum ++;
}


void displayGraph(ALGraph G)  //打印结点
{
 ArcNode* arcNodePt;
 int i=0;
 for(;i<G.vexnum;i++)
 {
  arcNodePt = G.vertices[i].firstarc;
  printf("vertex %d :",i);
  while(arcNodePt!=NULL)
  {
   printf("%d->%d,w=%d  ", i, arcNodePt->adjvex,arcNodePt->weight);
   arcNodePt = arcNodePt->nextarc;
  }
  printf("\n");
 }
}

int getEdgeWeight(ALGraph G,int vhead,int vtail)  //求边的权重
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



void minHeapify(int Q[],int d[],int i)
{ //smallest,l,r,i都是优先队列元素的下标，范围是从1 ~ heap-size[Q]
 int l = 2*i;
 int r = 2*i+1;
 int smallest;
 if(l<=Q[0] && d[ Q[l] ] < d[ Q[i] ])

 {
  smallest = l;
 } 
 else
 {
  smallest = i;
 }
 if(r<=Q[0] && d[ Q[r] ] < d[ Q[smallest] ])

 {
  smallest = r;
 }
 if(smallest!=i)
 {
  int temp = Q[i];
  Q[i] = Q[smallest];
  Q[smallest] = temp;  

  minHeapify(Q,d,smallest);
 }
}

void buildMinHeap(int Q[],int d[]) //建立最小堆
{
	int i=Q[0]/2;
 for(;i>=1;i--)
 {
  minHeapify(Q,d,i); //调用minHeapify，以保持堆的性质。
 }
}

int extractMin(int Q[],int d[])   //3、从最小队列中，抽取最小结点
{ //摘取优先队列中最小元素的内容，这里返回图中顶点的标号(0 ~ G.vexnum-1)，
 //这些标号是保存在Q[1..n]中的
 if(Q[0]<1)
 {
  printf("heap overflow!");
  return -10000;
 }
 int min = Q[1];
 Q[1] = Q[Q[0]];
 Q[0] = Q[0] - 1;
 minHeapify(Q,d,1);
 return min;
}



void initSingleSource(ALGraph G,int s,int d[],int pi[]) 
{       //1、初始化结点工作
 int i=0;
 for(;i<G.vexnum;i++)

 {
  d[i] = INFINITY;
  pi[i] = NIL;
 }
 d[s] = 0;
}

void relax(int u,int v,ALGraph G,int d[],int pi[])
{ //4、松弛操作。
        //u是新加入集合S的顶点的标号
 if(d[v]>d[u]+getEdgeWeight(G,u,v))

 {
  d[v] = d[u] + getEdgeWeight(G,u,v);
  pi[v] = u;
 }
}

void dijkstra(ALGraph G,int s,int d[],int pi[],int Q[])
{ //Q[]是最小优先队列，Q[1..n]中存放的是图顶点标号,Q[0]中存放堆的大小
 //优先队列中有key的概念，这里key可以从d[]中取得。比如说，Q[2]的大小(key)为 d[ Q[2] ]

 initSingleSource(G,s,d,pi);  //1、初始化结点工作
 
 //2、初始化队列
 Q[0] = G.vexnum;
 int i=1;
 for(;i<=Q[0];i++)

 {
  Q[i] = i-1;
 }
 Q[1] = s;
 Q[s+1] = 0;

 int u;
 int v;
 while(Q[0]!=0)

 {
  buildMinHeap(Q,d);     //3.1、建立最小堆
  u = extractMin(Q,d);   //3.2、从最小队列中，抽取最小结点
  ArcNode* arcNodePt = G.vertices[u].firstarc;
  while(arcNodePt!=NULL)
 {
   v = arcNodePt->adjvex;
   relax(u,v,G,d,pi);    //4、松弛操作。
   arcNodePt = arcNodePt->nextarc;
  }
 }

}





void printRoute(int i, int pi[]){

}









int main(){
 
 ALGraph G;
 ALGraph* GPt = &G;
 
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

 printf("显示出此构造的图:\n");
 displayGraph(G);
 printf("\n");
 
 int d[MAX_VERTEX_NUM];
 int pi[MAX_VERTEX_NUM];
 int Q[MAX_VERTEX_NUM+1]; 
        //Q[]的第一个元素只保存堆的大小，不保存元素。所以定义长度时+1

 dijkstra(G,0,d,pi,Q);
 int i=0,j=0;
 for(;i<G.vexnum;i++){
  printf("从源点0到点%d的最短路径信息:\n",i);
  printf("长度为%d\n",d[i]);
  printf("路径为: %d ", i);
  for(j=i;j>0;){
  	printf("%d ",pi[j]);
  	j=pi[j];
  }
  printf("\n");
  // printf("路径为");
  // printRoute(i,pi);
  // printf("\n");
  // if(i==G.vexnum-1)
  //               {
  //  printf("\n");
  // }
 }
 return 0; 
}