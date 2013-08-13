#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "db_bus_tool.h"
#include "db_bus_table.h"


#define STR_SAFE_COPY(dst,src)		strncpy(dst,src,sizeof(dst))
#define STR_S_CP(x, y)		STR_SAFE_COPY(x,y)	

#define BUS_LINE_INFO_TABLE_NAME	"bus_line_info_t"

#ifndef UNIT_TEST
#define UNIT_TEST	0
#endif

static int db_bus_line_info_find(bus_line_info_t **bus_line_info, int load, char *search_str);





/* 从 '计费信息表' 查找所有数据 */
int db_bus_line_info_findall(bus_line_info_t **bus_line_info, int load){
	return db_bus_line_info_find(bus_line_info, load, " "); 
}

/* 根据 '计费信息主键' 从 '计费信息表' 查找数据 */
int db_bus_line_info_findby_line_name(char *name, bus_line_info_t **bus_line_info, int load){
	CONSTRUCT_SQL_WHERE_SUB_BY_STR(search, "name", name);
	return db_bus_line_info_find(bus_line_info, load, search); 
}


/*
 * @param sql_str	传入的SQL语句的部分
 */
static int db_bus_line_info_find(bus_line_info_t **bus_line_info, int load, char *search_str){

	sqlite3 *ptr_db = NULL;
	sqlite3_stmt *pStmt = NULL;
	int32_t ret, row_count = 0,col_count = 0;
	bus_line_info_t *ptr_bus_line_info_t = NULL;
	
	if(NULL == search_str){
		return ERROR;
	}
	ptr_db = db_get_db_handler();

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
		fprintf(stderr, "<%s> <%d> malloc failed \n",_FUNCTION_, _LINE_);
		return ERROR;
	}
	memset(*bus_line_info_t, 0, sizeof(bus_line_info_t) * row_count);
	
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
	while(ret == SQLITE_ROW){
#if 1
		typedef struct{
			int id;
			char name[40];
			int line_type_id;
			char line_type[40];
			char status[40];
			char outbound[40];
			char inbound[40];
			char ticket[64];
			char bus_type[40];
			char bus_price[32];
			char company[64];
		}bus_line_info_t;

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
		STR_S_CP(ptr_bus_line_info_t->company, sqlite3_column_text(pStmt, 10)));
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
	return row_count;
}

#ifdef UNIT_TEST

int main(int argc, char * argv[]){
	int ret = 0;
	bus_line_info_t *bs_all=NULL, *bs_tmp =NULL;
	bus_line_info_t new_row;
	memset(&new_row, 0, sizeof(new_row));
	
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
	//条件查询
	if( (ret = db_bus_line_info_findby_line_name(1,&bs_all, 0)) <0){
		fprintf(stderr,"db_bus_line_info_findby_line_name error return %d\n", ret);
		return -1;
	}
	printf("db_bus_line_info_findby_line_name return %d\n", ret);
	db_free(bs_all);
	db_unlock();
	//测试完成
	db_close();
}
#endif

#undef  UNIT_TEST

