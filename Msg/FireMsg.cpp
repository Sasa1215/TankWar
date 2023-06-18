#include "FireMsg.h"

FireMsg::FireMsg(GameClient* client)
{
	this->m_client = client;
}

FireMsg::~FireMsg()
{

}

void FireMsg::send(ODSocket socket, const char* IP, int serverUdpPort)
{
	socket.Send("3", strlen("3"), 0); // 先发送数据包类型

	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value array(rapidjson::kArrayType);
	rapidjson::Value object(rapidjson::kObjectType);
	object.AddMember("tankID", m_client->getTank()->getID(), allocator);
	object.AddMember("tank_X", m_client->getTank()->getPositionX(), allocator);
	object.AddMember("tank_Y", m_client->getTank()->getPositionY(), allocator);
	array.PushBack(object, allocator);

	document.AddMember("Version", "v1.0", allocator);
	document.AddMember("Data", array, allocator);

	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	document.Accept(writer);

	CCLOG("sending msg:%s",buffer.GetString());

	std::string msg = buffer.GetString();  
	socket.Send(msg.c_str(), strlen(msg.c_str()), 0);  // 发送数据包
}

void FireMsg::parse(ODSocket socket)
{
	// 接收数据 Recv
	char data[1024] = "";
	int result = socket.Recv(data, 1024, 0);

	CCLOG("UDP rec Code : %d", result);

	// 与服务器的连接断开了
	if (result <= 0) 
	{
		CCLOG("Disconnected from the server!");
		return ;
	}
	CCLOG("Recv udp data : %s", data);

	rapidjson::Document d;
	d.Parse<0>(data);
	if (d.HasParseError())  
	{
		CCLOG("GetParseError %s",d.GetParseError());
	}

	if (d.IsObject() && d.HasMember("Version")) {
		CCLOG("Version:%s", d["Version"].GetString());
	}
	if (d.IsObject() && d.HasMember("Data"))
	{
		CCLOG("Data:");
		int tank_id = 0;
		float tank_x = 0.0;
		float tank_y = 0.0;

		rapidjson::Value& _array = d["Data"];
		for(int i=0; i<_array.Capacity(); i++)
		{
			rapidjson::Value& arraydoc = _array[i];
			if(arraydoc.HasMember("tankID"))
			{
				tank_id = arraydoc["tankID"].GetInt();
				CCLOG("tankID:%d", tank_id);
				if (m_client->getTank()->getID() == tank_id)
				{
					CCLOG("rec self bullet data , return!");   // 收到自己转发的数据，不做处理
					return;
				}
			}
			if (arraydoc.HasMember("tank_X"))
			{
				tank_x = arraydoc["tank_X"].GetDouble();
				CCLOG("tank_X:%lf", tank_x);
			}
			if (arraydoc.HasMember("tank_Y"))
			{
				tank_y = arraydoc["tank_Y"].GetDouble();
				CCLOG("tank_Y:%lf", tank_y);
			}
		}
		// 寻找此ID的坦克并做相应处理
		bool exist = false;
		for (int i=0;i < m_client->getTankList().size();i++)
		{
			auto tank = m_client->getTankList().at(i);
			if (tank->getID() == tank_id)
			{
				tank->setPosition(Vec2(tank_x, tank_y));
				m_client->addFire(tank); // 转交给主线程处理开火消息
				exist = true;
				break;
			}
		}
	}
}

