#
# Petidomo User Manual
#
# $Header$
#

include ../include/petidomo.mk

#
# Targets
#
.PHONY:	all clean distclean realclean

all:	petidomo.ps license.ps

petidomo.ps:		petidomo.dvi
	$(DVIPS) petidomo.dvi -o petidomo.ps

petidomo.dvi:		petidomo.tex ml-principle1.eps ml-principle2.eps directory-struct.eps
	$(LATEX) petidomo.tex
	$(MAKEINDEX) -c petidomo.idx
	$(LATEX) petidomo.tex

ml-principle1.eps:	ml-principle1.fig
	$(FIG2DEV) -L ps ml-principle1.fig ml-principle1.eps

ml-principle2.eps:	ml-principle2.fig
	$(FIG2DEV) -L ps ml-principle2.fig ml-principle2.eps

directory-struct.eps:	directory-struct.fig
	$(FIG2DEV) -L ps directory-struct.fig directory-struct.eps

license.ps:		license.dvi
	$(DVIPS) license.dvi -o license.ps

license.dvi:		license.tex
	$(LATEX) license.tex
	$(MAKEINDEX) -c license.idx
	$(LATEX) license.tex

clean:
	rm -f petidomo.dvi petidomo.log petidomo.aux petidomo.idx petidomo.toc
	rm -f petidomo.ilg petidomo.ind .toc-is-built petidomo.ps
	rm -f ml-principle1.eps ml-principle2.eps directory-struct.eps
	rm -f license.dvi license.log license.aux license.idx license.toc
	rm -f license.ilg license.ind .toc-is-built license.ps
