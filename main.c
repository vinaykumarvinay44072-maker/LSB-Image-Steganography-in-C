/*
 * Project Title: Image Steganography Using LSB Technique
 *
 * Name       : N. Vinay Kumar
 * Batch ID   : 26015_128
 * Language   : C Programming
 * Technology : File Handling, Bitwise Operations
 *
 * Description:
 * This project is used to hide secret text data inside a
 * BMP image using the Least Significant Bit (LSB) technique.
 *
 * The project consists of two main operations:
 *
 * 1. Encoding:
 *    The program reads a source BMP image and a secret text
 *    file. It hides the magic string, file extension, file
 *    size and secret data in the LSBs of the image bytes.
 *    The modified image is saved as a stego image.
 *
 * 2. Decoding:
 *    The program reads the stego image and extracts the
 *    hidden magic string, file extension, file size and
 *    secret data. It then creates an output file containing
 *    the recovered secret message.
 *
 * Main Objective:
 * To implement data hiding and recovery using C programming,
 * file handling, structures, pointers, dynamic memory
 * allocation and bitwise operators.
 */

#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include <string.h>

// Main function: Starts encoding or decoding
int main(int argc, char *argv[])
{
    // Initialize encoding and decoding structures
    EncodeInfo encInfo = {0};
    DecodeInfo decInfo = {0};

    // Check the number of command-line arguments
    if (argc < 3 || argc > 5)
    {
        printf("Invalid Arguments Count\n");
        return 0;
    }

    // Identify the selected operation
    int operation = check_operation_type(argv);

    // Check for an unsupported operation
    if (operation == 2)
    {
        printf("Invalid Operation Type.\n");
        return e_success;
    }

    // Initialize file extension validation flags
    int bmp_flag = 0;
    int txt_flag = 0;
    int opbmp_flag = 0;

    // ================= ENCODING OPERATION =================
    if (operation == e_encode)
    {
        // Check whether the source image is a BMP file
        if (strstr(argv[2], ".bmp") != NULL)
        {
            bmp_flag = 1;
        }

        // Check whether the secret file is a TXT file
        if (strstr(argv[3], ".txt") != NULL)
        {
            txt_flag = 1;
        }

        // Check whether an output BMP filename is provided
        if (argc == 5)
        {
            if (strstr(argv[4], ".bmp") != NULL)
            {
                opbmp_flag = 1;

                // Store the output image filename
                encInfo.stego_image_fname = argv[4];

                // Open the output image in binary write mode
                encInfo.fptr_stego_image = fopen(argv[4], "wb");
            }
        }

        // Validate the input file extensions
        if (bmp_flag == 0 || txt_flag == 0)
        {
            printf("Invalid Argument Extension.\n");
            printf("Files must have .bmp and .txt extensions only.\n");
            return 0;
        }
        else
        {
            // Store the source BMP image filename
            encInfo.src_image_fname = argv[2];

            // Open the source image in binary read mode
            encInfo.fptr_src_image = fopen(argv[2], "rb");

            // Store the secret text filename
            encInfo.secret_fname = argv[3];

            // Open the secret file in binary read mode
            encInfo.fptr_secret = fopen(argv[3], "rb");

            // Store the secret file extension
            strcpy(encInfo.extn_secret_file, ".txt");

            // Check whether the secret file opened successfully
            if (encInfo.fptr_secret == NULL)
            {
                printf("Error to Open %s\n", argv[3]);
                return 0;
            }

            // If no output filename is provided,
            // use the default filename "stego.bmp"
            if (opbmp_flag == 0)
            {
                encInfo.stego_image_fname = "stego.bmp";

                // Create the default stego image
                encInfo.fptr_stego_image =
                    fopen("stego.bmp", "wb");
            }
        }

        // Perform the encoding operation
        if (do_encoding(&encInfo) == e_failure)
        {
            printf("Error to Encode.\n");
            return e_failure;
        }
        else
        {
            // Display encoding completion message
            printf("Encode done successfully...\n");
            printf("Check completed\n");

            // Display the generated stego image filename
            printf("Stego file name: %s\n",
                   encInfo.stego_image_fname);

            return e_success;
        }
    }

    // ================= DECODING OPERATION =================
    else if (operation == e_decode)
    {
        // Check whether the input image is a BMP file
        if (strstr(argv[2], ".bmp") != NULL)
        {
            bmp_flag = 1;
        }

        // Check whether the output filename is provided
        if (argc == 4)
        {
            if (strstr(argv[3], ".txt") != NULL)
            {
                txt_flag = 1;

                // Store the output text filename
                decInfo.output_fname = argv[3];
            }
        }

        // Check whether the stego image exists
        FILE *test_fp = fopen(argv[2], "rb");

        if (test_fp == NULL)
        {
            printf("%s not created. Please encode first.\n",
                   argv[2]);
            return e_failure;
        }

        // Close the temporary file used for checking
        fclose(test_fp);

        // Validate the input BMP extension
        if (bmp_flag == 0)
        {
            printf("Invalid Argument Extension.\n");
            printf("Input file must have .bmp extension.\n");
            return 0;
        }
        else
        {
            // Store the stego image filename
            decInfo.stego_image_fname = argv[2];

            // Store the command-line argument count
            decInfo.argc = argc;

            // Perform the decoding operation
            if (do_decoding(&decInfo) == e_failure)
            {
                printf("Error to decode.\n");
                return e_failure;
            }

            // Display the decoded output filename
            if (argc == 4)
            {
                printf("Decode done successfully.\n");
                printf("Secret data: %s\n", argv[3]);
            }
            else
            {
                printf("Decode done successfully.\n");
                printf("Secret data: Output%s\n",
                       decInfo.extn_secret_file);
            }

            // Close the opened stego image
            fclose(decInfo.fptr_stego_image);

            // Close the decoded output file
            if (decInfo.fptr_output != NULL)
            {
                fclose(decInfo.fptr_output);
            }

            return e_success;
        }
    }
    else
    {
        // Display an error for an unsupported operation
        printf("Invalid Action Type\n");
    }

    // End of main function
    return 0;
}