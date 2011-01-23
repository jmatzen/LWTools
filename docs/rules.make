dirname := $(dir $(lastword $(MAKEFILE_LIST)))

.PHONY: manual
manual: manual-html manual-htmlm manual-pdf

.PHONY: manual-html
manual-html: $(dirname)/manual/manual.html

.PHONY: manual-htmlm
manual-htmlm: $(dirname)/manual/index.html

.PHONY: manual-pdf
manual-pdf: $(dirname)/manual/manual.pdf

$(dirname)/manual/manual.html: $(dirname)/manual.docbook.sgml
	docbook2html -o $(dirname) -u $(dirname)/manual.docbook.sgml && mv $(dirname)/manual.docbook.html $(dirname)/manual/manual.html

$(dirname)/manual/index.html: $(dirname)/manual.docbook.sgml
	docbook2html -o $(dirname)/manual $(dirname)manual.docbook.sgml

$(dirname)/manual/manual.pdf: $(dirname)/manual.docbook.sgml
	docbook2pdf -o $(dirname) -u $(dirname)manual.docbook.sgml && mv $(dirname)/manual.docbook.pdf $(dirname)/manual/manual.pdf && rm -f $(dirname)/manual.docbook.html
