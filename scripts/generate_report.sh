#!/bin/sh
pandoc report/*.md \
       -o report.pdf \
       --from markdown \
       --template ./pandoc-template/eisvogel.latex \
       --metadata-file ./pandoc-template/metadata.yaml \
       --listings \
       --number-sections \
       --highlight-style pygments
