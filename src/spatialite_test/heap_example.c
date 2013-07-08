/** 
 * @file 二叉堆的基本实现样例
 * 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "heap.h"

#define LEFT_CHILD(x)	(x << 1)
#define RIGHT_CHILD(x)	(LEFT_CHILD(x) +1)
#define PARENT(x)		(x >> 1)
#define DIV(x)			(x >> 1)
#define SWAP(a, b);	\
 		a^=b;b^=a;a^=b;



#define NOT_USE_REC			1	//是否使用递归
#ifdef UNIT_TEST
 		#define MAX_SIZE			10
#else
 		#define UNIT_TEST			0 	//是否包含测试
		#ifdef UNIT_TEST
			#define MAX_SIZE			10
		#endif
#endif




 /** 
  * 需要5个操作，dequeue顶点,enqueue到最后,heap_down,heap_up,heap_sort
  */
 

/** 
 * 最小堆的下沉排序
 * @param pq     队列头指针
 * @param cur    队列当前节点索引
 * @param d      distanceTo的指针
 * @param pq_len 队列长度
 */
void pq_min_heap_down(PQ_PTR pq, uint32_t cur, DIS_TYPE d[], uint32_t pq_len){
#if NOT_USE_REC	//不使用递归来成
	uint32_t min;	//min 为数组索引
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
	uint32_t left = LEFT_CHILD(cur);
	uint32_t right = RIGHT_CHILD(cur);
	uint32_t min = 0; //最小节点的索引
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
		pq_min_heap_down(pq, min, d, pq_len);
	}
#endif
}

void pq_min_heap_up(PQ_PTR pq, uint32_t cur, DIS_TYPE d[]){
#if NOT_USE_REC	//不使用递归来成
	uint32_t parent = 1;
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
	uint32_t parent = 1;
	if( cur > 1){
		parent = PARENT(cur);
		if(d[pq[cur]] < d[pq[parent]]){
			//表示当前节点比父节点小，则需要交换
			// int tmp = pq[cur];
			// pq[cur] = pq[parent];
			// pq[parent] = tmp;
			SWAP(pq[cur], pq[parent]);
			pq_min_heap_up(pq, parent, d);
		}
	}
	
#endif
}


/** 
 * 最小堆的下沉排序
 * @param pq     队列头指针
 * @param cur    队列当前节点索引
 * @param d      distanceTo的指针
 * @param pq_len 队列长度
 */
void pq_max_heap_down(PQ_PTR pq, uint32_t cur, DIS_TYPE d[], uint32_t pq_len){

	uint32_t max;
	while(cur <=  DIV(pq_len) ){	//确保当前节点有子节点
		max = LEFT_CHILD(cur);
		if( (max+ 1 <= pq_len) && (d[pq[max+1]] > d[pq[max]]) ){
			max = max+1; //right节点为最小
		}
		if(d[pq[max]] > d[pq[cur]] ){
			SWAP(pq[max], pq[cur]);
			cur = max;	//cur指向当前位置,即原来节点的子节点。故隐含cur会变大,其有可能不存在子节点的条件
		}else{
			break;	//表示符合堆特性
		}
	}
}

void pq_max_heap_up(PQ_PTR pq, uint32_t cur, DIS_TYPE d[]){

	uint32_t parent = 1;
	while(cur >1){
		parent = PARENT(cur);
		if(d[pq[cur]] > d[pq[parent]]){
			SWAP(pq[cur], pq[parent]);
		}else{
			//表示已经符合堆的特性
			break;
		}
	}
}

/***************************************************************************/


PQ_TYPE pq_dequeue_min_top(PQ_PTR pq, DIS_TYPE d[]){
 	
 	if(pq[0]){
 		PQ_TYPE t = pq[1];
 		pq[1] = pq[pq[0]];
 		//pq[pq[0]] = t; 因为长度会减一，自然会忽略掉此元素,为一个隐藏的出队动作。
 		pq[0] -= 1;
 		//按道理说这里应该进行一次排序,且为下沉
 		pq_min_heap_down(pq, 1, d, (uint32_t)pq[0]);
 		return t;
 	}
 	return 0;
 }

