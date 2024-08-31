{ pkgs ? import <nixpkgs> {}}:

pkgs.mkShell rec {
  nativeBuildInputs = with pkgs; [
    pkg-config
  ];
  
  buildInputs = with pkgs; [
    pkg-config
    gcc
    clang-tools
    gnumake
    lldb
    bear
    gdb
    moreutils
  ];

  LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath buildInputs;
}
