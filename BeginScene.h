#pragma once
#include "cocos2d.h" //包含头文件

using namespace cocos2d; //使用cocos2d命名空间

class BeginScene : public Layer //新建一个类，继承层（Layer）
{
public:
    static Scene *createScene(); // BeginScene场景创建实现
    virtual bool init(); // BeginScene层的初始化
    CREATE_FUNC(BeginScene); //创建BeginScene的Create方法
    void EnterSecondScene(Ref *pSender); //创建一个菜单回调函数，这里实现了场景的跳转
};