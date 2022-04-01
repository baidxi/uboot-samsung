obj-y:=board.o
ifdef CONFIG_SPL_BUILD
obj-y:=spl.o
ifndef CONFIG_SKIP_LOWLEVEL_INIT
obj-y+=lowlevel_init.o cache.o dram.o
endif
endif
