
/*******************************************************************************
* image
* filename: unsaved
* name: gradusnik
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


//11x18
const uint8_t image_data_gradusnik[36] = {
    0x00, 0x40, 
    0x18, 0xe0, 
    0x24, 0x40, 
    0x42, 0x00, 
    0x42, 0x00, 
    0x42, 0x00, 
    0x5a, 0x00, 
    0x5a, 0x00, 
    0x5a, 0x00, 
    0x5a, 0x00, 
    0x5a, 0x00, 
    0x5a, 0x00, 
    0x99, 0x00, 
    0xbd, 0x00, 
    0xbd, 0x00, 
    0x99, 0x00, 
    0x42, 0x00, 
    0x3c, 0x00
};



