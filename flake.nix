{
  description = "A very basic flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
  };

  outputs = {
    self,
    nixpkgs,
  }:
    with import nixpkgs {system = "x86_64-linux";}; {
      packages.x86_64-linux = rec {
        default = workrave-qt;

        workrave-qt = pkgs.stdenv.mkDerivation {
          pname = "WorkraveQt";
          version = "0.3";
          src = ./src;
          cmakeFlags = [
            "-DWARNINGS=ON"
            "-DWERROR=ON"
          ];
          qmakeFlags = ["WorkraveQt.pro"];

          installPhase = let
            desktopItem = pkgs.makeDesktopItem {
              name = "WorkraveQt";
              exec = "WorkraveQt";
              icon = "sheep";
              desktopName = "WorkraveQt";
              genericName = "WorkraveQt";
              categories = ["Utility"];
            };
          in ''
            install -Dm755 WorkraveQt -t $out/bin
            install -Dm644 images/sheep.svg -t $out/share/icons/hicolor/scalable/apps

            # Create Desktop Item
            mkdir -p "$out/share/applications"
            ln -s "${desktopItem}"/share/applications/* "$out/share/applications/"
          '';

          nativeBuildInputs = with pkgs; [
            libsForQt5.qmake
            pkgconfig
            qt5.wrapQtAppsHook
          ];

          buildInputs = with pkgs; [
            qt5.qtbase
          ];
        };
      };
    defaultPackage.x86_64-linux = self.packages.x86_64-linux.workrave-qt;
    };
}
