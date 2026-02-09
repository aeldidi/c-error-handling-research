{
  description = "Error-handling analysis article environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/25.11";
  inputs.flake-utils.url = "github:numtide/flake-utils";

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = import nixpkgs { inherit system; };
        llvm = pkgs.llvmPackages_21;
        python = pkgs.python311.withPackages (
          ps: with ps; [
            pandas
            matplotlib
          ]
        );
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = [
            llvm.clang
            llvm.llvm
            llvm.clang-tools
            llvm.libclang
            pkgs.cmake
            pkgs.sqlite
            python
          ];
          DATASET_ROOT = "${self.packages.${system}.datasets}";
          CMAKE_PREFIX_PATH = "${llvm.llvm};${llvm.clang}";
        };

        packages.datasets = import ./datasets.nix { inherit pkgs; };
      }
    );
}
