#include "GameClient.h"
#include "NetClient.h"
#include "Msg/TankMoveMsg.h"
#include "Msg/FireMsg.h"
#include "SimpleAudioEngine.h"

GameClient::GameClient()
{

}

GameClient::~GameClient()
{

}

bool GameClient::init()
{
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadBackgroundMusic("tankmusic.mp3");
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("tankmusic.mp3");

	if (!Scene::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// 背景
	createBackGround();

	// 玩家
	m_tank = Tank::create(110, WINDOWWIDTH/2, 100, 1, 2);
	m_tankList.pushBack(m_tank);

	// 碰撞检测
	this->scheduleUpdate();

	//创建replay按钮
	auto replayItem = MenuItemImage::create(
		"replay.png",
		"replay.png",
		CC_CALLBACK_1(GameClient::menuReplayCallback, this));
	replayItem->setScale(0.1);
	replayItem->setPosition(Vec2(origin.x + visibleSize.width - replayItem->getContentSize().width / 2 ,
	origin.y + replayItem->getContentSize().height / 5));
	// create menu, it's an autorelease object
	auto menu2 = Menu::create(replayItem, NULL);
	menu2->setPosition(Vec2::ZERO);
	this->addChild(menu2, 2);

	// 键盘事件
	auto key_listener = EventListenerKeyboard::create();
	key_listener->onKeyPressed = CC_CALLBACK_2(GameClient::onKeyPressed, this);
	key_listener->onKeyReleased = CC_CALLBACK_2(GameClient::onKeyReleased, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(key_listener, this);

	// 网络
	/*auto _netclient = NetClient::create(this);
	_netclient->connect("45.32.15.229", 8888); // 连接到服务器
	_netclient->setTag(NET_TAG);

	this->addChild(m_tank);
	m_drawList.pushBack(m_tank); // 联网后再加入，因为ID由服务器分配*/

	m_shouldFireList.clear(); 



	return true;
}

Scene* GameClient::createScene()
{
	auto scene = Scene::create();
	auto layer = GameClient::create();
	scene->addChild(layer);
	return scene;
}

void GameClient::update(float delta)
{
	// 收到传来的开火消息的坦克执行Fire
	if (m_shouldFireList.size() > 0)
	{
		auto tank = m_shouldFireList.at(0);
		tank->Fire();
		m_shouldFireList.clear();
	}

	// 维护坦克列表
	for (int i = 0;i < m_tankList.size(); i++)
	{
		auto nowTank = m_tankList.at(i);
		if (nowTank->getLife() <= 0)
		{
			m_tankList.eraseObject(nowTank);
		}
		bool notDraw = true;
		for (int j = 0;j < m_drawList.size(); j ++)
		{
			auto drawTank = m_drawList.at(j);
			if (drawTank->getID() == nowTank->getID() )
			{
				notDraw = false;
			}
		}

		// 绘制尚未绘制的坦克-针对后连进来的客户端
		if (notDraw)  
		{
			this->addChild(nowTank);
			m_drawList.pushBack(nowTank);
		}
	}

	// 坦克与 坦克，物品的碰撞检测
	for (int i = 0;i < m_tankList.size(); i++)
	{
		for (int j = 0; j < m_bgList.size(); j++)
		{
			auto nowTank = m_tankList.at(i);
			auto nowBrick = m_bgList.at(j);
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_UP))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_UP);
				nowTank->setPositionY(nowTank->getPositionY() - 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_UP);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_DOWN))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_DOWN); 
				nowTank->setPositionY(nowTank->getPositionY() + 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_DOWN);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_LEFT))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_LEFT); 
				nowTank->setPositionX(nowTank->getPositionX() + 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_LEFT);
			}
			if (nowTank->getLife() && (nowTank->getRect().intersectsRect(nowBrick->getRect())) && (nowTank->getDirection() == TANK_RIGHT))
			{
				// 方法1：履带持续转动
				nowTank->setHindered(TANK_RIGHT); 
				nowTank->setPositionX(nowTank->getPositionX() - 1); // 避免检测成功后坦克持续受，无法行动造成卡住

				// 方法2：履带停止转动
				// nowTank->Stay(TANK_RIGHT);
			}
		}
		// 坦克与坦克
		for (int j = 0; j < m_tankList.size(); j ++)
		{
			auto nowTank = m_tankList.at(i);
			auto anotherTank = m_tankList.at(j);
			if ((nowTank->getLife() && anotherTank->getLife()) && (anotherTank->getID() != nowTank->getID()) && (nowTank->getRect().intersectsRect(anotherTank->getRect())))
			{
				// 正在运动的坦克才作出如下动作
				if (nowTank->getDirection() == TANK_UP && nowTank->isMoving())
				{
					nowTank->Stay(TANK_UP);
				}
				if (nowTank->getDirection() == TANK_DOWN && nowTank->isMoving())
				{
					nowTank->Stay(TANK_DOWN);
				}
				if (nowTank->getDirection() == TANK_LEFT && nowTank->isMoving())
				{
					nowTank->Stay(TANK_LEFT);
				}
				if (nowTank->getDirection() == TANK_RIGHT && nowTank->isMoving())
				{
					nowTank->Stay(TANK_RIGHT);
				}
			}
		}

		// 坦克与子弹
		auto tank = m_tankList.at(i);
		for (int j = 0; j < tank->getBulletList().size(); j ++)
		{
			auto bullet = tank->getBulletList().at(j);
			for (int k = 0;k < m_tankList.size(); k ++)
			{
				auto tank_another = m_tankList.at(k);
				if (tank->getID() != tank_another->getID())
				{
					if (bullet->getRect().intersectsRect(tank_another->getRect()))
					{
						// 子弹消除
						m_deleteBulletList.pushBack(bullet);

						// 坦克消除
						m_deleteTankList.pushBack(tank_another);
					}
				}
			}
		}

		// 子弹和墙
		for (int j = 0; j < tank->getBulletList().size(); j ++)
		{
			auto bullet = tank->getBulletList().at(j);
			for (int k = 0; k < m_bgList.size(); k ++)
			{
				auto brick = m_bgList.at(k);
				if (bullet->getRect().intersectsRect(brick->getRect()))
				{
					// 子弹消除
					m_deleteBulletList.pushBack(bullet);

					// 砖块消除
					m_deleteBrickList.pushBack(brick);
				}
			}
		}

		// 清除删除子弹列表
		for (int j = 0; j < m_deleteBulletList.size(); j ++)
		{
			auto bullet = m_deleteBulletList.at(j);
			m_deleteBulletList.eraseObject(bullet);
			tank->getBulletList().eraseObject(bullet);
			bullet->Blast();
		}

		// 清除删除砖块列表
		for (int j = 0; j < m_deleteBrickList.size(); j ++)
		{
			auto brick = m_deleteBrickList.at(j);
			m_deleteBrickList.eraseObject(brick);
			m_bgList.eraseObject(brick);
			brick->Blast();
		}

		// 清除删除坦克列表
		for (int j = 0; j < m_deleteTankList.size(); j ++)
		{
			auto tank = m_deleteTankList.at(j);
			m_deleteTankList.eraseObject(tank);
			m_tankList.eraseObject(tank);
			tank->Blast();
		}
		m_deleteBulletList.clear();
		m_deleteBrickList.clear();
		m_deleteTankList.clear();
	}
}

