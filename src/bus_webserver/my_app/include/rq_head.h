/*
    rq_head.h -- webserver web layer Header
  
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

#ifndef RQ_HEAD_H_
#define RQ_HEAD_H_

#define BUS_SEARCH_LINE	"type_line"
#define BUS_SEARCH_STA		"type_sta"
#define BUS_SEARCH_RIDE	"type_ride"


enum{
	HELP_SUCCESS = 1,
	HELP_HW_ERROR,
	HELP_NETWORK_ERROR,
	HELP_FORBID,
	HELP_NOT_PERMIT
}help_status;


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

