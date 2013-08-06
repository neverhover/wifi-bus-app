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



PRIVATE int get_test_ride_info(char *start, char *end , char **pstr);

/* 真实数据查找 */
PRIVATE int get_line_info_from_db(char *name , char **pstr);
PRIVATE int get_line_info_fuzzy_from_db(char *name , char **pstr);

PRIVATE int get_sta_info_from_db(char *name , char **pstr);
PRIVATE int get_sta_info_fuzzy_from_db(char *name , char **pstr);

PRIVATE int get_ride_info_from_db(char *start, char *end , char **pstr);
PRIVATE int get_real_time_coordinate_from_db(char *name, char **pstr);



PUBLIC void action_bus_base_process(Webs *wp, char *path, char *query){
	char *bus_line, *bus_station, *search_type, *bus_start_point, *bus_end_point;
	
	bus_line = websGetVar(wp, "bus_line", "");
       	bus_station = websGetVar(wp, "bus_station", "");
	search_type = websGetVar(wp, "search_type", "");
       	bus_start_point = websGetVar(wp, "bus_start_point", "");
	bus_end_point = websGetVar(wp, "bus_end_point", "");

	WEB_STRACE("%s\n",__FUNCTION__);
	if(strcmp(search_type, BUS_SEARCH_LINE) == 0){
		process_bus_line_req(wp);
	}else if(strcmp(search_type, BUS_SEARCH_STA) == 0){
		process_bus_sta_req(wp);
	}else if(strcmp(search_type, BUS_SEARCH_RIDE) == 0){
		process_bus_ride_req(wp);
	}else{
		process_web_write_response(wp, HTTP_CODE_OK, "[]" , 3);
	}
#if 0	
	printf( "{ \"bus_line\":\"%s\" , \
		\"bus_station\":\"%s\", \
		\"search_type\":\"%s\", \
		\"bus_start_point\":\"%s\", \
		\"bus_end_point\":\"%s\" }\n", 
		bus_line, bus_station, search_type, bus_start_point, bus_end_point);
#endif
}


PRIVATE int process_bus_line_req(Webs *wp){
	
	char *bus_line,*fuzzy, *buff=NULL;
	int real_size = 0;
	
	bus_line = websGetVar(wp, "bus_line", "");
	fuzzy = websGetVar(wp, "fuzzy","0");
	//alog interface
	//get_line_info(bus_station, &prs);
	if(atoi(fuzzy) == SQL_SEARCH_FUZZY){
		real_size = get_line_info_fuzzy_from_db(bus_line, &buff);
	}else{
		real_size = get_line_info_from_db(bus_line, &buff);
	}
	
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, 0);
		JSON_STR_FREE(buff);
	}
	
	return RET_OK;
}

PRIVATE int process_bus_sta_req(Webs *wp){
	
	char *bus_station,*fuzzy, *buff=NULL;
	int real_size = 0;
	
	bus_station = websGetVar(wp, "bus_station", "");
	fuzzy = websGetVar(wp, "fuzzy","0");
	//alog interface
	//get_sta_info(bus_station, &prs);

	
	if(atoi(fuzzy) == SQL_SEARCH_FUZZY){
		real_size = get_sta_info_fuzzy_from_db(bus_station, &buff);
	}else{
		real_size = get_sta_info_from_db(bus_station, &buff);
	}
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, 0);
		JSON_STR_FREE(buff);
	}
	
	return RET_OK;
}

PRIVATE int process_bus_ride_req(Webs *wp){
	
	char *bus_start_point, *bus_end_point, *buff=NULL;
	int real_size = 0;
	
	bus_start_point = websGetVar(wp, "bus_start_point", "");
	bus_end_point = websGetVar(wp, "bus_end_point", "");
	
	real_size = get_test_ride_info(bus_start_point, bus_end_point, &buff);
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, real_size);
		JSON_STR_FREE(buff);
	}
	return RET_OK;
}

PRIVATE int process_bus_real_time_req(Webs *wp){
	char *line_name, *buff=NULL;
	int real_size = 0;
	
	line_name = websGetVar(wp, "bus_line_name", "");
	real_size = get_real_time_coordinate_from_db(line_name, &buff);
	if(real_size <= 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, real_size);
		JSON_STR_FREE(buff);
	}
	return RET_OK;
}


PRIVATE int get_test_ride_info(char *start, char *end , char **pstr){
	return 0;
}


