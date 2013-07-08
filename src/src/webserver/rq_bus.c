#include "web_app_api.h"
#include "rq_head.h"
#include "sys_library.h"
#include "db_bus_table.h"
#include "json.h"

#define BUS_BUFF_SIZE 1024* 4

#ifndef DEMO_TEST
#define DEMO_TEST 0
#endif


/*
 *		process request bus line data
 *		need get the line information from algorithm
 *		@return write json format data to page
 */
PRIVATE int process_bus_line_req(Webs *wp);

/*
 *		process request bus sta data
 *		need get the line information from algorithm
 *		@return write json format data to page
 */
PRIVATE int process_bus_sta_req(Webs *wp);

/*
 *		process request bus ride data
 *		need get the line information from algorithm
 *		@return write json format data to page
 */
PRIVATE int process_bus_ride_req(Webs *wp);

/*
 *		process request bus real time 
 *		need get the line information from algorithm
 *		@return write json format data to page
 */
PRIVATE int process_bus_real_time_req(Webs *wp);


/* 测试接口 */
PRIVATE int get_test_line_info(char *name , char *pstr, int size);
PRIVATE int get_test_sta_info(char *name , char *pstr, int size);
PRIVATE int get_test_ride_info(char *start, char *end , char *pstr, int size);

/* 真实数据查找 */
PRIVATE int get_line_info_from_db(char *name , char *pstr, int size);
PRIVATE int get_line_info_fuzzy_from_db(char *name , char *pstr, int size);

PRIVATE int get_sta_info_from_db(char *name , char *pstr, int size);
PRIVATE int get_sta_info_fuzzy_from_db(char *name , char *pstr, int size);

PRIVATE int get_ride_info_from_db(char *start, char *end , char *pstr, int size);
PRIVATE int get_real_time_coordinate_from_db(char *name, char *pstr, int size);



PUBLIC void action_bus_base_process(Webs *wp, char *path, char *query){
	char *bus_line, *bus_station, *search_type, *bus_start_point, *bus_end_point;
	
	bus_line = websGetVar(wp, "bus_line", NULL);
       	bus_station = websGetVar(wp, "bus_station", NULL);
	search_type = websGetVar(wp, "search_type", NULL);
       	bus_start_point = websGetVar(wp, "bus_start_point", NULL);
	bus_end_point = websGetVar(wp, "bus_end_point", NULL);

	printf("%s\n",__FUNCTION__);
	if(strcmp(search_type, BUS_SEARCH_LINE) == 0){
		process_bus_line_req(wp);
	}else if(strcmp(search_type, BUS_SEARCH_STA) == 0){
		process_bus_sta_req(wp);
	}else if(strcmp(search_type, BUS_SEARCH_RIDE) == 0){
		process_bus_ride_req(wp);
	}else{
		process_web_write_response(wp, HTTP_CODE_OK, "[]" , 3);
	}
	
	printf( "{ \"bus_line\":\"%s\" , \
		\"bus_station\":\"%s\", \
		\"search_type\":\"%s\", \
		\"bus_start_point\":\"%s\", \
		\"bus_end_point\":\"%s\" }\n", 
		bus_line, bus_station, search_type, bus_start_point, bus_end_point);
}


PRIVATE int process_bus_line_req(Webs *wp){
	
	char *bus_line,*fuzzy, buff[BUS_BUFF_SIZE];
	int real_size = 0;
	
	bus_line = websGetVar(wp, "bus_line", NULL);
	fuzzy = websGetVar(wp, "fuzzy","0");
	//alog interface
	//get_line_info(bus_station, &prs);
	if(atoi(fuzzy) == SQL_SEARCH_FUZZY){
		real_size = get_line_info_fuzzy_from_db(bus_line, buff, BUS_BUFF_SIZE);
	}else{
		real_size = get_line_info_from_db(bus_line, buff, BUS_BUFF_SIZE);
	}
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, real_size);
	}
	return 1;
}

