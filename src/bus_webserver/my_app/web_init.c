#include "web_app_api.h"

int web_func_init(){
	/* actions */
	printf("we defined bus_base\n");
	websDefineAction("bus_base", action_bus_base_process);
}

