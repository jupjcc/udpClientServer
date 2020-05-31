#ifndef FILE_WRITER_H
#define FILE_WRITER_H
    #include <stdint.h>
    #define MAX_UDP_PKT_LEN 8972
    #define FILE_XFR_HDR_SIZE 16
    struct FileXfrHeader_t
    {
        uint32_t msgSize;       // #bytes in this udp packet
        uint32_t totalSize;     // #bytes in entire file
        uint32_t msgSeq;        // packet #
        uint32_t nMsgsTot;      // # packets in entire file
    };
    
    void InitFileWriter();
    void AddToFwBuf(char buf[]);

#endif