PRIVATE int process_bus_sta_req(Webs *wp){
	
	char *bus_station,*fuzzy, buff[BUS_BUFF_SIZE];
	int real_size = 0;
	
	bus_station = websGetVar(wp, "bus_station", NULL);
	fuzzy = websGetVar(wp, "fuzzy","0");
	//alog interface
	//get_sta_info(bus_station, &prs);

	
	if(atoi(fuzzy) == SQL_SEARCH_FUZZY){
		real_size = get_sta_info_fuzzy_from_db(bus_station, buff, BUS_BUFF_SIZE);
	}else{
		real_size = get_sta_info_from_db(bus_station, buff, BUS_BUFF_SIZE);
	}
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, real_size);
	}
}

PRIVATE int process_bus_ride_req(Webs *wp){
	
	char *bus_start_point, *bus_end_point, buff[BUS_BUFF_SIZE];
	int real_size = 0;
	
	bus_start_point = websGetVar(wp, "bus_start_point", NULL);
	bus_end_point = websGetVar(wp, "bus_end_point", NULL);
	
	real_size = get_test_ride_info(bus_start_point, bus_end_point, buff, BUS_BUFF_SIZE);
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, real_size);
	}
}

PRIVATE int process_bus_real_time_req(Webs *wp){
	char *line_name, buff[BUS_BUFF_SIZE];
	int real_size = 0;
	
	line_name = websGetVar(wp, "bus_line_name", NULL);
	real_size = get_real_time_coordinate_from_db(line_name, buff, BUS_BUFF_SIZE);
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, real_size);
	}
}



PRIVATE int get_test_line_info(char *name , char *pstr, int size){
	/* 根据name 找到对应的线路信息 */
	/* 根据线路ID找到OR视图的站点信息,获取2个方向的信息 */
	int i = 0;
	json_addstr2 (pstr, size, i,  "[{");
	json_addstrav (pstr, size, i,  "line_name", name); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "line_info", "06:30-21:00 <br> \
	"); json_addstr2 (pstr, size, i,  ",");
	
	json_addstrav2 (pstr, size, i,  "line_up", " "); json_addstr2 (pstr, size, i,  "[");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  "],");
	
	json_addstrav2 (pstr, size, i,  "line_down", " "); json_addstr2 (pstr, size, i,  "[");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  ",");
	json_addstr (pstr, size, i,  "??????");json_addstr2 (pstr, size, i,  "],");

	json_addstrav2(pstr, size, i,  "line_loop", "false"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav2(pstr, size, i,  "line_active", "true");
	json_addstr2 (pstr, size, i,  "}]");
	return 1;
}
PRIVATE int get_test_sta_info(char *name , char *pstr, int size){
	int i = 0;
	json_addstr2 (pstr, size, i, "[{");
	json_addstrav (pstr, size, i,  "sta_name", name); json_addstr2 (pstr, size, i,  ",");
	json_addintav(pstr, size, i,  "sta_count", 10); json_addstr2 (pstr, size, i,  ",");

	
	json_addstrav2 (pstr, size, i,  "sta_line_arr", " "); json_addstr2 (pstr, size, i,  "[");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "<br>06:30-21:30 ");json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "<br>06:30-21:30 "); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "<br>06:30-21:30 ");json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "<br>06:30-21:30 ");json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "<br>06:30-21:30 "); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "],");
	
	json_addstrav2(pstr, size, i,  "sta_active", "true");
	json_addstr2 (pstr, size, i,  "}]");
	return 1;
}
PRIVATE int get_test_ride_info(char *start, char *end , char *pstr, int size){
	return 0;
}

