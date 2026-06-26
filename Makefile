

C_SOURCES = $(wildcard *.c)
ASM_SOURCES = $(wildcard *.s)

OBJECTS = $(patsubst %.c, %.o, $(C_SOURCES)) $(patsubst %.s, %.o, $(ASM_SOURCES))

HEADERS = $(wildcard *.h)

# Point to our newly installed i686 cross-compiler and linker
CC = i686-linux-gnu-gcc
LD = i686-linux-gnu-ld

# Keep your existing flags intact
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nodefaultlibs -Wall -Wextra -Werror -g
CFLAGS += -m32 -fno-pie -fno-pic -ffreestanding -nostdlib -fverbose-asm -Wno-error=div-by-zero
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
	#qemu-system-i386 -boot d -cdrom os.iso -m 128M -serial stdio -s -S -display none
	bochs -f bochsrc.txt -dbg

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o kernel.elf os.iso iso/
