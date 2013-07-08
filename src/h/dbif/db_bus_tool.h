#ifndef BUS_UTILS_H_
#define BUS_UTILS_H_


#define SQL_LEN			1024

#define SELECT_COUNT_FMT	"SELECT COUNT(1) FROM "
#define SELECT_ALL_FMT		"SELECT * FROM "
#define DELETE_ALL_FMT		"DELETE  FROM"
#define INSERT_STD_FMT		"INSERT INTO "	//INSERT INTO tableName [(column-list)] VALUES(value-list)
#define UPDATE_STD_FMT		"UPDATE"	//UPDATE tableName SET assignment [, assignment]* [WHERE expr]




#define MRULES_INIT_WALK(buf, num, ptr) \
    for ((ptr) = (buf); (num) > 0; (num) --, (ptr) ++)	

#define STRACE()\
			printf("--%s line:%d\n",__FUNCTION__,__LINE__)
#define DEBUG 1
#ifdef DEBUG			
#define DB_STRACE(format,...) \
		printf("\nDB_STRACE <%s> <%04d> "format"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define DB_STRACE(format,...)
#endif


//构造SQL条件子串,WHERE类型为int
#define CONSTRUCT_SQL_WHERE_SUB_BY_INT(name,sub,val, fuzzy)	\
char name[SQL_LEN];	\
do{	\
	memset(name, 0, SQL_LEN);	\
	if(fuzzy == SQL_SEARCH_FUZZY){		\
		snprintf(name,SQL_LEN, "WHERE %s LIKE '%%%d%%'", sub, val);	\
	}else{	\
		snprintf(name,SQL_LEN, "WHERE %s=%d", sub, val);	\
	}	\
}while(0)

//构造SQL条件子串,WHERE类型为string
#define CONSTRUCT_SQL_WHERE_SUB_BY_STR(name,sub,val,fuzzy)	\
char name[SQL_LEN];	\
do{	\
	memset(name, 0, SQL_LEN);	\
	if(NULL == val){	\
		snprintf(name,SQL_LEN, "WHERE %s=''", sub);	\
	}else{	\
		if(fuzzy == SQL_SEARCH_FUZZY){		\
			snprintf(name,SQL_LEN, "WHERE %s LIKE '%%%s%%'", sub, val);	\
		}else{	\
			snprintf(name,SQL_LEN, "WHERE %s='%s'", sub, val);	\
		}	\
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

#define CONSTRUCT_WHERE_STR_BY_STR(buf, buf_size, index, attribute, value, fuzzy) \
do { \
	if( (buf_size - index ) <=0) return -1; \
		if(fuzzy == SQL_SEARCH_FUZZY){		\
			index += snprintf (buf + index, buf_size - index, "%s %s LIKE '%%%s%%' ", (index == 0)? "WHERE" : "AND",attribute, value); \
		}else{	\
			index += snprintf (buf + index, buf_size - index, "%s %s='%s' ", (index == 0)? "WHERE" : "AND",attribute, value); \
		}	\
	if(index < 0) return -1; \
} while (0)

#define CONSTRUCT_WHERE_STR_BY_INT(buf, buf_size, index, attribute, value, fuzzy) \
	do { \
		if( (buf_size - index ) <=0) return -1; \
		if(fuzzy == SQL_SEARCH_FUZZY){		\
			index += snprintf (buf + index, buf_size - index, "%s %s LIKE '%%%d%%' ", (index == 0)? "WHERE" : "AND",attribute, value); \
		}else{	\
			index += snprintf (buf + index, buf_size - index, "%s %s=%d ", (index == 0)? "WHERE" : "AND",attribute, value); \
		}	\
		if(index < 0) return -1; \
	} while (0)




sqlite3 *db_get_db_handler();

/*
 * db_open() 打开数据库并获取信号量
 * @param	db_path	数据库路径
 * semid	根据数据库路径计算
 */

int db_open(char *db_path);
int db_close();
void db_free(void *ptr);


int db_construct_dbstr(sqlite3_stmt **pStmt, sqlite3 *db, const char *head_str, const char *table_name, char *sub_str);
int db_get_search_count(sqlite3 *db, const char *table_name, char *search_str , int *row_count);


#endif
