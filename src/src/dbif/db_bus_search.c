#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "db_bus_table.h"

#include "db_bus_tool.h"



#define STR_SAFE_COPY(dst,src)		strncpy(dst,src,sizeof(dst))
#define STR_S_CP(x, y)		STR_SAFE_COPY(x,y)	

#define BUS_LINE_INFO_TABLE_NAME		"bus_line_info"
#define BUS_V_LINE_STA_TABLE_NAME		"v_line_sta_info"
#define BUS_STATION_TABLE_NAME		"station_t"


#ifndef UNIT_TEST
#define UNIT_TEST	0
#endif

/* 视图查找 */
static int db_bus_line_info_find(bus_line_info_t **bus_line_info, int load, char *search_str);
static int db_v_line_sta_info_find(v_line_sta_info_t **info, int load, char *search_str);
static int construct_v_line_sta_info_sql_find_line_by_sta_name(char *buf, int buf_size,char *name, int fuzzy);
static int construct_bus_line_info_fuzzy_sql_find_line_by_line_name(char *buf, int buf_size,char *name);
static int construct_station_fuzzy_sql_find_sta_by_sta_name(char *buf, int buf_size,char *name);




/* 从 '计费信息表' 查找所有数据 */
int db_bus_line_info_findall(bus_line_info_t **bus_line_info, int load){
	return db_bus_line_info_find(bus_line_info, load, " "); 
}


/* 根据 线路ID 从 线路视图 查找数据 */
int db_bus_line_info_findby_line_id(int id, bus_line_info_t **bus_line_info, int load, int fuzzy){
	CONSTRUCT_SQL_WHERE_SUB_BY_INT(search, "id", id, fuzzy);
	return db_bus_line_info_find(bus_line_info, load, search);
}


/* 根据 '线路名' 从 线路视图 查找数据 */
int db_bus_line_info_findby_line_name(char *name, bus_line_info_t **bus_line_info, int load, int fuzzy){
	CONSTRUCT_SQL_WHERE_SUB_BY_STR(search, "name", name, fuzzy);
	return db_bus_line_info_find(bus_line_info, load, search);
}


/* 根据站点找到对应的线路信息 */
int db_bus_line_info_findby_sta_name(char *name, bus_line_info_t **bus_line_info, int load, int fuzzy){
	char sub_str[SQL_LEN];
	if (construct_v_line_sta_info_sql_find_line_by_sta_name(sub_str, SQL_LEN, name, fuzzy) <0){
		return ERROR;
	}
	DB_STRACE("our sql:%s",sub_str);
	return db_bus_line_info_find(bus_line_info, load, sub_str);
}



#if 0
#endif



/* 根据 线路ID 从 线路站点视图 查找数据 */
int db_v_line_sta_info_findby_line_id(int id, v_line_sta_info_t **bus_line_info, int load, int fuzzy){
	CONSTRUCT_SQL_WHERE_SUB_BY_INT(search, "id", id, fuzzy);
	return db_v_line_sta_info_find(bus_line_info, load, search);
}

/* 根据 站点名 从 线路站点视图 查找数据 */
int db_v_line_sta_info_findby_sta_name(char *name, v_line_sta_info_t **bus_line_info, int load, int fuzzy){
	CONSTRUCT_SQL_WHERE_SUB_BY_STR(search, "sta_name", name, fuzzy);
	return db_v_line_sta_info_find(bus_line_info, load, search);
}

/* 根据站点名模糊查询站点的信息,从视图查询 */
int db_v_line_sta_info_findby_sta_name_fuzzy(char *name, v_line_sta_info_t **bus_line_info, int load){
	char sub_str[SQL_LEN];
	if (construct_station_fuzzy_sql_find_sta_by_sta_name(sub_str, SQL_LEN, name) <0){
		return ERROR;
	}
	DB_STRACE("our sql:%s",sub_str);
	return db_v_line_sta_info_find(bus_line_info, load, sub_str);
}


