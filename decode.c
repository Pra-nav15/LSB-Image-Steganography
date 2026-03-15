#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, output file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_decode_args(char* argv[], DecodeInfo* decInfo)
{
    char* dest = strstr(argv[2], ".bmp"); // Check if argv[2] contains ".bmp" to validate encoded image file,if yes store the address of the occurence to a char pointer
    if (dest != NULL && strcmp(dest, ".bmp") == 0) // Validate encoded image file image file extension,Check if address is not NULL and compare it will the given extension type
    {
        decInfo->stego_image_fname = argv[2]; // Store source image filename to structure member
    }
    else
    {
        printf("\033[0;31mError ! Destination Image File is not a bmp File\033[0m\n");
        return d_failure;
    }
    if (argv[3]) // If output filename is provided
    {
        char* out = strtok(argv[3], "."); // Extract out extension from output filename
        strcpy(decInfo->output_fname,out); // Store base name only without extension
        printf("\033[0;32mAll Command Line Arguments Validated Successfully\033[0m\n");
        return d_success;
    }
    else // If no output filename given
    {
        strcpy(decInfo->output_fname,"output"); // Use default name
        printf("\033[0;32mAll Command Line Arguments Validated Successfully\033[0m\n");
        return d_success;
    }
}

Status open_decode_files(DecodeInfo *decInfo)
{
    // Destination Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "\033[0;31mERROR: Unable to open file %s\033[0m\n", decInfo->stego_image_fname);

        return d_failure;
    }
    return d_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char imageBuffer[8];
    int len = strlen(magic_string); //Find Length of the Magic String used for Encoding
    char buffer_store[len + 1]; // Buffer to store decoded string
    for (int i = 0;i < len;i++)
    {
        char data = 0; // Initialize decoded byte
        fread(imageBuffer, sizeof(char), 8, decInfo->fptr_stego_image); /// Read 8 bytes from encoded image file to image buffer
        decode_byte_from_lsb(&data, imageBuffer); // Decode 1 byte from LSBs
        buffer_store[i] = data; // Decode 1 byte from  to a Buffer
    }
    buffer_store[len] = '\0'; // Null-terminate string 
    printf("\033[0;33mDecoded Magic String \033[0m: %s\n", buffer_store);
    if (strcmp(buffer_store, magic_string) == 0) // Compare decoded data with expected Magic String
    {
        return d_success;
    }
    else
    {
        return d_failure;
    }
}
Status decode_output_file_extn_size(int* size, DecodeInfo *decInfo)
{
    char imageBuffer[32]; 
    fread(imageBuffer, sizeof(char), 32, decInfo->fptr_stego_image); // Read other 32 bytes from encoded image to image buffer
    decode_size_from_lsb(size, imageBuffer); // Decode integer from LSBs
    decInfo->extn_size = *size; // Store decoded size to structure member
    printf("\033[0;33mDecoded Secret File Extension Size :\033[0m %d\n", decInfo->extn_size);
    if (decInfo->extn_size >= 2) //should be greater than or equal to 2 for extension
    {
        return d_success;
    }
    else
    {
        return d_failure;
    }

}
Status decode_output_file_extn(char *file_extn, DecodeInfo *decInfo)
{
    char imageBuffer[8];
    int len = decInfo->extn_size;  // extension size
    char extn_buffer[len + 1];  // Buffer to store extension
    for (int i = 0;i < len;i++)
    {
        char data = 0;
        fread(imageBuffer, sizeof(char), 8, decInfo->fptr_stego_image); // Read other 8 bytes from encoded image to image buffer
        decode_byte_from_lsb(&data, imageBuffer); // Decode byte from LSBs
        extn_buffer[i]= data; // Store character to a buffer
    }
    extn_buffer[len] = '\0'; // Null-terminate string
    strcat(decInfo->output_fname, extn_buffer); // Append extension to output filename
    printf("\033[0;33mDecoded Secret File Extension :\033[0m %s\n", extn_buffer);
    decInfo->fptr_output = fopen(decInfo->output_fname, "w"); // Open output file in write mode to write the data to the output file
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        return d_failure;
    }
    return d_success;
}
Status decode_output_file_size(int* file_size, DecodeInfo* decInfo)
{
    char imageBuffer[32];
    fread(imageBuffer, sizeof(char), 32, decInfo->fptr_stego_image); // Read other 32 bytes from encoded image to image buffer
    *file_size = 0; // intialize file size as 0 at first
    decode_size_from_lsb(file_size, imageBuffer); // Decode size from LSBs
    decInfo->size_output_file = *file_size;  // store the size to a structure member
    printf("\033[0;33mDecoded Secret File Data Size \033[0m: %d bytes\n", 8*decInfo->size_output_file);  // correct format
    if (decInfo->size_output_file > 0) //output file size should not be 0
    {
        return d_success;
    }
    else
    {
        return d_failure;
    }
}

