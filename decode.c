
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Size of the BMP image header in bytes
#define BMP_HEADER_SIZE 54

// Function to open the stego image and output file
Status open_decode_files(DecodeInfo *decInfo)
{
    // Open the stego image in read mode
    decInfo->fptr_stego_image =
        fopen(decInfo->stego_image_fname, "r");

    // Check whether the stego image opened successfully
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                decInfo->stego_image_fname);
        return e_failure;
    }

    // If output filename is provided, open the output file
    if (decInfo->argc == 4)
    {
        // Open output file in write mode
        decInfo->fptr_output =
            fopen(decInfo->output_fname, "w");

        // Check whether output file opened successfully
        if (decInfo->fptr_output == NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n",
                    decInfo->output_fname);
            return e_failure;
        }
    }

    // Return success if files are opened
    return e_success;
}

// Main function to perform the decoding process
Status do_decoding(DecodeInfo *decInfo)
{
    // Open the stego image and output file
    if (open_decode_files(decInfo) == e_failure)
    {
        printf("Error to open files...\n");
        return e_failure;
    }

    // Skip the 54-byte BMP header
    if (skip_bmp_header(decInfo) == e_failure)
    {
        printf("Error to skip header file..\n");
        return e_failure;
    }
    else
    {
        printf("Header file skipped successfully..\n");
    }

    // Decode and verify the magic string
    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        printf("Invalid Magic String..\n");
        return e_failure;
    }

    // Decode the size of the secret file extension
    if (decode_secret_file_extn_size(decInfo) == e_failure)
    {
        printf("Error decoding extension size\n");
        return e_failure;
    }

    // Decode the secret file extension
    if (decode_secret_file_extn(decInfo) == e_failure)
    {
        printf("Error decoding extension\n");
        return e_failure;
    }

    // If the user did not provide an output filename,
    // create a default filename using "Output"
    // followed by the decoded file extension
    if (decInfo->argc == 3)
    {
        char file[10] = "Output";

        // Append the secret file extension to the filename
        strcat(file, decInfo->extn_secret_file);

        // Create the output file in read/write mode
        FILE *fp = fopen(file, "w+");

        // Store the output file pointer
        decInfo->fptr_output = fp;
    }
    else
    {
        printf("Secret message file matched successfully..\n");
    }

    // Decode the original secret file size
    decode_secret_file_size(decInfo);

    // Decode the secret file data and write it
    // into the output file
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        return e_failure;
    }

    // Return success after completing decoding
    return e_success;
}

// Function to skip the BMP image header
Status skip_bmp_header(DecodeInfo *decInfo)
{
    // Check whether the structure and image file are valid
    if (decInfo == NULL || decInfo->fptr_stego_image == NULL)
        return e_failure;

    // Move the file pointer to byte 54,
    // skipping the BMP header
    if (fseek(decInfo->fptr_stego_image,
              BMP_HEADER_SIZE, SEEK_SET) != 0)
        return e_failure;

    // Return success after skipping the header
    return e_success;
}

// Function to decode and verify the magic string
Status decode_magic_string(const char *magic_string,
                           DecodeInfo *decInfo)
{
    // Check whether the input pointers are valid
    if (magic_string == NULL || decInfo == NULL)
        return e_failure;

    // Calculate the length of the magic string
    int len = strlen(magic_string);

    // Create a buffer to store the decoded magic string
    char buffer[16] = {0};

    // Decode the magic string from the stego image
    if (decode_data_from_image(buffer, len,
                               decInfo->fptr_stego_image) == e_failure)
        return e_failure;

    // Add the null character at the end of the string
    buffer[len] = '\0';

    // Compare the decoded string with the original magic string
    if (strcmp(buffer, magic_string) != 0)
    {
        printf("Error Magic string mismatch (got \"%s\")\n",
               buffer);
        return e_failure;
    }

    // Print the verified magic string
    printf("Magic string verified: \"%s\"\n", buffer);

    // Return success if both strings match
    return e_success;
}

