
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "encode.h"
#include "types.h"
#include "common.h"

// Function to get the size of the BMP image
// Width * Height * 3 bytes per pixel (24-bit BMP)
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    // Move the file pointer to byte 18
    // BMP width information is stored at this offset
    fseek(fptr_image, 18, SEEK_SET);

    // Read the image width (4 bytes)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the image height (4 bytes)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Calculate and return the image capacity in bytes
    return width * height * 3;
}

/*
 * Function: open_files
 * Description:
 * Opens the source image, secret file and
 * destination stego image.
 *
 * Return:
 * e_success - if all files open successfully
 * e_failure - if any file cannot be opened
 */
Status open_files(EncodeInfo *encInfo)
{
    // Open the source BMP image in read mode
    encInfo->fptr_src_image =
        fopen(encInfo->src_image_fname, "r");

    // Check whether the source image opened successfully
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->src_image_fname);
        return e_failure;
    }

    // Open the secret file in read mode
    encInfo->fptr_secret =
        fopen(encInfo->secret_fname, "r");

    // Check whether the secret file opened successfully
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->secret_fname);
        return e_failure;
    }

    // Create the destination stego image in write mode
    encInfo->fptr_stego_image =
        fopen(encInfo->stego_image_fname, "w");

    // Check whether the stego image opened successfully
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n",
                encInfo->stego_image_fname);
        return e_failure;
    }

    // Return success if all three files are opened
    return e_success;
}

// Function to identify whether the user selected
// encoding or decoding operation using command-line arguments
OperationType check_operation_type(char *argv[])
{
    // Check whether the first argument is "-e"
    // strcasecmp compares strings without case sensitivity
    if (strcasecmp(argv[1], "-e") == 0)
    {
        // Return encoding operation
        return e_encode;
    }

    // Check whether the first argument is "-d"
    else if (strcasecmp(argv[1], "-d") == 0)
    {
        // Return decoding operation
        return e_decode;
    }

    // Return unsupported for invalid arguments
    else
    {
        return e_unsupported;
    }
}

// Function to calculate the size of a file
uint get_file_size(FILE *fptr)
{
    // Move the file pointer to the end of the file
    fseek(fptr, 0, SEEK_END);

    // Return the current file position as file size
    return ftell(fptr);
}

// Main function to perform the complete encoding process
Status do_encoding(EncodeInfo *encInfo)
{
    // Calculate the capacity of the source BMP image
    encInfo->image_capacity =
        get_image_size_for_bmp(encInfo->fptr_src_image);

    // Calculate the size of the secret file
    encInfo->size_secret_file =
        get_file_size(encInfo->fptr_secret);

    // Check whether the image has enough capacity
    if (check_capacity(encInfo) == e_failure)
    {
        printf("Insufficient image capacity\n");
        return e_failure;
    }

    // Copy the original BMP header into the stego image
    if (copy_bmp_header(encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) == e_failure)
    {
        printf("Error to encode Header file..\n");
        return e_failure;
    }
    else
    {
        printf("Encoded Header Data Successfully..\n");
    }

    // Encode the magic string into the source image
    // The magic string identifies the hidden data
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("Error to encode Magic String..\n");
        return e_failure;
    }
    else
    {
        printf("Encoded Magic string Successfully..\n");
    }

    // Encode the secret file extension and its length
    if (encode_secret_file_extn(encInfo) == e_failure)
    {
        printf("Error to encode Extension type...\n");
        return e_failure;
    }
    else
    {
        printf("Encoded extension type Successfully..\n");
    }

    // Encode the size of the secret file
    if (encode_secret_file_size(encInfo) == e_failure)
    {
        printf("Error to encode Secret file size...\n");
        return e_failure;
    }
    else
    {
        printf("Encoded Secret file size Successfully..\n");
    }

    // Encode the actual secret file data into the image
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("Error to encode Secret file Data...\n");
        return e_failure;
    }
    else
    {
        printf("Encoded Secret file data Successfully..\n");
    }

    // Copy the remaining original image data
    // after encoding the secret information
    if (copy_remaining_img_data(encInfo->fptr_src_image,
                                encInfo->fptr_stego_image) == e_failure)
    {
        printf("Error to Encode remaining data..\n");
        return e_failure;
    }
    else
    {
        printf("Encoded Remaining data Successfully..\n");
    }

    // Return success after completing all encoding steps
    return e_success;
}

