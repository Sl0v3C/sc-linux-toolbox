/*
 * mp3Pars use id3v2lib to decode mp3 then get some prameters.
 *                                                                                                                                                                                                                  
 * Copyright (C) 2016 Sl0v3C.
 *
 * Author: Sl0v3C <pyy101727@gmail.com>
 *
 * Based on id3v2lib
 *
 * License terms:
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "tinymp3.h"
#include "id3v2lib-master/id3v2lib.h"

struct mp3_header {
        uint16_t sync;    //16 bit
        uint8_t format1;  // 8 bit
        uint8_t format2;  // 8 bit
};

int parse_mp3_header(struct mp3_header *header, unsigned int *num_channels, 
                 unsigned int *sample_rate, unsigned int *bit_rate);

int main(int argc, char *argv[])
{
    FILE *file = NULL;
    ID3v2_tag* tag = NULL;
    struct mp3_header mp3header;
    unsigned int mp3_channels, mp3_rate, mp3_bits;
    const char *filename;

    if (argc != 2) {
        printf("Usage: %s [file: mp3]\n", argv[0]);
        return 1;
    }

    filename = argv[1];
    tag = (ID3v2_tag*)malloc(sizeof(ID3v2_tag));
    if (NULL == tag) {
        printf("Error tag is NULL\n");
        exit(1);
    }
    tag = load_tag(filename);
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Unable to open file '%s'\n", filename);
        return 1;
    }
    fseek(file, tag->tag_header->tag_size + 10, SEEK_SET);
    fread(&mp3header, sizeof(mp3header), 1, file);
    if (parse_mp3_header(&mp3header, &mp3_channels, &mp3_rate, &mp3_bits) == -1) {
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
    
    return 0;
}

int parse_mp3_header(struct mp3_header *header, unsigned int *num_channels,
                      unsigned int *sample_rate, unsigned int *bit_rate)
{
    int ver_idx, mp3_version, layer, bit_rate_idx, sample_rate_idx, channel_idx;
    fprintf(stderr, "header->sync %04x\n", header->sync);
    ver_idx = (header->sync >> 11) & 0x03;
    mp3_version = ver_idx == 0 ? MPEG25 : ((ver_idx & 0x1) ? MPEG1 : MPEG2);
    layer = 4 - ((header->sync >> 9) & 0x03);
    bit_rate_idx = ((header->format1 >> 4) & 0x0f);                                                                                                                                                             
    sample_rate_idx = ((header->format1 >> 2) & 0x03);
    channel_idx = ((header->format2 >> 6) & 0x03);

    if (sample_rate_idx == 3 || layer == 4 || bit_rate_idx == 15) {
        fprintf(stderr, "Error: Can't find valid header\n");
        return -1; 
    }   
    *num_channels = (channel_idx == MONO ? 1 : 2); 
    *sample_rate = mp3_sample_rates[mp3_version][sample_rate_idx];
    *bit_rate = (mp3_bit_rates[mp3_version][layer - 1][bit_rate_idx]) * 1000;

    printf("MP3 version %d, layer %d, channels %u, sampleRate %u, bit_rate %u\n", ver_idx, layer, *num_channels, *sample_rate, *bit_rate);
    
    return 0;
}




