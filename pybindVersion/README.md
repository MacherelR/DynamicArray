## How to use

Generate the shared library using the following command:

```bash
python setup.py build_ext --inplace
```

This will generate an .so file that can be used in a python app. The file will be generated in the same directory as the setup.py file.
C++ code is currently built using 03 flag for optimization. This can be changed in the setup.py file.

## External libraries

This code uses btree library from the following repository: https://github.com/Kronuz/cpp-btree/tree/master

## Specs
The main class is the one called DynamicArray. It contains the methods and functionalities needed for my personal use case. Those can be extended for further use cases.
A second class called DynamicCounter is actually a child of DynamicArray, tailored for a more specific use case (acting as a counter). This was created separatelly to avoid "polluting" the main class with specific methods and attributes.