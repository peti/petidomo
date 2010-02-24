#
# Petidomo User Manual
#

htmldir                 = $(pkgdatadir)/html
dist_html_DATA          = petidomo.html petidomo.css

petidomo.html:	petidomo.tex
	latex $<
	htlatex $<
