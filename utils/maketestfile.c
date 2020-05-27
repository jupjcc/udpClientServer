#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

int main(int argc, char *argv[])
{
    // create binary file containing uint32 value of 0 to 1 megabyte
    FILE *ptr;
    ptr = fopen("/home/jupjcc/udpcode/data/1mbuint32.bin", "wb");
    printf("ptr = 0x%08x\n", (int)ptr);
    uint32_t *buf;

    buf = (uint32_t*)malloc(1024*1024);

    int i;
    for (i=0; i<1024*1024/4; i++)
    {
        buf[i] = i;
    }
    fwrite(buf, 1024*1024, 1, ptr);
    fclose(ptr);
}
