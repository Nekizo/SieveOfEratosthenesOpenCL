from setuptools import Extension, setup
from Cython.Build import cythonize

setup(
    ext_modules = cythonize([
        Extension(
        "sieve_of_eratosthenes_binding", 
        [
            "sieve_of_eratosthenes_binding.pyx",
            "sieve_of_eratosthenes.c",
        ],
        libraries=[
            "OpenCL",
            ],
        )
    ])
)
