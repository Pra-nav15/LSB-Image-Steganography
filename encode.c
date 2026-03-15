#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE* fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("\033[0;35mwidth = %u\033[0m\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("\033[0;35mheight = %u\033[0m\n", height);

    // Return image capacity
    return width * height * 3;  // 3 is used because it is an rgb image used in this project and it has 3 colours 
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    // Move to end and get size
    fseek(fptr, 0, SEEK_END);
    long size_secret_file = ftell(fptr); // Determine the current offset position of file pointer
    rewind(fptr); //rewind to the beginning
    printf("\033[0;36mThere %ld Characters in the Secret File\033[0m\n", size_secret_file);
    return size_secret_file;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    char* src = strstr(argv[2], ".bmp"); // Check if argv[2] contains ".bmp" to validate source image file,if yes store the address of the occurence to a char pointer
    char* sec = strstr(argv[3], "."); // Check if argv[3] contains "." to validate secret file,if yes store the address of the occurence to a char pointer
    if (src != NULL && strcmp(src, ".bmp") == 0)   // Validate source image file extension,Check if address is not NULL and compare it will the given extension type
    {
        encInfo->src_image_fname = argv[2]; // Store source image filename to structure member
    }
    else
    {
        printf("\033[0;31mError ! Source Image is not a bmp File\033[0m\n");
        return e_failure;
    }
    if ((sec != NULL && strcmp(sec, ".txt") == 0) || (sec != NULL && strcmp(sec, ".c") == 0) || (sec != NULL && strcmp(sec, ".sh") == 0) || (sec != NULL && strcmp(sec, ".h") == 0))// Validate secret file extension if .txt or .sh or .c or .h ,Check if address is not NULL and compare it will the given extension type
    {
        encInfo->secret_fname = argv[3]; // Store source image filename to structure member
    }
    else
    {
        printf("\033[0;31mError ! Secret File Should be Only .txt or .sh or .c or .h\033[0m\n");
        return e_failure;
    }
     // Check if destination image filename is provided
    if (argv[4] != NULL)
    {
        // Validate destination image file extension
        char* dest = strstr(argv[4], ".bmp");
        if (dest != NULL && strcmp(dest, ".bmp") == 0)
        {
            encInfo->stego_image_fname = argv[4]; // Store destination image filename to structure member
            printf("\033[0;32mAll Command Line Arguments Validated Successfully\033[0m\n");
            return e_success;
        }
        else
        {
            printf("\033[0;31mError ! Destination Image is not a bmp File\033[0m\n");
            return e_failure;
        }
    }
    else
    {
         // If no destination file name is provided, use default name
        encInfo->stego_image_fname = "default.bmp";
        printf("\033[0;32mAll Command Line Arguments Validated Successfully\033[0m\n");
        return e_success;
    }
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("\033[0;31mfopen\033[0m");
        fprintf(stderr, "\033[0;31mERROR: Unable to open file %s\033[0m\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("\033[0;31mfopen\033[0m");
        fprintf(stderr, "\033[0;31mERROR: Unable to open file %s\033[0m\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("\033[0;31mfopen\033[0m");
        fprintf(stderr, "\033[0;31mERROR: Unable to open file %s\033[0m\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

Status check_capacity(EncodeInfo *encInfo)
{
    // Get the total number of usable bytes in the BMP image for encoding and store it to structure member
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // Get the size of the secret file in bytes and store it to structure member
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    // Check if the image has enough capacity to store:
    // - 16 bits for magic string
    // - 32 bits for extension size
    // - 32 bits for extension data
    // - 32 bits for secret file size
    // - (secret file size * 8) bits for actual secret data
    if (encInfo->image_capacity > (16 + 32 + 32 + 32 + (encInfo->size_secret_file * 8)))
    {
        return e_success; 
    }
    else
    {
        return e_failure;
    }

}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    rewind(fptr_src_image);   // Move the file pointer of source image to the beginning
    char imageBuffer[54];     // BMP header is 54 bytes long so take a Image Buffer
    fread(imageBuffer, sizeof(char), 54, fptr_src_image); // Read 54 bytes from source image into buffer
    fwrite(imageBuffer, sizeof(char), 54, fptr_dest_image); // Write the same 54 bytes into destination image
    int src_offset, dest_offset;
    src_offset = ftell(fptr_src_image);  // Get current position in source file
    dest_offset = ftell(fptr_dest_image);  // Get current position in destination file
    if (src_offset == dest_offset)  // Check if both file pointers are at the same offset
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char imageBuffer[8];  // Buffer to hold 8 bytes of image data for LSB encoding
    int len = strlen(magic_string); // Get the length of the magic string
     // Loop through each character of the magic string
    for (int i = 0;i < len;i++)
    {
        fread(imageBuffer, sizeof(char), 8, encInfo->fptr_src_image); // Read 8 bytes from the source image (each character needs 8 bits)
        encode_byte_to_lsb(magic_string[i], imageBuffer);   // Encode the current character into the LSBs of the 8 bytes
        fwrite(imageBuffer, sizeof(char), 8, encInfo->fptr_stego_image);  // Write the modified 8 bytes into the destination image
    }
    int src_offset, dest_offset;
    src_offset = ftell(encInfo->fptr_src_image);
    dest_offset = ftell(encInfo->fptr_stego_image);
    if (src_offset == dest_offset)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char imageBuffer[32]; // Buffer to hold 32 bytes of image data (each bit of size needs 1 byte)
    fread(imageBuffer, sizeof(char), 32, encInfo->fptr_src_image);   // Read 32 bytes from the source image to embed the extension size
    encode_size_to_lsb(size, imageBuffer); // Encode the 32-bit integer 'size' into the LSBs of the 32 bytes
    fwrite(imageBuffer, sizeof(char), 32, encInfo->fptr_stego_image);  // Write the modified 32 bytes into the destination Image
    int src_offset, dest_offset;
    src_offset = ftell(encInfo->fptr_src_image);
    dest_offset = ftell(encInfo->fptr_stego_image);
    if (src_offset == dest_offset)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char imageBuffer[8]; // Buffer to hold 8 bytes of image data for encoding one character
    int len = strlen(file_extn); // Get the length of the file extension string
    // Loop through each character of the file extension
    for (int i = 0;i < len;i++)
    {
        fread(imageBuffer, sizeof(char), 8, encInfo->fptr_src_image); // Read 8 bytes from the source image (1 byte per bit of the character)
        encode_byte_to_lsb(file_extn[i], imageBuffer);   // Encode the current character into the LSBs of the 8 bytes
        fwrite(imageBuffer, sizeof(char), 8, encInfo->fptr_stego_image); // Write the modified 8 bytes into the destination image
    }
    int src_offset, dest_offset;
    src_offset = ftell(encInfo->fptr_src_image);
    dest_offset = ftell(encInfo->fptr_stego_image);
    if (src_offset == dest_offset)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imageBuffer[32]; // Buffer to hold 32 bytes of image data (1 byte per bit of the file size)
    fread(imageBuffer, sizeof(char), 32, encInfo->fptr_src_image);  // Read 32 bytes from the source image to embed the 32-bit file size
    encode_size_to_lsb(file_size, imageBuffer);     // Encode the 32-bit file size into the LSBs of the 32 bytes
    fwrite(imageBuffer, sizeof(char), 32, encInfo->fptr_stego_image); // Write the modified 32 bytes into the destination image
    int src_offset, dest_offset;
    src_offset = ftell(encInfo->fptr_src_image);
    dest_offset = ftell(encInfo->fptr_stego_image);
    if (src_offset == dest_offset)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char secret_data[encInfo->size_secret_file];    // Allocate a buffer to hold the entire secret file content
    rewind(encInfo->fptr_secret);  // Move the file pointer of the secret file to the beginning
    char imageBuffer[8];
    int size = (8 * encInfo->size_secret_file); // Calculate the number of bits to encode
    fread(secret_data, sizeof(char), size, encInfo->fptr_secret);  // Read the entire secret file into allocated Buffer
    for (int i = 0;i < encInfo->size_secret_file;i++)
    {
        fread(imageBuffer, sizeof(char), 8, encInfo->fptr_src_image); // Read 8 bytes from the source image (1 byte per bit of the secret character)
        encode_byte_to_lsb(secret_data[i], imageBuffer); // Encode the current secret byte into the LSBs of the 8 image bytes
        fwrite(imageBuffer, sizeof(char), 8, encInfo->fptr_stego_image); // Write the modified 8 bytes into destination file
    }
    int src_offset, dest_offset;
    src_offset = ftell(encInfo->fptr_src_image);
    dest_offset = ftell(encInfo->fptr_stego_image);
    if (src_offset == dest_offset)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}


Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char temp[1]; // Temporary buffer to hold one byte at a time
    while (fread(temp, sizeof(char), 1, fptr_src) == 1)  // Loop until end of source file is reached
    {
     fwrite(temp, sizeof(char), 1, fptr_dest); // Write the byte from source to destination
    }
    int src_offset, dest_offset;
    src_offset = ftell(fptr_src);
    dest_offset = ftell(fptr_dest);
    if (src_offset == dest_offset)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer) 
{
    for (int i = 0;i < 8;i++)  // Loop through each bit of the character
    {
        image_buffer[i] = (image_buffer[i] & ~(1 << 0)); //Clear the LSB of the current byte
        image_buffer[i] = (image_buffer[i] | ((data >> i) & 1)); //Extract i-th bit from data and set it as LSB
    }
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for (int i = 0;i < 32;i++) // Loop through each bit of the 32-bit integer
    {
        imageBuffer[i] = (imageBuffer[i] & ~(1 << 0)); //Clear the LSB of the current byte
        imageBuffer[i] = (imageBuffer[i] | ((size >> i) & 1)); //Extract i-th bit from data and set it as LSB
    }
}

Status do_encoding(EncodeInfo *encInfo)
{
    //Open all required files (source image, secret file, destination image)
    if (open_files(encInfo) == e_success)
    {
        printf("\033[0;32mAll Files Opened Successfully\033[0m\n");
    }
    else
    {
        printf("\033[0;31mError! Unable to open files\033[0m\n");
        return e_failure;
    }
    //Check if the source image has enough capacity to hold the secret data
    if (check_capacity(encInfo) == e_success)
    {
        printf("\033[0;32mImage has Enough Capacity for Encoding\033[0m\n");
    }
    else
    {
        printf("\033[0;31mError! Image Capacity is Less\033[0m\n");
        return e_failure;
    }
    // Copy the 54-byte BMP header from source to destination image
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("\033[0;32mAll 54 Bytes of Header Copied Successfully to Destination File\033[0m\n");
        printf("\033[0;33mOFFSET at %ld after Copying All Header Files\033[0m\n",ftell(encInfo->fptr_src_image));
    }
    else
    {
        printf("\033[0;31mError! Unable to Copy Header files\033[0m\n");
        return e_failure;
    }
    // Encode the magic string to identify stego image
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("\033[0;32mMagic String Encoded Successfully to Destination File\033[0m\n");
        printf("\033[0;33mOFFSET at %ld after Encoding Magic String\033[0m\n",ftell(encInfo->fptr_src_image));
    }
    else
    {
        printf("\033[0;31mError! Unable to Encode Magic String\033[0m\n");
        return e_failure;
    }
    // Extract and store the secret file extension 
    char* secret_extension = strstr(encInfo->secret_fname, ".");
    strcpy(encInfo->extn_secret_file, secret_extension);
    printf("\033[0;32mSecret File Extension Chosen for Encoding is %s\033[0m\n", encInfo->extn_secret_file);
    //Encode the size of the secret file extension
    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
    {
        printf("\033[0;32mSecret File Extension Size Encoded Successfully to Destination File\033[0m\n");
        printf("\033[0;33mOFFSET at %ld after Encoding Secret File Extension Size\033[0m\n",ftell(encInfo->fptr_src_image));
    }
    else
    {
        printf("\033[0;31mError! Unable to Encode Secret File Extension Size\033[0m\n");
        return e_failure;
    }
    //Encode the actual secret file extension string
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("\033[0;32mSecret File Extension Encoded Successfully to Destination File\033[0m\n");
        printf("\033[0;33mOFFSET at %ld after Encoding Secret File Extension\033[0m\n",ftell(encInfo->fptr_src_image));
    }
    else
    {
        printf("\033[0;31mError! Unable to Encode Secret File Extension\033[0m\n");
        return e_failure;
    }
    // Encode the size of the secret file data
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("\033[0;32mSecret File Data Size Encoded Successfully to Destination File\033[0m\n");
        printf("\033[0;33mOFFSET at %ld after Encoding Secret File Data Size\033[0m\n",ftell(encInfo->fptr_src_image));
    }
    else
    {
        printf("\033[0;31mError! Unable to Encode Secret File Data Size\033[0m\n");
        return e_failure;
    }
    // Encode the actual secret file data byte-by-byte
    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("\033[0;32mSecret File Data Encoded Successfully to Destination File\033[0m\n");
        printf("\033[0;33mOFFSET at %ld after Encoding Secret File Data\033[0m\n",ftell(encInfo->fptr_src_image));
    }
    else
    {
        printf("\033[0;31mError! Unable to Encode Secret File Data\033[0m\n");
        return e_failure;
    }
    //Copy the remaining image data from source to destination image
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("\033[0;32mRemaining Image Data Copied Successfully to Destination File\033[0m\n");
        return e_success;
    }
    else
    {
        printf("\033[0;31mError! Unable to Copy Remaining Image Data\033[0m\n");
        return e_failure;
    }
}



