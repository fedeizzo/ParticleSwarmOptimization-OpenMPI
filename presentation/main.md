---
title:
- Particle Swarm Optimization
subtitle:
- A parallelized approach
aspectratio:
- 43
institute:
- University of Trento
author:
- \href{https://github.com/samuelebortolotti}{Samuele Bortolotti}
- \href{https://github.com/fedeizzo}{Federico Izzo}
date:
- \today
lang:
- en-US
section-titles:
- false
theme:
- Copenhagen
colortheme:
- default
navigation:
- horizontal
logo:
- images/logo.pdf
fontsize:
- 10mm
link-citations: true
biblio-style: IEEEtran
nocite: |
  @*
---

# Introduction

## Particle Swarm Optimization

Particle Swarm Optimization is an optimization algorithm for nonlinear functions based on bird swarms.

A particle is characterized by a position $x$, a velocity component $v$ and a performance measure $f(x)$.
Each particle needs to percieve the neighbors position, where $z$ is the position of the best neighboring particle, while $y$ is the particle personal best.

At each step, each particle updates:

$$v' = w \cdot v + \phi_1 U_1 \cdot (y-x) + \phi_2 U_2 \cdot (z-x)$$
$$x' = x+v'$$

# Double column slide

::: {.columns align=center}

:::: column

::::

:::: column

::::

:::

# References {.allowframebreaks}
