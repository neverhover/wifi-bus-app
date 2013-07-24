#include "web_app_api.h"
#include "rq_head.h"
#include "sys_library.h"


PUBLIC void action_help_process(Webs *wp, char *path, char *query){
	char *help_type, *other_infos;
	char status_str[8];
	
	help_type = websGetVar(wp, "help_type", NULL);
       	other_infos = websGetVar(wp, "other_infos", NULL);
	//the format of other_infos maybe like a json.
	
	memset(status_str, 0, 8);
	snprintf(status_str, 8, "%d", HELP_SUCCESS);
	printf("\n%s ,status_str =%s \n",__FUNCTION__, status_str);
	
	process_web_write_response(wp, HTTP_CODE_OK, status_str , 8);
#if 0
	printf( "{ \"help_type\":\"%s\" , \
		\"other_infos\":\"%s\",  }", 
		help_type, other_infos);
#endif
}

