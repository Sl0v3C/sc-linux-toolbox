/*
 * tinpyyplay_fifo use libmad & libid3v2 to create FIFO file then decoding & playing.
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
static int fifow;

static int decode(unsigned char const *, unsigned long);
int parse_mp3_header(struct mp3_header *header, unsigned int *num_channels, 
                 unsigned int *sample_rate, unsigned int *bit_rate, char *argv);
int createPcm(struct pcm_config *config, unsigned int channels, unsigned int sampleRate, int card, int device);

void stream_close(int sig)
{
    /* allow the stream to be closed gracefully */
    signal(sig, SIG_IGN);
    close_stream = 1;
}

int main(int argc, char *argv[])
{
    struct mp3_header mp3header;
    unsigned int mp3_channels, mp3_rate, mp3_bitsRate;
    struct stat stat;
    void *fdm;
    int fd, fifor, size, card, device, num_read;
    struct pcm_config config;
    char *buffer, buf[256];
    pid_t pid;
    mode_t mode = 0666; // 新创建的FIFO模式

    if(argc != 7) {
        printf("Usage: %s sample.mp3 FIFO_name [-D card] [-d device]\n", argv[0]);
        return -1;  
    }

    if ((mkfifo(argv[2], mode)) < 0) {
        printf("fail to mkfifo %s\n", argv[2]);
        return -1;
    }

    printf("=======================================================================\n");
    card = atoi(argv[4]);
    device = atoi(argv[6]);

// MP3 Parse ++
    if (parse_mp3_header(&mp3header, &mp3_channels, &mp3_rate, &mp3_bitsRate, argv[1]) == -1) {
        return -1;
    }
// MP3 Parse --

// open MP3 File & mmap to memory ++
    fd = open(argv[1], O_RDONLY);  
    if (fd < 0) {
        printf("Fail to open file: %s read\n", argv[1]);
        return -1;
    }

    if (fstat (fd, &stat) == -1 || stat.st_size == 0) {
        printf("fail to fstat\n");
        goto fail;
    }

    fdm = mmap (0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);//将文件内容拷贝到内存里面  

    if (fdm == MAP_FAILED) {
        printf("fail to mmap\n");
        goto fail;
    }
// open MP3 File & mmap to memory --
    
// init pcm config & open pcm device ++
    if (createPcm(&config, mp3_channels, mp3_rate, card, device) == -1)
        goto fail;
// init pcm config & open pcm device --

// caculate the pcm buffer size: Period_size * period_count * [2(16bit) | 4(32bit)] * 2 & malloc buffer ++
    size = pcm_frames_to_bytes(pcm_d, pcm_get_buffer_size(pcm_d));
    buffer = malloc(size);
    if (!buffer) {
        printf("Unable to allocate %d bytes\n", size);
        goto fail;
    }
// caculate the pcm buffer size: Period_size * period_count * [2(16bit) | 4(32bit)] * 2 & malloc buffer --

    pid = fork();
    /* catch ctrl-c to shutdown cleanly */
    signal(SIGINT, stream_close);

    if (pid < 0) {
        printf("Fail to fork\n");
        goto fail;
    } else if (pid == 0) {
        //printf("Child process to decode & write to FIFO\n"); 
        fifow = open(argv[2], O_WRONLY);
        if (fifow < 0) {
            printf("Fail to open FIFO write only\n");
            goto fail;
        }   
        decode (fdm, stat.st_size);//进行解码 
        close(fd);
        if (munmap (fdm, stat.st_size) == -1) { 
            printf("fail to unmap\n");
            return -1;  
        }
        exit(0);
    }
 
    printf("Play MP3 file: card %d device %d samplerate %d channels %d\n", card, device, config.rate, config.channels);
    fifor = open(argv[2], O_RDONLY);
    if (fifor < 0) {
        printf("Fail to open FIFO read only\n");
        return -1;
    }   

// PCM write ++
    do {
        num_read = read(fifor, buffer, size);
        if (num_read > 0) {
            if (pcm_write(pcm_d, buffer, num_read)) {
                    printf("Error playing sample\n");
                break;
            }
        }
    } while (!close_stream && num_read > 0);
// PCM write --

    if (!close_stream) {
        printf("Complete the playback\n");
        printf("Quitting...\n");
    } else {
        kill(pid, SIGKILL);
        printf("User force to stop the software\n");
    }

    printf("=======================================================================\n");
    close(fifor);
    close(fifow);
    free(buffer);
    pcm_close(pcm_d);
// Delete FIFO ++
    sprintf(buf, "rm -f %s", argv[2]);
    system(buf);
// Delete FIFO --
    
    return 0;

fail:
    if (out)
        fclose(out);
    if (fd)
        close(fd);
    if (fifor)
        close(fifor);
    if (fifow)
        close(fifow);
    if (buffer)
        free(buffer);
    if (fdm)
        if (munmap (fdm, stat.st_size) == -1) { 
            printf("fail to unmap\n");
            return -1;  
        }
    if (pcm_d)
        pcm_close(pcm_d);
// Delete FIFO ++
    sprintf(buf, "rm -f %s", argv[2]);
    system(buf);
// Delete FIFO --

    return -1;
}

