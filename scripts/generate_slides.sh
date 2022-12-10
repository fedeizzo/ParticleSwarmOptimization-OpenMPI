#!/bin/sh

cd presentation
pandoc main.md \
       --include-in-header=beamer-template/preamble.latex \
       --bibliography=../report/pandoc-template/bibliography.bib \
       --citeproc \
	--filter pandoc-fignos \
	--filter pandoc-eqnos \
	--filter pandoc-secnos \
       -t beamer \
       -o ../presentation.pdf
