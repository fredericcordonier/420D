PROJECT := AUTOEXEC
ADDRESS := 0x7E0000
RELEASE ?=

$(info RELEASE is defined as : [$(RELEASE)])
$(info ENABLE_DEBUG is : [$(ENABLE_DEBUG)])

ifndef CROSS_COMPILE
	CROSS_COMPILE := arm-none-eabi-
endif

ifdef RELEASE
	VERSION := 420D-$(RELEASE)
	RELNAME := 420D-$(RELEASE)
	D_FLAGS := -DRELEASE
else
	VERSION := REV-$(shell git rev-parse --short HEAD)
	D_FLAGS :=
endif

ifdef ENABLE_DEBUG
	D_FLAGS += -DENABLE_DEBUG
endif

USE_FONTS := -DUSE_FONT_SMALL

COMMON_FLAGS := \
	$(USE_FONTS)                      \
	-DVERSION='"$(VERSION)"'          \
	-Wall                             \
	-mcpu=arm946e-s                   \
	-march=armv5te                    \
	-fno-builtin                      \
	-nostdlib                         \
	-fomit-frame-pointer              \
	-fno-strict-aliasing              \
	-mfloat-abi=soft                  \
	-msoft-float                      \
	-mfpu=auto                        \

CC     := $(CROSS_COMPILE)gcc
CFLAGS += $(COMMON_FLAGS)              \
	$(D_FLAGS)                         \
	-Os                                \
	-nostdinc                          \
	-Ivxworks                          \
	-Iinc                              \
	-Werror                            \
	-Wstrict-prototypes                \
	-Wmissing-prototypes               \
	-Wno-char-subscripts               \
	-fdata-sections                    \
	-ffunction-sections                \

AS      := $(CROSS_COMPILE)as
ASFLAGS := $(COMMON_FLAGS)
ASFLAGS += -Iinc

LD      := $(CROSS_COMPILE)ld
LDFLAGS := -Wl,-Ttext,$(ADDRESS) -Wl,-T,link.script -Wl,-Map,autoexec.map -Wl,--gc-sections -e _start -lm -lgcc -lc -z muldefs

OBJCOPY := $(CROSS_COMPILE)objcopy

PYTHON := python3
GEN_SCRIPT := ../utils/gen_struct_hashtables.py

# === Source structure ===
CSRC_DIR := c
SSRC_DIR := asm
VX_DIR := vxworks
FW_DIR := firmware
DEF_DIR := def
OBJ_DIR := .
ASM_DIRS := $(SSRC_DIR) $(VX_DIR) $(FW_DIR)

