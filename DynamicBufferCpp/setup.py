import numpy
from Cython.Build import cythonize
from setuptools import setup, Extension, find_packages
import os

BASE_DIR = os.path.dirname(os.path.abspath(__file__))
extra_compile_args = ['-std=c++14', '-static', '-static-libgcc', '-static-libstdc++', '-O3']

extensions = [
    Extension("dynamic_buffer",
              sources=[
                  os.path.join(BASE_DIR, "src", "DynamicBufferWrapper.pyx"),  # path to your .pyx file
                  os.path.join(BASE_DIR, "src", "DynamicBuffer_lib", "DynamicBuffer.cpp"),  # path to your .cpp file
                  os.path.join(BASE_DIR, "src", "DynamicBuffer_lib", "LastKnownValuesBuffer.cpp"),  # and so on for other files
              ],
              include_dirs=[numpy.get_include()],
              language="c++",
              extra_compile_args=extra_compile_args)
]

setup(
    name="dynamic_buffer",
    packages=find_packages(include=[os.path.join(BASE_DIR, "src", "DynamicBuffer_lib")]),
    ext_modules=cythonize(extensions, language_level="3"),
    zip_safe=False,
)
