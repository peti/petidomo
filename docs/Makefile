#
# Petidomo User Manual
#
# $Header$
#

all:	petidomo.ps

%.dvi : %.tex
	latex \\nonstopmode\\input $<; \
	while egrep 'Rerun to' $*.log; do \
	     latex \\nonstopmode\\input $<; \
	done

%.ps : %.dvi
	dvips $< -o $@

%.eps:	%.fig
	fig2dev -L ps $< $@

petidomo.dvi:		ml-principle1.eps ml-principle2.eps directory-struct.eps

ml-principle1.eps:	ml-principle1.fig
ml-principle2.eps:	ml-principle2.fig
directory-struct.eps:	directory-struct.fig

clean distclean realclean:
	rm -f petidomo.aux petidomo.dvi petidomo.idx petidomo.log petidomo.ps petidomo.toc
	rm -f ml-principle1.eps ml-principle2.eps directory-struct.eps