PQ_TYPE pq_dequeue_max_top(PQ_PTR pq, DIS_TYPE d[]){
 	
 	if(pq[0]){
 		PQ_TYPE t = pq[1];
 		pq[1] = pq[pq[0]];
 		//pq[pq[0]] = t; 因为长度会减一，自然会忽略掉此元素,为一个隐藏的出队动作。
 		pq[0] -= 1;
 		//按道理说这里应该进行一次排序,且为下沉
 		pq_max_heap_down(pq, 1, d, (uint32_t)pq[0]);
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
 void pq_enqueue_min_end(PQ_PTR pq, uint32_t new_idx, DIS_TYPE d[]){
 	pq[0]++;
 	pq[pq[0]] = new_idx;
 	pq_min_heap_up(pq, (uint32_t)pq[0], d);
 }

/** 
 * 入队一个元素到堆尾
 * @param pq      队列头指针
 * @param new_idx 新入队的distanTo顶点index
 * @param d       distanceTo的指针,用于比较大小
 */
void pq_enqueue_max_end(PQ_PTR pq, PQ_TYPE new_idx, DIS_TYPE d[]){
	pq[0]++;
	pq[pq[0]] = new_idx;
	pq_max_heap_up(pq, pq[0], d);
}


void pq_queue_build_min(PQ_PTR pq, DIS_TYPE d[]){
	PQ_TYPE i= DIV(pq[0]);
	//原因是顶点总是最大或是最小，索引应该从最后一层父节点开始。
	for(;i>=1; i--){
		pq_min_heap_down(pq, i, d, (uint32_t)pq[0]);
	}
}

void pq_queue_build_mxn(PQ_PTR pq, DIS_TYPE d[]){
	PQ_TYPE i= DIV(pq[0]);
	//原因是顶点总是最大或是最小，索引应该从最后一层父节点开始。
	for(;i>=1; i--){
		pq_max_heap_down(pq, i, d, (uint32_t)pq[0]);
	}
}


/** 
 * 堆排序,保证数组1...n都是有序的,这里最终是形成大端堆
 * @param pq 队列头指针
 * @param d  distanceTo的指针,用于比较大小
 */
void pq_sort_to_max_heap(PQ_PTR pq, DIS_TYPE d[]){
	//build heap
	
	uint32_t cur = DIV(pq[0]);
	for(; cur >=1; cur --){
		pq_min_heap_down(pq, cur, d, (uint32_t)pq[0]);
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
			pq_min_heap_down(pq, 1, d, cur-1);
			pq_display(pq, d, (uint32_t)pq[0]);
		#else
			pq_min_heap_down(pq, 1, d, cur-1);		
		#endif

	}
}

/** 
 * 堆排序,保证数组1...n都是有序的,这里最终是形成小端堆
 * @param pq 队列头指针
 * @param d  distanceTo的指针,用于比较大小
 */
void pq_sort_to_min_heap(PQ_PTR pq, DIS_TYPE d[]){
	//build heap
	
	uint32_t cur = DIV(pq[0]);
	for(; cur >=1; cur --){
		pq_max_heap_down(pq, cur, d, pq[0]);
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
			pq_max_heap_down(pq, 1, d, cur-1);
			pq_display(pq, d, (uint32_t)pq[0]);
		#else
			pq_max_heap_down(pq, 1, d, cur-1);		
		#endif

	}
}




void pq_display(PQ_PTR pq, DIS_TYPE d[], uint32_t pq_len){
	uint32_t i = 1;
	for(; i<= pq_len; i++){
		#ifdef DOUBLE_TYPE
			printf("%d:[%d] %f  ", i, pq[i], d[pq[i]]);
		#else
			printf("%d:[%02d] %02d  ", i, pq[i], d[pq[i]]);
		#endif
		
	}
	printf("\n");
}

void pq_init(PQ_PTR pq, uint32_t pq_len, DIS_TYPE d[]){
	uint32_t i = 1;
	pq[0] = pq_len;
	DG("init:\n");
	for(;i<=pq_len;i++){
		pq[i] = i-1;
		#ifdef DOUBLE_TYPE
			DG("pq[%llu] index -->dis[%llu]=%llu \n", i,pq[i], d[pq[i]]);
		#else
			DG("pq[%d] index -->dis[%d]=%03d \n", i,pq[i], d[pq[i]]);
		#endif
		
	}
	DG("\n");
}

#if UNIT_TEST



int main(int argc, char const *argv[])
{
	/* code */
	DIS_TYPE d[MAX_SIZE]={10,20,30,100,50,21,18,17,3,58};
	PQ_PTR	pq = NULL;	//proprity queue
	uint32_t cur_min  = 0;
	if( ( pq=malloc(sizeof(PQ_TYPE) * (MAX_SIZE+1)) ) == NULL){
		printf("malloc pq error, malloc size:%d\n", MAX_SIZE+1);
		return -1;
	}
	pq_init(pq, MAX_SIZE, d);
	
	pq_queue_build_min(pq, d);
	printf("\nafter build heap:\n");
	pq_display(pq, d, MAX_SIZE);

	cur_min = pq_dequeue_top_min(pq, d);
	#ifdef DOUBLE_TYPE
		printf("\ndequeue min : distanceTo[%llu]=%llu\n", cur_min, d[cur_min]);
	#else
		printf("\ndequeue min : distanceTo[%d]=%d\n", cur_min, d[cur_min]);
	#endif
	
	pq_display(pq, d, pq[0]);

	// d[2] = 9;
	// pq_min_heap_up(pq, 2 ,d);
	// printf("\nupdate  : distanceTo[%d]=%d\n", pq[2], d[2]);
	// pq_display(pq, d, pq[0]);

	// pq_sort_to_max_top_heap(pq, d);
	// printf("\nsort to big first test:\n");
	// pq_display(pq, d, pq[0]);


	pq_sort_to_min_top_heap(pq, d);
	printf("\nsort to little first test:\n");
	pq_display(pq, d, pq[0]);
	return 0;
}

#endif
