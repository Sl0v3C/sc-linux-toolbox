/*
 * tinpyyplay use libmad & libid3v2 to decode mp3 then write to pcm file and playback.
 *                                                                                                                                                                                                                  
 * Copyright (C) 2016 Sl0v3C.
 *
 * Author: Sl0v3C <pyy101727@gmail.com>
 *
 * Based on libmad & libid3v2
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <tinyalsa/asoundlib.h>
#include <signal.h>
#include "tinymp3.h"
#include "id3v2lib-master/id3v2lib.h"
#include "mad.h"

/*
 * This is perhaps the simplest example use of the MAD high-level API.
 * Standard input is mapped into memory via mmap(), then the high-level API
 * is invoked with three callbacks: input, output, and error. The output
 * callback converts MAD's high-resolution PCM samples to 16 bits, then
 * writes them to standard output in little-endian, stereo-interleaved
 * format.
 */
#define PERIOD_SIZE 1024 
#define PERIOD_COUNT 4

struct mp3_header {
        uint16_t sync;    //16 bit
        uint8_t format1;  // 8 bit
        uint8_t format2;  // 8 bit
};

FILE *out; 
static struct pcm *pcm_d;
static int close_stream = 0;

static int decode(unsigned char const *, unsigned long);
int parse_mp3_header(struct mp3_header *header, unsigned int *num_channels, 
                 unsigned int *sample_rate, unsigned int *bit_rate);

void stream_close(int sig)
{
    /* allow the stream to be closed gracefully */
    signal(sig, SIG_IGN);
    close_stream = 1;
}

int main(int argc, char *argv[])
{
    FILE *File = NULL;
    ID3v2_tag* tag = NULL;
    struct mp3_header mp3header;
    unsigned int mp3_channels, mp3_rate, mp3_bitsRate;
    struct stat stat;
    void *fdm;
    char const *file;  
    int fd, size, card, device;
    struct pcm_config config;
    int num_read;
    char *buffer;

    if(argc != 7) {
        printf("Usage: %s sample.mp3 out.pcm [-D card] [-d device]\n", argv[0]);
        return -1;  
    }

    out = fopen(argv[2], "w+");  
    if (out == NULL) {
        printf("Fail to open file\n");
        return -1;
    }
    
    card = atoi(argv[4]);
    device = atoi(argv[6]);
    file = argv[1];

// MP3 Parse ++
    tag = (ID3v2_tag*)malloc(sizeof(ID3v2_tag));
    if (NULL == tag) {
        printf("Error: tag is null");
        exit(1);
    }
    tag = load_tag(file);
    File = fopen(file, "rb");
    if (File == NULL) {
        fclose(out);
        printf("Unable to open file '%s'\n", file);
        return 1;
    }

    fseek(File, tag->tag_header->tag_size + 10, SEEK_SET);
    fread(&mp3header, sizeof(mp3header), 1, File);
    if (parse_mp3_header(&mp3header, &mp3_channels, &mp3_rate, &mp3_bitsRate) == -1) {
        fclose(File);
        fclose(out);
        exit(EXIT_FAILURE);
    }
    fclose(File);
    free(tag);
// MP3 Parse --

// open MP3 File & mmap to memory ++
    fd = open(file, O_RDONLY);  
    if (fd < 0) {
        printf("Fail to open file: %s read\n", file);
        fclose(out);
        return -1;
    }

    if (fstat (fd, &stat) == -1 || stat.st_size == 0) {
        printf("fail to fstat\n");
        close(fd);
        fclose(out);
        return -1;  
    }

    fdm = mmap (0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);//将文件内容拷贝到内存里面  

    if (fdm == MAP_FAILED) {
        printf("fail to mmap\n");
        close(fd);
        fclose(out);
        return -1;  
    }
// open MP3 File & mmap to memory --
    
// init pcm config & open pcm device ++
    config.channels = mp3_channels;
    config.rate = 48000;//mp3_rate;
    config.period_size = PERIOD_SIZE;
    config.period_count = PERIOD_COUNT;
    config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;
    pcm_d = pcm_open(card, device, PCM_OUT, &config);

    if (!pcm_d || !pcm_is_ready(pcm_d)) {
        fprintf(stderr, "Unable to open PCM device %u (%s)\n",
                device, pcm_get_error(pcm_d));
        close(fd);
        fclose(out);
        return -1;
    }
// init pcm config & open pcm device --

    decode (fdm, stat.st_size);//进行解码 

    close(fd);
    fclose(out);//关闭文件指针  
    if (munmap (fdm, stat.st_size) == -1) { 
        printf("fail to unmap\n");
        return -1;  
    }
 
    /* catch ctrl-c to shutdown cleanly */
    signal(SIGINT, stream_close);

// open PCM file ++
    out = fopen(argv[2], "r");  
    if (out == NULL) {
        printf("Fail to open file %s read\n", argv[2]);
        return -1;
    }
// open PCM file --
// caculate the pcm buffer size: Period_size * period_count * [2(16bit) | 4(32bit)] * 2 & malloc buffer ++
    size = pcm_frames_to_bytes(pcm_d, pcm_get_buffer_size(pcm_d));
    buffer = malloc(size);
    if (!buffer) {
        fprintf(stderr, "Unable to allocate %d bytes\n", size);
        free(buffer);
        pcm_close(pcm_d);
        return -1;
    }
// caculate the pcm buffer size: Period_size * period_count * [2(16bit) | 4(32bit)] * 2 & malloc buffer --

    printf("Play MP3 file: card %d device %d samplerate %d channels %d\n", card, device, config.rate, config.channels);

// PCM write ++
    do {
        num_read = fread(buffer, 1, size, out);
        if (num_read > 0) {
            if (pcm_write(pcm_d, buffer, num_read)) {
                printf("Error playing sample\n");
                break;
            }
        }
    } while (!close_stream && num_read > 0);
// PCM write --

    free(buffer);
    pcm_close(pcm_d);
    fclose(out);//关闭文件指针  

    return 0;  
}

