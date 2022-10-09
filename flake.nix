{
  description = "High Performance Computing course project 2022";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.05";

  outputs = { self, nixpkgs }:
    let
      pname = "HPC_program";
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.x86_64-linux;
    in
    {
      packages."${system}".default = pkgs.stdenv.mkDerivation {
        name = pname;
        src = ./.;

        buildInputs = with pkgs;
          [ mpi gnumake ];

        buildPhase = ''
          make CC=mpicc build
        '';

        installPhase = ''
          mkdir -p $out
          cp -r bin $out
        '';
      };
      devShell."${system}" = pkgs.mkShell {
        name = "${pname}_shell";
        packages = with pkgs; [ mpi gnumake ];
      };
    };
}
