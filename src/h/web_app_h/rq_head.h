/*
    rq_head.h -- webserver web layer Header
  
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

#ifndef RQ_HEAD_H_
#define RQ_HEAD_H_

#define BUS_SEARCH_LINE	"type_line"
#define BUS_SEARCH_STA		"type_sta"
#define BUS_SEARCH_RIDE	"type_ride"


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


enum{
	HELP_SUCCESS = 1,
	HELP_HW_ERROR,
	HELP_NETWORK_ERROR,
	HELP_FORBID,
	HELP_NOT_PERMIT
}help_status;

enum{
	KIND_OUTBOUND = 1,
	KIND_INBOUND,
	KIND_LOOP
}line_kind;

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

