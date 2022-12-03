/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
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
    [ "Abstract", "md_report_01_abstract.html", null ],
    [ "Introduction {#sec:introduction}", "md_report_02_introduction.html", [
      [ "Particle Swarm Optimization", "md_report_02_introduction.html#autotoc_md13", [
        [ "Generalities", "md_report_02_introduction.html#autotoc_md14", null ],
        [ "Parametrization", "md_report_02_introduction.html#autotoc_md15", null ],
        [ "Continuous Optimization", "md_report_02_introduction.html#autotoc_md16", null ]
      ] ],
      [ "MPI", "md_report_02_introduction.html#autotoc_md17", null ],
      [ "OpenMP", "md_report_02_introduction.html#autotoc_md18", null ],
      [ "State-of-the art anaysis", "md_report_02_introduction.html#autotoc_md19", null ],
      [ "Project generalities", "md_report_02_introduction.html#autotoc_md20", [
        [ "Libraries", "md_report_02_introduction.html#autotoc_md21", null ],
        [ "Build", "md_report_02_introduction.html#autotoc_md22", [
          [ "Compile", "md_report_02_introduction.html#autotoc_md23", null ]
        ] ],
        [ "Execute", "md_report_02_introduction.html#autotoc_md24", null ]
      ] ]
    ] ],
    [ "Problem Analysis", "md_report_03_problem_analysis.html", [
      [ "Program configuration", "md_report_03_problem_analysis.html#autotoc_md26", null ],
      [ "Serial version of the algorithm", "md_report_03_problem_analysis.html#autotoc_md27", [
        [ "Serial algorithm optimization", "md_report_03_problem_analysis.html#autotoc_md28", [
          [ "Particles' initialization", "md_report_03_problem_analysis.html#autotoc_md29", null ],
          [ "Exchange particles' information", "md_report_03_problem_analysis.html#autotoc_md30", null ],
          [ "Sorting algorithm", "md_report_03_problem_analysis.html#autotoc_md31", null ],
          [ "Velocity and position update", "md_report_03_problem_analysis.html#autotoc_md32", null ]
        ] ]
      ] ],
      [ "Parallel version of the algorithm", "md_report_03_problem_analysis.html#autotoc_md33", [
        [ "Architecture", "md_report_03_problem_analysis.html#autotoc_md34", null ],
        [ "Message", "md_report_03_problem_analysis.html#autotoc_md35", null ],
        [ "Communication pattern", "md_report_03_problem_analysis.html#autotoc_md36", null ],
        [ "Logs", "md_report_03_problem_analysis.html#autotoc_md37", null ],
        [ "Output and SQLite", "md_report_03_problem_analysis.html#autotoc_md38", null ]
      ] ]
    ] ],
    [ "Benchmarking {#sec:benchmarking}", "md_report_04_Benchmarking.html", [
      [ "Problem configuration", "md_report_04_Benchmarking.html#autotoc_md40", null ],
      [ "Cluster jobs", "md_report_04_Benchmarking.html#autotoc_md41", null ],
      [ "Results", "md_report_04_Benchmarking.html#autotoc_md42", null ]
    ] ],
    [ "DevOps", "md_report_05_DevOps.html", [
      [ "Nix", "md_report_05_DevOps.html#autotoc_md44", null ],
      [ "Docker {#sec:docker}", "md_report_05_DevOps.html#autotoc_md45", null ],
      [ "GitHub actions", "md_report_05_DevOps.html#autotoc_md46", [
        [ "Container creation {#sec:container-creation}", "md_report_05_DevOps.html#autotoc_md47", null ],
        [ "Documentation compilation", "md_report_05_DevOps.html#autotoc_md48", null ]
      ] ],
      [ "Udocker", "md_report_05_DevOps.html#autotoc_md49", [
        [ "Build phase", "md_report_05_DevOps.html#autotoc_md50", null ],
        [ "OpenMPI communication", "md_report_05_DevOps.html#autotoc_md51", null ]
      ] ]
    ] ],
    [ "Conclusion", "md_report_06_Conclusion.html", [
      [ "Is parallelization always a good choice?", "md_report_06_Conclusion.html#autotoc_md53", null ],
      [ "Thread allocation pattern", "md_report_06_Conclusion.html#autotoc_md54", null ],
      [ "References", "md_report_06_Conclusion.html#autotoc_md55", null ]
    ] ],
    [ "Introduction", "md_report_short_report.html", [
      [ "Parametrization", "md_report_short_report.html#autotoc_md57", null ],
      [ "Continuous Optimization", "md_report_short_report.html#autotoc_md58", null ],
      [ "State-of-the-art analysis", "md_report_short_report.html#autotoc_md59", null ],
      [ "Method", "md_report_short_report.html#autotoc_md60", null ],
      [ "Footnotes", "md_report_short_report.html#autotoc_md61", null ],
      [ "Cites", "md_report_short_report.html#autotoc_md62", null ],
      [ "Conclusion", "md_report_short_report.html#autotoc_md63", null ],
      [ "References", "md_report_short_report.html#autotoc_md64", null ]
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
"annotated.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';