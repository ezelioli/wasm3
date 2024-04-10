###################################################################
#                       Generic variables                         #
###################################################################

ROOT   := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
SRCDIR := src
WASM3  := $(SRCDIR)/wasm3
PULPRT_HOME := $(ROOT)/runtime

TARGET_BUILD_DIR = $(CURDIR)/build$(build_dir_ext)
ifdef QUIET
V = @
endif

PULP_APP      := wasm3-on-pulp
PULP_APP_SRCS := $(shell find -L $(SRCDIR) -name "*.c") $(shell find -L $(WASM3) -name "*.c")

###################################################################
#                       C flags variables                         #
###################################################################

################### 
#    PULP flags   #
###################

CONFIG_IO_UART=0
ifdef io
ifeq '$(io)' 'uart'
CONFIG_IO_UART=1
endif
endif

PULP_CFLAGS  +=  -D__cv32e40p__ -D__riscv__
PULP_CFLAGS  += -fdata-sections -ffunction-sections -include chips/control-pulp/config.h -I$(PULPRT_HOME)/include/chips/control-pulp
PULP_CFLAGS  += -fno-jump-tables -fno-tree-loop-distribute-patterns
PULP_CFLAGS  += -D__PLATFORM__=ARCHI_PLATFORM_RTL
PULP_CFLAGS  += -DCONFIG_IO_UART_ITF=0
PULP_CFLAGS  += -DCONFIG_IO_UART_BAUDRATE=115200
PULP_CFLAGS  += -DCONFIG_IO_UART=$(CONFIG_IO_UART)
PULP_CFLAGS  += -DARCHI_ASIC_PER_FREQUENCY=100000000 -DARCHI_ASIC_FC_FREQUENCY=100000000 -DARCHI_ASIC_CL_FREQUENCY=100000000
# PULP_CFLAGS  += -fno-builtin -freestanding -std=gnu11
PULP_CFLAGS  += -I$(WASM3) -O2 -DDEBUG -Dd_m3FixedHeap=131072 -Dd_m3FixedHeapAlign=32 # custom -Dd_m3EnableExceptionBreakpoint
PULP_CFLAGS  += -Wno-builtin-declaration-mismatch

# Enable wasm3 logging (see wasm3/m3_config.h)
PULP_CFLAGS  += -Dd_m3LogParse 
PULP_CFLAGS +=-Dd_m3LogModule 
# PULP_CFLAGS +=-Dd_m3LogCompile 
# PULP_CFLAGS +=-Dd_m3LogWasmStack 
# PULP_CFLAGS +=-Dd_m3LogEmit 
# PULP_CFLAGS +=-Dd_m3LogCodePages 
# PULP_CFLAGS +=-Dd_m3LogRuntime 
# PULP_CFLAGS +=-Dd_m3LogNativeStack 
PULP_CFLAGS +=-Dd_m3LogHeapOps 
# PULP_CFLAGS +=-Dd_m3LogTimestamps

ifdef CONFIG_USE_ASM_OPTIM
PULP_CFLAGS  += -D__USE_ASM_OPTIM__=1
endif

ifdef CONFIG_TRACE_LEVEL
PULP_CFLAGS += -D__TRACE_LEVEL__=$(CONFIG_TRACE_LEVEL)
endif

ifdef CONFIG_TRACE_ALL
PULP_CFLAGS += -D__TRACE_ALL__=1
endif

PULP_LDFLAGS  += -lm # custom
PULP_LDFLAGS  += -nostartfiles -nostdlib -Wl,--gc-sections -L$(PULPRT_HOME)/kernel -Tchips/control-pulp/link.ld -lgcc


######################## 
#    PULP arch flags   #
########################

PULP_ARCH_CFLAGS    ?=  -march=rv32imac_zfinx_xcorev -mabi=ilp32 -mno-pulp-hwloop
PULP_ARCH_LDFLAGS   ?=  -march=rv32imac_zfinx_xcorev -mabi=ilp32 -mno-pulp-hwloop

PULP_ARCH_OBJDFLAGS ?=

######################## 
#    PULP app flags    #
########################

PULP_APP_CFLAGS += -I$(PULPRT_HOME)/lib/libc/minimal/include
PULP_APP_CFLAGS += -I$(PULPRT_HOME)/include -I$(PULPRT_HOME)/kernel
PULP_APP_CFLAGS += $(foreach inc,$(PULPOS_MODULES),-I$(inc)/include)