// Function to check whether the source image
// has enough capacity to store the secret file
Status check_capacity(EncodeInfo *encInfo)
{
    // Calculate total payload size in bytes:
    // Magic string length
    // + extension length integer
    // + extension characters
    // + secret file size integer
    // + secret file data
    uint payload_bytes =
        strlen(MAGIC_STRING) +
        sizeof(int) +
        strlen(encInfo->extn_secret_file) +
        sizeof(int) +
        encInfo->size_secret_file;

    // Each payload byte requires 8 image bytes
    // because one bit is stored in each image byte
    uint required = payload_bytes * 8;

    // Display available and required image capacity
    printf("Image capacity = %u, Required = %u\n",
           encInfo->image_capacity, required);

    // Check whether the image can hold the secret data
    if (encInfo->image_capacity < required)
    {
        return e_failure;
    }

    // Return success if sufficient capacity is available
    return e_success;
}

// Function to copy the original BMP header
// from the source image to the destination image
Status copy_bmp_header(FILE *fptr_src_image,
                       FILE *fptr_dest_image)
{
    // Check whether both file pointers are valid
    if (fptr_src_image == NULL || fptr_dest_image == NULL)
    {
        return e_failure;
    }

    // Create a buffer to store the 54-byte BMP header
    char buffer[54];

    // Move the source image file pointer to the beginning
    rewind(fptr_src_image);

    // Read the 54-byte BMP header
    fread(buffer, 1, 54, fptr_src_image);

    // Write the BMP header into the destination image
    if (fwrite(buffer, 1, 54, fptr_dest_image) != 54)
    {
        return e_failure;
    }

    // Return success after copying the header
    return e_success;
}

