#include <iostream>
#include <fstream>

#pragma pack(push, 1)
struct BmpFileHeader
{
  uint16_t signature;
  uint32_t filesize;
  uint32_t _reserved;
  uint32_t pixarroffset;
};

struct BmpDibHeader
{
  uint32_t dibsize;
  int32_t width;
  int32_t height; //Positive height means pixel array is stored from bottom to top, negative means top to bottom 
  uint16_t planes;
  uint16_t bitperpix;
  uint32_t compression;
  uint32_t imgsize;
  uint32_t xpixperm;
  uint32_t ypixperm;
  uint32_t colortab;
  uint32_t importantcolor;
};
#pragma pack(pop)

uint8_t* decode_bmp(std::fstream& fs, uint32_t& w, uint32_t& h, uint32_t& c)
{
  BmpFileHeader f_header;
  BmpDibHeader dib_header;

  //Read file header and DIB header
  fs.read((char*)&f_header, sizeof(BmpFileHeader));
  fs.read((char*)&dib_header, sizeof(BmpDibHeader));
  
#ifdef BMP_DBG  
  std::cout << "Size of file header = " << sizeof(BmpFileHeader) << std::endl;
  std::cout << "Size of DIB header = " << sizeof(BmpDibHeader) << std::endl;
  
  std::cout << "File size = " << f_header.filesize << std::endl;
  std::cout << "Pixel array offset = " << f_header.pixarroffset << std::endl;
  
  std::cout << "Image width = " << dib_header.width << std::endl;
  std::cout << "Image height = " << dib_header.height << std::endl;
  std::cout << "Bit per pixel = " << dib_header.bitperpix << std::endl;
  std::cout << "Compression = " << dib_header.compression << std::endl;
  std::cout << "Image size = " << dib_header.imgsize << std::endl;
#endif

  w = dib_header.width;
  c = dib_header.bitperpix / 8;
  
  bool t_to_b;  //Bool value indicating whether image is stores from top to bottom
  //If height value is positive
  if(dib_header.height > 0)
    t_to_b = false;
  //If height value is negative
  else
    t_to_b = true;

  h = std::abs(dib_header.height);
  
  uint32_t row_size = w * c;    //Row size, in bytes
  uint32_t img_size = row_size * h;  //Image size, in bytes. Paddings excluded
  //See if paddings are needed
  uint8_t padding = w * c / sizeof(char) % 4;
  padding = padding == 0 ? padding : (4 - padding);

  //Move to pixel array
  fs.seekg(f_header.pixarroffset, std::ios_base::beg);
  //Allocate pixel data buffer
  uint8_t* data_beg = new uint8_t[img_size];
  //Mark off-end position
  uint8_t* data_end = data_beg + img_size;
  //Working iterator
  uint8_t* data_it = data_end;
  
  if(t_to_b)
    for(data_it = data_beg; data_it != data_end; data_it += row_size)
      {
	fs.read((char*)data_it, row_size);
	fs.seekg(padding, std::ios::cur);
      }
  else
    while((data_it -= row_size) >= data_beg)
      {
	fs.read((char*)data_it, row_size);
	fs.seekg(padding, std::ios::cur);
      }
  
  return data_beg;
}

int main(int argc, char** argv)
{
  if(argc != 3)
    {
      std::cout << "Usage: " << argv[0] << " [bmp image name] [decoded data file name]" << std::endl;
      return 0;
    }

  //Open bmp file
  std::fstream fs;
  fs.open(argv[1], std::ios::in | std::ios::binary);
  if(!fs.is_open())
    {
      std::cout << "Cannot open " << argv[1] << " as binary!" << std::endl;
      return 0;
    }
  
  //Decode bmp file
  uint8_t* data = NULL;
  uint32_t width, height, channel;
  data = decode_bmp(fs, width, height, channel);
  fs.close();
  
  //Write data file
  fs.open(argv[2], std::ios::out);
  fs << "#Width = " << width << " "
     << "Height = " << height << " "
     << "Channel = " << channel << "\n";
  uint32_t total_len = height * width * channel;
  for(uint32_t idx = 0; idx < total_len; ++idx)
    fs << int(data[idx]) << " ";

  fs << std::endl;
  fs.close();
  
  return 0;
}

