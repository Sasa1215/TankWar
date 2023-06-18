#ifndef __TANK_NEW_MSG_H__
#define __TANK_NEW_MSG_H__

#include "Msg.h"
#include "../GameClient.h"
#include "../NetClient.h"

class TankNewMsg : public Msg
{
public:
	TankNewMsg(GameClient* client, NetClient* netclient);
	~TankNewMsg();

	virtual void send(ODSocket socket, const char* IP, int serverUdpPort);
	virtual void parse(ODSocket socket);

private:
	GameClient* m_client;
	NetClient*  m_netclient;
};

#endif
