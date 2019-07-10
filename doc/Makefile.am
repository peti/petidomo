#
# Petidomo User Manual
#

EXTRA_DIST		= petidomo.tex

htmldir                 = $(pkgdatadir)/html
dist_html_DATA          = petidomo.html petidomo.css

petidomo.html:	$(srcdir)/petidomo.tex
	latexmk $<
	htlatex $<

petidomo.css:	petidomo.html
	touch $@

CLEANFILES = \
  petidomo.4ct petidomo.4tc petidomo.aux petidomo.css petidomo.dvi  \
  petidomo.html petidomo.idv petidomo.lg petidomo.log petidomo.tmp  \
  petidomo.toc petidomo.xref

MAINTAINERCLEANFILES = Makefile.in
