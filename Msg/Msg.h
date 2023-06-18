#ifndef __MSG_H__
#define __MSG_H__

// json ½âÎö
#include "json/rapidjson.h"
#include "json/document.h"

// json ×é×°
#include "json/writer.h"
#include "json/stringbuffer.h"
using namespace  rapidjson;

#include "cocos2d.h"
#include "../ODSocket/ODSocket.h"
USING_NS_CC;
using namespace cocos2d;

class Msg
{
public:
	Msg() {};
	virtual ~Msg() {};

	virtual void send(ODSocket socket, const char* IP, int serverUdpPort);
	virtual void parse(ODSocket socket);

private:

};

#endif