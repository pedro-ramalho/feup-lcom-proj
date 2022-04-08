#include <lcom/lcf.h>
#include <stdint.h>
#include <stdbool.h>

#include "i8042.h"

<<<<<<< HEAD
int scancode_size;
int scancode_type;
=======
int      scancode_size = 1; // most interrupts are associated with 1 byte scancodes, ih will deal with this value changing;
int      scancode_type;
>>>>>>> 9b4100e03e62153eed134048cb3f2c4c4f8cabfa

int32_t  kbc_global_hook_id;
uint8_t  st_reg;
uint8_t  *scancodes = NULL; // scan codes have at most 3 bytes (?)

extern int32_t used_ids;

bool     ready = false, scancode_processed = false;
int      bytes_read = 0;

int(kbc_subscribe_int)(uint8_t *bit_no) {
  int hook_id; // random

  while (BIT(hook_id = (rand() % 32)) & used_ids)
    ;
  used_ids |= BIT(hook_id);

  *bit_no = (uint8_t) hook_id;

  int res = sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);

  kbc_global_hook_id = (uint8_t) hook_id;

  return res;
}

int(kbc_unsubscribe_int)() {
  return sys_irqrmpolicy( (int*) &kbc_global_hook_id );
}

void (kbc_ih)() {

  if (scancode_processed) {
    scancode_processed = false;
    scancode_size = 1; // reset scancode_size for next scancode, always at least 1
    free(scancodes);
    scancodes = NULL;
  }

  uint8_t scancode_byte;
  ready = false;

  util_sys_inb(KBC_OUT_BUF_STATUS, &st_reg); // query status register
  util_sys_inb(KBC_OUT_BUF_SCAN, &scancode_byte); // safe to read, KBC triggers interrupt on OUT_BUFF full
  bytes_read++;

  if (st_reg & KBC_STATUS_OK_MASK) { // if timeout/transmission error, abort current interrupt
    bytes_read = 0;
    scancode_size = 0;
    return;
  } 

  if (scancode_byte == KBC_SCAN_DOUBLE_BYTE || scancode_byte == KBC_SCAN_TRIPLE_BYTE) {// mark that we have to read more bytes
    scancode_size += (uint8_t) (scancode_byte & 0xF) + 1; // declare that we will need to read more bytes
  }

  scancodes = realloc(scancodes, sizeof(uint8_t)*bytes_read);

  if (!scancodes) { // in case realloc returns NULL -> error
    bytes_read = 0;
    scancode_size = 0;
    return;
  };

  scancodes[bytes_read - 1] = scancode_byte;

  if (bytes_read == scancode_size) {
    ready = true; // mark scancode fully read
    bytes_read = 0; // reset bytes read for next scancode
    scancode_type = (scancodes[scancode_size - 1] & BIT(7)) ? KBC_SCANCODE_BREAK : KBC_SCANCODE_MAKE;
  }
}

/* void (kbc_ih2)() {
  uint8_t scancode = 0;

  puts("pqp");

  scancode_size = 1;
  scancodes = (uint8_t*)malloc(sizeof(uint8_t)); // initialze array with size = 1

  util_sys_inb(KBC_OUT_BUF_STATUS, &st_reg);

  if (st_reg & KBC_STATUS_OK_MASK) 
    scancode_size = 0;
  
  if (st_reg & KBC_STATUS_OUTBUF_FULL) {
    util_sys_inb(KBC_OUT_BUF_SCAN, &scancode);<
    
    if (scancode_size == 0) 
      return;
    
    scancodes[0] = scancode;
    scancode_size = 1;
  }

  if (scancodes[0] == KBC_SCAN_DOUBLE_BYTE) {
    scancodes = (uint8_t*)realloc(scancodes, 2 * sizeof(uint8_t)); // reallocate memory

    util_sys_inb(KBC_OUT_BUF_SCAN, &scancode);
    scancodes[1] = scancode;
    
    scancode_size = 2;
  }

  if (scancodes[0] == KBC_SCAN_TRIPLE_BYTE) {
    scancodes = (uint8_t*)realloc(scancodes, 4 * sizeof(uint8_t)); // reallocate memory
    
    util_sys_inb(KBC_OUT_BUF_SCAN, &scancode);
    scancodes[1] = scancode;
    util_sys_inb(KBC_OUT_BUF_SCAN, &scancode);
    scancodes[2] = scancode;
    util_sys_inb(KBC_OUT_BUF_SCAN, &scancode);
    scancodes[3] = scancode;

    scancode_size = 4;
  }

  if (scancodes[scancode_size - 1] & KBC_BREAKCODE_BIT)
    scancode_type = KBC_SCANCODE_BREAK;
  else
    scancode_type = KBC_SCANCODE_MAKE;
} */

void (kbc_enable_int)() {

  uint8_t status_reg;
  
  do {
    status_reg = 0;
    util_sys_inb(KBC_OUT_BUF_STATUS, &status_reg);
  } while (status_reg & KBC_STATUS_INBUF_FULL);

  // input buffer no longer full, safe to write commands

  sys_outb(KBC_IN_BUF_CMD, KBC_READ_CMD_MASK);

  uint8_t command_byte;
  util_sys_inb(KBC_OUT_BUF_SCAN, &command_byte);

  command_byte |= KBC_ENABLE_INT_MASK;

  sys_outb(KBC_IN_BUF_CMD, KBC_WRITE_CMD_MASK);

  sys_outb(KBC_IN_BUF_DATA, command_byte);

}

int (kbc_outbuf_full)() {
  st_reg = 0;
  util_sys_inb(KBC_OUT_BUF_STATUS, &st_reg);

  return (st_reg & KBC_STATUS_OUTBUF_FULL);
}
