// -------------------------------------------------
//
//  MIT License
//
//  v_metalbackend.h
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/1/26.
//
// -------------------------------------------------

#ifndef V_METALBACKEND_H
#define V_METALBACKEND_H

#define MaxFramesInFlight 3
#define MaxLightsAllowed 64

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#import <QuartzCore/QuartzCore.h>

/// A minimal interface for a renderer type.
@protocol Renderer <NSObject>

- (nonnull instancetype) initWithMetalKitView:(nonnull MTKView *) view;

/// Informs the renderer when the size of the view changes.
/// - Parameter size: The new viewport size.
- (void) updateViewportSize:(CGSize) size;

/// Instructs the renderer to draw a frame for a view.
/// - Parameter view: A view the renderer draws to, which provides:
///     - A render pass descriptor that reflects the view's current configuration
///     - A drawable instance that the render draws to
- (void) renderFrameToView:(nonnull MTKView *) view;

@end

@interface MetalBackend : NSObject<Renderer>

// WARNING:
// Ignore the warning about pointer missing nullablity type
// Because it'll soon get a value... never said these autocorrect shit was smart
@property (nonatomic, readonly, nonnull) id<MTLDevice> v_device;

- (nonnull instancetype) initWithMetalKitView:(nonnull MTKView *)view;

- (void) updateViewportSize:(CGSize)size;

- (id<MTLRenderPipelineState>) CompileRenderPipeline:(MTLPixelFormat) ColorPixelFormat;

- (MTLRenderPipelineDescriptor*) ConfigureRenderPipeline:(MTLPixelFormat) ColorPixelFormat;

/// Creates a new buffer instances for the Scene shader's VertexData structure from the render device and returns them in a new array
/// - Parameter count: The number of buffers the method creates
- (nonnull NSArray<id<MTLBuffer>>*) MakeVertexDataBuffers:(NSUInteger) count;

/// Creates a new buffer instances for the Scene shader's SceneData structure from the render device and returns them
/// - Note: SceneData is a vertex structure not a pixel structure
- (nonnull id<MTLBuffer>) MakeSceneDataBuffers;

/// Reports if a resource is NOT valid by asserting with a message
/// - Parameter resource: The resource that will be used to assert if it's valid
/// - Parameter name: The name or description of the resource
/// - Parameter number: If the resource is part of a series it gives us the position in that series, otherwise just give negative number
/// - Parameter error: A pointer to an optional NSError if you're trying spice things up, otherwise just give `nil`
- (void) check:(nullable id) resource
          name:(nonnull NSString*) name
        number:(long) number
         error:(nullable NSError*) error;

@end

#endif
