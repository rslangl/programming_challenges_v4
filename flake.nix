{
  description = "Reproducible Rust dev environment without rustup";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: flake-utils.lib.eachDefaultSystem (system:
    let
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells = {
        default = pkgs.mkShell {
          buildInputs = [
            pkgs.rust-analyzer
            # NOTE:replace with `rust-bin.nightly` if needed
            pkgs.rustc
            pkgs.cargo
            pkgs.clang
            pkgs.pkg-config
            pkgs.openssl
            pkgs.zlib
            pkgs.rustfmt
            pkgs.clippy
          ];

          shellHook = ''
            export CARGO_TARGET_X86_64_UNKNOWN_LINUX_GNU_LINKER=${pkgs.clang}/bin/clang
            export RUSTFLAGS="-C linker=${pkgs.clang}/bin/clang"
            echo "🦀 Rust dev shell ready! Rust version: $(rustc --version)"

            echo "Available Rust tools:"
            for tool in rustc cargo rustfmt clippy rust-analyzer; do
              if command -v $tool >/dev/null 2>&1; then
                echo -n "  $tool: "
                $tool --version 2>/dev/null || echo "version info not available"
              fi
            done
          '';
        };
      };
    }
  );
}
