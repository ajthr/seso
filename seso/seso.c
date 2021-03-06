#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <numpy/arrayobject.h>

#include "include/sort.h"
#include "include/search.h"

static PyObject* seso_sort(PyObject *self, PyObject *args) {
    PyObject *arg=NULL, *arr=NULL;
    PyArray_Descr *descr = NULL;
    npy_intp dims[3];
    int nd=0;
    double *cArr=NULL;
    char* algorithm = "mergesort"; /* mergesort is the default sorting algorithm */

    if (!PyArg_ParseTuple(args, "O|s", &arg, &algorithm))
        return NULL;

    arr = PyArray_FROM_OT(arg, NPY_DOUBLE);

    nd = PyArray_NDIM((PyArrayObject *)arr);
    descr = PyArray_DESCR((PyArrayObject *)arr);
    Py_INCREF(descr); /* PyArray_AsCArray steals a reference to this */

    PyArray_AsCArray(&arr, (void *)&cArr, dims, nd, descr);
    int err = sort(cArr, dims[0], algorithm);

    PyArray_Free(arr, (void *)cArr);

    if(err==1) {
        PyErr_SetString( PyExc_ValueError, "specified sort type not available");
        Py_XDECREF(arg);
        return NULL;
    } else {
        return PyArray_Return(arr);
    }
}

static PyObject* seso_search(PyObject *self, PyObject *args) {
    PyObject *arr=NULL;
    double *cArr;
    double val;
    npy_intp dims[3];
    char* algorithm = "binarysearch"; /* binarysearch is the default searching algorithm */

    if (!PyArg_ParseTuple(args, "Od|s", &arr, &val, &algorithm))
        return NULL;

    PyArray_AsCArray(&arr, (void *)&cArr, dims, PyArray_NDIM(arr), PyArray_DescrFromType(NPY_DOUBLE));

    int out = search(cArr, val, dims[0], algorithm);
    
    PyArray_Free(arr, (void *)&cArr);

    if(out==1) {
        PyErr_SetString( PyExc_ValueError, "specified search type not available");
        return NULL;
    } else {
        return PyLong_FromLong(out);
    }
}

static PyMethodDef seso_methods[] = {
    {"sort", seso_sort, METH_VARARGS, "sorting algorithms. returns sorted array" },
    {"search", seso_search, METH_VARARGS, "searching algorithms. returns index of the value. returns -1 if not found" },
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef seso = {
        PyModuleDef_HEAD_INIT,
        "seso",
        "A Python module for searching and sorting",
        -1,
        seso_methods,
};

PyMODINIT_FUNC PyInit_seso(void) {
    Py_Initialize();
    import_array();
    return PyModule_Create(&seso);
}