###################################################################
#                     C toolchain variables                       #
###################################################################

PULP_CC       = riscv32-unknown-elf-gcc
PULP_AR      ?= riscv32-unknown-elf-ar
PULP_LD      ?= riscv32-unknown-elf-gcc
PULP_OBJDUMP ?= riscv32-unknown-elf-objdump
PULP_OBJCOPY ?= riscv32-unknown-elf-objcopy


ifdef PULP_RUNTIME_GCC_TOOLCHAIN
PULP_CC := $(PULP_RUNTIME_GCC_TOOLCHAIN)/bin/$(PULP_CC)
PULP_LD := $(PULP_RUNTIME_GCC_TOOLCHAIN)/bin/$(PULP_LD)
PULP_OBJCOPY := $(PULP_RUNTIME_GCC_TOOLCHAIN)/bin/$(PULP_OBJCOPY)
else
ifdef PULP_RISCV_GCC_TOOLCHAIN
PULP_CC := $(PULP_RISCV_GCC_TOOLCHAIN)/bin/$(PULP_CC)
PULP_LD := $(PULP_RISCV_GCC_TOOLCHAIN)/bin/$(PULP_LD)
PULP_OBJCOPY := $(PULP_RISCV_GCC_TOOLCHAIN)/bin/$(PULP_OBJCOPY)
else
$(warning "Warning: Neither PULP_RUNTIME_GCC_TOOLCHAIN nor PULP_RISCV_GCC_TOOLCHAIN is set.\
Using defaults.")
endif
endif


###################################################################
#                         Source files                            #
###################################################################

#############
# C sources #
#############

# FLL
PULP_SRCS     := kernel/fll-v1.c
PULP_SRCS     += kernel/freq-domains.c
PULP_SRCS     += kernel/chips/control-pulp/soc.c
# Libc minimal
PULP_SRCS     += lib/libc/minimal/io.c lib/libc/minimal/fprintf.c lib/libc/minimal/prf.c lib/libc/minimal/sprintf.c
# Peripherals
PULP_SRCS     += kernel/init.c kernel/kernel.c kernel/alloc.c kernel/alloc_pool.c kernel/irq.c kernel/soc_event.c kernel/bench.c drivers/uart.c
# Cluster 
PULP_SRCS     += kernel/cluster.c

PULP_SRCS := $(addprefix runtime/,$(PULP_SRCS))

###############
#  S sources  #
###############

PULP_ASM_SRCS := kernel/crt0.S
PULP_ASM_SRCS += kernel/irq_asm.S

PULP_ASM_SRCS := $(addprefix runtime/,$(PULP_ASM_SRCS))

###################################################################
#                           vsim flags                            #
###################################################################

vsim_flags ?= +ENTRY_POINT=0x1c008080 -dpicpppath /usr/bin/g++ -permit_unmatched_virtual_intf -gBAUDRATE=115200
ifdef bootmode
ifeq ($(bootmode), spi)
vsim_flags += -gSTIM_FROM=SPI_FLASH -gLOAD_L2=STANDALONE -gUSE_S25FS256S_MODEL=1
else
ifeq ($(bootmode), hyperflash)
vsim_flags += -gSTIM_FROM=HYPER_FLASH -gLOAD_L2=STANDALONE -gUSE_HYPER_MODELS=1
else
ifeq ($(bootmode), fast_debug)
vsim_flags += -gLOAD_L2=FAST_DEBUG_PRELOAD
else
ifeq ($(bootmode), jtag)
vsim_flags += -gLOAD_L2=JTAG
else
$(error Illegal value supplied for bootmode. Legal values are 'spi', 'hyperflash', 'fast_debug' and 'jtag')
endif
endif
endif
endif
else
vsim_flags += -gLOAD_L2=JTAG
endif
ifdef vsim_additional_flags
vsim_flags += $(vsim_additional_flags)
endif

###################################################################
#                           PULP app                              #
###################################################################

