#ifndef DECODE_H
#define DECODE_H

#include "types.h" 
#include "common.h"
#include <stdio.h>// Contains user defined types

/* 
 * Structure to store information required for
 * decoding the secret file from the stego Image
 * Info about output and intermediate data is
 * also stored
 */

#define DECODE_MAX_IMAGE_BUF_SIZE 8 
#define BMP_HEADER_SIZE 54

typedef struct _DecodeInfo {
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char *decoded_fname;
    FILE *fptr_decoded;

    char extn_secret_file[DECODE_MAX_IMAGE_BUF_SIZE];
    int size_secret_file;
} DecodeInfo;
/* Decoding function prototypes */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Decode magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode secret file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *ext_size);

/* Decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo, char *file_ext, int ext_size);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size);

/* Decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode data from image */
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode a byte from LSB of image data array */
Status decode_byte_from_lsb(char *data, FILE *fptr_stego_image);

/* Decode size from LSB */
Status decode_size_from_lsb(char *image_buffer, int *size);

/* Move file pointer to the start of the image data */

#endif
