#include "web_app_api.h"
#include "rq_head.h"


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
	if(real_size < 0){
		process_web_write_response(wp , HTTP_CODE_OK, "[]",  3);
	}else{
		process_web_write_response(wp , HTTP_CODE_OK, buff, real_size);
	}
}



