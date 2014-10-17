#include "headers.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ functions for work with database					                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

//check player auth and create player_info struct or ret 0
player_info * dbAuth(worklist * client){
	player_info * pl;
	if ((client->id=newPlayerId())<0){
		//not have ids
		close(client->sock);
		return 0;
	}
	if ((pl=malloc(sizeof(player_info)))==0){
		perror("malloc player_info");
		close(client->sock);
		client->id=delPlayerId(client->id);
		return 0;
	}
	pl->bitmask=0;
	pl->status=PLAYER_CONNECTED;
	pl->conn=CONNECTED;
//	pl->id=client->id;

	return pl;
}