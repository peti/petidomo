#
# Petidomo User Manual
#
# $Header$
#

.SUFFIXES:
.SUFFIXES:      .tex .dvi .ps

.tex.dvi:
	latex \\nonstopmode\\input $<; \
	while grep 'Rerun to' $*.log; do \
	     latex \\nonstopmode\\input $<; \
	done

.dvi.ps:
	dvips $< -o $@

all:	petidomo.ps

clean distclean realclean:
	rm -f petidomo.aux petidomo.dvi petidomo.log petidomo.ps petidomo.toc
