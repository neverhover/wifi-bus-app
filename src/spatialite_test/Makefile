OCTEON_GCC=/mnt/pc_disk/octeon_trunk/dev/SDK/OCTEON-SDK/tools/bin/mips64-octeon-linux-gnu-gcc
RALINK_GCC=/opt/buildroot-gcc342/bin/mipsel-linux-gcc
OCTEON_LIB_NAME=/mnt/pc_disk/octeon_trunk/dev/longteng/trunk/lib_so/octeon/
CFLAGS   = -c -O2 -DMIPSEL -mips3 -D__LITTLE_ENDIAN__ -D__ralink__
all:
	gcc -o test spatialite_main.c -lsqlite3  -L.
	gcc -pg -O2 -o heap_test heap_example.c dijkstra_heap_mine.c  -DDOUBLE_TYPE -lsqlite3 -L.
	$(OCTEON_GCC) -o heap_test_octeon heap_example.c dijkstra_heap_mine.c -DDOUBLE_TYPE -lsqlite3 -L $(OCTEON_LIB_NAME) 
	$(RALINK_GCC) -O2 -DMIPSEL -mips3  -lsqlite3_m -L. heap_example.c dijkstra_heap_mine.c -o heap_test_mips 
