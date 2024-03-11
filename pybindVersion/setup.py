from setuptools import setup, Extension
import pybind11

setup(
    name='dynamic_array',
    version='0.1',
    ext_modules=[
        Extension(
            'dynamic_array',
            ['DynamicArrayWrapper.cpp'],
            include_dirs=[pybind11.get_include()],
            language='c++',
            extra_compile_args=['-std=c++14', '-O3'],
        ),
    ],
    zip_safe=False,
)