PULP_APP_SRCS_ALL     = $(PULP_APP_SRCS) $(PULP_APP_FC_SRCS) $(PULP_SRCS) $(PULP_APP_CL_SRCS) $(PULP_CL_SRCS)
PULP_APP_ASM_SRCS_ALL = $(PULP_APP_ASM_SRCS) $(PULP_ASM_SRCS) $(PULP_APP_CL_ASM_SRCS) $(PULP_CL_ASM_SRCS)

PULP_APP_OBJS_ALL     = $(patsubst %.c,$(TARGET_BUILD_DIR)/$(PULP_APP)/%.o,$(PULP_APP_SRCS_ALL))
PULP_APP_OBJS_ALL    += $(patsubst %.S,$(TARGET_BUILD_DIR)/$(PULP_APP)/%.o,$(PULP_APP_ASM_SRCS_ALL))

PULP_APP_CFLAGS_ALL   = $(PULP_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_APP_CFLAGS)
PULP_APP_LDFLAGS_ALL  = $(PULP_ARCH_LDFLAGS) $(PULP_LDFLAGS) $(PULP_APP_LDFLAGS)

-include $(PULP_APP_OBJS_ALL:.o=.d)

$(TARGET_BUILD_DIR)/$(PULP_APP)/%.o: %.c
	@echo "CC  $<"
	$(V)mkdir -p `dirname $@`
	$(V)$(PULP_CC) -c $< -o $@ -MMD -MP $(PULP_APP_CFLAGS_ALL)

$(TARGET_BUILD_DIR)/$(PULP_APP)/%.o: %.cpp
	@echo "CXX $<"
	$(V)mkdir -p `dirname $@`
	$(V)$(PULP_CC) -c $< -o $@ -MMD -MP $(PULP_APP_CFLAGS_ALL)

$(TARGET_BUILD_DIR)/$(PULP_APP)/%.o: %.S
	@echo "CC  $<"
	$(V)mkdir -p `dirname $@`
	$(V)$(PULP_CC) -c $< -o $@ -MMD -MP -DLANGUAGE_ASSEMBLY $(PULP_APP_CFLAGS_ALL)

$(TARGET_BUILD_DIR)/$(PULP_APP)/$(PULP_APP): $(PULP_APP_OBJS_ALL)
	@echo "LD  $@"
	$(V)mkdir -p `dirname $@`
	$(V)$(PULP_LD) -o $@ $^ -MMD -MP $(PULP_APP_LDFLAGS_ALL)

$(TARGET_BUILD_DIR)/$(PULP_APP)/$(PULP_APP).srec: $(TARGET_BUILD_DIR)/$(PULP_APP)/$(PULP_APP)
	@echo "OBJCOPY  $@"
	$(V)mkdir -p `dirname $@`
	$(V)$(PULP_OBJCOPY) -O srec $^ $@

$(TARGET_INSTALL_DIR)/bin/$(PULP_APP): $(TARGET_BUILD_DIR)/$(PULP_APP)/$(PULP_APP)
	@echo "CP  $@"
	$(V)mkdir -p `dirname $@`
	$(V)cp $< $@

TARGETS         += $(TARGET_BUILD_DIR)/$(PULP_APP)/$(PULP_APP)
MORE_TARGETS     = $(TARGET_BUILD_DIR)/$(PULP_APP)/$(PULP_APP).srec
INSTALL_TARGETS += $(TARGET_INSTALL_DIR)/bin/$(PULP_APP)
INSTALL_TARGETS += $(TARGET_INSTALL_DIR)/bin/$(PULP_APP).srec


###################################################################
#                            Targets                              #
###################################################################

############
#   Info   #
############

.PHONY: info
info:
	@echo "ROOT              : $(ROOT)"
	@echo "WASM3             : $(WASM3)" 
	@echo "PULPRT_HOME       : $(PULPRT_HOME)"
	@echo "TARGET_BUILD_DIR  : $(TARGET_BUILD_DIR)"
	@echo "PULP_APP_SRCS     : $(PULP_APP_SRCS)"
	@echo "PULP_APP_SRCS_ALL : $(PULP_APP_SRCS_ALL)"

###########
#   All   #
###########

all: $(TARGETS) $(MORE_TARGETS)

###########
#  Clean  #
###########

.PHONY: clean
clean:
	@echo "RM  $(TARGET_BUILD_DIR)"
	$(V)rm -rf $(TARGET_BUILD_DIR)

#########
#  Run  #
#########

.PHONY: run