/* 根据 条件 从 线路站点视图 查找数据,且根据站点顺序来排序 */
int db_v_line_sta_info_findby_condition(v_line_sta_info_t *cond, v_line_sta_info_t **bus_line_info, int load, int fuzzy){
	char sub_str[SQL_LEN]; 
	int index = 0;
	memset(sub_str, 0, SQL_LEN);
	//id为线路ID，line_type为线路类型id
	DB_STRACE("fuzzy:%d,id:%d, kind_id:%d, type_id:%d\n",fuzzy, cond->id, cond->kind_id, cond->line_type_id);
	if(  cond->id > 0){
		CONSTRUCT_WHERE_STR_BY_INT(sub_str, SQL_LEN, index, "id", cond->id, fuzzy);
	}
	if( cond->kind_id >0){
		CONSTRUCT_WHERE_STR_BY_INT(sub_str, SQL_LEN, index, "kind_id", cond->kind_id, fuzzy);
	}

	if( cond->line_type_id >0){
		CONSTRUCT_WHERE_STR_BY_INT(sub_str, SQL_LEN, index, "line_type", cond->line_type_id, fuzzy);
	}
	snprintf (sub_str + index, SQL_LEN - index, "ORDER BY seq");
	return db_v_line_sta_info_find(bus_line_info, load, sub_str);
}


/* 构造查询SQL语句,针对于表bus_line_info
 * 查询经过指定站点的线路信息
 * @param buf 存放条件子句的buffer指针
   @param buf_size buffer 的 size ，函数会自动初始化为0一次
   @param name 要查询的站点名称
 */
static int construct_v_line_sta_info_sql_find_line_by_sta_name(char *buf, int buf_size,char *name, int fuzzy){
#if 0
		SELECT * FROM bus_line_info a 
		WHERE 
		a.id IN (
			SELECT distinct(id) FROM v_line_sta_info b 
			WHERE b.sta_name = 'xx'
		)
#endif
	if(NULL == buf){
		return ERROR;
	}
	memset(buf, 0, buf_size);
	/* 线路ID唯一,因为一个站点在同一条线路中可能出现多次 */
	if(SQL_SEARCH_FUZZY == fuzzy){
		snprintf(buf, buf_size, " WHERE %s.id IN ( SELECT distinct(id) FROM %s b \
	WHERE b.sta_name LIKE '%%%s%%')",BUS_LINE_INFO_TABLE_NAME, 
		BUS_V_LINE_STA_TABLE_NAME, name);
	}else{
		snprintf(buf, buf_size, " WHERE %s.id IN ( SELECT distinct(id) FROM %s b \
	WHERE b.sta_name='%s')",BUS_LINE_INFO_TABLE_NAME, 
		BUS_V_LINE_STA_TABLE_NAME, name);
	}
	
	return OK;
}

/* 构造查询SQL语句,针对于表station_t
 * 模糊查询站点的信息,取不重复的值
 * @param buf 存放条件子句的buffer指针
   @param buf_size buffer 的 size ，函数会自动初始化为0一次
   @param name 要查询的站点名称
 */
static int construct_station_fuzzy_sql_find_sta_by_sta_name(char *buf, int buf_size,char *name){
#if 0
		SELECT * FROM station_t  a inner join 
 (SELECT distinct(st_name),st_id FROM  station_t WHERE st_name LIKE '%路%' group by st_name) b
ON
b.st_id = a.st_id AND b.st_name = a.st_name
//case 2
SELECT * FROM v_line_sta_info a inner join 
 (SELECT distinct(st_name),st_id FROM  station_t WHERE st_name LIKE '%站%' group by st_name) b
ON
b.st_id = a.sta_id AND b.st_name = a.sta_name

#endif
	if(NULL == buf){
		return ERROR;
	}
	memset(buf, 0, buf_size);
	/* 使用方案2,从视图中查找 */
	snprintf(buf, buf_size, " a inner join  \
	 (SELECT distinct(st_name),st_id FROM  %s WHERE st_name LIKE '%%%s%%' group by st_name) b\
	 ON b.st_id = a.sta_id group by sta_name",BUS_STATION_TABLE_NAME, name);
	return OK;
}

/* 构造查询SQL语句,针对于表station_t
 * 模糊查询线路的信息
 * @param buf 存放条件子句的buffer指针
   @param buf_size buffer 的 size ，函数会自动初始化为0一次
   @param name 要查询的线路名称
 */
