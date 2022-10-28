/* Объяви свой SPI и заинициализируй его */

#define CS_PIN          GPIO_PIN_3
#define CS_PORT         GPIOC

#define CS_SET          HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_SET);
#define CS_RESET        HAL_GPIO_WritePin(CS_PORT, CS_PIN, GPIO_PIN_RESET);

#define PAGE_READ      256

#define ONE_MB         1024


#define ALL_SIZE       8388608 

#define WRITE_ADT_ST    0x00

#define ERR     0
#define OK      1

/*
typedef struct 
{
  char name[32];  
  uint32_t adr;  //4   
  uint32_t size; //4
}InfoFile_str;
*/

//#define MAX_FILES  100 //���������� ������ � ���� � ���

uint8_t ad45test(void);
uint8_t at45_reset(void);
uint8_t at45_read_status(void);
uint8_t at45_read_data(uint16_t page, uint16_t addr, uint32_t length, uint8_t *out);
uint8_t at45_page_write(uint16_t page, uint8_t *data, uint16_t length);
uint16_t at45_read_part(uint16_t pg, uint32_t size, uint8_t* reciv);
//uint8_t write_file(char* file_name, InfoFile_str* fill);
//uint8_t at45_audio_start(char* name);

void at45_set_512(void) ;
void at45_full_erase(void);
void SPI2_Init(void);
void SPI_GPIO_Init(void);

