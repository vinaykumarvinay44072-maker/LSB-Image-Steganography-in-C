#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
 
typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char *output_fname;
    FILE *fptr_output;
    int argc;

    int  extn_len;
    char extn_secret_file[8];
    long size_secret_file;

} DecodeInfo;

Status do_decoding(DecodeInfo *decInfo );
Status open_decode_files(DecodeInfo *decInfo);
Status skip_bmp_header(DecodeInfo *decInfo);
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);
Status decode_secret_file_extn_size(DecodeInfo *decInfo);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image);
Status decode_byte_from_lsb(char *data, char *image_buffer);

#endif