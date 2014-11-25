/** @file io.c
 *
 * @brief Kernel I/O syscall implementations
 *
 * @author Mike Kasick <mkasick@andrew.cmu.edu>
 * @date   Sun, 14 Oct 2007 00:07:38 -0400
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date   2008-11-16
 */

#include <types.h>
#include <bits/errno.h>
#include <bits/fileno.h>
#include <arm/physmem.h>
#include <syscall.h>
#include <exports.h>
#include <kernel.h>
#include <arm/reg.h>
#include <arm/psr.h>
#include <arm/exception.h>

#define EOT_CHAR 0x04
#define DEL_CHAR 0x7f

//some constants for memory bound check
#define SDRAM_START	0xa0000000
#define SDRAM_END	0xa3ffffff
#define STRATAROM_START	0x00000000
#define STRATAROM_END	0X00ffffff

//extern unsigned lr_reg;

/* Read count bytes (or less) from fd into the buffer buf. */
ssize_t read_syscall(int fd __attribute__((unused)), void *buf __attribute__((unused)), size_t count __attribute__((unused)))
{
    enable_interrupts();

  //check corner cases
    if (fd != STDIN_FILENO) {
        return -EBADF;
    }
    //check memory location
    if ((unsigned)buf < SDRAM_START ||
            (unsigned)buf + (unsigned)count > SDRAM_END) {
        return -EFAULT;
    }

    //start regular routine
    unsigned read_cnt = 0;
    char *buffer = (char *)buf;
    char tempc;
    for (; read_cnt < count; ) {
        tempc = (char)getc();
        switch(tempc) {
            // EOT. Return immediately with the number of character
            // read into the buffer
        case 4:
            return read_cnt;
            // backspace or delete. Remove previous character
            // and print "\b \b" to stdout
        case 127:
        case 16:
            puts("\b \b");
            if (read_cnt > 0) {
                buffer--;
                read_cnt--;
            }
            break;
            // newline or carriage return. Put a newline in stdout
            // and buffer; return with the number
            // placed in buffer thus far
        case 10:
        case 13:
            *buffer = '\n';
            read_cnt++;
            putc('\n');
            return read_cnt;
            // default. Write in buffer
        default:
            putc(tempc);
            read_cnt++;
            *buffer = tempc;
            buffer++;
        }
    }
    return read_cnt;
}

/* Write count bytes to fd from the buffer buf. */
ssize_t write_syscall(int fd  __attribute__((unused)), const void *buf  __attribute__((unused)), size_t count  __attribute__((unused)))
{
    enable_interrupts();

    //printf("lr_regs: %u\n", lr_reg);
    if (fd != STDOUT_FILENO) {
        puts("wrong fileno");
        return -EBADF;
    }
    // check if the memory location is valid
    if (((unsigned)buf < SDRAM_START ||
            (unsigned)buf + (unsigned)count > SDRAM_END) &&
            ((unsigned)buf + (unsigned)count > STRATAROM_END)) {
        puts("efault");
        return -EFAULT;
    }
    // convert void* pointer to char* pointer
    char *buffer = (char *)buf;
    unsigned write_cnt = 0;
    for (; write_cnt < count;) {
        putc(*buffer);
        buffer++;
        write_cnt++;
    }
    return write_cnt;
	
}

