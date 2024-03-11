# Dynamic map implementation

This project was designed in order to optimize the use of a btree in a python app. The idea was to optimize some btree operations, such as insert, delete and search. The project was developed in C++ and the python interface was made using the pybind11 library.

## How to use

Generate the shared library using the following command:

```bash
python setup.py build_ext --inplace
```

This will generate an .so file that can be used in a python app. The file will be generated in the same directory as the setup.py file.
C++ code is currently built using 03 flag for optimization. This can be changed in the setup.py file.

## External libraries

This code uses btre library from the following repository: https://github.com/Kronuz/cpp-btree/tree/master

## Specs
The main class is the one called DynamicArray. It contains the methods and functionalities needed for my personal use case. Those can be extended for further use cases.
A second class called DynamicCounter is actually a child of DynamicArray, tailored for a more specific use case (acting as a counter). This was created separatelly to avoid "polluting" the main class with specific methods and attributes.

## Cython version

In the purpose of maximizing the performances, the code was also adapted to Cython. The file dynamic_array_cython.pyx contains the Cython version of the code. The setup.py file was also adapted to compile the Cython code. The compilation can be done using the following command:

```bash
cd CythonVersion/
python setup.py build_ext --inplace
```

This will generate a .so combining the c++ and the Cython code file that can be used in a python app. The file will be generated in the same directory as the setup.py file.