// Function to encode the magic string into the image
Status encode_magic_string(const char *magic_string,
                           EncodeInfo *encInfo)
{
    // Check whether the input pointers are valid
    if (magic_string == NULL || encInfo == NULL)
    {
        return e_failure;
    }

    // Check whether all required file pointers are valid
    if (encInfo->fptr_src_image == NULL ||
        encInfo->fptr_secret == NULL ||
        encInfo->fptr_stego_image == NULL)
    {
        return e_failure;
    }

    // Calculate the length of the magic string
    int len = strlen(magic_string);

    // Encode the magic string into the source image
    // and write the modified image bytes to the stego image
    if (encode_data_to_image((char *)magic_string, len,
                             encInfo->fptr_src_image,
                             encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    // Return success after encoding the magic string
    return e_success;
}

// Function to encode multiple bytes of data into the image
Status encode_data_to_image(char *data, int size,
                            FILE *fptr_src_image,
                            FILE *fptr_stego_image)
{
    // Check whether the data and file pointers are valid
    if (data == NULL || fptr_src_image == NULL ||
        fptr_stego_image == NULL || size < 0)
    {
        return e_failure;
    }

    // Buffer to store 8 image bytes
    // One data byte requires 8 image bytes
    unsigned char buffer[8];

    // Repeat the process for each byte of secret data
    for (int i = 0; i < size; i++)
    {
        // Read 8 bytes from the source image
        if (fread(buffer, 1, 8, fptr_src_image) != 8)
        {
            return e_failure;
        }

        // Hide one data byte in the LSBs of 8 image bytes
        if (encode_byte_to_lsb(data[i], (char *)buffer) == e_failure)
        {
            return e_failure;
        }

        // Write the modified image bytes into the stego image
        if (fwrite(buffer, 1, 8, fptr_stego_image) != 8)
        {
            return e_failure;
        }
    }

    // Return success after encoding all data bytes
    return e_success;
}

// Function to encode one byte into the LSBs of 8 image bytes
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // Repeat the process for all 8 bits of the data byte
    for (int i = 0; i < 8; i++)
    {
        // Clear the least significant bit of the image byte
        // ~1 produces all bits set except the last bit
        image_buffer[i] = image_buffer[i] & ~1;

        // Extract one bit from the secret data byte
        // Shift the required bit to the LSB position
        // Use bitwise AND with 1 to isolate that bit
        // Store the extracted bit in the image byte's LSB
        image_buffer[i] =
            image_buffer[i] | ((data >> (7 - i)) & 1);
    }

    // Return success after encoding one byte
    return e_success;
}

// Function to encode the secret file extension
// and its length into the image
Status encode_secret_file_extn(EncodeInfo *encInfo)
{
    // Calculate the length of the secret file extension
    int len = strlen(encInfo->extn_secret_file);

    // Encode the extension length as an integer
    if (encode_data_to_image((char *)&len, sizeof(int),
                             encInfo->fptr_src_image,
                             encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    // Encode the actual extension characters
    // Example: ".txt", ".c"
    if (encode_data_to_image(encInfo->extn_secret_file, len,
                             encInfo->fptr_src_image,
                             encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    // Return success after encoding the extension
    return e_success;
}

// Function to encode the secret file size into the image
Status encode_secret_file_size(EncodeInfo *encInfo)
{
    // Store the secret file size in an integer variable
    int len = encInfo->size_secret_file;

    // Encode the file size as binary data into the image
    if (encode_data_to_image((char *)&len, sizeof(int),
                             encInfo->fptr_src_image,
                             encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    // Return success after encoding the file size
    return e_success;
}

// Function to encode the actual secret file contents
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // Check whether the structure and file pointers are valid
    if (encInfo == NULL ||
        encInfo->fptr_secret == NULL ||
        encInfo->fptr_src_image == NULL ||
        encInfo->fptr_stego_image == NULL)
    {
        return e_failure;
    }

    // Allocate memory to store the secret file contents
    char *secret_data = malloc(encInfo->size_secret_file);

    // Check whether memory allocation was successful
    if (secret_data == NULL)
    {
        return e_failure;
    }

    // Move the secret file pointer to the beginning
    rewind(encInfo->fptr_secret);

    // Read the complete secret file into memory
    if (fread(secret_data, 1, encInfo->size_secret_file,
              encInfo->fptr_secret) !=
              (size_t)encInfo->size_secret_file)
    {
        // Free allocated memory if reading fails
        free(secret_data);
        return e_failure;
    }

    // Encode the secret file data into the source image
    // and write the modified bytes to the stego image
    if (encode_data_to_image(secret_data,
                             encInfo->size_secret_file,
                             encInfo->fptr_src_image,
                             encInfo->fptr_stego_image) == e_failure)
    {
        // Free allocated memory if encoding fails
        free(secret_data);
        return e_failure;
    }

    // Free memory after encoding is completed
    free(secret_data);

    // Return success after encoding the secret data
    return e_success;
}

// Function to copy the remaining image data
// after the secret data has been encoded
Status copy_remaining_img_data(FILE *fptr_src,
                               FILE *fptr_dest)
{
    // Check whether both file pointers are valid
    if (fptr_src == NULL || fptr_dest == NULL)
    {
        return e_failure;
    }

    // Create a buffer to copy image data in chunks
    unsigned char buffer[1024];

    // Variable to store the number of bytes read
    size_t n;

    // Read the remaining source image data
    // in chunks of up to 1024 bytes
    while ((n = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        // Write the read bytes into the destination image
        if (fwrite(buffer, 1, n, fptr_dest) != n)
        {
            return e_failure;
        }
    }

    // Return success after copying all remaining data
    return e_success;
}