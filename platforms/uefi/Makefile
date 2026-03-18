
ARCH            = x86_64

# Platform-specific objects
PLATFORM_OBJS   = hda_player.o hda_discovery.o uefi_c_shims.o dectalk_efi.o

# DECtalk engine objects (pulled from ../../src via VPATH)
ENGINE_OBJS     = acxf1c.o brent.o charset.o circuit.o cm_char.o cm_cmd.o \
                  cm_copt.o cm_main.o cm_pars.o cm_phon.o cm_text.o cm_util.o \
                  cmd_init.o crypt2.o dbgwins.o decstd97.o dtmmio.o epsonapi.o \
                  frame.o hlframe.o inithl.o llinit.o loadable.o loaddict.o \
                  log10table.o ls_chari.o ls_dict.o ls_homo.o ls_math.o ls_proc.o \
                  ls_spel.o ls_speli.o ls_suff.o ls_suffi.o lsa_adju.o lsa_coni.o \
                  lsa_fr.o lsa_gr.o lsa_ir.o lsa_it.o lsa_ja.o lsa_rtbi.o \
                  lsa_rule.o lsa_sl.o lsa_sp.o lsa_task.o lsa_us.o lsa_util.o \
                  lsw_main.o maindict.o mmalloc.o nasalf1x.o par_ambi.o par_char.o \
                  par_dict.o par_pars.o par_rule.o ph_aloph.o ph_claus.o ph_draw.o \
                  ph_drwt0.o ph_inton.o ph_main.o ph_romi.o ph_setar.o ph_sort.o \
                  ph_syl.o ph_syntx.o ph_task.o ph_timng.o ph_vdefi.o ph_vset.o \
                  phinit.o phlog.o phprint.o playstub.o playtone.o reson.o \
                  sample.o services.o spc.o sqrttable.o usa_init.o voice.o \
                  vtm.o vtmiont.o

OBJS            = $(PLATFORM_OBJS) $(ENGINE_OBJS)

TARGET          = HdaPlayer.efi

# Pull source from local dir and engine src
VPATH           = ../../src

EFIINC          = /usr/include/efi
EFIINCS         = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
EFILIB          = /usr/lib
EFI_CRT_OBJS    = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS         = $(EFILIB)/elf_$(ARCH)_efi.lds

# UEFI-specific CFLAGS
CFLAGS          = $(EFIINCS) -fno-stack-protector -fpic \
                  -fshort-wchar -mno-red-zone -Wall \
                  -fno-asynchronous-unwind-tables \
                  -DEFI_FUNCTION_WRAPPER -O2 \
                  -I. -Iuefi_compat -I../../include -include uefi_shim.h

# DECtalk engine specific defines
$(ENGINE_OBJS): CFLAGS += -fpermissive -w -D_REENTRANT=1 -DNOMME=1 -DLTSSIM=1 -DTTSSIM=1 -DANSI=1 -DBLD_DECTALK_DLL=1 -DENGLISH=1 -DENGLISH_US=1 -DACCESS32=1 -DTYPING_MODE=1 -DACNA=1 -DDISABLE_AUDIO=1 -DSINGLE_THREADED=1 -DNO_FILESYSTEM=1 -D__unix__=1

LDFLAGS         = -nostdlib -znocombreloc -T $(EFI_LDS) -shared \
                  -Bsymbolic -L $(EFILIB) -e _start

all: $(TARGET)

HdaPlayer.so: $(OBJS)
	ld $(LDFLAGS) $(EFI_CRT_OBJS) $(OBJS) -o $@ -lefi -lgnuefi

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym  -j .rel  -j .rela  -j .reloc -j .rodata \
		--target=efi-app-$(ARCH) $^ $@

clean:
	rm -f $(TARGET) *.so $(OBJS)
