from setuptools import setup, Extension
from Cython.Build import cythonize
import numpy

extra_compile_args=['-std=c++14', '-O3']

extensions = [
    Extension("dynamic_array_cython",
              sources=["DynamicArrayWrapper.pyx", "DynamicArrayCython.cpp", "DynamicCounterCython.cpp"],
              include_dirs=[numpy.get_include()],
              language="c++",
              extra_compile_args=extra_compile_args)
]

setup(
    name="dynamic_array_cython",
    ext_modules=cythonize(extensions, language_level="3"),
    zip_safe=False,
)
