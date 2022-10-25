{
  inputs = { };
  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      packages."${system}".default = with pkgs; stdenv.mkDerivation {
        name = "image-factory";
        src = self;
        buildInputs = [ opencv cli11 ];
        nativeBuildInputs = [ ccls bear pkgconfig ];
        installPhase = ''
          mkdir -p $out/bin/
          cp image-factory $out/bin/image-factory
        '';
      };

    };

}
