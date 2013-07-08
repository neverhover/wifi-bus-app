#include <stdio.h>
#include <string.h>
#include "sqlite3.h"
//#include "gaiageo.h"
//#include "spatialite.h"

#define SPATIALITE	0

 static int callback(void *NotUsed, int argc, char **argv, char **azColName){
 int i;
    for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
  }

#if 1
static void db_search(sqlite3 *db, char * search_name){
	char *sql;
	sqlite3_stmt *stmt = NULL;
	sql= sqlite3_mprintf("select * from group_t where group_name=?");
	sqlite3_prepare(db,sql,-1,&stmt, NULL);
	sqlite3_bind_text(stmt, 1, search_name, strlen(search_name),  NULL);
/*
	sql=sqlite3_mprintf("update user set money=?,savedata=? where userID like ?") ;
	sqlite3_prepare(db,sql,-1,&stmt,&tail);

	sqlite3_bind_double(stmt,1,msg.msg_user.money);

	sqlite3_bind_blob(stmt,2,buffer,sizeof(tm),NULL);

	sqlite3_bind_text(stmt,3,msg.msg_user.userID,strlen(msg.msg_user.userID),NULL);
*/
	printf("[%d]\n",sqlite3_step(stmt));
	 sqlite3_free(sql);
}
#endif
  
  int main(int argc, char **argv){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc, ret = 0, j,i, row_count = 0;

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
#if 0 //采用标准做法
    rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }else{
    	printf("exec result %s\n", zErrMsg);
	sqlite3_free(zErrMsg);
    }
#endif

#if SPATIALITE
	void *cache;
	cache = spatialite_alloc_connection();
    	spatialite_init_ex(db , cache, 0);


/* showing the SQLite version */
    printf ("SQLite version: %s\n", sqlite3_libversion());
/* showing the SpatiaLite version */
    printf ("SpatiaLite version: %d\n", spatialite_version());
    printf ("\n\n");
#endif

	//使用被wrap的原始接口
	rc = sqlite3_prepare(db, argv[2], strlen(argv[2]), &pStmt, NULL);
	printf("rc =%d\n", rc);
	if(rc != SQLITE_OK){
		fprintf(stderr, "SQL error1: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 1;
	}
	

	ret = sqlite3_column_count(pStmt);
	printf("BEFORE STEP col count= %d, rc = %d\n", ret, rc);

	i = 0;
	//打印col名称
	while(i < ret){
		printf("%20s \t %d \t %20s \t %d\n",
			sqlite3_column_name(pStmt, i),
			sqlite3_column_type(pStmt, i),
			sqlite3_column_decltype(pStmt, i),
			sqlite3_column_bytes(pStmt, i));
		i++;
	}
	printf("\n------ ------ ------ ------ ------------ ------ ------ ------\n");
	rc = sqlite3_step(pStmt);
	if(rc != SQLITE_ROW){
      		fprintf(stderr, "SQL error2: %s\n", zErrMsg);
      		sqlite3_free(zErrMsg);
		return 1;
	}
	//开始读取每一行
	while(rc == SQLITE_ROW){
		
		i = 0;row_count++;
		for(j=0;j< ret;j++){
			printf("%s ", sqlite3_column_text(pStmt,j));
		}
		printf("\n");
		#if 0
		printf("%d, %s, %s, %s\n",
			sqlite3_column_int(pStmt, 0),
			sqlite3_column_text(pStmt, 1),
			sqlite3_column_text(pStmt, 2),
			sqlite3_column_text(pStmt, 3));
		#endif
		rc = sqlite3_step(pStmt);
	}
	printf("row_count=%d\n ", row_count);
	sqlite3_finalize(pStmt);

	db_search(db, "管理组");
    sqlite3_close(db);
  } 

