/*
 * 处理实时公交线路的设置,查询等,为核心业务
 */
#include "web_app_api.h"
#include "rq_head.h"
#include "sys_library.h"
#include "db_bus_table.h"
#include "json.h"

#define BUS_REALTIME_BUFF_SIZE 1024* 4

#ifndef DEMO_TEST
#define DEMO_TEST 0
#endif

PRIVATE int process_bus_realtime_gps_point_req(Webs *wp);


/* 公交实时信息处理 */
PUBLIC void action_bus_realtime_line_process(Webs *wp, char *path, char *query){
	char *rq_type,*act;
	int ret = 0;
	rq_type = websGetVar(wp, "rq_type", "");
	if(strcmp(rq_type, "gps_point") == 0){
		process_bus_realtime_gps_point_req(wp);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, "error",  0);
	}
}

PRIVATE int process_bus_realtime_gps_point_req(Webs *wp){
	char *act;
	int real_size = 0;
	act = websGetVar(wp, "act", "");
	json_object *new_obj = NULL, *tmp_obj = NULL;
	char gps_buff[128];
	const char *ptr_json;
	//调用数据接口
	real_size = 1;
	if(1){
		memset(gps_buff, 0, sizeof(gps_buff));
		new_obj = json_object_new_array();
		tmp_obj = json_object_new_object();
		json_object_object_add(tmp_obj, "lng", json_object_new_double(104.064856));
		json_object_object_add(tmp_obj, "lat", json_object_new_double(30.67858));
		json_object_array_add(new_obj, tmp_obj);
		snprintf(gps_buff,128,"%s", json_object_to_json_string(new_obj));
	}else{
	}
	
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		
		process_web_write_response(wp , HTTP_CODE_OK, gps_buff , 0);
		json_object_put(new_obj);
	}
	
	return RET_OK;
}


