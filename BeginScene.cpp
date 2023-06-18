#include "BeginScene.h"
#include "GameClient.h"
#include"SimpleAudioEngine.h"
#include"ui/CocosGUI.h"
#include"cocostudio/CocoStudio.h"
using namespace ui;
using namespace cocostudio;
// BeginScene场景创建实现
Scene *BeginScene::createScene()
{
    auto scene = Scene::create();      //创建一个场景
    auto layer = BeginScene::create(); //创建一个BeginScene层
    scene->addChild(layer);            //把BeginScene层加入刚刚创建的场景中，
    return scene;                      //返回这个场景
}
bool BeginScene::init()
{
    if (!Layer::init()) //初始化父类的init方法
    {
        return false;
    }
    Size VisibleSize = Director::getInstance()->getVisibleSize(); //获得屏幕大小
    auto test_UI = GUIReader::getInstance()->widgetFromJsonFile("beginScene/beginScene.json");
    this->addChild(test_UI);
    auto imageView = (ImageView*)test_UI->getChildByTag(4);
    auto btn = (Button*)imageView->getChildByTag(6);
    btn->addClickEventListener(CC_CALLBACK_1(BeginScene::EnterSecondScene, this));
    return true;
}
//菜单回调函数的实现
void BeginScene::EnterSecondScene(Ref *pSender)
{
    //这里跳转场景调用到的是导演类的这个接口：replaceScene(Scene *scene),里面传进去的是就是一个场景，这里需要注意的是场景二的头文件这是要加上的
    Director::getInstance()->replaceScene(GameClient::createScene());
}