Status decode_output_file_data(DecodeInfo* decInfo)
{
    int data_size = decInfo->size_output_file; // Total bytes to decode
    char imageBuffer[8];
    char out_buffer[data_size + 1]; // Buffer to store decoded data
    printf("\033[0;33mDecoded Secret File Data : \033[0m\n");
    for (int i = 0;i < data_size;i++) // Loop through each byte

    {
        char data = 0;
        fread(imageBuffer, sizeof(char), 8, decInfo->fptr_stego_image); // Read other 8 bytes from encoded image to image buffer
        decode_byte_from_lsb(&data, imageBuffer); // Decode each byte from LSBs
        out_buffer[i] = data; // store the decoded bytes to a buffer
        printf("\033[0;35m%c", out_buffer[i]);
    }
    printf("\n");
    out_buffer[data_size] = '\0'; // Null-terminate
    fwrite(out_buffer, sizeof(char), data_size, decInfo->fptr_output); // Write decoded data to output file
    fclose(decInfo->fptr_output);
    return d_success;
}

Status decode_byte_from_lsb(char* data, char* image_buffer)
{
    *data = 0;
    for (int i = 0; i < 8; i++) // Loop through 8 bits
    {
        *data = *data | ((image_buffer[i] & 1) << i); // Extract LSB and shift to correct position
    }

}

Status decode_size_from_lsb(int* size,char *imageBuffer)
{
    *size = 0;
    for (int i = 0; i < 32; i++) // Loop through 8 bits
    {
        *size = *size | ((imageBuffer[i] & 1) << i); // Extract LSB and shift to correct position
    }
}

Status do_decoding(DecodeInfo* decInfo)
{
    // Open the encoded image
    if (open_decode_files(decInfo) == d_success)
    {
        // Skip BMP Header (first 54 bytes)
        fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
        printf("\033[0;32mDestination File Opened Successfully\033[0m\n");
    }
    else
    {
        printf("\033[0;31mError! Unable to open Destination file\033[0m\n");
        return d_failure;
    }
    // Decode and verify the magic string to confirm presence of hidden data
    if (decode_magic_string(MAGIC_STRING, decInfo) == d_success)
    {
        printf("\033[0;32mMagic String Decoded Successfully from Destination File\033[0m\n");
    }
    else
    {
        printf("\033[0;31mError! Unable to Decode Magic \033[0m\n");
        return d_failure;
    }
    int temp_size = 0; // Initialize a temp variable to pass with zero before decoding the secret file's extension size to avoid garbage value
    //Decode the size of the secret file's extension
    if (decode_output_file_extn_size(&temp_size, decInfo) == d_success)
    {
        printf("\033[0;32mSecret File Extension Size Decoded Successfully from Destination File\033[0m\n");
    }
    else
    {
        printf("\033[0;31mError! Unable to Decode output File Extension Size\033[0m\n");
        return d_failure;
    }
    // Decode the actual extension and open the output file for writing
    if (decode_output_file_extn(decInfo->extn_output_file, decInfo) == d_success)
    {
        printf("\033[0;32mSecret File Extension Decoded Successfully from Destination File\033[0m\n");
    }
    else
    {
        printf("\033[0;31mError! Unable to Decode output File Extension\033[0m\n");
        return d_failure;
    }
    // Decode the size of the secret file data (number of bytes)
    int extn_size = 0; //Initialize a temp variable to pass with zero before decoding to avoid garbage value and ensure safe memory usage
    if (decode_output_file_size(&extn_size, decInfo) == d_success)
    {
        printf("\033[0;32mSecret File Data Size Decoded Successfully from Destination File\033[0m\n");
    }
    else
    {
        printf("\033[0;31mError! Unable to Decode output File Data Size\033[0m\n");
        return d_failure;
    }
    // Decode the actual secret file data and write it to the output file
    if (decode_output_file_data(decInfo) == d_success)
    {
        printf("\033[0;32mSecret File Data Decoded Successfully from Destination File\033[0m\n");
        return d_success;
    }
    else
    {
        printf("\033[0;31mError! Unable to Decode output File Data\033[0m\n");
        return d_failure;
    }
}
 
