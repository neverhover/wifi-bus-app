export PROJ_BASE = $(shell pwd)
export TARGET_BASE = $(PROJ_BASE)/../..

LIB_NAME = libwebserverStatic.a
SRC_BASE = $(TARGET_BASE)/src/webserver
TARGET_NAME = webserver

include $(TARGET_BASE)/rules/rules.var

LIB_S_PATH = $(TARGET_BASE)/lib/$(ARCH_NAME)
LIB_D_PATH = $(TARGET_BASE)/lib_so/$(ARCH_NAME)
LINK_D_LIBS = -lpthread -lm -lrt -ldl -lsqlite3 -ldbif -ljson-c  -L$(LIB_D_PATH)
LINK_S_LIBS = $(LIB_DIR)/$(LIB_NAME) $(LIB_S_PATH)/libgo.a

IFLAGS   = -I$(TARGET_BASE)/h  -I$(TARGET_BASE)/h/webserver  -I$(TARGET_BASE)/h/db_h  -I$(TARGET_BASE)/h/web_app_h  -I$(TARGET_BASE)/h/dbif -I$(TARGET_BASE)/h/json -I$(TARGET_BASE)/h/http_client
DFLAGS   += -D_REENTRANT -DPIC  $(patsubst %,-D%,$(filter BIT_%,$(MAKEFLAGS))) -DBIT_PACK_EST=$(BIT_PACK_EST) -DBIT_PACK_SSL=$(BIT_PACK_SSL)
LDFLAGS  += '-Wl,--enable-new-dtags' '-Wl,-rpath,$$ORIGIN/' '-rdynamic'

$(TARGET_DIR)/$(TARGET_NAME): $(OBJ_DIR) $(OBJS) $(SUBDIRS) $(LIB_DIR)/$(LIB_NAME)
	$(CC) -O2 $(OBJS) $(LINK_S_LIBS) $(LINK_D_LIBS) -o $(TARGET_DIR)/$(TARGET_NAME)
	-cp -f $(TARGET_DIR)/$(TARGET_NAME) $(TARGET_BASE)/exe/$(ARCH_NAME)/webserver/.

include $(TARGET_BASE)/rules/rules.target

