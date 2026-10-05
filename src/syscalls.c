#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "uart_init.h"

extern char _end; // from linker script
extern char _estack; // top of RAM

void _exit(int status) {
    if(status){}
    while(1) { /* Hang forever */ }
}

caddr_t _sbrk(int incr) {
    static char *heap_end = NULL;
    char *prev_heap_end;

    if (heap_end == NULL) {
        heap_end = &_end;
    }

    prev_heap_end = heap_end;

    // Convert addresses to integers to avoid "array-bounds" warnings
    uintptr_t current_heap_limit = (uintptr_t)heap_end + incr;
    uintptr_t stack_limit = (uintptr_t)&_estack - 0x400;

    if (current_heap_limit > stack_limit) {
        errno = ENOMEM;
        return (caddr_t) -1;
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

int _write(int file, char *ptr, int len){

    if (file == 1 || file == 2){
        if(HAL_UART_Transmit(&huart1, (uint8_t*)ptr, len, 100) == HAL_OK){
            return len;
        }
    }
    return -1;
}

int _read(int file, char *ptr, int len){
    if (file || ptr || len){}
    return -1;

}

int _close(int file){
    if (file){}
    return -1;
}

int _lseek(int file, int ptr, int dir){
    if (file || ptr || dir){}
    return 0;
}

int _fstat(int file, struct stat *st){
    if (file){}
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file){
    if (file){}
    return 1;
}

int _getpid(void){
    return 1;
}

int _kill(int pid, int sig){
    if (pid || sig){}
    return -1;
}

