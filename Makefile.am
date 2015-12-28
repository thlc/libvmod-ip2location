ACLOCAL_AMFLAGS = -I m4 -I ${LIBVARNISHAPI_DATAROOTDIR}/aclocal

SUBDIRS = src

doc_DATA = README.rst LICENSE

DISTCHECK_CONFIGURE_FLAGS = \
	VMOD_DIR='$${libdir}/varnish/vmods'

dist_man_MANS = vmod_ip2location.3
MAINTAINERCLEANFILES = $(dist_man_MANS)

vmod_ip2location.3: README.rst

%.1 %.2 %.3 %.4 %.5 %.6 %.7 %.8 %.9:
if HAVE_RST2MAN
	${RST2MAN} $< $@
else
	@echo "========================================"
	@echo "You need rst2man installed to make dist"
	@echo "========================================"
	@false
endif

