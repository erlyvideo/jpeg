// Authorship lost, no copyrights
#include <stdint.h>
#include "erl_nif.h"
#include <stdio.h>

#include <turbojpeg.h>
#include <libswscale/swscale.h>

struct SwsContext* scale_ctx = NULL;

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
  
  // fprintf(stderr, "YUV: %dx%d -> %d,%d\r\n", width, height, width*height*3/2, TJBUFSIZEYUV(width, height, jpegsubsamp));
  // enif_alloc_binary(TJBUFSIZEYUV(width, height, jpegsubsamp), &yuyv);
  enif_alloc_binary(TJBUFSIZE(width, height), &rgb);
  enif_alloc_binary(TJBUFSIZEYUV(width, height, TJ_420), &yuv);


  // tjDecompressToYUV(jpeg, data.data, data.size, yuyv.data, 0);
  tjDecompress(jpeg, data.data, data.size, rgb.data, width, 0, height, 3, 0);

  
  scale_ctx = sws_getCachedContext(scale_ctx,
    // width, height, PIX_FMT_YUYV422, 
    width, height, PIX_FMT_RGB24, 
    width, height, PIX_FMT_YUV420P, 
    SWS_FAST_BILINEAR, NULL, NULL, NULL
  );

  // int linesize[4] = {width, 0, 0, 0};
  // uint8_t *src[4] = {yuyv.data, 0, 0, 0};
  int linesize[4] = {width*3, 0, 0, 0};
  uint8_t *src[4] = {rgb.data, 0, 0, 0};
  
  int stride_size = width*height;
  uint8_t *plane[4] = {yuv.data, yuv.data+stride_size, yuv.data+stride_size+stride_size/4, NULL};
  int stride[4] = {width, width/2, width/2, 0};
  
  sws_scale(scale_ctx, src, linesize, 0, height, plane, stride);
  
  // enif_release_binary(&yuyv);
  enif_release_binary(&rgb);
  return enif_make_tuple3(env, enif_make_binary(env, &yuv), enif_make_int(env, width), enif_make_int(env, height));
}

static ErlNifFunc jpeg_funcs[] =
{
    {"decode", 1, decode_erl}
};

ERL_NIF_INIT(jpeg, jpeg_funcs, NULL, NULL, NULL, NULL)