// Function to decode multiple bytes from the stego image
Status decode_data_from_image(char *data, int size,
                              FILE *fptr_stego_image)
{
    // Check whether the image file is valid
    if (fptr_stego_image == NULL || data == NULL || size < 0)
    {
        return e_failure;
    }

    // Buffer to store 8 image bytes for decoding one byte
    char buffer[8];

    // Repeat the process for the required number of bytes
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the stego image
        // Each byte contains one hidden data bit
        if (fread(buffer, 1, 8, fptr_stego_image) != 8)
        {
            return e_failure;
        }

        // Extract one hidden byte from the 8 LSB bits
        if (decode_byte_from_lsb(&data[i], buffer) == e_failure)
        {
            return e_failure;
        }
    }

    // Return success after decoding all requested bytes
    return e_success;
}

// Function to decode one byte using LSB technique
Status decode_byte_from_lsb(char *data, char *image_buffer)
{
    // Check whether the input pointers are valid
    if (image_buffer == NULL || data == NULL)
    {
        return e_failure;
    }

    // Initialize the decoded character to zero
    char ch = 0;

    // Extract the least significant bit from each image byte
    for (int i = 0; i < 8; i++)
    {
        // Shift the current result left by one bit
        // Extract the LSB using bitwise AND with 1
        // Combine the extracted bit using bitwise OR
        ch = (ch << 1) | (image_buffer[i] & 1);
    }

    // Store the decoded character in the output variable
    *data = ch;

    // Return success after decoding one byte
    return e_success;
}

// Function to decode the size of the secret file extension
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    // Initialize extension length to zero
    int len = 0;

    // Decode the integer value from the stego image
    if (decode_data_from_image((char *)&len, sizeof(int),
                               decInfo->fptr_stego_image) == e_failure)
        return e_failure;

    // Check whether the decoded length fits in the extension array
    if (len <= 0 || len >= (int)sizeof(decInfo->extn_secret_file))
        return e_failure;

    // Store the decoded extension length in the structure
    decInfo->extn_len = len;

    // Return success after decoding extension size
    return e_success;
}

// Function to decode the original secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    // Initialize secret file size to zero
    int size = 0;

    // Decode the file size from the stego image
    if (decode_data_from_image((char *)&size, sizeof(int),
                               decInfo->fptr_stego_image) == e_failure)
        return e_failure;

    // Check whether the decoded size is valid
    if (size <= 0)
        return e_failure;

    // Store the decoded file size in the structure
    decInfo->size_secret_file = size;

    // Return success after decoding file size
    return e_success;
}

// Function to decode the secret file extension
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    // Create an array to store the decoded extension
    char type[8] = {0};

    // Decode the extension characters from the stego image
    if (decode_data_from_image(type, decInfo->extn_len,
                               decInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    // Add the null character at the end of the extension
    type[decInfo->extn_len] = '\0';

    // Copy the decoded extension into the structure
    strcpy(decInfo->extn_secret_file, type);

    // Display the decoded file extension
    printf("Decoded extension = %s\n", type);

    // Return success after decoding the extension
    return e_success;
}

// Function to decode the secret file contents
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    // Check whether the structure and output file are valid
    if (decInfo == NULL || decInfo->fptr_output == NULL)
        return e_failure;

    // Allocate memory to store the decoded secret data
    char *secret_data = malloc(decInfo->size_secret_file);

    // Check whether memory allocation was successful
    if (secret_data == NULL)
        return e_failure;

    // Decode the secret file data from the stego image
    if (decode_data_from_image(secret_data,
                               (int)decInfo->size_secret_file,
                               decInfo->fptr_stego_image) == e_failure)
    {
        // Release allocated memory if decoding fails
        free(secret_data);
        return e_failure;
    }

    // Write the decoded secret data into the output file
    if (fwrite(secret_data, 1, decInfo->size_secret_file,
               decInfo->fptr_output) !=
               (size_t)decInfo->size_secret_file)
    {
        // Release allocated memory if writing fails
        free(secret_data);
        return e_failure;
    }

    // Release the allocated memory after writing
    free(secret_data);

    // Return success after decoding and writing the data
    return e_success;
}