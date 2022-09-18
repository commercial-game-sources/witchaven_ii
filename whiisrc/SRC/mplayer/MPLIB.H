#ifndef mplib_h
#define mplib_h

// give up time slice
void Yield(void);
void MGenWakeupDll(void);

// post a message to win32 side
void PostWindowsMessage(void);

// get # of items on qNo
int MGenGetQueueCtr(int qNo);

// move first node from qFrom to qTo
RTQ_NODE *MGenMoveTo(int qFrom, int qTo);

// get first node from q
RTQ_NODE *MGenGetNode(int q);

// get master node, returning size of RTQ_NODE for size verification
RTQ_NODE *MGenGetMasterNode(unsigned *size);

// move all nodes from qFrom to qTo
RTQ_NODE *MGenFlushNodes(int qFrom, int qTo);

// count number of nodes in queues designated by bitmask
// lowerOrderBits == 0..31, upperOrderBits == 32-63
int MGenMCount(unsigned lowerOrderBits, unsigned upperOrderBits);

// perform consistency check on chunnel address space
int MGenSanityCheck(void);

#endif

