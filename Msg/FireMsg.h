#ifndef __FIRE_MSG_H__
#define __FIRE_MSG_H__

#include "Msg.h"
#include "../GameClient.h"

class FireMsg : public Msg
{
public:
	FireMsg(GameClient* m_client);
	~FireMsg();

	virtual void send(ODSocket socket, const char* IP, int serverUdpPort);
	virtual void parse(ODSocket socket);

private:
	GameClient* m_client;
};

#endif