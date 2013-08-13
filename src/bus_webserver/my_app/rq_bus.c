#include "web_app_api.h"
#include "rq_head.h"
#include "sys_library.h"
#include "bus_utils.h"

#define BUS_BUFF_SIZE 1024* 4

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

PRIVATE int get_test_line_info(char *name , char *pstr, int size);
PRIVATE int get_test_sta_info(char *name , char *pstr, int size);
PRIVATE int get_test_ride_info(char *start, char *end , char *pstr, int size);




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
		\"bus_end_point\":\"%s\" }", 
		bus_line, bus_station, search_type, bus_start_point, bus_end_point);
}


PRIVATE int process_bus_line_req(Webs *wp){
	
	char *bus_line, buff[BUS_BUFF_SIZE];
	int real_size = 0;
	
	bus_line = websGetVar(wp, "bus_line", NULL);
	//alog interface
	//get_line_info(bus_station, &prs);
	real_size = get_test_line_info(bus_line, buff, BUS_BUFF_SIZE);
	if(real_size < 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, real_size);
	}
	return 1;
}

PRIVATE int process_bus_sta_req(Webs *wp){
	
	char *bus_station, buff[BUS_BUFF_SIZE];
	int real_size = 0;
	
	bus_station = websGetVar(wp, "bus_station", NULL);
	//alog interface
	//get_sta_info(bus_station, &prs);
	real_size = get_test_sta_info(bus_station, buff, BUS_BUFF_SIZE);
	if(real_size < 0){
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


PRIVATE int get_test_line_info(char *name , char *pstr, int size){
	/* 根据name 找到对应的线路信息 */
	/* 根据线路ID找到OR视图的站点信息,获取2个方向的信息 */
	int line_id = 0;

	
	
	int i = 0;
	json_addstr2 (pstr, size, i,  "[{");
	json_addstrav (pstr, size, i,  "line_name", name); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "line_info", "???? ??????--???<br>??????06:30-21:00 <br>????????06:30-20:30??06:30-21:00??07:00-21:00??07:00-20:30 <br>???? ???? ???? ???2? \
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
	json_addstrav (pstr, size, i,  "info", "??????-????<br>06:30-21:30 ");json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "??????-????<br>06:30-21:30 "); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "??????-????<br>06:30-21:30 ");json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "??????-????<br>06:30-21:30 ");json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "distance", " 1"); json_addstr2 (pstr, size, i,  ",");
	json_addstr2 (pstr, size, i,  "},");
	json_addstr2 (pstr, size, i,  "{");
	json_addstrav (pstr, size, i,  "name", " 19?"); json_addstr2 (pstr, size, i,  ",");
	json_addstrav (pstr, size, i,  "info", "??????-????<br>06:30-21:30 "); json_addstr2 (pstr, size, i,  ",");
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

