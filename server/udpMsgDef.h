#ifndef UDP_MSG_DEF_H
#define UDP_MSG_DEF_H
    #include <inttypes.h>

    #define MAX_UDP_PKT_LEN 8972
    #define FILE_XFR_HDR_SIZE 16
    
    struct FileXfrHeader_t
    {
        uint32_t pktSize;       // #bytes in this udp packet
        uint32_t pktSeq;        // packet # within msg
        uint32_t nPktsTot;      // # packets in entire file
        uint32_t nBytesMsg;     // #bytes in entire msg
    };

#endif