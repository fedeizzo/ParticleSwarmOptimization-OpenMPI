(TeX-add-style-hook
 "bibliography"
 (lambda ()
   (LaTeX-add-bibitems
    "KennedyEberhart"
    "NedJahMoraes"
    "MoraesMitre"))
 '(or :bibtex :latex))

