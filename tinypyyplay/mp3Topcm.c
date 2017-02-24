/*
 * mp3Topcm use libmad to decode mp3 then write to pcm file
 *                                                                                                                                                                                                                  
 * Copyright (C) 2016 Sl0v3C.
 *
 * Author: Sl0v3C <pyy101727@gmail.com>
 *
 * Based on libmad
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
#include <signal.h>

#include "mad.h"

FILE *out; 
static int decode(unsigned char const *, unsigned long);

int main(int argc, char *argv[])
{
    struct stat stat;
    void *fdm;
    char const *file;  
    int fd, size;

    if(argc != 3 ) {
        printf("Wrong arguments\n");
        return -1;  
    }

    out = fopen(argv[2], "w+");  
    if (out == NULL) {
        printf("Fail to open file\n");
        return -1;
    }

    file = argv[1];  
    fd = open(file, O_RDONLY);  
    if (fd < 0) {
        printf("Fail to open file\n");
        fclose(out);
        return -1;
    }

    if (fstat (fd, &stat) == -1 || stat.st_size == 0) {
        printf("fail to fstat\n");
        close(fd);
        fclose(out);
        return -1;  
    }

    fdm = mmap (0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0); //将文件内容映射到内存里面  

    if (fdm == MAP_FAILED) {
        printf("fail to mmap\n");
        close(fd);
        fclose(out);
        return -1;  
    }
    
    decode (fdm, stat.st_size);   //进行解码  

    fclose(out); 
    close(fd);

    if (munmap (fdm, stat.st_size) == -1) { 
        printf("fail to unmap\n");
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
