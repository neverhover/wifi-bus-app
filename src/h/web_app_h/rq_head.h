/*
    rq_head.h -- webserver web layer Header
  
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

#ifndef RQ_HEAD_H_
#define RQ_HEAD_H_

#define BUS_SEARCH_LINE	"type_line"
#define BUS_SEARCH_STA		"type_sta"
#define BUS_SEARCH_RIDE	"type_ride"

/******************** bus设置部分  ***************************/
#define BUS_CONFIG_CUR_LINE		"cur_line"
#define BUS_CONFIG_STA_STATUS		"sta_status"

#define RET_ERROR		-1
#define RET_OK			0

#define TEST_PRA_STR_NOT_VOLID(x)		\
	( (NULL == x) || (strcmp(x, "") == 0) || (strlen(x) <=0) )

#define DEBUG 1
#if DEBUG			
#define WEB_STRACE(format,...) \
		printf("WEB_STRACE <%10s> <%04d> "format"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
	#define WEB_STRACE(format,...)
#endif

#define JSON_STR_FREE(x)	\
	if(x){ free(x); }

/* 紧急求助状态 */
enum{
	HELP_SUCCESS = 1,
	HELP_HW_ERROR,
	HELP_NETWORK_ERROR,
	HELP_FORBID,
	HELP_NOT_PERMIT
}help_status;
/* 线路类型 */
enum{
	KIND_OUTBOUND = 1,
	KIND_INBOUND,
	KIND_LOOP
}line_kind;

/* 行车状态 */
enum{
	BUS_STATUS_IN_STATION = 1,	 //到站
	BUS_STATUS_INBOUND,		//入站,指正在驶向该站
	BUS_STATUS_OUTBOUND,	//离站,指正在驶离该站
	BUS_STATUS_TERMINAL,		//到达终点站
	BUS_STATUS_BEGIN,			//开始运行
	BUS_STATUS_MAX			//状态最大值
}bus_drv_status;

enum{
	STA_PASSED = 1,				//已经经过
	STA_IN_STA,					//在站点
	STA_INBOUND,				//入站
	STA_OUTBOUND,				//离站
	STA_NOT_REACH				//未经过
}sta_status;

/* ******************* function prototype ******************* */

/*
 *		a wrap function of write response
 *		@param code 
 				the http response status number
 *		@param buf 
 				the buffer pointer which will output to browser
 * 		@param size
 				the buffer size
 *		@return write json format data to page
 */
PUBLIC  int process_web_write_response(Webs *wp, int code, char *buf, int size);

#endif