PRIVATE int get_line_info_from_db(char *name , char *pstr, int size){
	
		if(TEST_PRA_STR_NOT_VOLID(name)){
			WEB_STRACE("param error");
			return RET_ERROR;
		}
		int ret_out = 0, ret_in=0, i = 0, ret;
		bus_line_info_t *info =NULL, *tmp_info = NULL;
		v_line_sta_info_t *out_all =NULL, *out_tmp =NULL,cond;
		v_line_sta_info_t *in_all =NULL, *in_tmp =NULL;
		memset(&cond, 0, sizeof(&cond));
		
		printf("your line name is %s\n", name);
		//获取线路信息
		if( (ret = db_bus_line_info_findby_line_name(name, &info, 1, SQL_SEARCH_EQUAL)) <=0 ){
			WEB_STRACE("db error return %d",ret);
			return RET_ERROR;
		}
		
		cond.id= info->id;
		cond.kind_id = KIND_OUTBOUND;
		//获取去程信息
		if( (ret_out = db_v_line_sta_info_findby_condition(&cond, &out_all, 1, SQL_SEARCH_EQUAL)) <=0 ){
			WEB_STRACE("db error return %d",ret);
			return RET_ERROR;
		}
		
		cond.id= info->id;
		cond.kind_id = KIND_INBOUND;
		//获取回程信息
		if( (ret_in = db_v_line_sta_info_findby_condition(&cond, &in_all, 1, SQL_SEARCH_EQUAL)) <=0 ){
			WEB_STRACE("db error return %d",ret);
			return RET_ERROR;
		}
		
		
		json_object *new_obj = NULL, *sta_obj = NULL;
		json_object *sta_out = NULL, *sta_in =NULL;
		
		sta_out = json_object_new_array();
		sta_in = json_object_new_array();
		new_obj = json_object_new_object();
		
		
		json_object_object_add(new_obj, "line_name", json_object_new_string(name));
		json_object_object_add(new_obj, "line_type", json_object_new_string(info->line_type));
		
		json_object_object_add(new_obj, "line_out", json_object_new_string(info->out_sta));
		json_object_object_add(new_obj, "line_in", json_object_new_string(info->in_sta));
		json_object_object_add(new_obj, "line_out_time", json_object_new_string(info->outbound));
		json_object_object_add(new_obj, "line_in_time", json_object_new_string(info->inbound));
		
		json_object_object_add(new_obj, "line_ticket", json_object_new_string(info->ticket));
		json_object_object_add(new_obj, "line_bus_type", json_object_new_string(info->bus_type));
		json_object_object_add(new_obj, "line_price", json_object_new_string(info->bus_price));
		json_object_object_add(new_obj, "line_company", json_object_new_string(info->company));
		json_object_object_add(new_obj, "line_loop", json_object_new_boolean(FALSE));
		
		json_object_object_add(new_obj, "line_active", json_object_new_boolean(TRUE));
		
	
		for ((out_tmp) = (out_all); (ret_out) > 0; (ret_out) --, (out_tmp) ++){
			json_object_array_add( sta_out,json_object_new_string(out_tmp->sta_name));
		}
		for ((in_tmp) = (in_all); (ret_in) > 0; (ret_in) --, (in_tmp) ++){
			json_object_array_add( sta_in,json_object_new_string(in_tmp->sta_name));
		}
		
		json_object_object_add(new_obj, "line_up", sta_out);
		json_object_object_add(new_obj, "line_down", sta_in);
		i = json_object_object_length(new_obj);
		
		printf("%s\n",json_object_to_json_string(new_obj));
		snprintf(pstr,size,"[%s]",json_object_to_json_string(new_obj));
		
		//json_object_put(sta_out);
		//json_object_put(sta_in);
		
		json_object_put(new_obj);
		
		db_free(info);
		return i+2;
	
}


PRIVATE int get_line_info_fuzzy_from_db(char *name , char *pstr, int size){
	if(TEST_PRA_STR_NOT_VOLID(name)){
		WEB_STRACE("param error");
		return RET_ERROR;
	}
	int  i = 0, ret, p_ret;
	bus_line_info_t *info =NULL, *tmp_info = NULL;
	json_object *outer_arr = NULL, *line_obj = NULL;
	json_object *line_obj_arr = NULL;
	printf("your line name is %s\n", name);
	//获取线路信息
	if( (ret = db_bus_line_info_findby_line_name(name, &info, 1, SQL_SEARCH_FUZZY)) <=0 ){
		WEB_STRACE("db error return %d",ret);
		return RET_ERROR;
	}
	line_obj_arr = json_object_new_array();
	MRULES_INIT_WALK_SAFE(info, ret, p_ret, tmp_info){
		line_obj = json_object_new_object();
		json_object_object_add(line_obj, "line_id", json_object_new_int(tmp_info->id));
		json_object_object_add(line_obj, "line_name", json_object_new_string(tmp_info->name));
		json_object_object_add(line_obj, "line_status", json_object_new_string(tmp_info->status));
		json_object_object_add(line_obj, "line_type", json_object_new_string(tmp_info->line_type));
		json_object_object_add(line_obj, "line_out_sta", json_object_new_string(tmp_info->out_sta));
		json_object_array_add(line_obj_arr, line_obj);
	}
	i = json_object_object_length(line_obj_arr);
		
	printf("Data:  %s\n",json_object_to_json_string(line_obj_arr));
	snprintf(pstr,size,"%s",json_object_to_json_string(line_obj_arr));
	WEB_STRACE();
	json_object_put(line_obj_arr);
	WEB_STRACE();
	db_free(info);
	WEB_STRACE();
	return i;
}


