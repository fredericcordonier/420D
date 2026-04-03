/***********************************************************************************************************
*   File name: param_def.h
*
*   File used for the serialization of the structure containing parameters within 420D. Structure and field names are hashed, the hash is
*     used as a tag for serialized object. The data in this file are the hash, the index of the field in
*     the structure, and the size of the field.
*
*   This file is generated automatically. It should not be modified, as modifications will be overwritten.
************************************************************************************************************/
#pragma once



// Struct used to define a structure field
typedef struct {
    unsigned int i_field_name_hash;
    int          i_field_offset_in_struct;
    int          i_field_size;
} field_def_t;

typedef struct {
    int size;
    const field_def_t *data;
} list_field_def_t;

