#ifndef CMD_LINE_OPTS
#define CMD_LINE_OPTS

    #include <inttypes.h>

    void ReadCmdLine(int argc, char *argv[]);
    void ShowCmdLineHelp();
    int GetSvrPort(void);
    uint32_t GetBufSize();

#endif
