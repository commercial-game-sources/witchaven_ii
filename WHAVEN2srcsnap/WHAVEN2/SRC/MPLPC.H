#ifndef mplpc_h
#define mplpc_h

typedef long HGAME;             // game Handle returned by Create/Find game 

typedef struct {
   HGAME hGame;            // handle of this game
   long myAddrMP;          // my MPath player address
   long myAcctMP;          // My Mpath account number
   long classID;           // Class ID number of game
   long numPlayers;        // Expected Number of players in game
}  TGAMEDEF;

void GetGameDef(void *, int *);


#endif // mplpc_h
