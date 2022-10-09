{
  description = "High Performance Computing course project 2022";

  inputs.nixpkgs.url = "nixpkgs/nixos-22.05";

  outputs = { self, nixpkgs }:
    let
      pname = "HPC_program";
      supportedSystems = [ "x86_64-linux" ];
      forAllSystems = nixpkgs.lib.genAttrs supportedSystems;
      nixpkgsFor = forAllSystems (system: import nixpkgs { inherit system; overlays = [ self.overlay ]; });
    in
    {
      defaultPackage = stdenv.mkDerivation {
        name = pname;

        buildInputs = with pkgs; [ mpi gnumake ];

        buildPhase = ''
        '';

        installPhase = ''
          mkdir -p $out
        '';
      };
    };
}
