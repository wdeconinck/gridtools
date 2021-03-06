// This file is generated!
#pragma once

#include <cpp_bindgen/array_descriptor.h>
#include <cpp_bindgen/handle.h>

#ifdef __cplusplus
extern "C" {
#endif

bindgen_handle* create_copy_stencil(bindgen_handle*, bindgen_handle*);
bindgen_handle* create_data_store(unsigned int, unsigned int, unsigned int, double*);
bindgen_handle* generic_create_data_store0(unsigned int, unsigned int, unsigned int, double*);
bindgen_handle* generic_create_data_store1(unsigned int, unsigned int, unsigned int, float*);
void run_stencil(bindgen_handle*);
void sync_data_store(bindgen_handle*);

#ifdef __cplusplus
}
#endif
