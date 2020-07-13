########################################
#   文件名称: makefile
#   内容描述: APP层代码编译文件 
#   作    者: 姚亮
#   日	  期: 2009-12-02
#   COPYRIGHT (C) 2009 CHANGHONG
########################################

MODULE_NAME = gtest

##############################定义库文件最终存放路径###########################
$(MODULE_NAME)_OBJ_DIR = obj
LIBDIR = $(subst $(BAD_SLASH),$(GOOD_SLASH),$(CURDIR)/lib)$(SLASH)


##################################目标文件#####################################
$(MODULE_NAME)_OBJECTS =gmain.o gtest-all.o
$(MODULE_NAME)_OBJECTS += gtest_broken.o
$(MODULE_NAME)_OBJECTS += test_menu.o
$(MODULE_NAME)_OBJECTS += chdrv_evironment.o
$(MODULE_NAME)_OBJECTS += fw_env.o
$(MODULE_NAME)_OBJECTS += gtest_cmd.o
$(MODULE_NAME)_OBJECTS += readline.o
$(MODULE_NAME)_OBJECTS += gtest_io.o
$(MODULE_NAME)_OBJECTS += psi_parse.o
$(MODULE_NAME)_OBJECTS += parse_ts.o
$(MODULE_NAME)_OBJECTS += compile_info.o
$(MODULE_NAME)_OBJECTS += mem_dbg.o
$(MODULE_NAME)_OBJECTS += mem_tester.o
$(MODULE_NAME)_OBJECTS += mem_tests.o
$(MODULE_NAME)_OBJECTS += debug_audio.o
$(MODULE_NAME)_OBJECTS += debug_avfilter.o
$(MODULE_NAME)_OBJECTS += debug_route.o
$(MODULE_NAME)_OBJECTS += debug_video.o
$(MODULE_NAME)_OBJECTS += debug_vwin.o
$(MODULE_NAME)_OBJECTS += debug_osd.o
$(MODULE_NAME)_OBJECTS += debug_vout.o
$(MODULE_NAME)_OBJECTS += debug_fsys.o
$(MODULE_NAME)_OBJECTS += debug_tuner.o
$(MODULE_NAME)_OBJECTS += debug_section.o
$(MODULE_NAME)_OBJECTS += debug_aout.o
$(MODULE_NAME)_OBJECTS += debug_rtos.o
$(MODULE_NAME)_OBJECTS += debug_time.o

##############################源文件路径和头文件路径############################
SRC     = $(subst $(BAD_SLASH),$(GOOD_SLASH),$(CURDIR))$(SLASH)
INCDIR  = -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(CURDIR))
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/drv/include)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/drv/$(CHSSA_PLATFORM)/$(CHSSA_CHIP)/include)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/drv/config)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/mid/include)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/mid/config)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/mid/ca/novel/)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/mid/ca/dvt)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/mid/ca/dvn)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/mid/ca/dvn/dvn4.0)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/mid/ca/changhong)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/mid/ci)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SRC)case)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SRC)case/module)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SRC)case/base)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SRC)case/debug)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SRC)lib)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SRC)lib/include)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SRC)lib/include/gtest)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(SRC)lib/include/internal)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/project/config/include)
INCDIR  += -I$(subst $(BAD_SLASH),$(GOOD_SLASH),$(PRJROOT)/stdio)

VPATH = case case/module case/debug case/base lib lib/include/gtest lib/src

#################################包含标准头#####################################
include $(PRJROOT)/project/$(CHSSA_PLATFORM)/makefile_head

ifeq "$(CHSSA_PLATFORM)" ""
CHSSA_PLATFORM := unkown
CHSSA_CHIP := unkown
endif

ifeq "$(CFG_CHSSA_OS_TYPE)" "ECOS"
CFLAGS := $(filter-out  -fno-builtin -Winline, $(CFLAGS))
CFLAGS +=  -ffunction-sections -fdata-sections
endif

ifeq "$(CFG_CHSSA_OS_TYPE)" "LINUX"
CFLAGS +=  -fexceptions -Wno-enum-compare -Wfatal-errors 
endif

CFLAGS += -fpermissive -Wwrite-strings -Wsign-compare -Wno-format -Wno-sign-compare -Wno-write-strings  -Wno-undef

CFLAGS := $(filter-out  -Wstrict-prototypes, $(CFLAGS))


platform_file = $(subst $(BAD_SLASH),$(GOOD_SLASH),$(CURDIR)/case/compile_info.cpp)

m_tail = $(PRJROOT)/project/$(CHSSA_PLATFORM)/makefile_tail

all : $(m_tail).plus create_directories  $(platform_file)_1 $(LIBDIR)$(MODULE_NAME).lib

$(platform_file)_1:
	@echo "/*------------->>>" > $(platform_file)
	@echo "This file is created by complier, please don't edit it!" >> $(platform_file)
	@echo "<<<-------------*/" >> $(platform_file)
	@echo "const char* guc_CHSSA_PLATFORM = \"$(CHSSA_PLATFORM)\";" >> $(platform_file)
	@echo "const char* guc_CHSSA_CHIP = \"$(CHSSA_CHIP)\";" >> $(platform_file) 
	@echo "const char* guc_PATH = \"$(subst \,/,$(PRJROOT))\";" >> $(platform_file) 
	@echo "" >> $(platform_file)


################################包含标准尾####################################

$(m_tail).plus:
	cp $(m_tail) $(m_tail).tmp1
	sed 's,(CC),(CC_PLUS),g' $(m_tail).tmp1 > $(m_tail).tmp2
	sed 's,%.c,%.cpp,g' $(m_tail).tmp2 > $(m_tail).plus
	$(RM) -f $(m_tail).tmp1
	$(RM) -f $(m_tail).tmp2


include $(m_tail).plus



#################################特殊清除规则#####################################
special_clean :
	



### END OF FILE ###

