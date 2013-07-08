#ifndef BUS_TABLE_H_
#define BUS_TABLE_H_


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
}bus_line_t;

/* 线路 状态表 */
typedef struct{
	int s_id;
	char s_name[40];
	char s_desc[128];
};

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
	char ticket[64];
	char kind[40];
	int kind_id;
	int sta_status;
	int seq;
}v_line_sta_info_t;


int db_bus_line_info_findby_line_name(char *name, bus_line_info_t **bus_info, int load);


#endif
