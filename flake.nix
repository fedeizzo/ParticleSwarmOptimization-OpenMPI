{
  description = "High Performance Computing course project 2022";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.05";
  outputs = { self, nixpkgs }:
    let
      pname = "pso";
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
      runtimeDeps = with pkgs; [
        mpi
        sqlite
      ];
      buildDeps = with pkgs; [
        gnumake
        pkg-config
        glib
        llvmPackages_13.openmp
      ];
      reportDeps = with pkgs; [
        pandoc
        texlive.combined.scheme-full
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
        pandoc
      ];
      CPackage = pkgs.stdenv.mkDerivation {
        name = pname;
        src = ./.;

        buildInputs = runtimeDeps;
        depsBuildBuild = buildDeps;
        buildPhase = ''
          make CC=mpicc build
        '';

        installPhase = ''
          mkdir -p $out
          cp -r bin $out
        '';
      };
      report = pkgs.stdenv.mkDerivation rec {
        name = "report";
        src = ./.;
        # builder = ./generate_report.sh;
        nativeBuildInputs = reportDeps;
        PATH = pkgs.lib.makeBinPath nativeBuildInputs;
        buildPhase = ''
          ./generate_report.sh
        '';
        installPhase = ''
          mkdir -p $out
          cp report.pdf $out
        '';
      };

      dockerImage = pkgs.dockerTools.buildImage {
        name = pname;
        contents = CPackage;
        fromImageName = "scratch";
        config = {
          # Entrypoint = [ "/bin/mpirun" ];
        };
      };

    in
    {
      packages."${system}" = {
        report = report;
        CPackages = CPackage;
        docker = dockerImage;
        default = CPackage;
      };
      devShell."${system}" = pkgs.mkShell {
        name = "${pname}_shell";
        nativeBuildInputs = buildDeps ++ runtimeDeps ++ shellDeps;
      };
    };
}
