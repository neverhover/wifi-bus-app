/*
 * web action ×¢²áÈë¿Ú
 */

#include "web_app_api.h"
#define DB_BUS_PATH 		"/mnt/pc_disk/wifi_bus_app/wifi-bus-app/src/bus_base.sqlite"

static void process_sig_kill(int sig){
	printf("close webserver...clean..before..exit\n");
	db_close();
}

int web_func_init(){
	db_open(DB_BUS_PATH);
	/* actions */
	printf("we defined bus_base\n");
	websDefineAction("bus_base", action_bus_base_process);
	websDefineAction("bus_realtime", action_bus_realtime_line_process);
	websDefineAction("bus_config", action_bus_config_process);
}

