/*
 * 处理公交的配置
 */
#include "web_app_api.h"
#include "rq_head.h"
#include "sys_library.h"
#include "db_bus_table.h"
#include "json.h"

#define BUS_LINE_INFO_BUFF_SIZE 1024* 10
#define BUS_NUM_BUFF_SIZE		32
#define BUS_STA_MX_COUNT		64


#ifndef DEMO_TEST
#define DEMO_TEST 0
#endif

static char bus_num[32]={'\0'};	//当前公交线路名,如K1,地铁1号线
static int bus_derection;	//当前公交的方向
static char bus_cur_line_info[BUS_LINE_INFO_BUFF_SIZE]={'\0'};
static int bus_sta_status[BUS_STA_MX_COUNT]={0};	//公交的站点状态集合
static int bus_cur_sta;		//公交当前的站点编号
static int bus_status;		//公交当前的行车状态
/*
 *		process request bus line config
 *		need get the line information from algorithm
 *		@return write json format data to page
 */
PRIVATE int process_bus_config_cur_line_req(Webs *wp);


PRIVATE int process_bus_config_sta_status_req(Webs *wp);






PUBLIC int  get_bus_config_bus_num(char *buff, int buf_size);
PUBLIC int  get_bus_config_bus_derection(char *buff, int buf_size);
PUBLIC int  get_bus_config_bus_cur_line_info(char *buff, int buf_size);
PUBLIC int  get_bus_config_bus_driving_data(char *buf, int size);


PUBLIC int set_bus_config_bus_num(char *buff);
PUBLIC int set_bus_config_bus_derection( int buff);

/*  	
 *		set or update the cur_line_info;
 *		it contains the bus stations and the bus already passed statsions
 *		return code;
 */
PRIVATE int set_bus_config_bus_cur_line_info();
PRIVATE int update_bus_config_bus_cur_line_info();

/* 
 * 设置行车的状态
 * @sta_req 站点的序号
 * @status 此时的状态
 */
PUBLIC int set_bus_config_bus_driving_data(int sta_req, int status);



PUBLIC void action_bus_config_process(Webs *wp, char *path, char *query){
	char *rq_type;
	int ret = 0;
	rq_type = websGetVar(wp, "rq_type", "");
      
	if(strcmp(rq_type, BUS_CONFIG_CUR_LINE) == 0){
		ret = process_bus_config_cur_line_req(wp);
	}else if(strcmp(rq_type, BUS_CONFIG_STA_STATUS) == 0) {
		ret = process_bus_config_sta_status_req(wp);
	}else{
		process_web_write_response(wp, HTTP_CODE_OK, "no param" , 0);
	}
	
}		

PRIVATE int process_bus_config_cur_line_req(Webs *wp){
	char *line_name,*line_direction,*act;
	act = websGetVar(wp, "act", "");
	int ret = RET_OK;
	if(strcmp(act, "set") == 0){
		line_name = websGetVar(wp, "line_name", "");
		line_direction = websGetVar(wp, "line_direction", "");
		ret = set_bus_config_bus_num(line_name);
		ret = set_bus_config_bus_derection( atoi(line_direction) );
		ret = update_bus_config_bus_cur_line_info();
		if(ret == RET_OK){
			process_web_write_response(wp, HTTP_CODE_OK, "ok" , 0);
		}else{
			process_web_write_response(wp, HTTP_CODE_OK, "error" , 0);
		}
		
	}else if(strcmp(act, "get") == 0){
		if(strlen(bus_cur_line_info) < 10){
			process_web_write_response(wp, HTTP_CODE_OK, "[]" , 0);
		}else{
			process_web_write_response(wp, HTTP_CODE_OK, bus_cur_line_info , 0);
		}
		
	}else{
		process_web_write_response(wp, HTTP_CODE_OK, "param error" , 0);
	}
}


PRIVATE int process_bus_config_sta_status_req(Webs *wp){
	char *seq, *status, *act;
	act = websGetVar(wp, "act", "");
	int ret = RET_OK;
	char sta_buff[2048];
	if(strcmp(act, "set") == 0){
		seq = websGetVar(wp, "seq", "");
		status = websGetVar(wp, "status", "");
		ret = set_bus_config_bus_driving_data(atoi(seq), atoi(status));
		if(ret == RET_OK){
			process_web_write_response(wp, HTTP_CODE_OK, "ok" , 0);
		}else{
			process_web_write_response(wp, HTTP_CODE_OK, "error" , 0);
		}
		
	}else if(strcmp(act, "get") == 0){
		
		memset(sta_buff, 0, sizeof(sta_buff));
		ret = get_bus_config_bus_driving_data(sta_buff, 2048);
		if( (strlen(sta_buff) < 2) || (ret !=RET_OK)){
			process_web_write_response(wp, HTTP_CODE_OK, "[]" , 0);
		}else{
			process_web_write_response(wp, HTTP_CODE_OK, sta_buff , 0);
		}
	}else{
		process_web_write_response(wp, HTTP_CODE_OK, "param error" , 0);
	}
}


