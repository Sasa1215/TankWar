#ifndef __NET_CLIENT_H__
#define __NET_CLIENT_H__

// json 解析
#include "json/rapidjson.h"
#include "json/document.h"

// json 组装
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace  rapidjson;

#include "cocos2d.h"
#include "GameClient.h"
#include "ODSocket/ODSocket.h"
#include "Msg/Msg.h"
USING_NS_CC;
using namespace cocos2d;

#define MAX_MSG_LEN 1024

class NetClient : public Node
{
public:
	NetClient() {};  
	~NetClient() {};

	static NetClient* create(GameClient* parent);
	virtual bool init(GameClient* parent);

	void connect(const char* ip, int port);
	void send(Msg* msg);

	void UDPrecThread();

private:
	GameClient*  m_parent;
	ODSocket     m_socket;        // 用于发送TCP数据
	ODSocket     m_udpSocket;     // 用于发送udp数据
	int          m_udpPort;       // 客户端udp端口号 : 由服务端动态分配
	int          m_serverUdpPort; // 保存服务端udp端口号
	int          m_tankID;        // 服务端动态分配坦克唯一的标识ID
	std::string  m_serverIP;      // 服务器IP
};

#endif