TARGET	= metro

CC		= clang
CXX		= clang++
LD		= $(CXX)

BINDIR	= /usr/local/bin

TOPDIR	?= $(CURDIR)
BUILD		= build
INCLUDE	= include
SOURCES	= src \
	src/Application \
	src/Builtin \
	src/Error \
	src/GC \
	src/MetroDriver \
	src/MetroDriver/Evaluator \
	src/MetroDriver/Lexer \
	src/MetroDriver/Parser \
	src/MetroDriver/Sema \
	src/SyntaxAnalyzer \
	src/Types/ \
	src/Types/AST \
	src/Utils

OPTFLAGS		= -O3
WARNFLAGS		= -Wall -Wextra -Wno-switch
COMMONFLAGS	= $(INCLUDES) $(OPTFLAGS) $(WARNFLAGS)
CFLAGS			= $(COMMONFLAGS) -std=c17
CXXFLAGS		= $(COMMONFLAGS) -std=c++20
LDFLAGS			= -Wl,--gc-sections

%.o: %.c
	@echo $(notdir $<)
	@$(CC) $(CFLAGS) -MP -MMD -MF $*.d -c -o $@ $<

%.o: %.cc
	@echo $(notdir $<)
	@$(CXX) $(CXXFLAGS) -MP -MMD -MF $*.d -c -o $@ $<

ifneq ($(notdir $(CURDIR)),$(BUILD))

export OUTPUT		= $(TOPDIR)/$(TARGET)
export VPATH		= $(foreach dir,$(SOURCES),$(TOPDIR)/$(dir))
export INCLUDES	= $(foreach dir,$(INCLUDE),-I$(TOPDIR)/$(dir))

CFILES			= $(notdir $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c)))
CXXFILES		= $(notdir $(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cc)))

export OFILES		= $(CFILES:.c=.o) $(CXXFILES:.cc=.o)

.PHONY: $(BUILD) all debug clean re install

all: $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

debug: $(BUILD)
	@$(MAKE) --no-print-directory OPTFLAGS="-g -O0" WARNFLAGS="-Wall -Wextra" LDFLAGS="" -C $(BUILD) -f $(CURDIR)/Makefile

$(BUILD):
	@[ -d $@ ] || mkdir -p $@

clean:
	rm -rf $(BUILD) $(TARGET)

re: clean all

install: all
	@echo install...
	@install $(notdir $(OUTPUT)) $(BINDIR)/$(TARGET)

else

DEPENDS	= $(OFILES:.o=.d)

$(OUTPUT): $(OFILES)
	@echo linking...
	@$(LD) $(LDFLAGS) -pthread -o $@ $^

-include $(DEPENDS)

endif