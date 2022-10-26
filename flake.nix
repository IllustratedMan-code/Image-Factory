{
  inputs = {
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        # opencv with tbb support, doesn't seem to affect the time
        # in tests
        opencvtbb = pkgs.opencv.override {
          enableTbb = true;
        };
        image-factory = opencvPackage: with pkgs;
          stdenv.mkDerivation {
            name = "image-factory";
            src = self;
            buildInputs = [ opencvPackage cli11 ];
            nativeBuildInputs = [ ccls bear pkgconfig ];
            installPhase = ''
              mkdir -p $out/bin/
              cp image-factory $out/bin/image-factory
            '';
          };
      in
      {
        packages = flake-utils.lib.flattenTree {
          default = image-factory pkgs.opencv;
        };
      }
    );

}
