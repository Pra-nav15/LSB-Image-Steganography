#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding output file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
   
    /* output File Info */
    char output_fname[50];    // To store the output file name
    FILE* fptr_output;        // To store the output file address
    int extn_size;            // To store output file extension size
    char extn_output_file[5]; // To store the output file extension
    char output_data[100];    // To store the output data
    int size_output_file;     // To store the size of the output data

    /* Stego Image Info */
    char *stego_image_fname; // To store the dest file name
    FILE *fptr_stego_image;  // To store the address of stego image

} DecodeInfo;

/* Encoding function prototype */

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo* decInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/*Decode extension size*/
Status decode_output_file_extn_size(int* size, DecodeInfo *decInfo);

/* Decode output file extenstion */
Status decode_output_file_extn(char *file_extn, DecodeInfo *decInfo);

/* Decode output file size */
Status decode_output_file_size(int* file_size, DecodeInfo *decInfo);

/* Decode output file data*/
Status decode_output_file_data(DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char* data,char* image_buffer);

/* Decode a size from LSB of image data array */
Status decode_size_from_lsb(int* size, char* imageBuffer);

#endif
