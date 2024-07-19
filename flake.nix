{
  inputs.nixpkgs.url = "nixpkgs/nixos-unstable";

  outputs = { self, nixpkgs }:
  let pkgs = nixpkgs.legacyPackages.x86_64-linux;
      deps = with pkgs; [ raylib ];
  in {
    devShells.x86_64-linux.default = pkgs.mkShell {
      buildInputs = deps;
    };
  };
}
