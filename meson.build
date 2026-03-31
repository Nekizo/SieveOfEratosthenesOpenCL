# sieve_of_eratosthenes as shere library
project('sieve_of_eratosthenes', 'c')
sources = ['sieve_of_eratosthenes.c', 'sieve_of_eratosthenes.h']

opencl_dep = dependency('OpenCL')

run_command('cp', meson.current_source_dir()+'/sieve_of_eratosthenes.cl', meson.current_build_dir()+'/sieve_of_eratosthenes.cl',check:true)
sieve_of_eratosthenes_lib = shared_library(
    'sieve_of_eratosthenes',
    sources,
    dependencies: opencl_dep,
    )

install_data('sieve_of_eratosthenes.cl')

