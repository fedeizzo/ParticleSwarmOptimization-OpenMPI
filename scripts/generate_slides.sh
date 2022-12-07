#!/bin/sh

cd presentation
pandoc main.md --include-in-header=beamer-template/preamble.latex  --citeproc --bibliography=../report/pandoc-template/bibliography.bib -t beamer -o ../presentation.pdf
