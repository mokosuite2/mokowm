MAINTAINERCLEANFILES = Makefile.in

filesdir = $(datadir)/mokosuite/wm
files_DATA = qwo4.portrait.edj vkbd.portrait.edj vkbd.landscape.edj

xmodmapdir = $(sysconfdir)/X11/xmodmap
xmodmap_DATA = mokosuite.xmodmap

bin_SCRIPTS = mokosession

EXTRA_DIST = $(files_DATA) $(xmodmap_DATA) vkbd

clean-local:
	$(RM) *.edj

vkbd.portrait.edj: vkbd/vkbd.portrait.edc
	@edje_cc@ -v -fd vkbd -id vkbd $< $@

vkbd.landscape.edj: vkbd/vkbd.landscape.edc
	@edje_cc@ -v -fd vkbd -id vkbd $< $@

qwo4.portrait.edj: qwo4/qwo4.portrait.edc
	@edje_cc@ -v -fd qwo4 -id qwo4 $< $@
