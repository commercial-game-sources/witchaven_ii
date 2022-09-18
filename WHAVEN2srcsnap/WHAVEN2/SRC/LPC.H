#ifndef lpc_h
#define lpc_h

typedef struct {
   short  version;            // version of LPC requested
   short  sizeOfArgs;         // size of arguments
   short  service;            // service # requested
   char   Data[1];            // data
} LPCData;

typedef struct {
   short version;             // LPC version
   short sizeOfReturn;        // return data size
   short error;               // any error codes
   short noRet;               // number of returns
   char  Data[1];             // data
} LPCReturn;

#endif // lpc_h

