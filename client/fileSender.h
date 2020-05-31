#ifndef FILE_SENDER_H
#define FILE_SENDER_H
    #include <inttypes.h>

    #define FILE_XFR_HDR_SIZE 16
    struct FileXfrHeader_t
    {
        uint32_t msgSize;       // #bytes in this udp packet
        uint32_t totalSize;     // #bytes in entire file
        uint32_t msgSeq;        // packet #
        uint32_t nMsgsTot;      // # packets in entire file
    };
    void InitFileSender();
    int SendFile();
    void CloseUdp();

#endif