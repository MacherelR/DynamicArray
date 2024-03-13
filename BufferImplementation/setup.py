from setuptools import setup, Extension
from Cython.Build import cythonize
import numpy

extra_compile_args=['-std=c++14', '-O3']

extensions = [
    Extension("dynamic_buffer",
              sources=["DynamicBufferWrapper.pyx", "DynamicBuffer.cpp"],
              include_dirs=[numpy.get_include()],
              language="c++",
              extra_compile_args=extra_compile_args)
]

setup(
    name="dynamic_buffer",
    ext_modules=cythonize(extensions, language_level="3"),
    zip_safe=False,
)
