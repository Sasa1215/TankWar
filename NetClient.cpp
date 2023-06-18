#include "NetClient.h"
#include "Msg/TankNewMsg.h"
#include "Msg/TankMoveMsg.h"
#include "Msg/FireMsg.h"

//	打开Socket连接
void NetClient::connect(const char* ip, int port)
{
	m_serverIP = ip;
	// 初始化
	m_socket.Init();
	m_socket.Create(AF_INET, SOCK_STREAM, 0); // tcp

	bool result = m_socket.Connect(ip, port);

	if (result) {
		CCLOG("connect to server success!");
	
		// 接收网络连接基本信息，json格式：
		// {"serverUdpPort":9999 ,
		//        "udpPort":10001,
		//         "tankID":101  }
		char connectInfo[MAX_MSG_LEN] = "";
		int result = m_socket.Recv(connectInfo, MAX_MSG_LEN, 0);
		if (result <= 0)
		{
			CCLOG("server stop OR other error!"); // 未接收到服务器数据，程序不能往下执行
			return; 
		}
		CCLOG("recv connectInfo data : %s", connectInfo);
		rapidjson::Document d;
		d.Parse<0>(connectInfo);
		if (d.HasParseError())  
		{
			CCLOG("GetParseError %s",d.GetParseError());
		}

		if (d.IsObject() && d.HasMember("serverUdpPort")) {
			// 解析服务器端udp端口号
			m_serverUdpPort = d["serverUdpPort"].GetInt();
			CCLOG("serverUdpPort:%d", m_serverUdpPort);
		}
		if (d.IsObject() && d.HasMember("udpPort")) {
			// 解析服务器分配给客户端的udp端口号
			m_udpPort = d["udpPort"].GetInt();
			CCLOG("udpPort:%d", m_udpPort);
		}
		if (d.IsObject() && d.HasMember("tankID")) {
			// 解析服务器分配给客户端的坦克ID号
			m_tankID = d["tankID"].GetInt();
			CCLOG("tankID:%d", m_tankID);

			// 设置坦克ID
			m_parent->getTank()->setID(m_tankID); 
		}

		// 关闭TCP连接
		m_socket.Close(); 
	}
	else {
		CCLOG("can not connect to server!");
		return;
	}
	
	// 服务器做好分配后可以开启udp线程

	m_udpSocket.Init();
	m_udpSocket.Create(AF_INET, SOCK_DGRAM, 0); // udp
	m_udpSocket.Bind(m_udpPort);	            // 本地udp port
	m_udpSocket.Connect(ip, m_serverUdpPort);   // 指定服务器IP和udp端口
	
	auto tankNewMsg = new TankNewMsg(this->m_parent, this);  // 客户端连接发送基本消息
	send(tankNewMsg);

	std::thread recvUdpThread = std::thread(&NetClient::UDPrecThread, this);
	recvUdpThread.detach(); // 从主线程分离
}

bool NetClient::init(GameClient* parent)
{
	if (!Node::init())
	{
		return false;
	}
	m_parent = parent;
	m_parent->addChild(this);
	return true;
}

NetClient* NetClient::create(GameClient* parent)
{
	NetClient* pRet = new(std::nothrow) NetClient();
	if (pRet && pRet->init(parent))
	{
		pRet->autorelease();
		return pRet;
	} else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}

void NetClient::UDPrecThread()
{
	while (true) {

		// 接收数据包类型
		char msgType[10] = "";
		int result = m_udpSocket.Recv(msgType, 10, 0);
		CCLOG("udp code for msg type :%d", result);
		
		// 与服务器的连接断开了
		if (result <= 0) 
		{
			CCLOG("server stop!");
			break;
		}
		CCLOG("recv udp data msg type: %s", msgType);
		switch (atoi(msgType))
		{
		case 1: // Tank New Msg
			{
				// 接收其他坦克登录消息
				auto newTankMsg = new TankNewMsg(m_parent, this);
				newTankMsg->parse(m_udpSocket);
			}
			break;
		case 2: // Tank Move Msg
			{
				// 接收其他坦克移动消息
				auto tankMoveMsg = new TankMoveMsg(m_parent);
				tankMoveMsg->parse(m_udpSocket);
			}
			break;
		case 3: // Tank Fire Msg
			{
				// 接收其他坦克开火消息
				auto tankFireMsg = new FireMsg(m_parent);
				tankFireMsg->parse(m_udpSocket);
			}
		}
	}
}

void NetClient::send(Msg* msg)
{
	msg->send(m_udpSocket, m_serverIP.c_str(), m_serverUdpPort);
}
