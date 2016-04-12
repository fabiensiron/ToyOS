# ToyOS Primary Build Script
ifneq ($(MAKECMDGOALS),toolchain)
 ifeq ($(TOOLCHAIN),)
  $(error No toolchain available and you did not ask to build it. Did you forget to source the toolchain config?)
 endif
endif


# We always build with our targetted cross-compiler
# TODO: Change this to i686-pc-toyos
CC = i686-pc-toyos-gcc
NM = i686-pc-toyos-nm
CXX= i686-pc-toyos-g++
AR = i686-pc-toyos-ar
AS = i686-pc-toyos-as

# Build flags
CFLAGS  = -O2 -std=c99
CFLAGS += -finline-functions -ffreestanding
CFLAGS += -Wall -Wextra -Wno-unused-function -Wno-unused-parameter -Wno-format
CFLAGS += -pedantic -fno-omit-frame-pointer
CFLAGS += -D_KERNEL_

ASFLAGS = --32

# Kernel autoversioning with git sha
CFLAGS += -DKERNEL_GIT_TAG=`util/make-version`

# We have some pieces of assembly sitting around as well...
YASM = yasm

# All of the core parts of the kernel are built directly.
KERNEL_OBJS = $(patsubst %.c,%.o,$(wildcard kernel/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/*/*.c))
KERNEL_OBJS += $(patsubst %.c,%.o,$(wildcard kernel/*/*/*.c))

# Loadable modules
MODULES = $(patsubst modules/%.c,hdd/mod/%.ko,$(wildcard modules/*.c))

# We also want to rebuild when a header changes.
# This is a naive approach, but it works...
HEADERS     = $(shell find kernel/include/ -type f -name '*.h')

# Userspace build flags
USER_CFLAGS   = -O3 -m32 -Wa,--32 -g -Iuserspace -std=c99 -U__STRICT_ANSI__
USER_CXXFLAGS = -O3 -m32 -Wa,--32 -g -Iuserspace
USER_BINFLAGS =

# Userspace binaries and libraries
USER_CFILES   = $(shell find userspace -not -wholename '*/lib/*' -name '*.c')
USER_CXXFILES = $(shell find userspace -not -wholename '*/lib/*' -name '*.c++')
USER_LIBFILES = $(shell find userspace -wholename '*/lib/*' -name '*.c')

# Userspace output files (so we can define metatargets)
USERSPACE  = $(foreach file,$(USER_CFILES),$(patsubst %.c,hdd/bin/%,$(notdir ${file})))
USERSPACE += $(foreach file,$(USER_CXXFILES),$(patsubst %.c++,hdd/bin/%,$(notdir ${file})))
USERSPACE += $(foreach file,$(USER_LIBFILES),$(patsubst %.c,%.o,${file}))

CORE_LIBS = $(patsubst %.c,%.o,$(wildcard userspace/lib/*.c))

# Pretty output utilities.
BEG = util/mk-beg
END = util/mk-end
INFO = util/mk-info
ERRORS = 2>>/tmp/.`whoami`-build-errors || util/mk-error
ERRORSS = >>/tmp/.`whoami`-build-errors || util/mk-error
BEGRM = util/mk-beg-rm
ENDRM = util/mk-end-rm

# Hard disk image generation
GENEXT = genext2fs
DISK_SIZE = `util/disk_size.sh`
DD = dd conv=notrunc

# Specify which modules should be included on startup.
# There are a few modules that are kinda required for a working system
# such as all of the dependencies needed to mount the root partition.
# We can also include things like the debug shell...
# Note that ordering matters - list dependencies first.
BOOT_MODULES := zero random serial
BOOT_MODULES += procfs tmpfs ata
#BOOT_MODULES += dospart
BOOT_MODULES += ext2
BOOT_MODULES += debug_shell
BOOT_MODULES += ps2mouse ps2kbd
BOOT_MODULES += lfbvideo
BOOT_MODULES += packetfs
BOOT_MODULES += snd
BOOT_MODULES += pcspkr
BOOT_MODULES += ac97
BOOT_MODULES += net rtl

# This is kinda silly. We're going to form an -initrd argument..
# which is basically -initrd "hdd/mod/%.ko,hdd/mod/%.ko..."
# for each of the modules listed above in BOOT_MODULES
COMMA := ,
EMPTY :=
SPACE := $(EMPTY) $(EMPTY)
BOOT_MODULES_X = -initrd "$(subst $(SPACE),$(COMMA),$(foreach mod,$(BOOT_MODULES),hdd/mod/$(mod).ko))"

# Emulator settings
EMU = qemu-system-i386
EMUARGS  = -sdl -kernel toyos-kernel -m 1024
EMUARGS += -serial stdio -vga std
EMUARGS += -hda toyos-disk.img -k en-us
EMUARGS += -rtc base=localtime -net nic,model=rtl8139 -net user -soundhw pcspk,ac97
EMUARGS += -net dump -no-kvm-irqchip
EMUARGS += $(BOOT_MODULES_X)
EMUKVM   = -enable-kvm -cpu host

DISK_ROOT = root=/dev/hda
VID_QEMU  = vid=qemu,,1280,,720
START_VGA = start=--vga
START_SINGLE = start=--single
START_LIVE = start=live-welcome
WITH_LOGS = logtoserial=1

.PHONY: all system install test toolchain userspace modules cdrom toyos.iso
.PHONY: clean clean-soft clean-hard clean-user clean-mods clean-core clean-disk clean-once
.PHONY: run vga term headless
.PHONY: kvm vga-kvm term-kvm headless-kvm
.PHONY: debug debug-kvm debug-term debug-term-kvm

# Prevents Make from removing intermediary files on failure
.SECONDARY:

# Disable built-in rules
.SUFFIXES:

all: system tags userspace
system: toyos-disk.img toyos-kernel modules
userspace: ${USERSPACE}
modules: ${MODULES}

# Various different quick options
run: system
	${EMU} ${EMUARGS} -append "$(VID_QEMU) $(DISK_ROOT)"
kvm: system
	${EMU} ${EMUARGS} ${EMUKVM} -append "$(VID_QEMU) $(DISK_ROOT)"
debug: system
	${EMU} ${EMUARGS} -append "$(VID_QEMU) $(WITH_LOGS) $(DISK_ROOT)"
debug-kvm: system
	${EMU} ${EMUARGS} ${EMUKVM} -append "$(VID_QEMU) $(WITH_LOGS) $(DISK_ROOT)"
vga: system
	${EMU} ${EMUARGS} -append "$(START_VGA) $(DISK_ROOT)"
vga-kvm: system
	${EMU} ${EMUARGS} ${EMUKVM} -append "$(START_VGA) $(DISK_ROOT)"
debug-vga: system
	${EMU} ${EMUARGS} -append "$(WITH_LOGS) $(START_VGA) $(DISK_ROOT)"
term: system
	${EMU} ${EMUARGS} -append "$(VID_QEMU) $(START_SINGLE) $(DISK_ROOT)"
term-kvm: system
	${EMU} ${EMUARGS} ${EMUKVM} -append "$(VID_QEMU) $(START_SINGLE) $(DISK_ROOT)"
debug-term: system
	${EMU} ${EMUARGS} -append "$(VID_QEMU) $(START_SINGLE) $(WITH_LOGS) $(DISK_ROOT)"
debug-term-kvm: system
	${EMU} ${EMUARGS} ${EMUKVM} -append "$(VID_QEMU) $(START_SINGLE) $(WITH_LOGS) $(DISK_ROOT)"
headless: system
	${EMU} ${EMUARGS} -display none -append "$(START_VGA) $(DISK_ROOT)"
headless-kvm: system
	${EMU} ${EMUARGS} ${EMUKVM} -display none -append "$(START_VGA) $(DISK_ROOT)"
live: system
	${EMU} ${EMUARGS} -append "$(VID_QEMU) $(START_LIVE) $(DISK_ROOT)"
live-kvm: system
	${EMU} ${EMUARGS} ${EMUKVM} -append "$(VID_QEMU) $(START_LIVE) $(DISK_ROOT)"

test: system
	expect util/test.exp

toolchain:
	@cd toolchain; ./toolchain-build.sh

KERNEL_ASMOBJS = $(filter-out kernel/symbols.o,$(patsubst %.S,%.o,$(wildcard kernel/*.S)))

################
#    Kernel    #
################
toyos-kernel: ${KERNEL_ASMOBJS} ${KERNEL_OBJS} kernel/symbols.o
	@${BEG} "CC" "$<"
	@${CC} -T kernel/link.ld ${CFLAGS} -nostdlib -o toyos-kernel ${KERNEL_ASMOBJS} ${KERNEL_OBJS} kernel/symbols.o -lgcc ${ERRORS}
	@${END} "CC" "$<"
	@${INFO} "--" "Kernel is ready!"

kernel/symbols.o: ${KERNEL_ASMOBJS} ${KERNEL_OBJS} util/generate_symbols.py
	@-rm -f kernel/symbols.o
	@${BEG} "NM" "Generating symbol list..."
	@${CC} -T kernel/link.ld ${CFLAGS} -nostdlib -o toyos-kernel ${KERNEL_ASMOBJS} ${KERNEL_OBJS} -lgcc ${ERRORS}
	@${NM} toyos-kernel -g | python2 util/generate_symbols.py > kernel/symbols.S
	@${END} "NM" "Generated symbol list."
	@${BEG} "AS" "kernel/symbols.S"
	@${AS} ${ASFLAGS} kernel/symbols.S -o $@ ${ERRORS}
	@${END} "AS" "kernel/symbols.S"

kernel/sys/version.o: kernel/*/*.c kernel/*.c

hdd/mod/%.ko: modules/%.c ${HEADERS}
	@${BEG} "CC" "$< [module]"
	@${CC} -T modules/link.ld -I./kernel/include -nostdlib ${CFLAGS} -c -o $@ $< ${ERRORS}
	@${END} "CC" "$< [module]"

kernel/%.o: kernel/%.S
	@${BEG} "AS" "$<"
	@${AS} ${ASFLAGS} $< -o $@ ${ERRORS}
	@${END} "AS" "$<"

kernel/%.o: kernel/%.c ${HEADERS}
	@${BEG} "CC" "$<"
	@${CC} ${CFLAGS} -nostdlib -g -I./kernel/include -c -o $@ $< ${ERRORS}
	@${END} "CC" "$<"

#############
# Userspace #
#############

# Libraries
userspace/%.o: userspace/%.c
	@${BEG} "CC" "$<"
	@${CC} ${USER_CFLAGS} $(shell util/auto-dep.py --cflags $<) -c -o $@ $< ${ERRORS}
	@${END} "CC" "$<"

# Binaries from C sources
define user-c-rule
$1: $2 $(shell util/auto-dep.py --deps $2)
	@${BEG} "CC" "$$<"
	@${CC} -o $$@ $(USER_CFLAGS) $(USER_BINFLAGS) $$(shell util/auto-dep.py --cflags $$<) $$< $$(shell util/auto-dep.py --libs $$<) ${ERRORS}
	@${END} "CC" "$$<"
endef
$(foreach file,$(USER_CFILES),$(eval $(call user-c-rule,$(patsubst %.c,hdd/bin/%,$(notdir ${file})),${file})))

# Binaries from C++ sources
define user-cxx-rule
$1: $2 $(shell util/auto-dep.py --deps $2)
	@${BEG} "C++" "$$<"
	@${CXX} -o $$@ $(USER_CXXFLAGS) $(USER_BINFLAGS) $$(shell util/auto-dep.py --cflags $$<) $$< $$(shell util/auto-dep.py --libs $$<) ${ERRORS}
	@${END} "C++" "$$<"
endef
$(foreach file,$(USER_CXXFILES),$(eval $(call user-cxx-rule,$(patsubst %.c++,hdd/bin/%,$(notdir ${file})),${file})))

hdd/usr/lib/libtoyos.a: ${CORE_LIBS}
	@${BEG} "AR" "$@"
	@${AR} rcs $@ ${CORE_LIBS}
	@mkdir -p hdd/usr/include/toyos
	@cp userspace/lib/*.h hdd/usr/include/toyos/
	@${END} "AR" "$@"

####################
# Hard Disk Images #
####################

toyos-disk.img: ${USERSPACE} util/devtable
	@${BEG} "hdd" "Generating a Hard Disk image..."
	@-rm -f toyos-disk.img
	@${GENEXT} -B 4096 -d hdd -D util/devtable -U -b ${DISK_SIZE} -N 6144 toyos-disk.img ${ERRORS}
	@${END} "hdd" "Generated Hard Disk image"
	@${INFO} "--" "Hard disk image is ready!"

#########
# cdrom #
#########

cdrom: toyos.iso

toyos.iso:
	util/make-cdrom.sh

##############
#    ctags   #
##############
tags: kernel/*/*.c kernel/*.c userspace/**/*.c modules/*.c
	@${BEG} "ctag" "Generating CTags..."
	@-ctags -R --c++-kinds=+p --fields=+iaS --extra=+q kernel userspace modules util ${ERRORS}
	@${END} "ctag" "Generated CTags."

###############
#    clean    #
###############

clean-soft:
	@${BEGRM} "RM" "Cleaning kernel objects..."
	@-rm -f kernel/*.o
	@-rm -f kernel/*/*.o
	@-rm -f ${KERNEL_OBJS}
	@${ENDRM} "RM" "Cleaned kernel objects"

clean-user:
	@${BEGRM} "RM" "Cleaning userspace products..."
	@-rm -f ${USERSPACE}
	@${ENDRM} "RM" "Cleaned userspace products"

clean-mods:
	@${BEGRM} "RM" "Cleaning kernel modules..."
	@-rm -f hdd/mod/*
	@${ENDRM} "RM" "Cleaned kernel modules"

clean-core:
	@${BEGRM} "RM" "Cleaning final output..."
	@-rm -f toyos-kernel
	@${ENDRM} "RM" "Cleaned final output"

clean-disk:
	@${BEGRM} "RM" "Deleting hard disk image..."
	@-rm -f toyos-disk.img
	@${ENDRM} "RM" "Deleted hard disk image"

clean: clean-soft clean-core
	@${INFO} "--" "Finished soft cleaning"

clean-hard: clean clean-user clean-mods
	@${INFO} "--" "Finished hard cleaning"


# vim:noexpandtab
# vim:tabstop=4
# vim:shiftwidth=4
