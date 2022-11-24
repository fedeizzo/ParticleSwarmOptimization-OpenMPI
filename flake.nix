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
        gnumake
        pandoc
        (texlive.combine {
          inherit (texlive)
            scheme-small
            adjustbox babel-german background bidi collectbox csquotes everypage filehook
            footmisc footnotebackref framed fvextra letltxmacro ly1 mdframed mweights
            needspace pagecolor sourcecodepro sourcesanspro titling ucharcat ulem
            unicode-math upquote xecjk xurl zref;
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

    in
    {
      packages."${system}" = {
        report = report;
        particle-swarm-optimization = CPackage;
        default = CPackage;
      };
      devShell."${system}" = pkgs.mkShell {
        name = "${pname}_shell";
        nativeBuildInputs = buildDeps ++ runtimeDeps ++ shellDeps ++ reportDeps;
      };
    };
}
