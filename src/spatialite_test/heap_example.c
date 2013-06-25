/** 
 * @file 二叉堆的基本实现样例
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LEFT_CHILD(x)	(x << 1)
#define RIGHT_CHILD(x)	(LEFT_CHILD(x) +1)
#define PARENT(x)		(x >> 1)
#define DIV(x)			(x >> 1)
#define SWAP(a, b);	\
 		a^=b;b^=a;a^=b;



#define NOT_USE_REC			1	//是否使用递归
#define UNIT_TEST			1 	//是否包含测试
#ifdef UNIT_TEST
#define MAX_SIZE			10
#endif

#define DEBUG 0
#if DEBUG
	#define DG(x,...) printf(x,##__VA_ARGS__);
#else
 	#define DG(x,...)	
#endif


typedef int *PQ_PTR;
typedef int PQ_TYPE;
typedef int DIS_TYPE;
 /** 
  * 需要5个操作，dequeue顶点,enqueue到最后,heap_down,heap_up,heap_sort
  */
 
#if UNIT_TEST
void pq_display(PQ_PTR pq, DIS_TYPE d[], int pq_len);
#endif
/** 
 * 最小堆的下沉排序
 * @param pq     队列头指针
 * @param cur    队列当前节点索引
 * @param d      distanceTo的指针
 * @param pq_len 队列长度
 */
void pq_min_heapify_down(PQ_PTR pq, int cur, DIS_TYPE d[], int pq_len){
#if NOT_USE_REC	//不使用递归来成
	int min;
	while(cur <=  DIV(pq_len) ){	//确保当前节点有子节点
		min = LEFT_CHILD(cur);
		if( (min+ 1 <= pq_len) && (d[pq[min+1]] < d[pq[min]]) ){
			min = min+1; //right节点为最小
		}
		if(d[pq[min]] < d[pq[cur]] ){
			SWAP(pq[min], pq[cur]);
			cur = min;	//cur指向当前位置,即原来节点的子节点。故隐含cur会变大,其有可能不存在子节点的条件
		}else{
			break;	//表示符合堆特性
		}
	}

#else
	int left = LEFT_CHILD(cur);
	int right = RIGHT_CHILD(cur);
	int min = 0; //最小节点的索引
	//先和自己的左子节点比较
	if( (left <= pq_len) && ( d[pq[left]] < d[pq[cur]] ) ){
		min = left;
	}else{
		min = cur;
	}
	//然后和自己的右节点比较
	if( (right <= pq_len) && ( d[pq[right]] < d[pq[min]] ) ){
		min = right;
	}
	//如果2次比较完，没有变化，那么min == cur 成立
	if(min != cur){
		//交换'节点索引'位置
		// int tmp = pq[cur];
		// pq[cur] = pq[min];
		// pq[min] = tmp;
		SWAP(pq[cur], pq[min]);
		//表示此节点发生了变化需要再次比较,下沉左子或右子
		//以此保证父节点一定小于子节点
		pq_min_heapify_down(pq, min, d, pq_len);
	}
#endif
}

void pq_min_heapify_up(PQ_PTR pq, int cur, DIS_TYPE d[]){
#if NOT_USE_REC	//不使用递归来成
	int parent = 1;
	while(cur >1){
		parent = PARENT(cur);
		if(d[pq[cur]] < d[pq[parent]]){
			SWAP(pq[cur], pq[parent]);
		}else{
			//表示已经符合堆的特性
			break;
		}
	}
#else
	int parent = 1;
	if( cur > 1){
		parent = PARENT(cur);
		if(d[pq[cur]] < d[pq[parent]]){
			//表示当前节点比父节点小，则需要交换
			// int tmp = pq[cur];
			// pq[cur] = pq[parent];
			// pq[parent] = tmp;
			SWAP(pq[cur], pq[parent]);
			pq_min_heapify_up(pq, min, d);
		}
	}
	
#endif
}


