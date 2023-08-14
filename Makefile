TOP=$(shell pwd)
BUILD_DIR = Objects
TARGET = stm32_test
CORE_DIR = CORE
BIN_DIR = BIN

#设定包含文件目录
INC_FLAGS += -I $(TOP)/CORE
INC_FLAGS += -I $(TOP)/FWLIB/inc
INC_FLAGS += -I $(TOP)/User


CROSS_COMPILE=arm-none-eabi-
ASM=$(CROSS_COMPILE)gcc -W
CC=$(CROSS_COMPILE)gcc
CPP=$(CROSS_COMPILE)cpp
LD=$(CROSS_COMPILE)ld
HEX=$(CROSS_COMPILE)objcopy
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump

#######################################
# ASM sources
# 注意，如果使用.c的startup文件，请把下面两句注释掉，避免编译出现错误
#######################################
ASM_SOURCES =  $(CORE_DIR)/startup_stm32f10x_md.s
# ASM_SOURCES =  $(CORE_DIR)/startup_stm32f103xb.s

#######################################
# CFLAGS
#######################################
# debug build?
DEBUG = 1

# 代码优化级别
OPT = -Os

# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# C defines
# 这个地方的定义根据实际情况来，如果在代码里面有定义(如#define STM32F10X_HD)，这里可以不写
C_DEFS =  \
-D USE_STDPERIPH_DRIVER \ #这个宏定义用于开启或启用一个名为 USE_STDPERIPH_DRIVER 的特性。可能是为了启用标准外设驱动库（Standard Peripheral Library）的相关功能
-D STM32F10X_MD # 这个宏定义是针对 STM32F10x 系列微控制器的。STM32F10X_MD 可能是指示编译器在编译过程中针对 STM32F10x 系列的微控制器进行适当的配置和优化。

# PreProcess
CFLAGS =  -g $(MCU) $(C_DEFS) $(INC_FLAGS) $(OPT) -std=gnu99 -W -Wall -fdata-sections -ffunction-sections
# -std=gnu99: 这个选项指定使用 C99 标准的 GNU 扩展，以确定源代码中可以使用的语法和特性。
# -W -Wall: 这些选项启用了编译器的警告，其中 -W 表示启用所有警告，-Wall 表示启用常见的警告
# -fdata-sections -ffunction-sections: 这些选项启用了代码和数据分区。/
#    使用这些选项，编译器会尝试将不同的函数和数据放置在独立的节（sections）中，这有助于优化内存使用和执行速度



#ifeq ($(DEBUG), 1)
#CFLAGS += -g -gdwarf-2
#endif
# 在$(BUILD_DIR)目录下生成依赖关系信息，依赖关系以.d结尾
CFLAGS += -MMD -MP -MF"$(addprefix $(BUILD_DIR)/, $(notdir $(@:%.o=%.d)))"
# -MMD: 这个选项告诉编译器在编译源代码的同时生成依赖关系文件（Dependency Files）。依赖
# -MP: 这个选项生成一个伪目标（Phony Target），用于确保在重新编译时更新依赖关系。这是为了防止因为某个头文件的改变而导致重新编译出现问题。
# -MF"$(addprefix $(BUILD_DIR)/, $(notdir $(@:%.o=%.d)))": 这个选项用于指定生成的依赖关系文件的路径和文件名。

# $@：这是一个Makefile中的自动变量，表示当前规则中的目标文件名。
# %.o：这是一个模式，表示所有以 .o 为扩展名的文件。
# %.d：这也是一个模式，表示所有以 .d 为扩展名的文件。
# 所以，$(@:%.o=%.d) 这个表达式的意思是，将当前规则的目标文件名中的 .o 扩展名替换为 .d 扩展名，从而得到对应的依赖关系文件名。



#######################################
# LDFLAGS
#######################################
# LD文件,链接文件
LDSCRIPT = STM32F10x.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections
#######################################
# C/CPP源代码
#######################################
C_SRC = $(shell find ./ -name '*.c' | grep -v './SZC_Testfile/' | grep -v './en.stm32cubef1/')
C_SRC += $(shell find ./ -name '*.cpp')

C_OBJ = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SRC:%.c=%.o)))
vpath %.c $(sort $(dir $(C_SRC)))
#C_OBJ += $(ASM_SOURCES)
C_OBJ += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:%.s=%.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))


.PHONY: all clean update


all: $(BUILD_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).hex $(BIN_DIR)/$(TARGET).bin

# 从vpath中读取所有的.c文件，编译成.o文件
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) -o $@ $<
#比如：
# $(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
#     $(CC) -c $(CFLAGS) -o $@ $<
# 就会被翻译成
# $(CC) -c $(CFLAGS) -o build/example.o example.c


# 从vpath中u读取所有的.s文件，编译成.o文件
$(BUILD_DIR)/%.o: $(CORE_DIR)/%.s | $(BUILD_DIR)
	$(ASM) -c $< -o $@

# 将所有的.o文件依据.ld文件，编译组成.elf文件
##################################引入c的标准库libc#####################
##因为原本的build是用的arm-none-eabi-ld,来链接目标文件，不能直接在LIBS=-lc -lm -lnosys，会报错=====
##它使用链接器 $(LD) 来链接这些目标文件，并且 -T $(LDSCRIPT) 选项用于指定链接脚本
# $(BUILD_DIR)/$(TARGET).elf: $(C_OBJ)
# 	$(LD) -T $(LDSCRIPT) $(LIBS) $(C_OBJ)    -o $@
#需这里改成用编译器  arm-none-eabi-gcc 来将这些目标文件链接起来，$(LDFLAGS) 表示链接时需要的其他选项
##还有一个附加的命令 $(SZ) $@，它使用了 $(SZ) 工具来计算生成的可执行文件的大小
$(BUILD_DIR)/$(TARGET).elf: $(C_OBJ) Makefile
	$(CC) $(C_OBJ) $(LDFLAGS) -o $@

# 将.elf文件转为.hex格式
$(BIN_DIR)/%.hex: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O ihex $< $@
	
# 将.elf文件转为.bin格式
$(BIN_DIR)/%.bin: $(BUILD_DIR)/%.elf
	$(OBJCOPY) -O binary $< $@	

# 用于生成BUILD_DIR目录
$(BUILD_DIR):
	mkdir $@		

clean:
	rm -rf $(BUILD_DIR)/*