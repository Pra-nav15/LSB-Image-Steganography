/*
                                  Project Documentation
Name: Pranav Prasad
Date: 11/11/2025
Description:
• This project is an Image Steganography System made in C using the Least Significant Bit (LSB) technique.
• It allows users to hide secret text data inside a .bmp image file and retrieve it back safely.
• The system has two main modes: Encoding (to hide data) and Decoding (to extract data).
• During encoding, it validates all files, checks image capacity, and stores a magic string, file extension, size, and data into the image.
• During decoding, it identifies the magic string and extracts the hidden message accurately.
• It supports only .bmp image files because they are uncompressed and easy to manipulate at byte level.
• Error handling is included for invalid files, missing arguments, or insufficient image space.
• It is mainly used in secure data transmission, defense communication, watermarking, and confidential information sharing.
• Advantages include data confidentiality, simplicity, reliability, and lossless image quality.
• file handling, pointers, structures, bitwise operations, and command-line arguments are used in this project.
• This project helps in understanding how digital data can be hidden inside images using bit-level manipulation.
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
OperationType check_operation_type(char *); // Function prototype to determine operation type
EncodeInfo encInfo; // Structure to hold encoding-related information
DecodeInfo decInfo; // Structure to hold decoding-related information
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("\033[0;31mError ! Arguments Not Sufficient.\033[0m\n");
        return e_unsupported;
    }
    if (check_operation_type(argv[1]) == e_encode)
    {
        printf("\033[0;36mYou Have Chosen Encoding....\033[0m\n");
        if (argc < 4)  // Check if enough arguments are provided for encoding
        {
            printf("\033[0;31mNot Sufficient number of Command Line Arguments\033[0m\n");
            printf("\033[0;33mEncoding Format : ./a.out -e <source_filename> <secret_filename> <destination_filename(optional)>\033[0m\n");
            printf("\033[0;36mMinimum 4 Arguments is Required for Encoding\033[0m\n");
            return e_failure;
        }
        else
        {
            if (read_and_validate_encode_args(argv, &encInfo) == e_success) // Validate encoding args
            {
                if (do_encoding(&encInfo) == e_success) // Perform encoding
                {
                    printf("\033[0;32mEncoding Process is Completed Successfully\033[0m\n");
                }
                else
                {
                    printf("\033[0;31mError ! Encoding Process is Failed\033[0m\n");
                }
            }
            else
            {
                printf("\033[0;31mCommand Line Arguments is not proper\033[0m\n");
                return e_failure;
            }
        }
    }
    else if (check_operation_type(argv[1]) == e_decode) // Check if user selected decoding
    {
        printf("\033[0;36mYou Have Chosen Decoding....\033[0m\n");
        if (argc < 3)  // Check if enough arguments are provided for decoding
        {
            printf("\033[0;31mNot Sufficient number of Command Line Arguments\033[0m\n");
             printf("\033[0;33mEncoding Format : ./a.out -d <encoded_filename> <decoded_filename(optional)>\033[0m\n");
            printf("\033[0;36mMinimum 3 Arguments is Required for Decoding\033[0m\n");
            return d_failure;
        }
        else
        {
            if (read_and_validate_decode_args(argv, &decInfo) == d_success)  // Validate decoding args
            {
                if (do_decoding(&decInfo) == d_success) // Perform decoding
                {
                    printf("\033[0;32mDecoding Process is Completed Successfully\033[0m\n");
                }
                else
                {
                    printf("\033[0;31mError ! Decoding Process is Failed\033[0m\n");
                }
            }
            else
            {
                printf("\033[0;31mCommand Line Arguments is not proper\033[0m\n");
                return d_failure;
            }
        }
    }
    else  // If neither encoding nor decoding selected
    {
         printf("\033[0;31mInvalid Argument\033[0m\n");
        printf("\033[0;31mError ! Neither Selected Encoding or Decoding\033[0m\n");
        return e_unsupported;
    }
   
}

OperationType check_operation_type(char* symbol)
{
    if (strcmp(symbol, "-e") == 0)
    {
        return e_encode; //Check symbol is -e or not, true->return e_encode 
    }
    else if (strcmp(symbol, "-d") == 0)
    {
        return e_decode; //Check symbol is -d or not, true->return e_decode 
    }
    else
    {
        return e_unsupported; // if not encoding or decoding return e_unsupported
    }
}

   
