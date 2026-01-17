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

@interface MetalBackend : NSObject

@end

@interface Metal4Backend : NSObject<Renderer>

// WARNING:
// Ignore the warning about pointer missing nullablity type
// Because it'll soon get a value... never said these autocorrect shit was smart
@property (nonatomic, readonly, nonnull) id<MTLDevice> v_device;

// WARNING:
// Ignore the warning about pointer missing nullablity type
// Because it'll soon get a value... never said these autocorrect shit was smart
@property (nonatomic, readonly, nonnull) id<MTLLibrary> v_lib;

- (void) renderFrameToView:(nonnull MTKView*) view;

/// Checks whether or not the viewer has everything required to render a frame properly
/// - Parameter view: The viewer that is going to be use to verify whether or not we can properly render the current viewer
- (BOOL) isMissingRequirementsFromView:(nonnull MTKView*) view;

/// Creates a new argument table for the Metal 4 Renderer device that stores two arguments
- (nullable id<MTL4ArgumentTable>) makeArgumentTable;

/// Returns a new residency set from the Metal 4 Renderer device 
- (nullable id<MTLResidencySet>) makeResidencySet;

///  Creates a new command allocator instance for Metal 4 Renderer device and returns them in a new array
///  - Parameter count: The number of allocators the method creates
- (nonnull NSArray<id<MTL4CommandAllocator>> *) makeCommandAllocators:(NSUInteger) count;

- (void) waitOnSharedEvent:(nonnull id<MTLSharedEvent>) SharedEvent forEarlierFrame:(uint64_t) EarlierFrameNumber;

- (nonnull id<MTLRenderPipelineState>) compileRenderPipeline:(MTLPixelFormat) ColorPixelFormat;

/// Creates the default metal compiler for the RenderPipelineState
/// - Note: This is nullable because it will give out an error it finds a null value
- (nullable id<MTL4Compiler>) createDefaultMetalCompiler;

/// This function configures the Render Pipeline
- (nonnull MTL4RenderPipelineDescriptor*) configureRenderPipeline:(MTLPixelFormat) ColorPixelFormat;

- (nullable MTL4CompilerTaskOptions*) configureCompilerTaskOptions;

/// This make the vertex shader configuration for making vertex shader bindings for the render pipeline
/// - Parameter VertexShaderFuncName: The name of the Vertex Shader function so we don't have to manually input the names
- (nonnull MTL4LibraryFunctionDescriptor*) makeVertexShaderConfiguration:(nonnull NSString*) VertexShaderFuncName;

/// This makes the fragment shader configuration for making fragment shader bindings for the render pipeline
/// - Parameter FragmentShaderFuncName: The name of the Fragment Shader function so we don't have to manually input the names
- (nonnull MTL4LibraryFunctionDescriptor*) makeFragmentShaderConfiguration:(nonnull NSString*) FragmentShaderFuncName;

/// This updates the viewport size
/// - Parameter size: The new size of the viewport
- (void) updateViewportSize:(CGSize) size;

/// This passes all the render pass agurments to the Render Pass Encoder to excute
/// - Parameter RenderPassEncoder: This is the Metal 4 Render Pass Encoder used for recording rendering commands for the current render pass. This will record the state and resource bindings but does not execute on the GPU
///
/// - Parameter FrameNumber: Index of the current frame
///
/// - Parameter ArgumentTable: This table contains all GPU resources (A.K.A Textures, Buffers, Samplers) that the shaders will need. Note Metal 4 uses argument tables instead of manually binding each resource one by one
///
/// - Parameter VertexBuffer: The vertex buffer containing geometry data for the current draw calls
///
/// - Parameter ViewportSizeBuffer: A constant buffer containing the viewport size, that will be used by shaders for coordinate normalization and screen-space calculations
- (void) setRenderPassArguments:(nonnull id<MTL4RenderCommandEncoder>) RenderPassEncoder
                    forFrame:(NSUInteger) FrameNumber
                        with:(nonnull id<MTL4ArgumentTable>) ArgumentTable
                vertexBuffer:(nonnull id<MTLBuffer>) VertexBuffer
                viewportSize:(nonnull id<MTLBuffer>) ViewportSizeBuffer;

- (void) setViewportSize:(simd_uint2) size
        forRenderEncoder:(nonnull id<MTL4RenderCommandEncoder>) RenderPassEncoder;

/// Reports when a resource is not valid via asserting with a message
///
/// - Parameter resource: The pointer to a resource
/// - Parameter name: The name or description given for said `resource`
/// - Parameter number: Only given a non-negative number if a `resource` is part of  a series; otherwise negative
/// - Parameter error: The pointer to an optional error instance, if not given then it is `nil`
- (void) check:(nullable id) resource name:(nonnull NSString*) name number:(long) number error:(nullable NSError*) error;

@end

#endif
