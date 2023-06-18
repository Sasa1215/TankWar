#ifndef __BULLET_H__
#define __BULLET_H__

#include "BaseObject.h"
#include "Global.h"

#define BULLET_UP      1	  // BULLET向上状态
#define BULLET_DOWN    2      // BULLET向下状态
#define BULLET_LEFT    3      // BULLET向左状态
#define BULLET_RIGHT   4      // BULLET向右状态

class Bullet : public BaseObject
{
public:
	Bullet();
	~Bullet();
	static Bullet* create(Vec2 position, float speed, int dir);

	// 爆炸时已自动设置life为0
	void Blast();    

	Rect getRect() { return m_rect; };

private:
	virtual bool init(Vec2 position, float speed, int dir);
	void update(float delta);
	void deleteObj(Sprite* obj);

private:
	Sprite*     m_sprite;       // 图片精灵
	Texture2D*  m_texture;      // 文理
	Rect        m_rect;			// 碰撞矩形
};

#endif