int parse_mp3_header(struct mp3_header *header, unsigned int *num_channels,
                      unsigned int *sample_rate, unsigned int *bit_rate)
{
    int ver_idx, mp3_version, layer, bit_rate_idx, sample_rate_idx, channel_idx;
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


/*
 * This is a private message structure. A generic pointer to this structure
 * is passed to each of the callback functions. Put here any data you need
 * to access from within the callbacks.
 */

struct buffer {
  unsigned char const *start;
  unsigned long length;
};

/*
 * This is the input callback. The purpose of this callback is to (re)fill
 * the stream buffer which is to be decoded. In this example, an entire file
 * has been mapped into memory, so we just call mad_stream_buffer() with the
 * address and length of the mapping. When this callback is called a second
 * time, we are finished decoding.
 */

static
enum mad_flow input(void *data,
		    struct mad_stream *stream)
{
  struct buffer *buffer = data;

  if (!buffer->length)
    return MAD_FLOW_STOP;

  mad_stream_buffer(stream, buffer->start, buffer->length);

  buffer->length = 0;

  return MAD_FLOW_CONTINUE;
}

/*
 * The following utility routine performs simple rounding, clipping, and
 * scaling of MAD's high-resolution samples down to 16 bits. It does not
 * perform any dithering or noise shaping, which would be recommended to
 * obtain any exceptional audio quality. It is therefore not recommended to
 * use this routine if high-quality output is desired.
 */

static inline
signed int scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return sample >> (MAD_F_FRACBITS + 1 - 16);
}

/*
 * This is the output callback function. It is called after each frame of
 * MPEG audio data has been completely decoded. The purpose of this callback
 * is to output (or play) the decoded PCM audio.
 */

static
enum mad_flow output(void *data,
		     struct mad_header const *header,
		     struct mad_pcm *pcm)
{
    unsigned int nchannels, nsamples, n;  
    mad_fixed_t const *left_ch, *right_ch;  
    unsigned char output[4608], *ptr;  
    nchannels = pcm->channels;  
    n = nsamples = pcm->length;  
    left_ch = pcm->samples[0];  
    right_ch = pcm->samples[1];  
    ptr = output;  //将OutputPtr指向Output  
    while (nsamples--)  
    {  
        signed int sample;  
        sample = scale (*left_ch++);  
        *(ptr++) = sample >> 0;  
        *(ptr++) = sample >> 8;  
        if (nchannels == 2)  
        {  
            sample = scale (*right_ch++);  
            *(ptr++) = sample >> 0;  
            *(ptr++) = sample >> 8;  
        }  
    }  
    ptr = output;   //由于之前的操作将OutputPtr的指针指向了最后，这时需要将其指针移动到最前面  
    fwrite(ptr, 1, n * 2 * nchannels, out);  
    ptr = output;   //写完文件后，OutputPtr的指针也移动到了最后，这时需要将其指针移动到最前面  

    return MAD_FLOW_CONTINUE;  
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or stream.h)
 * header file.
 */

static
enum mad_flow error(void *data,
		    struct mad_stream *stream,
		    struct mad_frame *frame)
{
  return MAD_FLOW_CONTINUE;
}

/*
 * This is the function called by main() above to perform all the decoding.
 * It instantiates a decoder object and configures it with the input,
 * output, and error callback functions above. A single call to
 * mad_decoder_run() continues until a callback function returns
 * MAD_FLOW_STOP (to stop decoding) or MAD_FLOW_BREAK (to stop decoding and
 * signal an error).
 */

static
int decode(unsigned char const *start, unsigned long length)
{
  struct buffer buffer;
  struct mad_decoder decoder;
  int result;

  /* initialize our private message structure */

  buffer.start  = start;
  buffer.length = length;

  /* configure input, output, and error functions */

  mad_decoder_init(&decoder, &buffer,
		   input, 0 /* header */, 0 /* filter */, output,
		   error, 0 /* message */);
  /* start decoding */
  mad_decoder_options (&decoder, 0);

  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

  /* release the decoder */

  mad_decoder_finish(&decoder);

  return result;
}
