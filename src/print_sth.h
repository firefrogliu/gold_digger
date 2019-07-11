#ifndef PRINT_STH
#define PRINT_STH

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif
EXTERNC void print_hello();
#endif
