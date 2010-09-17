DESCRIPTION = "A minimal X11 window manager for the Mokosuite"
HOMEPAGE = "http://gitorious.org/mokosuite2"
AUTHOR = "Daniele Ricci"
LICENSE = "GPLv3"
DEPENDS = "glib-2.0 ecore evas edje"
SECTION = "x11/clients"

PV = "0.1+gitr${SRCPV}"
#SRCREV = "719a200354d9b7aa825de75d19169067fd4871b0"

SRC_URI = "git://git.gitorious.org/mokosuite2/mokowm.git;proto=http"
S = "${WORKDIR}/mokowm"

CFLAGS += "-DOPENMOKO"
EXTRA_OECONF = " --with-edje-cc=${STAGING_BINDIR_NATIVE}/edje_cc"
FILES_${PN} += "${datadir}/mokosuite ${sysconfdir}/X11"

inherit autotools update-alternatives

ALTERNATIVE_PATH = "${bindir}/mokosession"
ALTERNATIVE_NAME = "x-window-manager"
ALTERNATIVE_LINK = "${bindir}/x-window-manager"
ALTERNATIVE_PRIORITY = "20"
