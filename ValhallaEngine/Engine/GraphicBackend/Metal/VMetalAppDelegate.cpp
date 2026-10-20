// -------------------------------------------------
//
//  MIT License
//
//  VMetalAppDelegate.cpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 10/15/26.
//
// -------------------------------------------------

#define NS_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION
#define MTK_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#include "headers/VMetalRHI.hpp"

using namespace ValhallaEngine::Metal;

VMacAppDelegate::~VMacAppDelegate()
{
    VViewport->release();
    VWindow->release();
    VDevice->release();
    delete VViewDelegate;
}

NS::Menu* VMacAppDelegate::CreateMenuBar()
{
    constexpr auto UTF8StringEncoding = NS::StringEncoding::UTF8StringEncoding;

    NS::Menu*       VMainMenu       = NS::Menu::alloc()->init();
    NS::MenuItem*   VAppMenuItem    = NS::MenuItem::alloc()->init();
    NS::Menu*       VAppMenu        = NS::Menu::alloc()->init(NS::String::string("ValhallaMetalRHI", UTF8StringEncoding));
    NS::String*     ApplicationName = NS::RunningApplication::currentApplication()->localizedName();
    NS::String*     QuitItemName    = NS::String::string("Quit ", UTF8StringEncoding)->stringByAppendingString(ApplicationName);

    SEL QuitCb = NS::MenuItem::registerActionCallback("appQuit", [](void*, SEL, const NS::Object* pSender)
    {
        auto TempApp = NS::Application::sharedApplication();
        TempApp->terminate(pSender);
    });

    NS::MenuItem* AppQuitItem = VAppMenu->addItem(QuitItemName, QuitCb, NS::String::string("q", UTF8StringEncoding));
    AppQuitItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    AppQuitItem->setSubmenu(VAppMenu);
    
    NS::MenuItem* WindowMenuItem = NS::MenuItem::alloc()->init();
    NS::Menu* WindowMenu = NS::Menu::alloc()->init(NS::String::string("Window", UTF8StringEncoding));

    SEL CloseWindowCb = NS::MenuItem::registerActionCallback("windowClose", [](void*, SEL, const NS::Object*){
        auto TempApp = NS::Application::sharedApplication();
        TempApp->windows()->object < NS::Window > (0)->close();
    });

    NS::MenuItem* CloseWindowItem = WindowMenu->addItem(NS::String::string("Close Window", UTF8StringEncoding), CloseWindowCb, NS::String::string("w", UTF8StringEncoding));
    CloseWindowItem->setKeyEquivalentModifierMask(NS::EventModifierFlagCommand);
    WindowMenuItem->setSubmenu(WindowMenu);

    VMainMenu->addItem(VAppMenuItem);
    VMainMenu->addItem(WindowMenuItem);

    VAppMenuItem->release();
    WindowMenuItem->release();
    VAppMenu->release();
    WindowMenu->release();

    // it's short for now because most of the things we need we don't have yet

    return VMainMenu->autorelease();
}

void VMacAppDelegate::applicationWillFinishLaunching(NS::Notification* pNotification)
{
    NS::Menu* VMenu = CreateMenuBar();

    NS::Application* VApplication = reinterpret_cast<NS::Application*>(pNotification->object());
    VApplication->setMainMenu(VMenu);
    VApplication->setActivationPolicy(NS::ActivationPolicy::ActivationPolicyRegular);
}

void VMacAppDelegate::applicationDidFinishLaunching(NS::Notification* pNotification)
{
    CGRect Frame = (CGRect){{100.0, 100.0}, {1980.0, 1080.0}};

    VWindow = NS::Window::alloc()->init(
        Frame, 
        NS::WindowStyleMaskClosable | NS::WindowStyleMaskTitled,
        NS::BackingStoreBuffered,
        false
    );

    VDevice = MTL::CreateSystemDefaultDevice();

    VViewport = MTK::View::alloc()->init(Frame, VDevice);
    VViewport->setColorPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm_sRGB);
    VViewport->setClearColor(MTL::ClearColor::Make(1.0, 1.0, 1.0, 1.0)); // black

    VViewDelegate = new VMetalViewDelegate(VDevice);
    VViewport->setDelegate(VViewDelegate);

    VWindow->setContentView(VViewport);
    VWindow->setTitle(NS::String::string("Valhalla Engine - 1.0.0", NS::StringEncoding::UTF8StringEncoding));
    VWindow->makeKeyAndOrderFront(nullptr);

    NS::Application* VApp = reinterpret_cast<NS::Application*>(pNotification->object());
    VApp->activateIgnoringOtherApps(true);
}

bool VMacAppDelegate::applicationShouldTerminateAfterLastWindowClosed(NS::Application* pSender)
{
    return true;
}