# === Generate structure definition files (used for serializiation) ===
DEFS := $(wildcard $(DEF_DIR)/*.def)
DEF_NAMES := $(notdir $(basename $(DEFS)))
GEN_HEADERS := $(addprefix $(INC_420D)/,$(addsuffix .h,$(DEF_NAMES)))
GEN_SOURCES := $(addprefix $(SRC_420D)/,$(addsuffix .c,$(DEF_NAMES)))
PARAM_HEADER := $(INC_420D)/param_def.h
GEN_STAMP := gen_stamp
GENERATED := $(GEN_HEADERS) $(GEN_SOURCES) $(PARAM_HEADER)

# === Source discovery ===
C_SRCS := $(wildcard $(CSRC_DIR)/*.c)
ASM_SRCS := $(wildcard $(SSRC_DIR)/*.S)
VX_SRCS := $(wildcard $(VX_DIR)/*.S)
FW_SRCS := $(wildcard $(FW_DIR)/*.S)
SRCS   := $(C_SRCS) $(VX_SRCS) $(FW_SRCS) $(ASM_SRCS)

OBJS := $(notdir $(SRCS))
OBJS := $(OBJS:.c=.o)
OBJS := $(OBJS:.S=.o)
OBJS := $(addprefix $(OBJ_DIR)/,$(OBJS))

DEPS := $(OBJS:%.o=%.d)

# $(info OBJS = $(OBJS))
# $(info SRCS = $(SRCS))

ECHO := "/bin/echo"

ifdef TERM
	BOLD := "\033[1m"
	NORM := "\033[0m"
endif

install: all
ifdef INSTALL_HOST
	@$(ECHO) -e $(BOLD)[UPLOAD]:AUTOEXEC.BIN$(NORM)
	@curl -s http://$(INSTALL_HOST)/upload.cgi?UPDIR=/                                | fgrep -io SUCCESS
	@curl -s -F file=@AUTOEXEC.BIN -F submit=submit http://$(INSTALL_HOST)/upload.cgi | fgrep -io SUCCESS

	@$(ECHO) -e $(BOLD)[UPLOAD]:languages.ini$(NORM)
	@curl -s http://$(INSTALL_HOST)/upload.cgi?UPDIR=/420D                          | fgrep -io SUCCESS
	@curl -s -F file=@languages.ini -F submit=submit http://$(INSTALL_HOST)/upload.cgi | fgrep -io SUCCESS
endif
ifdef INSTALL_PATH
	@install    AUTOEXEC.BIN  $(INSTALL_PATH)/
	@install -D languages.ini $(INSTALL_PATH)/420D
endif

all: $(PROJECT).BIN languages.ini languages/new_lang.ini
	@$(ECHO) -e $(BOLD)[ALL]$(NORM)
	@ls -l AUTOEXEC.BIN

release: clean
	@$(ECHO) -e $(BOLD)[RELEASE]$(NORM)
	@git checkout-index -a --prefix $(RELNAME)/src/
	@zip -9 -r $(RELNAME).src.zip $(RELNAME)

	@mkdir $(RELNAME)/bin
	@cd $(RELNAME)/src && CFLAGS="" make
	@cp $(RELNAME)/src/AUTOEXEC.BIN $(RELNAME)/src/languages.ini $(RELNAME)/bin/
	@zip -9 -r $(RELNAME).bin.zip $(RELNAME)

	@$(ECHO) -e $(BOLD)[ZIP]$(NORM)
	@rm -rf $(RELNAME)
	@ls -l $(RELNAME).src.zip $(RELNAME).bin.zip

$(PROJECT).BIN: $(PROJECT).arm.elf
	@$(ECHO) -e $(BOLD)[OBJCOPY]:$(NORM) $@
	$(OBJCOPY) -O binary $(PROJECT).arm.elf $(PROJECT).BIN

$(PROJECT).arm.elf: $(OBJS)
	@$(ECHO) -e $(BOLD)[LINK]:$(NORM) $@
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(GEN_STAMP): $(DEFS) $(GEN_SCRIPT)
	@$(ECHO) -e python3 $@
	python3 $(GEN_SCRIPT) $(DEF_DIR) $(INC_DIR) $(SRC_DIR)
	touch $(GEN_STAMP)

$(GENERATED): $(GEN_STAMP)

# === Pattern rules for compilation ===
# --- Pattern rules for C and ASM ---
vpath %.c $(CSRC_DIR)
vpath %.S $(ASM_DIRS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MF $(OBJ_DIR)/$*.d -MT $@ -c $< -o $@

$(OBJ_DIR)/%.o: %.S
	@mkdir -p $(OBJ_DIR)
	$(CC) $(ASFLAGS) -c $< -o $@

# === Dependency include ===
-include $(DEPS)

clean:
	@$(ECHO) -e $(BOLD)[CLEAN]$(NORM)
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d
	rm -f $(PROJECT).arm.elf $(PROJECT).BIN $(GENERATED)

languages.ini: inc/languages.h languages/*.ini
	@$(ECHO) -e $(BOLD)[I18N]:$(NORM) $@
	@./languages/lang_tool.pl -q -f languages -l inc/languages.h -o languages.ini

languages/new_lang.ini: inc/languages.h
	@$(ECHO) -e $(BOLD)[I18N]:$(NORM) $@
	@./languages/lang_tool.pl -q -f languages -l inc/languages.h -g

.PHONY: all clean release install
