export PROJ_BASE = $(shell pwd)
export TARGET_BASE = $(PROJ_BASE)/../..

LIB_NAME = libdbif.a
SRC_BASE = $(TARGET_BASE)/src/dbif
TARGET_NAME = dbif
LIBSO_NAME	= libdbif.so

include $(TARGET_BASE)/rules/rules.var

LIB_S_PATH = $(TARGET_BASE)/lib/$(ARCH_NAME)
LIB_D_PATH = $(TARGET_BASE)/lib_so/$(ARCH_NAME)
LINK_D_LIBS = -lpthread -ldl -lsqlite3  -L$(LIB_D_PATH) 
LINK_S_LIBS = $(LIB_DIR)/$(LIB_NAME) 

IFLAGS   =  -I$(TARGET_BASE)/h/db_h  -I$(TARGET_BASE)/h/dbif 


$(TARGET_DIR)/$(TARGET_NAME): $(OBJ_DIR) $(OBJS) $(SUBDIRS) $(LIB_DIR)/$(LIB_NAME)
	$(CC) -fPIC -shared $(OBJS) $(LINK_S_LIBS) $(LINK_D_LIBS)  -o $(TARGET_DIR)/$(LIBSO_NAME)
	-cp -f $(TARGET_DIR)/$(LIBSO_NAME) $(TARGET_BASE)/lib_so/$(ARCH_NAME)/.
	$(CC) -O2 $(OBJS) $(LINK_S_LIBS)  $(LINK_D_LIBS)  -o $(TARGET_DIR)/$(TARGET_NAME) -DUNIT_TEST=1
	-cp -f $(TARGET_DIR)/$(TARGET_NAME) $(TARGET_BASE)/exe/$(ARCH_NAME)/.
	


include $(TARGET_BASE)/rules/rules.target