PRIVATE int get_line_info_from_db(char *name , char **pstr){
	
	if(TEST_PRA_STR_NOT_VOLID(name)){
		WEB_STRACE("param error");
		return RET_ERROR;
	}
	int ret_out = 0, ret_in=0, i = 0, ret;
	bus_line_info_t *info =NULL, *tmp_info = NULL;
	v_line_sta_info_t *out_all =NULL, *out_tmp =NULL,cond;
	v_line_sta_info_t *in_all =NULL, *in_tmp =NULL;
	const char *tmp_str = NULL;
	char  *p_pstr =NULL;
	
	memset(&cond, 0, sizeof(&cond));
	
	WEB_STRACE("your line name is %s\n", name);
	//获取线路信息
	if( (ret = db_bus_line_info_findby_line_name(name, &info, 1, SQL_SEARCH_EQUAL)) <=0 ){
		//使用模糊查询
		if( (ret = db_bus_line_info_findby_line_name(name, &info, 1, SQL_SEARCH_FUZZY)) <=0 ){
			WEB_STRACE("db error return %d",ret);
			return RET_ERROR;
		}
	}
	
	
	cond.id= info->id;
	cond.kind_id = KIND_OUTBOUND;
	cond.line_type_id = 0;
	//获取去程信息
	if( (ret_out = db_v_line_sta_info_findby_condition(&cond, &out_all, 1, SQL_SEARCH_EQUAL)) <=0 ){
		WEB_STRACE("db error return %d",ret);
		return RET_ERROR;
	}
	
	cond.id= info->id;
	cond.kind_id = KIND_INBOUND;
	cond.line_type_id = 0;
	//获取回程信息
	if( (ret_in = db_v_line_sta_info_findby_condition(&cond, &in_all, 1, SQL_SEARCH_EQUAL)) <=0 ){
		WEB_STRACE("db error return %d",ret);
		return RET_ERROR;
	}
	
	
	json_object *new_obj = NULL, *sta_obj = NULL,*outer_arr=NULL;
	json_object *sta_out = NULL, *sta_in =NULL;
	
	sta_out = json_object_new_array();
	sta_in = json_object_new_array();
	new_obj = json_object_new_object();
	outer_arr = json_object_new_array();
	
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
	//json_object_object_add(new_obj, "line_loop", json_object_new_boolean(FALSE));
	if(info->line_type_id == KIND_LOOP){
		json_object_object_add(new_obj, "line_loop", json_object_new_boolean(TRUE));
	}else{
		json_object_object_add(new_obj, "line_loop", json_object_new_boolean(FALSE));
	}
	json_object_object_add(new_obj, "line_active", json_object_new_boolean(TRUE));
	

	for (out_tmp = out_all; ret_out > 0; ret_out --, out_tmp ++){
		json_object_array_add( sta_out,json_object_new_string(out_tmp->sta_name));
	}
	for (in_tmp = in_all; ret_in > 0; ret_in --, in_tmp ++){
		json_object_array_add( sta_in,json_object_new_string(in_tmp->sta_name));
	}
	
	json_object_object_add(new_obj, "line_up", sta_out);
	json_object_object_add(new_obj, "line_down", sta_in);
	json_object_array_add(outer_arr, new_obj);
	
	WEB_STRACE("%s\n",json_object_to_json_string(outer_arr));
	
	tmp_str = json_object_to_json_string(outer_arr);
	
	ret = strlen(tmp_str);
	
	*pstr = calloc( ret+1, sizeof(char) );
	
	p_pstr = *pstr;
	
	if(*pstr == NULL){
		
		ret = RET_ERROR;
	}else{
		
		memcpy(*pstr, tmp_str, ret);
		
		p_pstr[ret]= '\0';
		
	}
	
	json_object_put(new_obj);
	
	db_free(info);
	db_free(out_all);
	db_free(in_all);
	
	return ret;
	
}


PRIVATE int get_line_info_fuzzy_from_db(char *name , char **pstr){
	if(TEST_PRA_STR_NOT_VOLID(name)){
		WEB_STRACE("param error");
		return RET_ERROR;
	}
	int  ret, p_ret;
	bus_line_info_t *info =NULL, *tmp_info = NULL;
	json_object *outer_arr = NULL, *line_obj = NULL;
	json_object *line_obj_arr = NULL;
	const char *tmp_str = NULL;
	char  *p_pstr =NULL;
	
	WEB_STRACE("your line name is %s\n", name);
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
		
	WEB_STRACE("Data:  %s\n",json_object_to_json_string(line_obj_arr));
	
	tmp_str = json_object_to_json_string(line_obj_arr);
	ret = strlen(tmp_str);
	*pstr = calloc( ret+1, sizeof(char) );
	p_pstr = *pstr;
	if(*pstr == NULL){
		ret = RET_ERROR;
	}else{
		memcpy(*pstr, tmp_str, ret);
		p_pstr[ret]= '\0';
	}
	
	json_object_put(line_obj_arr);
	db_free(info);
	
	return ret;
}


