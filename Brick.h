#ifndef __BRICK_H__
#define __BRICK_H__

#include "cocos2d.h"
USING_NS_CC;
using namespace cocos2d;

class Brick : public Sprite
{
public:
	Brick();
	~Brick();
	static Brick* create(Vec2 position);
	virtual bool init(Vec2 position);
	void Blast();

	// get
	Rect getRect() { return m_rect; };

private:
	Sprite* m_sprite;  // ×©¿é¾«Áé
	Rect m_rect;       // °üÎ§¿ò
};

#endif