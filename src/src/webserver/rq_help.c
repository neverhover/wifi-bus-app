
#include "web_app_api.h"
#include "rq_head.h"
#include "sys_library.h"
#include "db_bus_table.h"
#include "json.h"
#include "http-client-c.h"

PUBLIC void action_bus_help_process(Webs *wp, char *path, char *query){
	char *help_type, *other_infos;
	char status_str[8];
	
	help_type = websGetVar(wp, "help_type", "");
       	other_infos = websGetVar(wp, "other_infos", "");
	//the format of other_infos maybe like a json.
	
	memset(status_str, 0, 8);
	snprintf(status_str, 8, "%d", HELP_SUCCESS);
	WEB_STRACE(" other_infos=%s \n", other_infos);
	WEB_STRACE(" status_str =%s \n", status_str);
	
	process_web_write_response(wp, HTTP_CODE_OK, status_str , 0);
#if 0
	printf( "{ \"help_type\":\"%s\" , \
		\"other_infos\":\"%s\",  }", 
		help_type, other_infos);
#endif
}

/*  
 * @brief 跨域调用HTTP URI
 */
PUBLIC void action_bus_web_uri_process(Webs *wp, char *path, char *query){
	char *m_url, *m_header;
	const char *tmp_str = NULL;
	char *p_body_str = NULL;
	int ret = 0;
	m_url = websGetVar(wp, "m_url", "");
	//自定义头部可以为空
       	m_header = websGetVar(wp, "m_header", NULL);
	WEB_STRACE("%s\n",m_url);
	//char *agent = "User-agent:MyUserAgent\r\n";
	struct http_response *resp = http_get(m_url, m_header);
	if(resp == NULL){
		process_web_write_response(wp, HTTP_CODE_OK, "error" , 6);
	}
	
	WEB_STRACE("status:%s\n",resp->status_code);
	WEB_STRACE("body:\n%s\n",resp->body);

	json_object *new_obj = NULL;
	new_obj = json_object_new_object();
	WEB_STRACE();
	json_object_object_add(new_obj, "code", json_object_new_int(resp->status_code_int));
	json_object_object_add(new_obj, "resp_header", json_object_new_string(resp->response_headers));
	json_object_object_add(new_obj, "resp_body", json_object_new_string(resp->body));
	WEB_STRACE();
	tmp_str =  json_object_to_json_string(new_obj);
	ret = strlen(tmp_str);
	p_body_str = calloc( ret+1, sizeof(char) );
	
	
	if(p_body_str == NULL){
		ret = RET_ERROR;
		goto out;
	}else{
		memcpy(p_body_str, tmp_str, ret);
		p_body_str[ret]= '\0';
	}
	process_web_write_response(wp, HTTP_CODE_OK, p_body_str , 0);
	WEB_STRACE();
	free(p_body_str);
	WEB_STRACE();
out:
	
	WEB_STRACE();
	http_response_free(resp);
	WEB_STRACE();
	json_object_put(new_obj);
}
