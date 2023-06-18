#include "Brick.h"

Brick::Brick()
{

}

Brick::~Brick()
{

}

bool Brick::init(Vec2 position)
{
	if (!Sprite::init())
	{
		return false;
	}

	m_sprite = Sprite::create("Chapter12/tank/tile.png",Rect(0, 0, 16, 16));
	
	this->addChild(m_sprite);
	m_sprite->setPosition(Vec2::ZERO);

	this->setPosition(position);
	m_rect = Rect(this->getPositionX() - 8, this->getPositionY() - 8, 16, 16);
	
	return true;
}

Brick* Brick::create(Vec2 position)
{
	Brick* pRet = new(std::nothrow) Brick();
	if (pRet && pRet->init(position))
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

void Brick::Blast()
{
	this->setVisible(false);   // ×©¿éÏûÊ§
	this->removeFromParent();  
}

