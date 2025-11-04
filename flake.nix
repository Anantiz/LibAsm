{
  description = "libasm project dev environment (42 School)";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in {
      devShells.${system}.default = pkgs.mkShell {
        name = "libasm-dev";
        buildInputs = with pkgs; [
          nasm          # assembler
          gcc           # compiler for linking C test files
          gnumake       # Makefile support
          gdb           # debugging assembly and C
          valgrind      # memory testing
          clang-tools   # optional: linting and formatting tools
        ];

        shellHook = ''
          echo "🧩 Welcome to the libasm dev environment!"
          echo "Available tools: nasm, gcc, make, gdb, valgrind"
          nasm -v
          gcc --version | head -n1
        '';
      };
    };
}