// 绘制4个回字砖块
void GameClient::createBackGround()
{
	auto map = TMXTiledMap::create("Chapter12/tank/map.tmx");
	this->addChild(map, 10);

	drawBigBG(Vec2(16 * 16, 25 * 16));
	drawBigBG(Vec2(44 * 16, 25 * 16));
	drawBigBG(Vec2(16 * 16, 14 * 16));
	drawBigBG(Vec2(44 * 16, 14 * 16));
}

// 绘制单个回字砖块
void GameClient::drawBigBG(Vec2 position)
{
	for (int i = -2;i < 4;i ++)
	{
		for (int j = -2;j < 4;j ++)
		{
			if ((i == 1)&&(j == 0) || (i == 0)&&(j == 0) || (i == 1)&&(j == 1) || (i == 0)&&(j == 1))
			{
				// 中间留空形成回字
				continue;
			}
			auto brick = Brick::create(Vec2(position.x + (0.5 - i) * 16, position.y + (0.5 - j) * 16));
			m_bgList.pushBack(brick);
			this->addChild(brick, 2);
		}
	}
}


void GameClient::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	//auto _netclient = (NetClient*)getChildByTag(NET_TAG);
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		{
			m_tank->MoveLeft();
			auto msg = new TankMoveMsg(this, TANK_LEFT, false);
			//_netclient->send(msg);  // 发送坦克移动消息
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		// m_tank->MoveUP();
		{
			m_tank->MoveUP();
			auto msg = new TankMoveMsg(this, TANK_UP, false);
			//_netclient->send(msg);  // 发送坦克移动消息
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		// m_tank->MoveDown();
		{
			m_tank->MoveDown();
			auto msg = new TankMoveMsg(this, TANK_DOWN, false);
			//_netclient->send(msg);  // 发送坦克移动消息
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		// m_tank->MoveRight();
		{
			m_tank->MoveRight();
			auto msg = new TankMoveMsg(this, TANK_RIGHT, false);
			//_netclient->send(msg);  // 发送坦克移动消息
		}
		break;
	}
}

void GameClient::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	//auto _netclient = (NetClient*)getChildByTag(NET_TAG);
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		{
			m_tank->Stay(TANK_LEFT);
			auto msg = new TankMoveMsg(this, TANK_LEFT, true);
			//_netclient->send(msg);  // 发送坦克停下消息
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_W:
		{
			m_tank->Stay(TANK_UP);
			auto msg = new TankMoveMsg(this, TANK_UP, true);
			//_netclient->send(msg);  // 发送坦克停下消息
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_S:
		{
			m_tank->Stay(TANK_DOWN);
			auto msg = new TankMoveMsg(this, TANK_DOWN, true);
			//_netclient->send(msg);  // 发送坦克停下消息
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		{
			m_tank->Stay(TANK_RIGHT);
			auto msg = new TankMoveMsg(this, TANK_RIGHT, true);
			//_netclient->send(msg);  // 发送坦克停下消息
		}
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_K:
		{
			m_tank->Fire();
			auto msg = new FireMsg(this);
			//_netclient->send(msg);  // 发送开火消息
		}
		break;
	}
}

void GameClient::menuReplayCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif
	//score = 0;
	Director::getInstance()->replaceScene(GameClient::createScene());
	Director::getInstance()->resume();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}


//////////////////////////////////////////////////////////////////////////
// 处理网络传输数据时作出的响应
void GameClient::addTank(int id, float x, float y, int dir, int kind)
{
	m_maxTank[tankcount] = Tank::create(id, x, y, dir, kind);
	m_tankList.pushBack(m_maxTank[tankcount++]);
}

void GameClient::addFire(Tank* tank)
{
	m_shouldFireList.pushBack(tank);
}
