#include "decode.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "types.h"

// Function to read and validate decode arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] == NULL)
    {
        printf("Error: Stego image file name is missing.\n");
        return e_failure;
    }

    if (strstr(argv[2], ".bmp") == NULL)
    {
        printf("Error: Stego image file should be a .bmp file.\n");
        return e_failure;
    }
    decInfo->stego_image_fname = argv[2];

    if (argv[3] != NULL)
    {
        decInfo->decoded_fname = argv[3];
    }
    else
    {
        decInfo->decoded_fname = "decode.txt";
    }

    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("Error: Unable to open file %s for reading.\n", decInfo->stego_image_fname);
        return e_failure;
    }
    decInfo->fptr_decoded = fopen(decInfo->decoded_fname, "w");
    if (decInfo->fptr_decoded == NULL)
    {
        printf("Error: Unable to open file %s for writing.\n", decInfo->decoded_fname);
        fclose(decInfo->fptr_stego_image);
        return e_failure;
    }

    return e_success;
}

Status decode_size_from_lsb(char *image_buffer, int *size)
{
    *size = 0;
    for (int i = 0; i < sizeof(int) * 8; i++)
    {
        *size |= ((image_buffer[i / 8] >> (i % 8)) & 1) << i;
    }
    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image, BMP_HEADER_SIZE, SEEK_SET);

    char decoded_magic_string[3] = {0};
    for (int i = 0; i < 2; i++)
    {
        if (decode_byte_from_lsb(&decoded_magic_string[i], decInfo->fptr_stego_image) == e_failure)
        {
            printf("Error: Failed to decode magic string.\n");
            return e_failure;
        }
    }
    if (strcmp(decoded_magic_string, magic_string) != 0)
    {
        printf("Error: Magic string does not match\n");
        return e_failure;
    }

    return e_success;
}

// Function to decode secret file extension size
Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *ext_size)
{
    char image_buffer[4];
    if (fread(image_buffer, 1, 4, decInfo->fptr_stego_image) != 4)
    {
        printf("Error: Failed to read secret file extension size.\n");
        return e_failure;
    }

    return decode_size_from_lsb(image_buffer, ext_size);
}

// Function to decode secret file extension
Status decode_secret_file_extn(DecodeInfo *decInfo, char *file_ext, int ext_size)
{
    if (fread(file_ext, 1, ext_size, decInfo->fptr_stego_image) != ext_size)
    {
        printf("Error: Failed to read secret file extension.\n");
        return e_failure;
    }

    file_ext[ext_size] = '\0'; // Null-terminate the string

    return e_success;
}

// Function to decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo, long *file_size)
{
    char image_buffer[4];
    if (fread(image_buffer, 1, 4, decInfo->fptr_stego_image) != 4)
    {
        printf("Error: Failed to read secret file size.\n");
        return e_failure;
    }

    return decode_size_from_lsb(image_buffer, (int *)file_size);
}

// Function to decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        if (decode_byte_from_lsb(&ch, decInfo->fptr_stego_image) == e_failure)
        {
            printf("Error: Failed to decode secret file data.\n");
            return e_failure;
        }
        fputc(ch, decInfo->fptr_decoded);
    }

    return e_success;
}
// Function to decode byte from lsb
Status decode_byte_from_lsb(char *data, FILE *fptr_stego_image)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        char ch;
        if (fread(&ch, 1, 1, fptr_stego_image) != 1)
        {
            return e_failure;
        }
        *data |= ((ch & 1) << i);
    }
    return e_success;
}

// Function to perform decoding
Status do_decoding(DecodeInfo *decInfo)
{
    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        return e_failure;
    }
    int ext_size;
    if (decode_secret_file_extn_size(decInfo, &ext_size) == e_failure)
    {
        return e_failure;
    }
    if (decode_secret_file_extn(decInfo, decInfo->extn_secret_file, ext_size) == e_failure)
    {
        return e_failure;
    }
    long file_size;
    if (decode_secret_file_size(decInfo, &file_size) == e_failure)
    {
        return e_failure;
    }

    decInfo->size_secret_file = file_size;
    if (decode_secret_file_data(decInfo) == e_failure)
    {
        return e_failure;
    }

    return e_success;
}