int parse_mp3_header(struct mp3_header *header, unsigned int *num_channels,
                      unsigned int *sample_rate, unsigned int *bit_rate, char *argv)
{
    ID3v2_tag* tag = NULL;
    const char *filename;
    FILE *file = NULL;
    int ver_idx, mp3_version, layer, bit_rate_idx, sample_rate_idx, channel_idx;

    filename = argv;
    tag = (ID3v2_tag*)malloc(sizeof(ID3v2_tag));
    if (NULL == tag) {
        printf("Error: tag is null");
        exit(1);
    }
    tag = load_tag(filename);
    file = fopen(filename, "rb");
    if (NULL == file) {
        printf("Unable to open file '%s'\n", file);
        return -1;
    }

    fseek(file, tag->tag_header->tag_size + 10, SEEK_SET);
    fread(header, sizeof(header), 1, file);

    ver_idx = (header->sync >> 11) & 0x03;
    mp3_version = ver_idx == 0 ? MPEG25 : ((ver_idx & 0x1) ? MPEG1 : MPEG2);
    layer = 4 - ((header->sync >> 9) & 0x03);
    bit_rate_idx = ((header->format1 >> 4) & 0x0f);                                                                                                                                                             
    sample_rate_idx = ((header->format1 >> 2) & 0x03);
    channel_idx = ((header->format2 >> 6) & 0x03);

    if (sample_rate_idx == 3 || layer == 4 || bit_rate_idx == 15) {
        printf("Error: Can't find valid header\n");
        fclose(file);
        return -1; 
    }   
    *num_channels = (channel_idx == MONO ? 1 : 2); 
    *sample_rate = mp3_sample_rates[mp3_version][sample_rate_idx];
    *bit_rate = (mp3_bit_rates[mp3_version][layer - 1][bit_rate_idx]) * 1000;

    printf("MP3 version %d, layer %d, channels %u, sampleRate %u, bit_rate %u\n", ver_idx, layer, *num_channels, *sample_rate, *bit_rate);
    fclose(file);
    free(tag);
    
    return 0;
}

int createPcm(struct pcm_config *config, unsigned int channels, unsigned int sampleRate, int card, int device)
{
    config->channels = channels;
    config->rate = 48000;//sampleRate;
    config->period_size = PERIOD_SIZE;
    config->period_count = PERIOD_COUNT;
    config->format = PCM_FORMAT_S16_LE;
    config->start_threshold = 0;
    config->stop_threshold = 0;
    config->silence_threshold = 0;
    pcm_d = pcm_open(card, device, PCM_OUT, config);

    if (!pcm_d || !pcm_is_ready(pcm_d)) {
        printf("Unable to open PCM device %u (%s)\n", device, pcm_get_error(pcm_d));
        return -1;
    }

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
    if ((write(fifow, output, n * nchannels * 2)) < 0) {
            printf("fail to write\n");
            close(fifow);
    }
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
