#include "headers.h"

int newPlayerId(){
	int i;
	for(i=0;i<PLAYER_MAX;i++)
		if (config.player.ids[i]==0)
			return i;
	return -1;
}



void syncTime(struct timeval *t, unsigned int time){
	struct timeval end;
	gettimeofday(&end, NULL);
	int out=((end.tv_sec - *t.tv_sec)*1000+
			(end.tv_usec - *t.tv_usec)/1000.0);
	if (out<time)
		usleep((time-out)*1000);
	*t.tv_sec=end.tv_sec;
	*t.tv_usec=end.tv_usec;
}