/*
    manual_action.h -- manual action Header
  
    Copyright (c) All Rights Reserved. See details at the end of the file.
 */

#ifndef MANUAL_ACTION_H_
#define MANUAL_ACTION_H_

#include "goahead.h"


PUBLIC void action_bus_base_process(Webs *wp, char *path, char *query);

PUBLIC void action_bus_realtime_line_process(Webs *wp, char *path, char *query);

PUBLIC void action_bus_realtime_station_process(Webs *wp, char *path, char *query);

PUBLIC void action_bus_config_process(Webs *wp, char *path, char *query);

/*
 * @Brief help process
 */
PUBLIC void action_bus_help_process(Webs *wp, char *path, char *query);

PUBLIC void action_bus_web_uri_process(Webs *wp, char *path, char *query);


#endif

