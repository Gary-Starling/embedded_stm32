
/*******************************************************************************
* image
* filename: unsaved
* name: arrow_down
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


//9x14
const uint8_t image_data_arrow_down[28] = {
    0x1c, 0x00, 
    0x1c, 0x00, 
    0x1c, 0x00, 
    0x1c, 0x00, 
    0x1c, 0x00, 
    0x1c, 0x00, 
    0x1c, 0x00, 
    0x1c, 0x00, 
    0x1c, 0x00, 
    0xff, 0x80, 
    0x7f, 0x00, 
    0x3e, 0x00, 
    0x1c, 0x00, 
    0x08, 0x00
};