static int construct_bus_line_info_fuzzy_sql_find_line_by_line_name(char *buf, int buf_size,char *name){
#if 0
		SELECT * FROM bus_line_info a inner join 
 (SELECT distinct(name),id FROM bus_line_info WHERE name LIKE '%1%' )  b
ON
b.id = a.id

#endif
	if(NULL == buf){
		return ERROR;
	}
	memset(buf, 0, buf_size);
	/* 线路ID唯一,因为一个站点在同一条线路中可能出现多次 */
	snprintf(buf, buf_size, " a inner join  \
	 (SELECT distinct(name),id FROM  %s WHERE name LIKE '%%%s%%' ) b \
	 ON a.id = b.id",BUS_LINE_INFO_TABLE_NAME, name);
	return OK;
}





#if 0
#endif
/*
 * @param search_str	传入的SQL语句的部分
 */
static int db_bus_line_info_find(bus_line_info_t **bus_line_info, int load, char *search_str){

	sqlite3 *ptr_db = NULL;
	sqlite3_stmt *pStmt = NULL;
	int32_t ret, row_count = 0,real_row_count = 0,col_count=0;
	bus_line_info_t *ptr_bus_line_info_t = NULL;
	STRACE();
	if(NULL == search_str){
		DB_STRACE("search string is null");
		return ERROR;
	}
	
	ptr_db = db_get_db_handler();
	if(NULL == ptr_db){
		fprintf(stderr, "<%s> <%d>db get handler error \n",__FUNCTION__, __LINE__);
	}
	//查询 条件查询SQL返回的行数
	ret  = db_get_search_count(ptr_db, BUS_LINE_INFO_TABLE_NAME, search_str, &row_count);
	
	if(ret != OK){
		return ERROR;
	}
	//只是查看结果,则返回结果数量
	if( (LOAD_NONE == load) || (0 == row_count) ){
		return row_count;
	}
	
	/* 开始实际查询 */
	if( NULL == (*bus_line_info = malloc(sizeof(bus_line_info_t) * row_count) ) ){
		fprintf(stderr, "<%s> <%d> malloc failed \n",__FUNCTION__, __LINE__);
		return ERROR;
	}
	memset(*bus_line_info, 0, sizeof(bus_line_info_t) * row_count);
	
	ret = db_construct_dbstr(&pStmt, ptr_db, 
		SELECT_ALL_FMT, BUS_LINE_INFO_TABLE_NAME, search_str);
	//执行语句
	ret = sqlite3_step(pStmt);
	if(ret != SQLITE_ROW){
		fprintf(stderr, "SQL error: %s,rc =%d\n", sqlite3_errmsg(ptr_db) , ret);
		sqlite3_finalize(pStmt);
		return ERROR;
	}
	col_count = sqlite3_column_count(pStmt);
	ptr_bus_line_info_t = *bus_line_info;
#if 0
	int i = 0;
	//打印col名称
	while(i < col_count){
		DB_STRACE("%20s \t %d \t %20s \t %d\n",
			sqlite3_column_name(pStmt, i),
			sqlite3_column_type(pStmt, i),
			sqlite3_column_decltype(pStmt, i),
			sqlite3_column_bytes(pStmt, i));
		i++;
	}
	DB_STRACE("return rows %d \n", row_count);
#endif
	while(ret == SQLITE_ROW){
#if 1
		
		real_row_count++;
		ptr_bus_line_info_t->id =  sqlite3_column_int(pStmt, 0);
		STR_S_CP(ptr_bus_line_info_t->name,sqlite3_column_text(pStmt, 1));
		ptr_bus_line_info_t->line_type_id= sqlite3_column_int(pStmt, 2);
		STR_S_CP(ptr_bus_line_info_t->line_type , sqlite3_column_text(pStmt, 3));
		STR_S_CP(ptr_bus_line_info_t->status,sqlite3_column_text(pStmt, 4));
		STR_S_CP(ptr_bus_line_info_t->outbound, sqlite3_column_text(pStmt, 5));
		STR_S_CP(ptr_bus_line_info_t->inbound , sqlite3_column_text(pStmt, 6));
		STR_S_CP(ptr_bus_line_info_t->ticket ,sqlite3_column_text(pStmt, 7));
		STR_S_CP(ptr_bus_line_info_t->bus_type ,sqlite3_column_text(pStmt, 8));
		STR_S_CP(ptr_bus_line_info_t->bus_price , sqlite3_column_text(pStmt, 9));
		STR_S_CP(ptr_bus_line_info_t->company, sqlite3_column_text(pStmt, 10));
		STR_S_CP(ptr_bus_line_info_t->out_sta, sqlite3_column_text(pStmt, 11));
		STR_S_CP(ptr_bus_line_info_t->in_sta, sqlite3_column_text(pStmt, 12));
		
		ptr_bus_line_info_t++;
		
#else
		printf("%d, %s, %s, %s, %s, %s\n",
		  sqlite3_column_int(pStmt, 0),
		  sqlite3_column_text(pStmt, 1),
		  sqlite3_column_text(pStmt, 2),
		  sqlite3_column_text(pStmt, 3),
		  sqlite3_column_text(pStmt, 4),
		  sqlite3_column_text(pStmt, 5));
#endif
		ret = sqlite3_step(pStmt);
	}
	
	sqlite3_finalize(pStmt);
	if(row_count > real_row_count){
		row_count = real_row_count;
	}
	return row_count;
}

