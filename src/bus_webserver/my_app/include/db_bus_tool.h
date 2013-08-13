#ifndef BUS_UTILS_H_
#define BUS_UTILS_H_




#define LOAD_DATA		1
#define LOAD_NONE		0

#define SELECT_COUNT_FMT	"SELECT COUNT(1) FROM "
#define SELECT_ALL_FMT		"SELECT * FROM "
#define DELETE_ALL_FMT		"DELETE  FROM"
#define INSERT_STD_FMT		"INSERT INTO "	//INSERT INTO tableName [(column-list)] VALUES(value-list)
#define UPDATE_STD_FMT		"UPDATE"	//UPDATE tableName SET assignment [, assignment]* [WHERE expr]


#define MRULES_INIT_WALK(buf, num, ptr) \
    for ((ptr) = (buf); (num) > 0; (num) --, (ptr) ++)	

#define STRACE()\
			printf("--%s line:%d\n",__FUNCTION__,__LINE__)
//#define DEBUG 0
#ifdef DEBUG			
#define DB_BUS_STRACE(format,...) \
		printf("DB_MEM <%20s> <%04d> "format"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define DB_BUS_STRACE(format,...)
#endif


//构造SQL条件子串,WHERE类型为int
#define CONSTRUCT_SQL_WHERE_SUB_BY_INT(name,sub,val)	\
char name[SQL_LEN];	\
do{	\
	memset(name, 0, SQL_LEN);	\
	snprintf(name,SQL_LEN, "WHERE %s=%d", sub, val);	\
}while(0)

//构造SQL条件子串,WHERE类型为string
#define CONSTRUCT_SQL_WHERE_SUB_BY_STR(name,sub,val)	\
char name[SQL_LEN];	\
do{	\
	memset(name, 0, SQL_LEN);	\
	if(NULL == val){	\
		snprintf(name,SQL_LEN, "WHERE %s=''", sub);	\
	}else{	\
		snprintf(name,SQL_LEN, "WHERE %s='%s'", sub, val);	\
	}	\
}while(0)

//构造SQL插入子串,INSERT类型为string
#define CONSTRUCT_SQL_INSERT_SUB_STR(name,val)	\
char name[SQL_LEN]; \
do{ \
	memset(name, 0, SQL_LEN);	\
	snprintf(name,SQL_LEN, " VALUES (%s)", val); \
}while(0)

//构造SQL插入子串,INSERT类型为string
#define CONSTRUCT_SQL_UPDATE_SUB_STR(name,val)	\
char name[SQL_LEN]; \
do{ \
	memset(name, 0, SQL_LEN);	\
	snprintf(name,SQL_LEN, " SET %s", val); \
}while(0)



sqlite3 *db_mem_get_db_handler();

/*
 * db_mem_open() 打开数据库并获取信号量
 * @param	db_path	数据库路径
 * semid	根据数据库路径计算
 */

int db_mem_open(char *db_path);
int db_mem_close();
void db_mem_free(void *ptr);


int db_mem_construct_dbstr(sqlite3_stmt **pStmt, sqlite3 *db, const char *head_str, const char *table_name, char *sub_str);
int db_mem_get_search_count(sqlite3 *db, const char *table_name, char *search_str , int *row_count);


#endif
