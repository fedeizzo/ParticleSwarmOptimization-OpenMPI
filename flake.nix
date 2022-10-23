{
  description = "High Performance Computing course project 2022";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.05";

  outputs = { self, nixpkgs }:
    let
      pname = "HPC_program";
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
      ];
      shellDeps = with pkgs; [
        python310
        python310Packages.numpy
        python310Packages.jupyterlab
        python310Packages.pandas
        python310Packages.matplotlib
        python310Packages.scipy
        sqlite-web
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
        CPackages = CPackage;
        docker = dockerImage;
        default = CPackage;
      };
      devShell."${system}" = pkgs.mkShell {
        name = "${pname}_shell";
        packages = buildDeps ++ runtimeDeps ++ shellDeps;
      };
    };
}
