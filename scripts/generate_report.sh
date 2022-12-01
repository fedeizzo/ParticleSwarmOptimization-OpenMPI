#!/bin/sh
cd report

if [[ $1 == "--long" ]]; then
    files=$(find . -name '0*.md' | sort)
    pandoc $files \
	-o ../report.pdf \
	-V fontsize=10pt \
	--from markdown \
	--template ./pandoc-template/eisvogel.latex \
	--metadata-file ./pandoc-template/eisvogel-metadata.yaml \
	--listings \
	--number-sections \
	--highlight-style pygments
elif [[ $1 == "--short" ]]; then
    pandoc short-report.md \
	-o ../short-report.pdf \
	-V fontsize=9pt \
	--from markdown \
	--template ./pandoc-template/ieee-template.latex \
	--metadata-file ./pandoc-template/ieee-metadata.yaml \
	--bibliography=./pandoc-template/bibliography.bib \
	--csl=./pandoc-template/bibliography.csl \
	--listings \
	--number-sections \
	--highlight-style pygments
else
    echo "usage $0: only allowed options are --long and --shrt"
fi
    