PUBLIC int set_bus_config_bus_num(char *buff){
	if( NULL == buff){
		return RET_ERROR;
	}
	snprintf(bus_num, BUS_NUM_BUFF_SIZE, "%s", buff);
	return RET_OK;
}
PUBLIC int set_bus_config_bus_derection(int buff){
	if( buff <=0){
		return RET_ERROR;
	}
	bus_derection = buff;
	return RET_OK;
}

PUBLIC int  get_bus_config_bus_cur_line_info(char *buff, int buf_size){
	if( (NULL == buff) || (buf_size < BUS_LINE_INFO_BUFF_SIZE)){
		return RET_ERROR;
	}
	snprintf(buff, buf_size, "%s", bus_cur_line_info);
	return RET_OK;
}

/* 设置bus当前线路的信息，可以被更新
 * 此buff被更新: 1.当前线路改变
 */
PRIVATE int set_bus_config_bus_cur_line_info(){
	
	if(TEST_PRA_STR_NOT_VOLID(bus_num)){
		WEB_STRACE("param error");
		return RET_ERROR;
	}
	int ret_out = 0, ret_in=0, i = 0, ret;
	bus_line_info_t *info =NULL, *tmp_info = NULL, null_line;
	v_line_sta_info_t *out_all =NULL, *out_tmp =NULL,cond;
	v_line_sta_info_t *in_all =NULL, *in_tmp =NULL;
	int t_flag = 0;
	#define NOT_FOUND_LINE 	(1 << 0)
	#define NOT_FOUND_OUT	(1 << 1 )
	#define NOT_FOUND_IN	(1 << 2 )
	memset(&cond, 0, sizeof(&cond));
	
	WEB_STRACE("cur line name is %s\n", bus_num);
	//获取线路信息
	if( (ret = db_bus_line_info_findby_line_name(bus_num, &info, 1, SQL_SEARCH_EQUAL)) <=0 ){
		//使用模糊查询
		if( (ret = db_bus_line_info_findby_line_name(bus_num, &info, 1, SQL_SEARCH_FUZZY)) <=0 ){
			WEB_STRACE("db error return %d,please try use baidu",ret);
			//不再返回,而是单纯的保存线路和方向,用于后续直接使用百度数据
			//return RET_ERROR;
			t_flag |= NOT_FOUND_LINE;
			memset(&null_line, 0, sizeof(bus_line_info_t));
			info = &null_line;
		}
	}
	
	if(t_flag & NOT_FOUND_LINE){
		t_flag |= NOT_FOUND_OUT;
		t_flag |= NOT_FOUND_IN;
	}else{
		cond.id= info->id;
		cond.kind_id = KIND_OUTBOUND;
		cond.line_type_id = 0;
		//获取去程信息
		if( (ret_out = db_v_line_sta_info_findby_condition(&cond, &out_all, 1, SQL_SEARCH_EQUAL)) <=0 ){
			WEB_STRACE("db error return %d",ret);
			//return RET_ERROR;
			t_flag |= NOT_FOUND_OUT;
		}
		
		cond.id= info->id;
		cond.kind_id = KIND_INBOUND;
		cond.line_type_id = 0;
		//获取回程信息
		if( (ret_in = db_v_line_sta_info_findby_condition(&cond, &in_all, 1, SQL_SEARCH_EQUAL)) <=0 ){
			WEB_STRACE("db error return %d",ret);
			//return RET_ERROR;
			t_flag |= NOT_FOUND_IN;
		}
	}
	
	
	
	json_object *new_obj = NULL, *sta_obj = NULL;
	json_object *sta_out = NULL, *sta_in =NULL;
	json_object *tmp_sta=NULL, *tmp_sta2;
	
	sta_out = json_object_new_array();
	sta_in = json_object_new_array();
	new_obj = json_object_new_object();
	
	
	json_object_object_add(new_obj, "line_name", json_object_new_string(bus_num));
	json_object_object_add(new_obj, "cur_dec", json_object_new_int(bus_derection));

	json_object_object_add(new_obj, "line_type", json_object_new_string(info->line_type));
	json_object_object_add(new_obj, "line_out", json_object_new_string(info->out_sta));
	json_object_object_add(new_obj, "line_in", json_object_new_string(info->in_sta));
	json_object_object_add(new_obj, "line_out_time", json_object_new_string(info->outbound));
	json_object_object_add(new_obj, "line_in_time", json_object_new_string(info->inbound));
	json_object_object_add(new_obj, "line_ticket", json_object_new_string(info->ticket));
	json_object_object_add(new_obj, "line_bus_type", json_object_new_string(info->bus_type));
	json_object_object_add(new_obj, "out_sta", json_object_new_string(info->out_sta));
	json_object_object_add(new_obj, "in_sta", json_object_new_string(info->in_sta));
	json_object_object_add(new_obj, "line_company", json_object_new_string(info->company));
	if(info->line_type_id == KIND_LOOP){
		json_object_object_add(new_obj, "line_loop", json_object_new_boolean(TRUE));
	}else{
		json_object_object_add(new_obj, "line_loop", json_object_new_boolean(FALSE));
	}


	json_object_object_add(new_obj, "line_active", json_object_new_string(info->status));
	
	
#if 1
	if( !(t_flag & NOT_FOUND_IN ) ){
		
		for (in_tmp = in_all; ret_in > 0; ret_in --, in_tmp ++){
			tmp_sta2 = json_object_new_object();
			json_object_object_add(tmp_sta2, "id", json_object_new_int(in_tmp->seq));
			json_object_object_add(tmp_sta2, "name", json_object_new_string(in_tmp->sta_name));
			json_object_object_add(tmp_sta2, "lng", json_object_new_string(in_tmp->st_lng));
			json_object_object_add(tmp_sta2, "lat", json_object_new_string(in_tmp->st_lat));
			json_object_array_add(sta_in, tmp_sta2);
		}
	}
	json_object_object_add(new_obj, "line_in", sta_in);
#endif
	if( !(t_flag & NOT_FOUND_OUT ) ){
		for (out_tmp = out_all; ret_out > 0; ret_out --, out_tmp ++){
			tmp_sta = json_object_new_object();
			json_object_object_add(tmp_sta, "id", json_object_new_int(out_tmp->seq));
			json_object_object_add(tmp_sta, "name", json_object_new_string(out_tmp->sta_name));
			json_object_object_add(tmp_sta, "lng", json_object_new_string(out_tmp->st_lng));
			json_object_object_add(tmp_sta, "lat", json_object_new_string(out_tmp->st_lat));
			json_object_array_add(sta_out, tmp_sta);
		}
	}
	json_object_object_add(new_obj, "line_out", sta_out);

	
	WEB_STRACE(" %s\n",json_object_to_json_string(new_obj));
	snprintf(bus_cur_line_info, BUS_LINE_INFO_BUFF_SIZE, "[%s]",json_object_to_json_string(new_obj));
	
	
	ret = json_object_put(new_obj);
	if( !(t_flag & NOT_FOUND_LINE ) ){
		db_free(info);
	}
	if( !(t_flag & NOT_FOUND_OUT ) ){
		db_free(out_all);
	}
	if( !(t_flag & NOT_FOUND_IN ) ){
		db_free(in_all);
	}
	
	WEB_STRACE("ret:%d ,info NULL: %d, sta_in :%d, sta_out : %d, tmp_sta:%d\n", 
		ret,
		(new_obj == NULL)? 1:0,
		(sta_in == NULL) ?1:0,
		(sta_out == NULL) ?1:0,
		(tmp_sta == NULL) ?1:0);
	return RET_OK;
	
}

