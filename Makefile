
BUILD_DIR = out

SRCDIR   := src

C_SOURCES     := $(shell find $(SRCDIR) -type f -name "*.c")
ASM_SOURCES := $(shell find $(SRCDIR) -type f -name "*.s")

C_OBJS   := $(patsubst $(SRCDIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
ASM_OBJS := $(patsubst $(SRCDIR)/%.s, $(BUILD_DIR)/%.o, $(ASM_SOURCES))

OBJECTS := $(C_OBJS) $(ASM_OBJS)

HEADERS = $(wildcard *.h)

# Point to our newly installed i686 cross-compiler and linker
CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-ld

INC_DIRS := $(shell find $(SRCDIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Keep your existing flags intact
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nodefaultlibs -Wall -Wextra -Werror -g
CFLAGS += -m32 -fno-pie -fno-pic -ffreestanding -nostdlib -fverbose-asm -Wno-error=div-by-zero $(INC_FLAGS) -MMD -MP
LDFLAGS = -T link.ld -m elf_i386 -g
AS = nasm
ASFLAGS = -f elf32 -g

all: kernel.elf os.iso

kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $(OBJECTS) -o kernel.elf
os.iso: kernel.elf
	rm -f os.iso
	rm -rf /tmp/iso_staging
	
	# 1. Build the target directory path tree
	mkdir -p /tmp/iso_staging/boot/grub
	
	# 2. Stage the workspace assets
	cp kernel.elf /tmp/iso_staging/boot/kernel.elf
	cp stage2_eltorito /tmp/iso_staging/boot/grub/stage2_eltorito
	cp menu.lst /tmp/iso_staging/boot/grub/menu.lst
	
	# 3. Burn a sector-padded, standard-compliant El Torito image track
	genisoimage -R                              \
	            -b boot/grub/stage2_eltorito    \
	            -c boot/grub/boot.catalog       \
	            -no-emul-boot                   \
	            -boot-load-size 4               \
	            -pad                            \
	            -sysid "ISO9660"                \
	            -A os                           \
	            -input-charset utf8             \
	            -quiet                          \
	            -boot-info-table                \
	            -d                              \
	            -N                              \
	            -o os.iso                       \
	            /tmp/iso_staging
	
	rm -rf /tmp/iso_staging
build: os.iso
run: os.iso
	qemu-system-i386 -boot d -cdrom os.iso -m 128M -serial stdio -s -S -display none -vnc :0
	#bochs -f bochsrc.txt -dbg

qemu: os.iso
	qemu-system-i386 -boot d -cdrom os.iso -m 128M -serial stdio  -display none -vnc :0 -no-reboot

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Assembles src/boot/header.s into out/boot/header.o
$(BUILD_DIR)/%.o: $(SRCDIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

clangd:
	rm -f compile_flags.txt
	@echo "-m32" >> compile_flags.txt
	@echo "-ffreestanding" >> compile_flags.txt
	@for dir in $$(find src -type d); do \
		echo "-I$$dir" >> compile_flags.txt; \
	done

clean:
	rm -rf *.o kernel.elf os.iso iso/ out/
