dirname := $(dir $(lastword $(MAKEFILE_LIST)))

lwlink_srcs_local := main.c lwlink.c util.c readfiles.c expr.c script.c link.c output.c map.c
lwobjdump_srcs_local := objdump.c util.c


lwlink_srcs := $(lwlink_srcs) $(addprefix $(dirname),$(lwlink_srcs_local))
lwobjdump_srcs := $(lwobjdump_srcs) $(addprefix $(dirname),$(lwobjdump_srcs_local))
