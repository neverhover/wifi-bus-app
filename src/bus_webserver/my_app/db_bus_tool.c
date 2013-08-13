#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#include "sqlite3.h"
#include "db_bus_tool.h"


//#define DB_MEM_PATH "/tmp/mem_longteng.db3"	//remove it
#define IPCKEY 0x111


static sqlite3 *db_handle;	//db指针
static char *db_path_cur;	//当前数据库路径
static int semid;			//信号量

/*
 * semaphore_set_value() 初始化信号量,用于第一次
 * 
 */
static int semaphore_set_value(int semid);
/*
 * semaphore_set_value() 删除信号量
 * 
 */
static int semaphore_del_value(int semid);

static int semaphore_p(int semid);
static int semaphore_v(int semid);

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
    struct seminfo *__buf;
};



static int semaphore_set_value(int semid){
	union semun sem_union;

	sem_union.val = 1;
	if(semctl(semid, 0, SETVAL, sem_union) == -1) {
		fprintf(stderr, "Failed to init semaphore \n");
		return ERROR;
	}
	return OK;
}
static int semaphore_del_value(int semid){
	union semun sem_union;

	if(semctl(semid, 0, IPC_RMID, sem_union) == -1){
		fprintf(stderr, "Failed to delete semaphore \n");
		return ERROR;
	}
	return OK;
}

static int semaphore_p(int semid){
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO;
	if(semop(semid, &sb, 1)  == -1){
		perror("lock sem");
	}
}
static int semaphore_v(int semid){
	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = 1;
	sb.sem_flg = SEM_UNDO;
	if(semop(semid, &sb, 1)  == -1){
		perror("unlock sem");
	}
}


/*
 * db_mem_get_db_handler()	返回db操作的指针
 * @return	db操作的指针
 *
 */
sqlite3 *db_mem_get_db_handler(){
	return db_handle;
}


int db_mem_open(char *db_path){
	int rc = 0;
	if(NULL == db_path){
		return ERROR;
	}
	rc = sqlite3_open(db_path, &db_handle);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db_handle));
		db_mem_close();
		return ERROR;
	}
	db_path_cur=(char *)malloc(strlen(db_path));
	if( NULL == db_path_cur){
		return ERROR;
	}
	//Get key
	key_t key= ftok(db_path_cur, IPCKEY);
	semid = semget(key, 1,  0666 | IPC_CREAT |IPC_EXCL);
	if(semid < 0){
		//means already create.so try again
		DB_MEM_STRACE("semid alreay exist\n");
		semid = semget(key, 1,  0666);
	}else{
		DB_MEM_STRACE("new semid init it\n");
		semaphore_set_value(semid);
	}
	DB_MEM_STRACE("semid=%d\n",semid);
	return OK;
}

int db_mem_close(){
	sqlite3_close(db_handle);
	free(db_path_cur);
	db_path_cur=NULL;
	//delete sem
	//semaphore_del_value(semid);
	return OK;
}

void db_mem_free(void *ptr){
	free(ptr);
	if(NULL!=ptr){
		ptr =NULL;
	}
}

void db_mem_lock(){
	semaphore_p(semid);
}

void db_mem_unlock(){
	semaphore_v(semid);
}


/*
 * 构造sqlte_stmt
 *
 */
int db_mem_construct_dbstr(sqlite3_stmt **pStmt, sqlite3 *db, const char *head_str, const char *table_name, char *sub_str){
	int ret;
	char *sql =NULL;
	
	sql = sqlite3_mprintf("%s %s %s", head_str, table_name, sub_str);
	ret = sqlite3_prepare(db, sql, -1, pStmt, NULL);
	
	if(ret != SQLITE_OK){
		fprintf(stderr, "SQL prepare error: %s, ret = %d\n",sqlite3_errmsg(db), ret);
		//fprintf(stderr,"SQL:%s\n",sql);
		sqlite3_free(sql);
		return ERROR;
	}
	sqlite3_free(sql);
	return ret;
}

/*
 * 查询 SQL查询语句返回的记录数
 */
int db_mem_get_search_count(sqlite3 *db, const char *table_name, char *search_str , int *row_count){
	int ret;
	sqlite3_stmt *pStmt = NULL;

	if(NULL == db || NULL == table_name  ||NULL== search_str || NULL == row_count ){
		return ERROR;
	}
	ret = db_mem_construct_dbstr(&pStmt, db, SELECT_COUNT_FMT, table_name, search_str);
	if(ERROR == ret){
		return ERROR;
	}
	//执行语句
	ret = sqlite3_step(pStmt);
	if(ret != SQLITE_ROW){
		fprintf(stderr, "SQL get rows rount error: %s,rc =%d\n", sqlite3_errmsg(db) , ret);
		sqlite3_finalize(pStmt);
		return ERROR;
	}
	*row_count = sqlite3_column_int(pStmt,0);
	
	sqlite3_finalize(pStmt);
	return OK;
}

/*
 * 删除表的所有记录
 */
int db_mem_delete_table_rows(sqlite3 *db, const char *table_name, char *condition_str , int *row_count){
	int ret;
	sqlite3_stmt *pStmt = NULL;

	if(NULL == db || NULL == table_name  ||NULL== condition_str ){
		return ERROR;
	}
	ret = db_mem_construct_dbstr(&pStmt, db, DELETE_ALL_FMT, table_name, condition_str);
	if(ERROR == ret){
		return ERROR;
	}
	//执行语句
	ret = sqlite3_step(pStmt);
	if(ret != SQLITE_DONE){
		fprintf(stderr, "SQL delete rows error: %s,rc =%d\n", sqlite3_errmsg(db) , ret);
		sqlite3_finalize(pStmt);
		return ERROR;
	}
	//*row_count = sqlite3_column_int(pStmt,0);
	
	sqlite3_finalize(pStmt);
	return OK;
}