PRIVATE int update_bus_config_bus_cur_line_info(){
	return set_bus_config_bus_cur_line_info();
}

PUBLIC int set_bus_config_bus_driving_data(int sta_req, int status){
	if( (sta_req <0) || (status <= 0) ){
		return RET_ERROR;
	}
	
	if( (status >= BUS_STATUS_MAX) || (sta_req >= BUS_STA_MX_COUNT) ){
		return RET_ERROR;
	}
	bus_drv_status = BUS_STATUS_IN_STATION;
	bus_cur_sta = sta_req;
	switch(status){
		case BUS_STATUS_IN_STATION:
			bus_sta_status[sta_req] = STA_IN_STA;
			break;
		case BUS_STATUS_INBOUND:
			bus_sta_status[sta_req] = STA_INBOUND;
			if(sta_req - 1 >=0){
				bus_sta_status[sta_req-1] = STA_PASSED;
			}
			break;
		case BUS_STATUS_OUTBOUND:
			bus_sta_status[sta_req] = STA_OUTBOUND;
			if(sta_req + 1 <= BUS_STA_MX_COUNT){
				bus_sta_status[sta_req + 1] = STA_INBOUND;
			}
			break;
		case BUS_STATUS_TERMINAL:
			memset(bus_sta_status, STA_PASSED, BUS_STA_MX_COUNT);
			break;
		case BUS_STATUS_BEGIN:
			memset(bus_sta_status, STA_NOT_REACH, BUS_STA_MX_COUNT);
			break;
	}
	return RET_OK;
}

PUBLIC int  get_bus_config_bus_driving_data(char *buf, int size){
	int sta_count,ret;
	if( size <= 1024){
		return RET_ERROR;
	}
	json_object *new_obj = NULL, *arr_obj=NULL;
	new_obj = json_object_new_array();
	
	for (sta_count = 0; sta_count < BUS_STA_MX_COUNT ; sta_count ++){
		arr_obj = json_object_new_object();
		json_object_object_add(arr_obj, "id", json_object_new_int(sta_count));

		json_object_object_add(arr_obj, "st", json_object_new_int(bus_sta_status[sta_count] ));
		json_object_array_add(new_obj, arr_obj);
	}
	snprintf(buf, size, "%s",json_object_to_json_string(new_obj));
	ret = json_object_put(new_obj);
	WEB_STRACE(" ret:%d ,info NULL: %d, arr_obj :%d\n", ret,(new_obj == NULL)? 1:0,
		(arr_obj == NULL) ?1:0);
	
	return RET_OK;
}