/*
 * @param search_str	传入的SQL语句的部分
 */
static int db_v_line_sta_info_find(v_line_sta_info_t ** info, int load, char *search_str){

	sqlite3 *ptr_db = NULL;
	sqlite3_stmt *pStmt = NULL;
	int32_t ret, row_count = 0, real_row_count = 0;
	v_line_sta_info_t *ptr_v_line_sta_info_find_t = NULL;
	
	if(NULL == search_str){
		return ERROR;
	}
	ptr_db = db_get_db_handler();

	//查询 条件查询SQL返回的行数
	ret  = db_get_search_count(ptr_db, BUS_V_LINE_STA_TABLE_NAME, search_str, &row_count);
	if(ret != OK){
		return ERROR;
	}
	//只是查看结果,则返回结果数量
	if( (LOAD_NONE == load) || (0 == row_count) ){
		return row_count;
	}

	/* 开始实际查询 */
	if( NULL == (*info = malloc(sizeof(bus_line_info_t) * row_count) ) ){
		fprintf(stderr, "<%s> <%d> malloc failed \n",__FUNCTION__, __LINE__);
		return ERROR;
	}
	memset(*info, 0, sizeof(v_line_sta_info_t) * row_count);
	
	ret = db_construct_dbstr(&pStmt, ptr_db, 
		SELECT_ALL_FMT, BUS_V_LINE_STA_TABLE_NAME, search_str);
	//执行语句
	ret = sqlite3_step(pStmt);
	if(ret != SQLITE_ROW){
		fprintf(stderr, "SQL error: %s,rc =%d\n", sqlite3_errmsg(ptr_db) , ret);
		sqlite3_finalize(pStmt);
		return ERROR;
	}
	//col_count = sqlite3_column_count(pStmt);
	ptr_v_line_sta_info_find_t = *info;
	while(ret == SQLITE_ROW){
#if 1
	
		real_row_count++;
		ptr_v_line_sta_info_find_t->id =  sqlite3_column_int(pStmt, 0);
		STR_S_CP(ptr_v_line_sta_info_find_t->line_name,sqlite3_column_text(pStmt, 1));
		ptr_v_line_sta_info_find_t->line_type_id= sqlite3_column_int(pStmt, 2);
		STR_S_CP(ptr_v_line_sta_info_find_t->line_type , sqlite3_column_text(pStmt, 3));
		ptr_v_line_sta_info_find_t->sta_id = sqlite3_column_int(pStmt, 4);
		STR_S_CP(ptr_v_line_sta_info_find_t->sta_name, sqlite3_column_text(pStmt, 5));
		STR_S_CP(ptr_v_line_sta_info_find_t->st_lng , sqlite3_column_text(pStmt, 6));
		STR_S_CP(ptr_v_line_sta_info_find_t->st_lat,sqlite3_column_text(pStmt, 7));
		STR_S_CP(ptr_v_line_sta_info_find_t->ticket ,sqlite3_column_text(pStmt, 8));
		STR_S_CP(ptr_v_line_sta_info_find_t->kind , sqlite3_column_text(pStmt, 9));
		ptr_v_line_sta_info_find_t->kind_id = sqlite3_column_int(pStmt, 10);
		ptr_v_line_sta_info_find_t->sta_status = sqlite3_column_int(pStmt, 11);
		ptr_v_line_sta_info_find_t->seq = sqlite3_column_int(pStmt, 12);
		ptr_v_line_sta_info_find_t++;
		
#else
		printf("%d, %s, %s, %s, %s, %s\n",
		  sqlite3_column_int(pStmt, 0),
		  sqlite3_column_text(pStmt, 1),
		  sqlite3_column_text(pStmt, 2),
		  sqlite3_column_text(pStmt, 3),
		  sqlite3_column_text(pStmt, 4),
		  sqlite3_column_text(pStmt, 5));
#endif
		ret = sqlite3_step(pStmt);
	}
	
	sqlite3_finalize(pStmt);
	if(row_count > real_row_count){
		row_count = real_row_count;
	}
	return row_count;
}



