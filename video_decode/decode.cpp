extern "C"
{
  #include <libavutil/opt.h>
  #include <libavcodec/avcodec.h>
  #include <libavformat/avformat.h>
  #include <libswresample/swresample.h>
}

int main(int argc, char** argv)
{
  av_register_all();



  return 0;
}
