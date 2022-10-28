#ifndef _FLASH_H_
#define _FLASH_H_

#define N_UST             13    
#define FLASH_START_ADDR 0x08007C00
#define FLASH_END_ADDR   0x08008000


#define TEMP1      Ustanovki[1]
#define TEMP2      Ustanovki[2]
#define HYST1      Ustanovki[3]
#define HYST2      Ustanovki[4]
#define OGRUP1     Ustanovki[5]
#define OGRDOWN1   Ustanovki[6]
#define OGRUP2     Ustanovki[7]
#define OGRDOWN2   Ustanovki[8]
#define CORRECT1   Ustanovki[9]
#define CORRECT2   Ustanovki[10]
#define INV1       Ustanovki[11]
#define INV2       Ustanovki[12]


void EEPROM_erase(void);
void Write_ustanovki(void);
void Read_ustanovki(void);
void EEPROM_checking(void);
void def_write(void);


#endif