/* 根据输入的站点名称查找线路 */
PRIVATE int get_sta_info_from_db(char *name , char *pstr, int size){
	
	if(TEST_PRA_STR_NOT_VOLID(name)){
		WEB_STRACE("param error");
		return RET_ERROR;
	}
	int ret = 0, i = 0;
	bus_line_info_t *info =NULL, *tmp_info = NULL;
	printf("your station name is %s\n", name);
	if( (ret = db_bus_line_info_findby_sta_name(name, &info, 1, SQL_SEARCH_EQUAL)) <=0 ){
		WEB_STRACE("db error return %d",ret);
		return RET_ERROR;
	}
	printf("ret = %d, line=%s\n",ret,info->name);
	
	json_object *new_obj = NULL, *sta_obj = NULL;
	json_object *sta_line_arr = NULL;
	
	sta_line_arr = json_object_new_array();
	new_obj = json_object_new_object();
	
	
	json_object_object_add(new_obj, "sta_name", json_object_new_string(name));
	
	json_object_object_add(new_obj, "sta_active", json_object_new_boolean(TRUE));
	

	for ((tmp_info) = (info); (ret) > 0; (ret) --, (tmp_info) ++){
		sta_obj = json_object_new_object();
		json_object_object_add(sta_obj, "name", json_object_new_string(info->name));
		WEB_STRACE();
		json_object_object_add(sta_obj, "info", json_object_new_string(info->out_sta));
		WEB_STRACE();
		json_object_object_add(sta_obj, "time", json_object_new_string(info->outbound));
		WEB_STRACE();
		json_object_object_add(sta_obj, "distance", json_object_new_int(1));
		json_object_array_add(sta_line_arr,sta_obj);
	}
	
	json_object_object_add(new_obj, "sta_line_arr", sta_line_arr);
	i = json_object_object_length(new_obj);
	
	printf("%s\n",json_object_to_json_string(new_obj));
	snprintf(pstr,size,"[%s]",json_object_to_json_string(new_obj));
	
	json_object_put(sta_line_arr);
	
	json_object_put(new_obj);
	
	db_free(info);
	return i+2;
}

PRIVATE int get_sta_info_fuzzy_from_db(char *name , char *pstr, int size){
	if(TEST_PRA_STR_NOT_VOLID(name)){
		WEB_STRACE("param error");
		return RET_ERROR;
	}
	int  i = 0, ret, p_ret;
	v_line_sta_info_t *info =NULL, *tmp_info = NULL;
	json_object *outer_arr = NULL, *line_obj = NULL;
	json_object *line_obj_arr = NULL;
	printf("your line name is %s\n", name);
	//获取线路信息
	if( (ret = db_v_line_sta_info_findby_sta_name_fuzzy(name, &info, 1)) <=0 ){
		WEB_STRACE("db error return %d",ret);
		return RET_ERROR;
	}
	line_obj_arr = json_object_new_array();
	MRULES_INIT_WALK_SAFE(info, ret, p_ret, tmp_info){
		line_obj = json_object_new_object();
		json_object_object_add(line_obj, "sta_name", json_object_new_string(tmp_info->sta_name));
		json_object_array_add(line_obj_arr, line_obj);
	}
	i = json_object_object_length(line_obj_arr);
		
	printf("Data:  %s\n",json_object_to_json_string(line_obj_arr));
	snprintf(pstr,size,"%s",json_object_to_json_string(line_obj_arr));
	WEB_STRACE();
	json_object_put(line_obj_arr);
	WEB_STRACE();
	db_free(info);
	WEB_STRACE();
	return i;
}


/* 实时公交,根据线路名查询坐标,然后比对坐标返回站点信息 */
PRIVATE int get_real_time_coordinate_from_db(char *name, char *pstr, int size){

}

