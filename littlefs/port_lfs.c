#include "lfs.h"
#include "at45db641e.h"


int block_at45_read(const struct lfs_config *c, lfs_block_t block,
                    lfs_off_t off, void *buffer, lfs_size_t size)
{
  int res = 0;
  
  res = at45_read_page(block, off, (uint8_t *)buffer, size);
  
  
  if(res == AT45_ERR)
    return LFS_ERR_IO;
  
  return LFS_ERR_OK;
}




int block_at45_prog(const struct lfs_config *c, lfs_block_t block,
                    lfs_off_t off, const void *buffer, lfs_size_t size)
{
  int res = 0; 
  
  /* не нужно делать запись вместе со стиранием, т.к. при lfs_file_close
  будет сначала произведено стирание блока, а потом зпись, т.е. 
  мы два раза потрём сектор. */
  res = at45_page_write_noer(block, off, (uint8_t *)buffer, size, BUFFER1);
  
  if(res == AT45_ERR)
    return LFS_ERR_IO;
  
  return LFS_ERR_OK;
}

int block_at45_erase(const struct lfs_config *c, lfs_block_t block)
{
  int res = 0;
  
  res = at45_page_erase(block);
  
  if(res == AT45_ERR)
    return LFS_ERR_IO;
  
  
  return LFS_ERR_OK;                              
}                           

int block_at45_sync(const struct lfs_config *c)
{
  return LFS_ERR_OK;
}


uint8_t lfs_read_buf[256] = {0};       
uint8_t lfs_prog_buf[256] = {0};
uint8_t lfs_lookahead_buf[256] = {0};	
uint8_t lfs_file_buf[256] = {0};        //т.к. LFS_NO_MALLOC

const struct lfs_config cfg = {
 /* block device operations */
  .read  = block_at45_read,
  .prog  = block_at45_prog,
  .erase = block_at45_erase,
  .sync  = block_at45_sync,
  
  /* block device configuration */
  .read_size = 256,
  .prog_size = 256,
  .block_size = 256,
  .block_count = 32768,
  .cache_size = 256,
  .lookahead_size = 256,
  .block_cycles = 1000,
  .name_max = 50,
  
  /* static buffers */
  .read_buffer = lfs_read_buf,
  .prog_buffer = lfs_prog_buf,
  .lookahead_buffer = lfs_lookahead_buf
};

const struct lfs_file_config file_config = {
  .buffer = lfs_file_buf
};
