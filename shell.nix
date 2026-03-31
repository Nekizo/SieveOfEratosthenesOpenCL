{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = [
    pkgs.opencl-headers
    pkgs.opencl-clhpp
    pkgs.libclc
    pkgs.ocl-icd
    pkgs.libcxx
    pkgs.glibc
    pkgs.meson
    pkgs.ninja
    pkgs.pkg-config
    pkgs.python313Packages.cython
    pkgs.python313Packages.setuptools
    pkgs.python313
    # pkgs.gsl
  ];
  shellHook = ''
    echo "Welcome to the dev environment!"
  '';
}
