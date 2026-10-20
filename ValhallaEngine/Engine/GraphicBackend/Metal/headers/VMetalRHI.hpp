// -------------------------------------------------
//
//  MIT License
//
//  VMetalRHI.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 10/15/26.
//
// -------------------------------------------------

#pragma once 
#include <Metal/Metal.hpp>
#include <AppKit/AppKit.hpp>
#include <MetalKit/MetalKit.hpp>

namespace ValhallaEngine::Metal
{
    class VMetalRHI
    {
    public:
        VMetalRHI(MTL::Device* Dev);
        ~VMetalRHI();

        void Draw(MTK::View* Viewport);

    protected:
        MTL::Device* VDevice;
        MTL::CommandQueue* VCommandQueue;
    };

    class VMetalViewDelegate : public MTK::ViewDelegate
    {
    public:
        VMetalViewDelegate(MTL::Device* Dev);
        virtual ~VMetalViewDelegate() override;

        virtual void drawInMTKView(MTK::View* pView) override;

    protected:
        VMetalRHI* VRenderer;
    };

    class VMacAppDelegate : public NS::ApplicationDelegate 
    {
    public:
        ~VMacAppDelegate();

        NS::Menu* CreateMenuBar();

        virtual void applicationWillFinishLaunching( NS::Notification* pNotification ) override;
        virtual void applicationDidFinishLaunching( NS::Notification* pNotification ) override;
        virtual bool applicationShouldTerminateAfterLastWindowClosed( NS::Application* pSender ) override;

    protected:
        NS::Window*     VWindow;
        MTK::View*      VViewport;
        MTL::Device*    VDevice;

        VMetalViewDelegate* VViewDelegate = nullptr;
    };
}