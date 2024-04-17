## Cython version

In the purpose of maximizing the performances, the code was also adapted to Cython. The file dynamic_array_cython.pyx contains the Cython version of the code. The setup.py file was also adapted to compile the Cython code. The compilation can be done using the following command:

```bash
cd CythonVersion/
python setup.py build_ext --inplace
```

This will generate a .so combining the c++ and the Cython code file that can be used in a python app. The file will be generated in the same directory as the setup.py file.