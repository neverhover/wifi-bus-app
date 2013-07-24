#ifndef BUS_TABLE_H_
#define BUS_TABLE_H_


#define LOAD_DATA		1
#define LOAD_NONE		0
#define ERROR			-1
#define OK				0

enum {
	SQL_SEARCH_EQUAL = 0,
	SQL_SEARCH_FUZZY = 1,
	
};



/* 线路信息主表 */
typedef struct {
	int line_id;
	char line_name[40];
	int company_id;
	int bus_type;
	char bus_price[32];
	int ticket_type;
	char outbound_time[40];
	char inbound_time[40];
	int kind;
	int line_type;
	int bus_status;
	char outbound_sta[128];
	char inbount_sta[128];
}bus_line_t;

typedef struct{
	int st_id;
	char st_name[64];
	char st_lng[24];
	char st_lat[24];
	char st_status;
	char st_kind;
}station_t;

/* 线路信息 视图 */
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
	char out_sta[128];		/* 去程起始及结束站点 */
	char in_sta[128];		/* 回程起始及结束站点 */
}bus_line_info_t;


/* 站点信息 视图 */
typedef struct{
	int id;	//line_id
	char line_name[40];
	int line_type_id;
	char line_type[40];
	int sta_id;
	char sta_name[64];
	char st_lng[24];
	char st_lat[24];
	char kind[40];
	int kind_id;
	int sta_status;
	int seq;
}v_line_sta_info_t;


/* 视图线路信息查询,站点查询类接口 ************************/
int db_bus_line_info_findby_line_id( int id, bus_line_info_t **bus_info, int load, int fuzzy);

int db_bus_line_info_findby_line_name(char *name, bus_line_info_t **bus_info, int load, int fuzzy);

/* 根据 站点名 从 线路视图 查找数据 */
int db_bus_line_info_findby_sta_name(char *name, bus_line_info_t **bus_line_info, int load, int fuzzy);





/* 主表线路基本信息视图类接口 *****************************/

/* 根据 线路ID 从 线路站点视图 查找数据 */
int db_v_line_sta_info_findby_line_id(int id, v_line_sta_info_t **bus_line_info, int load, int fuzzy);

/* 根据 站点名 从 线路站点视图 查找数据 */
int db_v_line_sta_info_findby_sta_name(char *name, v_line_sta_info_t **bus_line_info, int load, int fuzzy);

/* 根据 条件 从 线路站点视图 查找数据 */
int db_v_line_sta_info_findby_condition(v_line_sta_info_t *cond, v_line_sta_info_t **bus_line_info, int load, int fuzzy);


#endif
