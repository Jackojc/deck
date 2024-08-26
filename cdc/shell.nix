{ pkgs ? import <nixpkgs> {}}:

pkgs.mkShell rec {
  nativeBuildInputs = with pkgs; [
    pkg-config
  ];
  
  buildInputs = with pkgs; [
    # tokei
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
  # LOG_FILTER = "trace";

  # shellHook = ''
  #   export PATH="$PWD/util:$PATH"
  # '';
}
