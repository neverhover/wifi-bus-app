

#ifndef SYS_LIBRARY_H_
#define SYS_LIBRARY_H_

#define MRULES_INIT_WALK_SAFE(buf, num, pnum, ptr) \
    for ((ptr) = (buf), (pnum = num); (pnum) > 0; (pnum) --, (ptr) ++)	


#define json_addstr(buf, buf_size, index, str) \
do { \
	index += snprintf (buf + index, buf_size - index, "\"%s\"", str); \
} while (0);

#define json_addstr2(buf, buf_size, index, str) \
do { \
	index += snprintf (buf + index, buf_size - index, "%s", str); \
} while (0);


#define json_addintav(buf, buf_size, index, attribute, value) \
do { \
	index += snprintf (buf + index, buf_size - index, "\"%s\":\"%d\"", attribute, value); \
} while (0);


#define json_addstrav(buf, buf_size, index, attribute, value) \
do { \
	index += snprintf (buf + index, buf_size - index, "\"%s\":\"%s\"", attribute, value); \
} while (0);

#define json_addstrav2(buf, buf_size, index, attribute, value) \
do { \
	index += snprintf (buf + index, buf_size - index, "\"%s\":%s", attribute, value); \
} while (0);



#endif