/* 根据输入的站点名称查找线路 */
PRIVATE int get_sta_info_from_db(char *name , char **pstr){
	
	if(TEST_PRA_STR_NOT_VOLID(name)){
		WEB_STRACE("param error");
		return RET_ERROR;
	}
	int ret = 0;
	bus_line_info_t *info =NULL, *tmp_info = NULL;
	const char *tmp_str = NULL;
	char *p_pstr =NULL;
	
	WEB_STRACE("your station name is %s\n", name);
	if( (ret = db_bus_line_info_findby_sta_name(name, &info, 1, SQL_SEARCH_EQUAL)) <=0 ){
		WEB_STRACE("db error return %d",ret);
		return RET_ERROR;
	}
	WEB_STRACE("ret = %d, line=%s\n",ret,info->name);
	
	json_object *new_obj = NULL, *sta_obj = NULL, *outer_arr = NULL;
	json_object *sta_line_arr = NULL;
	
	sta_line_arr = json_object_new_array();
	new_obj = json_object_new_object();
	outer_arr = json_object_new_array();
	
	json_object_object_add(new_obj, "sta_name", json_object_new_string(name));
	
	json_object_object_add(new_obj, "sta_active", json_object_new_boolean(TRUE));
	

	for (tmp_info = info; ret > 0; ret --, tmp_info ++){
		sta_obj = json_object_new_object();
		json_object_object_add(sta_obj, "name", json_object_new_string(tmp_info->name));
		WEB_STRACE();
		json_object_object_add(sta_obj, "info", json_object_new_string(tmp_info->out_sta));
		WEB_STRACE();
		json_object_object_add(sta_obj, "time", json_object_new_string(tmp_info->outbound));
		WEB_STRACE();
		json_object_object_add(sta_obj, "distance", json_object_new_int(1));
		json_object_array_add(sta_line_arr,sta_obj);
	}
	
	json_object_object_add(new_obj, "sta_line_arr", sta_line_arr);
	json_object_array_add(outer_arr, new_obj);
	WEB_STRACE("%s\n",json_object_to_json_string(outer_arr));
	
	tmp_str = json_object_to_json_string(outer_arr);
	ret = strlen(tmp_str);
	*pstr = calloc( ret+1, sizeof(char) );
	p_pstr = *pstr;
	if(*pstr == NULL){
		ret = RET_ERROR;
	}else{
		memcpy(*pstr, tmp_str, ret);
		p_pstr[ret]= '\0';
	}
	
	json_object_put(sta_line_arr);
	json_object_put(new_obj);
	
	db_free(info);
	return ret;
}

PRIVATE int get_sta_info_fuzzy_from_db(char *name , char **pstr){
	if(TEST_PRA_STR_NOT_VOLID(name)){
		WEB_STRACE("param error");
		return RET_ERROR;
	}
	int   ret, p_ret;
	v_line_sta_info_t *info =NULL, *tmp_info = NULL;
	json_object *outer_arr = NULL, *line_obj = NULL;
	json_object *line_obj_arr = NULL;
	const char *tmp_str = NULL;
	char *p_pstr =NULL;
	
	WEB_STRACE("your line name is %s\n", name);
	//获取站点信息
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
		
	WEB_STRACE("Data:  %s\n",json_object_to_json_string(line_obj_arr));
	
	tmp_str = json_object_to_json_string(line_obj_arr);
	ret = strlen(tmp_str);
	*pstr = calloc( ret+1, sizeof(char) );
	p_pstr = *pstr;
	if(*pstr == NULL){
		ret = RET_ERROR;
	}else{
		memcpy(*pstr, tmp_str, ret);
		p_pstr[ret]= '\0';
	}

	json_object_put(line_obj_arr);
	db_free(info);
	
	return ret;
}


/* 实时公交,根据线路名查询坐标,然后比对坐标返回站点信息 */
PRIVATE int get_real_time_coordinate_from_db(char *name, char **pstr){

}

