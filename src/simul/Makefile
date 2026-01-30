CPP := g++

CFLAGS := -Iinc  -I../inc              \
	-Werror                            \
	-g                                 \
	-DSIMUL

UT_SRC_DIR := src
C420D_DIR := ../c
OBJ_DIR := obj
BIN_DIR := bin

# === Source discovery ===
CPP_SRCS := $(wildcard $(UT_SRC_DIR)/*.cpp) $(SRC_420D)/settings.c $(SRC_420D)/float.c $(C420D_DIR)/serialize.c $(C420D_DIR)/ini.c $(C420D_DIR)/utils.c

OBJS := $(notdir $(CPP_SRCS))
OBJS := $(OBJS:.c=.o)
OBJS := $(OBJS:.cpp=.o)
OBJS := $(addprefix $(OBJ_DIR)/,$(OBJS))

DEPS := $(OBJS:%.o=%.d)

$(info OBJS = $(OBJS))
$(info CPP_SRCS = $(CPP_SRCS))

ECHO := "/bin/echo"

ifdef TERM
	BOLD := "\033[1m"
	NORM := "\033[0m"
endif

all: $(BIN_DIR)/simul languages.ini languages/new_lang.ini
	@$(ECHO) -e $(BOLD)[ALL]$(NORM)
	@ls -l $(BIN_DIR)

$(BIN_DIR)/simul: $(OBJS)
	@$(ECHO) -e $(BOLD)[LINK]:$(NORM) $@
	@mkdir -p $(BIN_DIR)
	$(CPP) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# === Pattern rules for compilation ===
vpath %.c $(CSRC_DIR)

$(OBJ_DIR)/%.o: $(UT_SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CPP) $(CFLAGS) -MMD -MF $(OBJ_DIR)/$*.d -MT $@ -c $< -o $@

$(OBJ_DIR)/%.o: $(C420D_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CPP) $(CFLAGS) -MMD -MF $(OBJ_DIR)/$*.d -MT $@ -c $< -o $@

# === Dependency include ===
-include $(DEPS)

clean:
	@$(ECHO) -e $(BOLD)[CLEAN]$(NORM)
	rm -rf $(OBJ_DIR)/*.o $(OBJ_DIR)/*.d

distclean: clean
	rm -f $(BIN_DIR)

languages.ini: ../inc/languages.h ../languages/*.ini
	@$(ECHO) -e $(BOLD)[I18N]:$(NORM) $@
	@../languages/lang_tool.pl -q -f ../languages -l ../inc/languages.h -o $(BIN_DIR)/languages.ini

languages/new_lang.ini: ../inc/languages.h
	@$(ECHO) -e $(BOLD)[I18N]:$(NORM) $@
	@../languages/lang_tool.pl -q -f ../languages -l ../inc/languages.h -g

.PHONY: all clean release
