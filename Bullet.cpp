#include "Bullet.h"

Bullet::Bullet()
{

}

Bullet::~Bullet()
{

}

Bullet* Bullet::create(Vec2 position, float speed, int dir)
{
	Bullet* pRet = new(std::nothrow) Bullet();
	if (pRet && pRet->init(position, speed, dir))
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

bool Bullet::init(Vec2 position, float speed, int dir)
{
	if (!BaseObject::init())
	{
		return false;
	}
	m_texture = Director::getInstance()->getTextureCache()->addImage("Chapter12/tank/bullet.png");
	switch (dir)
	{
	case BULLET_UP:
		m_sprite = Sprite::createWithTexture(m_texture, Rect(  0, 0, 8, 8));
		break;
	case BULLET_DOWN:
		m_sprite = Sprite::createWithTexture(m_texture, Rect(8*2, 0, 8, 8));
		break;
	case BULLET_LEFT:
		m_sprite = Sprite::createWithTexture(m_texture, Rect(8*3, 0, 8, 8));
		break;
	case BULLET_RIGHT:
		m_sprite = Sprite::createWithTexture(m_texture, Rect(  8, 0, 8, 8));
		break;
	}
	m_sprite->setPosition(Vec2::ZERO);
	this->addChild(m_sprite);

	this->setLife(1);
	this->setPosition(position);
	this->setDirection(dir);
	this->setSpeed(speed);
	this->scheduleUpdate();

	m_rect = Rect(position.x - 4, position.y - 4, 8, 8);  // rect
	return true;
}

void Bullet::update(float delta)
{
	m_rect = Rect(getPositionX() - 4, getPositionY() - 4, 8, 8); // 更新rect
	if (this->getLife() <= 0)
	{
		this->unscheduleUpdate();
	}
	if (this->getDirection() == BULLET_UP && this->getLife())
	{
		if (this->getPositionY() <= WINDOWHEIGHT - 10)
		{
			this->setPositionY(this->getPositionY() + this->getSpeed());
		} else
		{
			Blast();
		}
	}
	if (this->getDirection() == BULLET_DOWN && this->getLife())
	{
		if (this->getPositionY() >= 10)
		{
			this->setPositionY(this->getPositionY() - this->getSpeed());
		} else
		{
			Blast();
		}
	}
	if (this->getDirection() == BULLET_LEFT && this->getLife())
	{
		if (this->getPositionX() >= 10)
		{
			this->setPositionX(this->getPositionX() - this->getSpeed());
		} else
		{
			Blast();
		}
	}
	if (this->getDirection() == BULLET_RIGHT && this->getLife())
	{
		if (this->getPositionX() <= WINDOWWIDTH - 10)
		{
			this->setPositionX(this->getPositionX() + this->getSpeed());
		} else
		{
			Blast();
		}
	}
}

void Bullet::Blast()
{
	this->setVisible(false);   // 子弹消失
	setLife(0);
	auto explode = Sprite::create("Chapter12/tank/explode1.png");
	this->getParent()->addChild(explode, 10);
	explode->setPosition(this->getPosition());  // 显示爆炸
	explode->runAction(Sequence::create(
		DelayTime::create(0.1f),
		FadeOut::create(0.1f),                   // 爆炸消失
		CallFunc::create(CC_CALLBACK_0(Bullet::deleteObj, this, explode)),
		NULL
		));
}

void Bullet::deleteObj(Sprite* obj)
{
	obj->removeFromParent();
	this->removeFromParent();
}