$(TARGET_BUILD_DIR)/modelsim.ini:
ifndef VSIM_PATH
	$(error "VSIM_PATH is undefined. Either call \
	'source $$YOUR_HW_DIR/setup/vsim.sh' or set it manually.")
endif
	ln -s $(VSIM_PATH)/modelsim.ini $@

$(TARGET_BUILD_DIR)/work:
ifndef VSIM_PATH
	$(error "VSIM_PATH is undefined. Either call \
	'source $$YOUR_HW_DIR/setup/vsim.sh' or set it manually.")
endif
	ln -s $(VSIM_PATH)/work $@

$(TARGET_BUILD_DIR)/boot:
ifndef VSIM_PATH
	$(error "VSIM_PATH is undefined. Either call \
	'source $$YOUR_HW_DIR/setup/vsim.sh' or set it manually.")
endif
	ln -s $(VSIM_PATH)/boot $@

$(TARGET_BUILD_DIR)/tcl_files:
ifndef VSIM_PATH
	$(error "VSIM_PATH is undefined. Either call \
	'source $$YOUR_HW_DIR/setup/vsim.sh' or set it manually.")
endif
	ln -s $(VSIM_PATH)/tcl_files $@

$(TARGET_BUILD_DIR)/waves:
ifndef VSIM_PATH
	$(error "VSIM_PATH is undefined. Either call \
	'source $$YOUR_HW_DIR/setup/vsim.sh' or set it manually.")
endif
	ln -s $(VSIM_PATH)/waves $@

$(TARGET_BUILD_DIR)/stdout:
	mkdir -p $@

$(TARGET_BUILD_DIR)/fs:
	mkdir -p $@


run: $(TARGET_BUILD_DIR)/modelsim.ini  $(TARGET_BUILD_DIR)/boot $(TARGET_BUILD_DIR)/tcl_files $(TARGET_BUILD_DIR)/stdout $(TARGET_BUILD_DIR)/fs $(TARGET_BUILD_DIR)/waves $(TARGET_BUILD_DIR)/work
	$(PULPRT_HOME)/bin/stim_utils.py --binary=$(TARGETS) --vectors=$(TARGET_BUILD_DIR)/vectors/stim.txt
	$(PULPRT_HOME)/bin/plp_mkflash  --flash-boot-binary=$(TARGETS)  --stimuli=$(TARGET_BUILD_DIR)/vectors/qspi_stim.slm --flash-type=spi --qpi
	$(PULPRT_HOME)/bin/slm_hyper.py  --input=$(TARGET_BUILD_DIR)/vectors/qspi_stim.slm  --output=$(TARGET_BUILD_DIR)/vectors/hyper_stim.slm
ifndef VSIM_PATH
	$(error "VSIM_PATH is undefined. Either call \
	'source $$YOUR_HW_DIR/setup/vsim.sh' or set it manually.")
endif
ifdef gui
	cd $(TARGET_BUILD_DIR) && export VSIM_RUNNER_FLAGS='$(vsim_flags)' && export VOPT_ACC_ENA="YES" && vsim -64 -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' -do 'source $(VSIM_PATH)/tcl_files/run.tcl; '
else
	pwd
	cd $(TARGET_BUILD_DIR) && export VSIM_RUNNER_FLAGS='$(vsim_flags)' && vsim -64 -c -do 'source $(VSIM_PATH)/tcl_files/config/run_and_exit.tcl' -do 'source $(VSIM_PATH)/tcl_files/run.tcl; run_and_exit;'
endif


#########
#  Dis  #
#########

dis:
	$(PULP_OBJDUMP) $(PULP_ARCH_OBJDFLAGS) -d $(TARGETS)

##########
#  Size  #
##########

size:
	$(PULPRT_HOME)/bin/pos-size --binary=$(TARGETS) --depth=10

##########
#  Help  #
##########

help:
	@echo "Makefile options:"
	@echo "  CONFIG_TRACE_LEVEL=<level>    Activate traces for the specified level (0=none, 1=fatal, 2=error, 3=warning, 4=info, 5=debug, 6=trace)."
	@echo "  CONFIG_TRACE_ALL=1            Activate all traces. Other traces can be individually activated with CONFIG_TRACE_<NAME>."
	@echo "  CONFIG_IO_UART=1"             Activate UART output.

