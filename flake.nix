{
  description = "d-SEAMS Lua and Fennel library (require(\"dseams\"))";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    # Source only: meson builds seams-core as a static subproject.
    seams-core = {
      url = "github:d-SEAMS/seams-core/44f91badb1db0a92c3ba8eac29d94ef67dd4348e";
      flake = false;
    };
  };

  outputs =
    { self, nixpkgs, seams-core }:
    let
      inherit (nixpkgs) lib;
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];
      forAllSystems = lib.genAttrs systems;
      pkgsFor = system: nixpkgs.legacyPackages.${system};
    in
    {
      packages = forAllSystems (
        system:
        let
          pkgs = pkgsFor system;
          dseams = pkgs.callPackage ./nix/package.nix {
            seams-core-src = seams-core;
          };
        in
        {
          inherit dseams;
          default = dseams;
        }
      );

      checks = forAllSystems (system: {
        dseams = self.packages.${system}.default;
        default = self.checks.${system}.dseams;
      });

      devShells = forAllSystems (
        system:
        let
          pkgs = pkgsFor system;
          dseams = self.packages.${system}.default;
        in
        {
          default = pkgs.mkShell {
            name = "dseams-dev";
            inputsFrom = [ dseams ];
            packages = with pkgs; [
              lua5_4
              gdb
            ];
            env = {
              LUA_PATH = "${dseams}/share/luadseams/lua/?.lua;;";
              LUA_CPATH = "${dseams}/lib/?.so;;";
            };
          };
        }
      );

      formatter = forAllSystems (system: (pkgsFor system).nixfmt);
    };
}
