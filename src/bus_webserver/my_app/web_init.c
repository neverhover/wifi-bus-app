#include "goahead.h"

static void buy(Webs *wp)
{
    websSetStatus(wp, 200);
    websWriteHeaders(wp, 0, 0);
    websWriteEndHeaders(wp);
    websWrite(wp, "Name %s", websGetVar(wp, "name", "")); 
    websWrite(wp,  "Age %s", websGetVar(wp, "age", ""));
    websDone(wp);
}


static void actionTest(Webs *wp, char *path, char *query)
{
        char    *name, *address;

        name = websGetVar(wp, "name", NULL);
        address = websGetVar(wp, "address", NULL);
    websSetStatus(wp, 200);
    websWriteHeaders(wp, -1, 0);
    websWriteEndHeaders(wp);
        websWrite(wp, "<html><body><h2>name: %s, address: %s</h2></body></html>\
n", name, address);
        websDone(wp);
}


int test(){
	websDefineAction("buy", buy);
	websDefineAction("zhu", actionTest);
}