int pq_dequeue_top(PQ_PTR pq, DIS_TYPE d[]){
 	
 	if(pq[0]){
 		int t = pq[1];
 		pq[1] = pq[pq[0]];
 		//pq[pq[0]] = t; 因为长度会减一，自然会忽略掉此元素,为一个隐藏的出队动作。
 		pq[0] -= 1;
 		//按道理说这里应该进行一次排序,且为下沉
 		pq_min_heapify_down(pq, 1, d, pq[0]);
 		return t;
 	}
 	return 0;
 }

/** 
 * 入队一个元素到堆尾
 * @param pq      队列头指针
 * @param new_idx 新入队的distanTo顶点index
 * @param d       distanceTo的指针,用于比较大小
 */
 void pq_enqueue_end(PQ_PTR pq, int new_idx, DIS_TYPE d[]){
 	pq[0]++;
 	pq[pq[0]] = new_idx;
 	pq_min_heapify_up(pq, pq[0], d);
 }



void pq_queue_build(PQ_PTR pq, DIS_TYPE d[]){
	int i= DIV(pq[0]);
	//原因是顶点总是最大或是最小，索引应该从最后一层父节点开始。
	for(;i>=1; i--){
		pq_min_heapify_down(pq, i, d, pq[0]);
	}
}

/** 
 * 堆排序,保证数组1...n都是有序的
 * @param pq 队列头指针
 * @param d  distanceTo的指针,用于比较大小
 */
void pq_sort(PQ_PTR pq, DIS_TYPE d[]){
	//build heap
	
	int cur = DIV(pq[0]);
	for(; cur >=1; cur --){
		pq_min_heapify_down(pq, cur, d, pq[0]);
		//DG("cur :%d ## ",cur);
		//pq_display(pq, d, pq[0]);
	}
	
	//@brief sort 
	//cur指向堆尾,因为前面建立堆保证了堆顶一定是最大或者是最小
	//那么这个时候将堆顶和堆尾交换,就可以保证每次换到堆尾的都是最小
	for(cur = pq[0]; cur >=2; cur--){
		SWAP(pq[1], pq[cur]);
		//就是一个出队，然后下沉的动作，但是需要注意的是,队列长度不会变
		#if DEBUG
			//printf("\n");
			//pq_display(pq, d, pq[0]);
			pq_min_heapify_down(pq, 1, d, cur-1);
			pq_display(pq, d, pq[0]);
		#else
			pq_min_heapify_down(pq, 1, d, cur-1);		
		#endif

	}
}

#if UNIT_TEST

void pq_display(PQ_PTR pq, DIS_TYPE d[], int pq_len){
	int i = 1;
	for(; i<= pq_len; i++){
		printf("%d:[%02d]%02d  ", i, pq[i], d[pq[i]]);
	}
	printf("\n");
}

void pq_init(PQ_PTR pq, int pq_len, DIS_TYPE d[]){
	int i = 1;
	pq[0] = pq_len;
	DG("init:\n");
	for(;i<=pq_len;i++){
		pq[i] = i-1;
		DG("pq[%d] index -->dis[%d]=%03d \n", i,pq[i], d[pq[i]]);
	}
	DG("\n");
}

int main(int argc, char const *argv[])
{
	/* code */
	DIS_TYPE d[MAX_SIZE]={10,20,30,100,50,21,18,17,3,58};
	PQ_PTR	pq = NULL;	//proprity queue
	int cur_min  = 0;
	if( ( pq=malloc(sizeof(PQ_TYPE) * (MAX_SIZE+1)) ) == NULL){
		printf("malloc pq error, malloc size:%d\n", MAX_SIZE+1);
		return -1;
	}
	pq_init(pq, MAX_SIZE, d);
	
	pq_queue_build(pq, d);
	printf("\nafter build heap:\n");
	pq_display(pq, d, MAX_SIZE);

	cur_min = pq_dequeue_top(pq, d);
	printf("\ndequeue : distanceTo[%d]=%d\n", cur_min, d[cur_min]);
	pq_display(pq, d, pq[0]);

	pq_sort(pq, d);
	printf("\nsort test:\n");
	pq_display(pq, d, pq[0]);
	return 0;
}

#endif