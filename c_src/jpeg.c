// Authorship lost, no copyrights
#include <stdint.h>
#include "erl_nif.h"
#include <stdio.h>

#include <turbojpeg.h>

static ERL_NIF_TERM
decode_erl(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
  ERL_NIF_TERM bin = argv[0];
  ErlNifBinary data, yuyv, yuv, rgb;
  uint32_t res;
  if (!enif_inspect_binary(env, bin, &data)) {
    return enif_make_badarg(env);
  }

  tjhandle jpeg = tjInitDecompress();
  int width, height, jpegsubsamp;
  tjDecompressHeader2(jpeg, data.data, data.size, &width, &height, &jpegsubsamp);
  
  enif_alloc_binary(TJBUFSIZEYUV(width, height, jpegsubsamp), &yuyv);


  tjDecompressToYUV(jpeg, data.data, data.size, yuyv.data, 0);
  if(jpegsubsamp != TJ_422) {
    fprintf(stderr, "Invalid subsampler\r\n");
  }

  enif_alloc_binary(TJBUFSIZEYUV(width, height, TJ_420), &yuv);
  
  int stride_size = width*height;
  memcpy(yuv.data, yuyv.data, stride_size);
  int i;
  for(i = 0; i < height/2; i++) {
    memcpy(yuv.data + stride_size + i*width/2, yuyv.data + stride_size + i*width, width/2);
    memcpy(yuv.data + stride_size*5/4 + i*width/2, yuyv.data + stride_size*3/2 + i*width, width/2);
  }
  
  enif_release_binary(&yuyv);
  return enif_make_tuple3(env, enif_make_binary(env, &yuv), enif_make_int(env, width), enif_make_int(env, height));
}

static ErlNifFunc jpeg_funcs[] =
{
    {"decode_yuv", 1, decode_erl}
};

ERL_NIF_INIT(jpeg, jpeg_funcs, NULL, NULL, NULL, NULL)
