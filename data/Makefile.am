MAINTAINERCLEANFILES = Makefile.in

filesdir = $(datadir)/mokosuite
files_DATA = vkbd.portrait.edj vkbd.landscape.edj

xmodmapdir = $(sysconfdir)/X11/xmodmap
xmodmap_DATA = mokosuite.xmodmap

bin_SCRIPTS = mokosession

EXTRA_DIST = $(files_DATA) $(xmodmap_DATA) vkbd

clean-local:
	$(RM) $(files_DATA)

vkbd.portrait.edj: vkbd/vkbd.portrait.edc
	@edje_cc@ -fd vkbd -id vkbd $< $@

vkbd.landscape.edj: vkbd/vkbd.landscape.edc
	@edje_cc@ -fd vkbd -id vkbd $< $@
