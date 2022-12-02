{
  description = "High Performance Computing course project 2022";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.05";
  outputs = { self, nixpkgs }:
    let
      pname = "pso";
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
      runtimeDeps = with pkgs; [
      ];
      buildDeps = with pkgs; [
        mpi
        sqlite
        gnumake
        pkg-config
        glib
        llvmPackages_13.openmp
      ];
      reportDeps = with pkgs; [
        doxygen
        gnumake
        pandoc
        (texlive.combine {
          inherit (texlive)
            scheme-small
            adjustbox babel-german background bidi collectbox csquotes everypage filehook
            footmisc footnotebackref framed fvextra letltxmacro ly1 mdframed mweights
            needspace pagecolor sourcecodepro sourcesanspro titling ucharcat ulem
            unicode-math upquote xecjk xurl zref algorithms algorithm2e ifoddpage
            algpseudocodex algorithmicx hyperref ieeetran supertabular standalone preview
            relsize;
        })
      ];
      shellDeps = with pkgs; [
        python39
        python39Packages.numpy
        python39Packages.jupyterlab
        python39Packages.pandas
        python39Packages.matplotlib
        python39Packages.scipy
        sqlite-web
        doxygen
        check
      ];
      CPackage = pkgs.stdenv.mkDerivation {
        name = pname;
        src = ./.;

        buildInputs = buildDeps;

        buildPhase = ''
          make CC=mpicc build
        '';

        installPhase = ''
          mkdir -p $out
          cp -r bin/particle-swarm-optimization $out
        '';
      };
      report = pkgs.stdenv.mkDerivation rec {
        name = "report";
        src = ./.;
        buildInputs = reportDeps;
        buildPhase = ''
          make report
        '';
        installPhase = ''
          mkdir -p $out
          cp report.pdf $out
        '';
      };
      shortReport = pkgs.stdenv.mkDerivation rec {
        name = "report";
        src = ./.;
        buildInputs = reportDeps;
        buildPhase = ''
          make REPORT_TYPE=short report
        '';
        installPhase = ''
          mkdir -p $out
          cp short-report.pdf $out
        '';
      };
      doxygen-awesome = pkgs.fetchFromGitHub {
        owner = "jothepro";
        repo = "doxygen-awesome-css";
        rev = "a3c119b4797be2039761ec1fa0731f038e3026f6";
        sha256 = "sha256-kslmiwgezjPz7EuMBt3c12bAzGYLjIE1otetw2ccNzE=";
      };
      documentation = pkgs.stdenv.mkDerivation rec {
        name = "documentation";
        src = ./.;
        buildInputs = reportDeps;
        buildPhase = ''
          mkdir -p doxygen-awesome-css
          cp -r ${doxygen-awesome}/* doxygen-awesome-css/
          cd report
          find . -name '*.md' | xargs sed -i 's/{ width=.*px }//g'
          find . -name '*.md' | xargs sed -i 's/\\newpage//g'
          cd ..
          make doc
          touch docs/html/.nojekyll
          make report
          make REPORT_TYPE=short report
        '';
        installPhase = ''
          mkdir -p $out
          cp report.pdf $out/
          cp short-report.pdf $out/
          cp -a docs/html/* $out/
        '';
      };
    in
    {
      packages."${system}" = {
        report = report;
        shortReport = shortReport;
        particle-swarm-optimization = CPackage;
        documentation = documentation;
        default = CPackage;
      };
      devShell."${system}" = pkgs.mkShell {
        name = "${pname}_shell";
        nativeBuildInputs = buildDeps ++ runtimeDeps ++ shellDeps ++ reportDeps;
      };
    };
}