#ifdef UNIT_TEST

int main(int argc, char * argv[]){
	int ret = 0;
	bus_line_info_t *bs_all=NULL, *bs_tmp =NULL;
	v_line_sta_info_t *sta_all = NULL, *sta_tmp = NULL;
	bus_line_info_t new_row;
	v_line_sta_info_t condition_sta;
	memset(&new_row, 0, sizeof(new_row));
	memset(&condition_sta, 0, sizeof(condition_sta));
	
	db_open("./bus_base.sqlite");
	//查询
	if( (ret = db_bus_line_info_findall(&bs_all, 1)) <0){
		fprintf(stderr,"db_bus_line_info_findall error return %d\n", ret);
		return -1;
	}
	printf("db_bus_line_info_findall return %d\n", ret);
	MRULES_INIT_WALK(bs_all, ret, bs_tmp){
		printf("index:%d name:%s company:%s\n",
			bs_tmp->id, bs_tmp->name, bs_tmp->company);
	}
	db_free(bs_all);
	bs_all =NULL;
	//条件查询1
	if( (ret = db_bus_line_info_findby_line_name("1",&bs_all, LOAD_NONE , SQL_SEARCH_EQUAL)) <0){
		fprintf(stderr,"db_bus_line_info_findby_line_name error return %d\n", ret);
		return -1;
	}
	printf("<1>:db_bus_line_info_findby_line_name return %d\n", ret);
	db_unlock();

	//条件查询2
	if( (ret = db_v_line_sta_info_findby_line_id(1,&sta_all, LOAD_NONE , SQL_SEARCH_EQUAL)) <0){
		fprintf(stderr,"db_v_line_sta_info_findby_line_id error return %d\n", ret);
		return -1;
	}
	printf("<2>:db_v_line_sta_info_findby_line_id return %d\n", ret);

	//条件查询3
	char *name="组装";
	printf("name is %s\n",name);
	if( (ret = db_v_line_sta_info_findby_sta_name("abc",&sta_all, LOAD_NONE , SQL_SEARCH_EQUAL)) <0){
		fprintf(stderr,"db_v_line_sta_info_findby_sta_name error return %d\n", ret);
		return -1;
	}
	printf("\n<3>:db_v_line_sta_info_findby_sta_name return %d\n", ret);

	//条件查询4
	condition_sta.id = 1;
	condition_sta.kind_id = 1;
	if( (ret = db_v_line_sta_info_findby_condition(&condition_sta,&sta_all, LOAD_NONE , SQL_SEARCH_EQUAL)) <0){
		fprintf(stderr,"db_bus_line_info_findby_sta_name error return %d\n", ret);
		return -1;
	}
	printf("<4>:db_bus_line_info_findby_sta_name return %d\n", ret);
	db_unlock();
	//测试完成
	db_close();
}
#endif

#undef  UNIT_TEST

