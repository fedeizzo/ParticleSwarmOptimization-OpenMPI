/*
@licstart  The following is the entire license notice for the
JavaScript code in this file.

Copyright (C) 1997-2019 by Dimitri van Heesch

This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

@licend  The above is the entire license notice
for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "PSO - OpenMPI, OpenMP", "index.html", [
    [ "Particle Swarm Optimization OpenMPI", "index.html#autotoc_md0", [
      [ "Build", "index.html#autotoc_md1", [
        [ "Binary", "index.html#autotoc_md2", null ],
        [ "Docker", "index.html#autotoc_md3", null ],
        [ "Nix", "index.html#autotoc_md4", null ],
        [ "Udocker", "index.html#autotoc_md5", null ]
      ] ],
      [ "Run", "index.html#autotoc_md6", [
        [ "Local", "index.html#autotoc_md7", null ],
        [ "Cluster (through PBS)", "index.html#autotoc_md8", null ]
      ] ],
      [ "Report", "index.html#autotoc_md9", null ],
      [ "Dependencies", "index.html#autotoc_md10", null ]
    ] ],
    [ "LICENSE", "md_LICENSE.html", null ],
    [ "Abstract", "md_report_00-abstract.html", null ],
    [ "01-introduction", "md_report_01-introduction.html", [
      [ "Introduction", "md_report_01-introduction.html#autotoc_md12", [
        [ "Particle Swarm Optimization", "md_report_01-introduction.html#autotoc_md13", [
          [ "Generalities", "md_report_01-introduction.html#autotoc_md14", null ],
          [ "Parametrization", "md_report_01-introduction.html#autotoc_md15", null ],
          [ "Continuous Optimization", "md_report_01-introduction.html#autotoc_md16", null ]
        ] ],
        [ "MPI", "md_report_01-introduction.html#autotoc_md17", null ],
        [ "OpenMP", "md_report_01-introduction.html#autotoc_md18", null ],
        [ "Project generalities", "md_report_01-introduction.html#autotoc_md19", [
          [ "Libraries", "md_report_01-introduction.html#autotoc_md20", null ],
          [ "Build", "md_report_01-introduction.html#autotoc_md21", [
            [ "Compile", "md_report_01-introduction.html#autotoc_md22", null ]
          ] ],
          [ "Execute", "md_report_01-introduction.html#autotoc_md23", null ]
        ] ]
      ] ]
    ] ],
    [ "02-problem-analysis", "md_report_02-problem-analysis.html", [
      [ "Problem Analysis", "md_report_02-problem-analysis.html#autotoc_md24", [
        [ "Program configuration", "md_report_02-problem-analysis.html#autotoc_md25", null ],
        [ "Serial version of the algorithm", "md_report_02-problem-analysis.html#autotoc_md26", [
          [ "Serial algorithm optimization", "md_report_02-problem-analysis.html#autotoc_md27", [
            [ "Particles' initialization", "md_report_02-problem-analysis.html#autotoc_md28", null ],
            [ "Exchange particles' information", "md_report_02-problem-analysis.html#autotoc_md29", null ],
            [ "Sorting algorithm", "md_report_02-problem-analysis.html#autotoc_md30", null ],
            [ "Velocity and position update", "md_report_02-problem-analysis.html#autotoc_md31", null ]
          ] ]
        ] ],
        [ "Parallel version of the algorithm", "md_report_02-problem-analysis.html#autotoc_md32", [
          [ "Architecture", "md_report_02-problem-analysis.html#autotoc_md33", null ],
          [ "Message", "md_report_02-problem-analysis.html#autotoc_md34", null ],
          [ "Communication pattern", "md_report_02-problem-analysis.html#autotoc_md35", null ],
          [ "Logs", "md_report_02-problem-analysis.html#autotoc_md36", null ],
          [ "Output and SQLite", "md_report_02-problem-analysis.html#autotoc_md37", null ]
        ] ]
      ] ]
    ] ],
    [ "03-Benchmarking", "md_report_03-Benchmarking.html", [
      [ "Benchmarking", "md_report_03-Benchmarking.html#autotoc_md38", [
        [ "Problem configuration", "md_report_03-Benchmarking.html#autotoc_md39", null ],
        [ "Cluster jobs", "md_report_03-Benchmarking.html#autotoc_md40", null ],
        [ "Results", "md_report_03-Benchmarking.html#autotoc_md41", null ]
      ] ]
    ] ],
    [ "04-Conclusion", "md_report_04-Conclusion.html", [
      [ "Conclusion", "md_report_04-Conclusion.html#autotoc_md42", [
        [ "Is parallelization always a good choice?", "md_report_04-Conclusion.html#autotoc_md43", null ],
        [ "Thread allocation pattern", "md_report_04-Conclusion.html#autotoc_md44", null ]
      ] ]
    ] ],
    [ "05-DevOps", "md_report_05-DevOps.html", [
      [ "DevOps", "md_report_05-DevOps.html#autotoc_md45", [
        [ "Nix", "md_report_05-DevOps.html#autotoc_md46", null ],
        [ "Docker", "md_report_05-DevOps.html#autotoc_md47", null ],
        [ "GitHub actions", "md_report_05-DevOps.html#autotoc_md48", [
          [ "Container creation", "md_report_05-DevOps.html#autotoc_md49", null ],
          [ "Documentation compilation", "md_report_05-DevOps.html#autotoc_md50", null ]
        ] ],
        [ "Udocker", "md_report_05-DevOps.html#autotoc_md51", [
          [ "Build phase", "md_report_05-DevOps.html#autotoc_md52", null ],
          [ "OpenMPI communication", "md_report_05-DevOps.html#autotoc_md53", null ]
        ] ]
      ] ]
    ] ],
    [ "Introduction", "md_report_short-report.html", [
      [ "Method", "md_report_short-report.html#autotoc_md55", null ],
      [ "Footnotes", "md_report_short-report.html#autotoc_md56", null ],
      [ "Cites", "md_report_short-report.html#autotoc_md57", null ],
      [ "Conclusion", "md_report_short-report.html#autotoc_md58", null ],
      [ "References", "md_report_short-report.html#autotoc_md59", null ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", null ],
        [ "Variables", "functions_vars.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Typedefs", "globals_type.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"structsolution__t.html#a339901c9f46b5c02db2feb76e2e37e3a"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';