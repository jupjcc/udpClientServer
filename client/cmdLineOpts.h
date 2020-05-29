#ifndef CMD_LINE_OPTS
#define CMD_LINE_OPTS
    void ShowCmdLineHelp();
    void ReadCmdLine(int argc, char *argv[]);
    void GetSvrIp(char *iparg);
    int GetSvrPort(void);
    void GetFileName(char *fn);
    int GetSingleStepDbg(void);
#endif
