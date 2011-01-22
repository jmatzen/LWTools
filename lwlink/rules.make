dirname := $(dir $(lastword $(MAKEFILE_LIST)))

lwlink_srcs_local := main.c lwlink.c readfiles.c expr.c script.c link.c output.c map.c

lwobjdump_srcs_l := objdump.c


lwlink_srcs := $(lwlink_srcs) $(addprefix $(dirname),$(lwlink_srcs_local))

lwobjdump_srcs := $(lwobjdump_srcs) $(addprefix $(dirname),$(lwobjdump_srcs_l))
