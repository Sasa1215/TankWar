#ifndef __TANK_MOVE_MSG_H__
#define __TANK_MOVE_MSG_H__

#include "Msg.h"
#include "../GameClient.h"

class TankMoveMsg : public Msg
{
public:
	TankMoveMsg(GameClient* client, int dir, bool stay);
	TankMoveMsg(GameClient* client);
	~TankMoveMsg();

	virtual void send(ODSocket socket, const char* IP, int serverUdpPort);
	virtual void parse(ODSocket socket);

private:
	GameClient* m_client;
	int         m_dir;  // 移动方向
	bool        m_stay; // 是否停下
};

#endif