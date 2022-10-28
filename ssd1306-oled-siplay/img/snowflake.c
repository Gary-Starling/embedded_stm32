
/*******************************************************************************
* image
* filename: unsaved
* name: snowflake_102701
*
* preset name: Monochrome
* data block size: 8 bit(s), uint8_t
* RLE compression enabled: no
* conversion type: Monochrome, Diffuse Dither 128
* bits per pixel: 1
*
* preprocess:
*  main scan direction: top_to_bottom
*  line scan direction: forward
*  inverse: yes
*******************************************************************************/

/*
 typedef struct {
     const uint8_t *data;
     uint16_t width;
     uint16_t height;
     uint8_t dataSize;
     } tImage;
*/
#include <stdint.h>


//13x13
const uint8_t image_data_snow[26] = 
{
    0x0a, 0x80, 
    0x07, 0x00, 
    0x92, 0x48, 
    0x52, 0x50, 
    0x22, 0x20, 
    0xd7, 0x58, 
    0x0d, 0x80, 
    0xd7, 0x58, 
    0x22, 0x20, 
    0x52, 0x50, 
    0x92, 0x48, 
    0x07, 0x00, 
    0x0a, 0x80
};


