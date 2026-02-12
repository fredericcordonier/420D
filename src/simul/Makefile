# Compilers
CPP := g++
CC  = gcc

CFLAGS := -Iinc -Iinc/vxworks -I../inc              \
	-Werror                            \
	-g                                 \
	-DSIMUL								\
	-O2
LDFLAGS :=

UT_SRC_DIR := src
SRC_420D := ../c
OBJ_DIR := obj
BIN_DIR := bin
GTK_CFLAGS := $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS   := $(shell pkg-config --libs gtk+-3.0)

# Source files
CPP_SRCS := $(wildcard $(UT_SRC_DIR)/*.cpp)
C_SRCS   := $(SRC_420D)/ini.c $(SRC_420D)/settings.c $(SRC_420D)/serialize.c $(SRC_420D)/languages.c $(SRC_420D)/button.c \
			$(SRC_420D)/menu.c $(SRC_420D)/shortcuts.c $(SRC_420D)/menu_main.c $(SRC_420D)/actions.c $(SRC_420D)/viewfinder.c \
			$(SRC_420D)/autoiso.c $(SRC_420D)/af_patterns.c $(SRC_420D)/menu_cmodes.c $(SRC_420D)/menu_developer.c $(SRC_420D)/menu_info.c \
			$(SRC_420D)/menu_params.c $(SRC_420D)/menu_rename.c $(SRC_420D)/menu_scripts.c $(SRC_420D)/menu_settings.c $(SRC_420D)/menuitem.c \
			$(SRC_420D)/menuoptions.c $(SRC_420D)/menupage.c $(SRC_420D)/exposure.c $(SRC_420D)/cmodes.c $(SRC_420D)/snapshots.c \
			$(SRC_420D)/scripts.c $(SRC_420D)/shutter.c $(SRC_420D)/float.c  $(SRC_420D)/display.c $(SRC_420D)/menu_dlg.c $(SRC_420D)/csv.c \
			$(SRC_420D)/msm.c $(SRC_420D)/qexp.c $(SRC_420D)/persist.c $(SRC_420D)/fexp.c $(SRC_420D)/debug.c $(SRC_420D)/bmp.c \
			$(SRC_420D)/font-large.c $(SRC_420D)/font-med.c $(SRC_420D)/font-small.c $(SRC_420D)/memspy.c $(SRC_420D)/intercom.c

# CPP_SRCS := $(wildcard $(UT_SRC_DIR)/*.cpp) $(SRC_420D)/ini.c $(SRC_420D)/settings.c $(SRC_420D)/serialize.c $(SRC_420D)/languages.c $(SRC_420D)/button.c \
# 			$(SRC_420D)/menu.c $(SRC_420D)/shortcuts.c $(SRC_420D)/menu_main.c $(SRC_420D)/actions.c $(SRC_420D)/viewfinder.c \
# 			$(SRC_420D)/autoiso.c $(SRC_420D)/af_patterns.c $(SRC_420D)/menu_cmodes.c $(SRC_420D)/menu_developer.c $(SRC_420D)/menu_info.c \
# 			$(SRC_420D)/menu_params.c $(SRC_420D)/menu_rename.c $(SRC_420D)/menu_scripts.c $(SRC_420D)/menu_settings.c $(SRC_420D)/menuitem.c \
# 			$(SRC_420D)/menuoptions.c $(SRC_420D)/menupage.c $(SRC_420D)/exposure.c $(SRC_420D)/cmodes.c $(SRC_420D)/snapshots.c \
# 			$(SRC_420D)/scripts.c $(SRC_420D)/shutter.c $(SRC_420D)/float.c  $(SRC_420D)/display.c \
# 			$(SRC_420D)/msm.c $(SRC_420D)/qexp.c $(SRC_420D)/persist.c $(SRC_420D)/fexp.c $(SRC_420D)/debug.c $(SRC_420D)/bmp.c \
# 			$(SRC_420D)/font-large.c $(SRC_420D)/font-med.c $(SRC_420D)/font-small.c $(SRC_420D)/memspy.c $(SRC_420D)/intercom.c

SRCS     := $(CPP_SRCS) $(C_SRCS)
# Object files (mirror source paths inside obj/)
OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(C_SRCS))) \
        $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(notdir $(CPP_SRCS)))

# OBJS := $(notdir $(CPP_SRCS))
# OBJS := $(OBJS:.c=.o)
# OBJS := $(OBJS:.cpp=.o)
# OBJS := $(addprefix $(OBJ_DIR)/,$(OBJS))

DEPS := $(OBJS:%.o=%.d)

$(info OBJS = $(OBJS))
$(info SRCS = $(SRCS))

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
	$(CPP) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(GTK_LIBS)

# === Pattern rules for compilation ===
vpath %.c $(CSRC_DIR)

$(OBJ_DIR)/%.o: $(UT_SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CPP) $(CFLAGS) $(GTK_CFLAGS) -MMD -MF $(OBJ_DIR)/$*.d -MT $@ -c $< -o $@

# Compile C sources
$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_420D)/%.c | $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -MMD -MF $(OBJ_DIR)/$*.d -MT $@ -c $< -o $@


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
