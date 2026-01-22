// -------------------------------------------------
//
//  MIT License
//
//  v_metalbackend.m
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 1/1/26.
//
// -------------------------------------------------

// This is incomplete! So, don't expect it to work!

#import "headers/v_metalbackend.h"
#import "../../../Shaders/Metal/ValhallaShaderTypes.h"
#import "../../headers/v_platform.h"

using namespace ValhallaEngine::Platform;

@implementation MetalBackend
{
    id<MTLCommandQueue> VCommandQueue;
    
    //id<MTLCommandBuffer>    v_command_buffer;
    
    /// This synchronizes the barrier between the CPU and GPU so these two can communicate essentially
    id<MTLSharedEvent> VSharedEvent;
    
    /// This stores the current frame number so we can track it later
    uint64 VFrameNumber; // AKA frame number
    
    simd_uint2 VViewportSize;
    
    id<MTLBuffer> VViewportSizeBuffer;
    
    id<MTLBuffer> VSceneDataBuffer;
    
    NSArray<id<MTLBuffer>>* VVertexBuffers;
    
    id<MTLBuffer> VSceneLightingBuffer;
    
    id<MTLBuffer> VLightDataBuffer;
    
    id<MTLRenderPipelineState> VRenderPipelineState;
}

- (nonnull instancetype) initWithMetalKitView:(nonnull MTKView *)view
{
    self = [super init];
    if (nil == self)
    {
        ErrorMessageBox("Valhalla MetalKit Initialization", "Failed to initialize MetalKit View!");
        return nil;
    }
    
    _v_device = view.device;
    VCommandQueue = [self.v_device newCommandQueue];
    
    // Essential resources
    VRenderPipelineState    = [self CompileRenderPipeline:view.colorPixelFormat];
    VVertexBuffers          = [self MakeVertexDataBuffers:MaxFramesInFlight];
    VSceneDataBuffer = [self MakeSceneDataBuffers];
    VViewportSizeBuffer     = [self.v_device newBufferWithLength:sizeof(VViewportSize) options:MTLResourceStorageModeShared];
    
    [self updateViewportSize:view.drawableSize];
    VFrameNumber = 0;
    
    VSharedEvent = [self.v_device newSharedEvent];
    VSharedEvent.signaledValue = VFrameNumber;
    
    return self;
}

- (void) updateViewportSize:(CGSize)size
{
    VViewportSize.x = size.width;
    VViewportSize.y = size.height;
    
    // Then copy the updated ViewportSize into memory
    memcpy(VViewportSizeBuffer.contents, &VViewportSize, sizeof(VViewportSize));
}

- (id<MTLRenderPipelineState>) CompileRenderPipeline:(MTLPixelFormat) ColorPixelFormat
{
    MTLRenderPipelineDescriptor* RenderPipelineDesc;
    RenderPipelineDesc = [self ConfigureRenderPipeline:ColorPixelFormat];
    
    NSError* error = nil;
    id<MTLRenderPipelineState> RenderPipelineState;
    RenderPipelineState = [self.v_device newRenderPipelineStateWithDescriptor:RenderPipelineDesc error:&error];
    
    // Next we verify the GPU has successfully create the pipeline state
    NSAssert(nil != RenderPipelineState, @"The Valhalla Metal Device cannot compile a pipeline state: %@\n%@", error, @"Check the Valhalla Metal Descriptor's configuration and turn on Metal API validation for more information");
    
    ErrorMessageBox("Valhalla Metal Render Pipeline State", "The Valhalla Metal Device could not compile a pipeline state, check the descriptor's configuration and turn on Metal API validation for more information");
    
    return RenderPipelineState;
}

- (MTLRenderPipelineDescriptor*) ConfigureRenderPipeline:(MTLPixelFormat) ColorPixelFormat
{
    id<MTLLibrary> DefaultLibrary = [self.v_device newDefaultLibrary];
    id<MTLFunction> SceneVertex = [DefaultLibrary newFunctionWithName:@"ValhallaSceneVS"];
    id<MTLFunction> SceneFragment = [DefaultLibrary newFunctionWithName:@"ValhallaScenePS"];
    
    MTLRenderPipelineDescriptor* RenderPipelineDesc = [MTLRenderPipelineDescriptor new];
    RenderPipelineDesc.label = @"Valhalla Metal Render Pipeline";
    RenderPipelineDesc.vertexFunction = SceneVertex;
    RenderPipelineDesc.fragmentFunction = SceneFragment;
    RenderPipelineDesc.colorAttachments[0].pixelFormat = ColorPixelFormat;
    
    return RenderPipelineDesc;
}

/// Creates a new buffer instances for the Scene shader's VertexData structure from the render device and returns them in a new array
/// - Parameter count: The number of buffers the method creates
- (nonnull NSArray<id<MTLBuffer>>*) MakeVertexDataBuffers:(NSUInteger) count
{
    NSMutableArray<id<MTLBuffer>>* BufferArray;
    BufferArray = [[NSMutableArray alloc] initWithCapacity:count];
    for (uint BufferNumber = 0; BufferNumber < count; BufferNumber += 1)
    {
        id<MTLBuffer> VertexBuffer;
        VertexBuffer = [self.v_device newBufferWithLength:sizeof(VertexData) options:MTLResourceStorageModeShared];
        
        [self check:VertexBuffer name:@"VertexDataBuffer" number:BufferArray.count error:nil];
        [BufferArray addObject:VertexBuffer];
    }
    
    return BufferArray;
}

- (nonnull id<MTLBuffer>) MakeSceneDataBuffers;
{
    id<MTLBuffer> SceneDataBuffer;
    SceneDataBuffer = [self.v_device newBufferWithLength:sizeof(SceneData) options:MTLResourceStorageModeShared];
    
    [self check:SceneDataBuffer name:@"SceneDataBuffer" number:-1 error:nil];
    
    return SceneDataBuffer;
}

- (void) check:(nullable id) resource
          name:(nonnull NSString*) name
        number:(long) number
         error:(nullable NSError*) error
{
    if (nil != resource)
    {
        return;
    }
    
    NSMutableString* ErrorString;
    ErrorString = [NSMutableString stringWithString:@"Valhalla Metal Device cannot create"];
    [ErrorString appendFormat:@" %@", name];
    
    if (number >= 0)
    {
        [ErrorString appendFormat:@ "%ld", number];
    }
    
    if (error != nil)
    {
        [ErrorString appendFormat:@": %@\n", error];
    }
    else
    {
        [ErrorString appendFormat:@"."];
    }
    
    NSAssert(false, ErrorString);
}

- (void) WaitOnSharedEvent:(nonnull id<MTLSharedEvent>) SharedEvent
           ForEarlierFrame:(uint64_t) EarlierFrameNumber
{
    // Ten Milliseconds
    const uint64_t WaitTime = 10;
    
    BOOL BeforeMetalTimeout = [SharedEvent waitUntilSignaledValue:EarlierFrameNumber timeoutMS:WaitTime];
    
    if (false == BeforeMetalTimeout)
    {
        NSLog(@"No signal from frame %llu to shared event after %llums", EarlierFrameNumber, WaitTime);
    }
}

- (void) renderFrameToView:(nonnull MTKView*) view
{
    NSAssert(view.device == VCommandQueue.device, @"The Valhalla Metal View Devive is not the same as the render device.");
    
    MTLRenderPassDescriptor* RenderPassDesc = view.currentRenderPassDescriptor;
    
    if (nil == RenderPassDesc)
    {
        ErrorMessageBox("Valhalla MetalRHI Error", "RenderPassDescriptor was null. Ceasing function!");
        return;
    }
    
    VFrameNumber += 1;
    
    const uint32_t FrameIndex = VFrameNumber % MaxFramesInFlight;
    NSString* Label = [NSString stringWithFormat:@"Frame: %llu", VFrameNumber];
    
    if (VFrameNumber > MaxFramesInFlight)
    {
        [self WaitOnSharedEvent:VSharedEvent ForEarlierFrame:VFrameNumber - MaxFramesInFlight];
    }
    
    id<MTLCommandBuffer> CommandBuffer = [VCommandQueue commandBuffer];
    CommandBuffer.label = Label;
    
    id<MTLRenderCommandEncoder> Encoder;
    Encoder = [CommandBuffer renderCommandEncoderWithDescriptor:RenderPassDesc];
    
    [self SetViewportSize:VViewportSize ForRenderEncoder:Encoder];
    
    [Encoder setRenderPipelineState:VRenderPipelineState];
    
    /// Vertex buffers
    /// --------------------
    
    id<MTLBuffer> VertexBuffer = VVertexBuffers[FrameIndex];
    
    id<MTLBuffer> SceneDataBuffer = VSceneDataBuffer;
    
    /// Fragment buffers
    /// --------------------
    
    id<MTLBuffer> SceneLightingBuffer = VSceneLightingBuffer;
    
    id<MTLBuffer> LightDataBuffer = VLightDataBuffer;
    
    /// Setting vertex buffers
    
    [Encoder setVertexBuffer:VertexBuffer offset:0 atIndex:0];
    [Encoder setVertexBuffer:SceneDataBuffer offset:0 atIndex:1];
    
    /// Setting fragment buffers
    
    [Encoder setFragmentBuffer:SceneLightingBuffer offset:0 atIndex:0];
    [Encoder setFragmentBuffer:LightDataBuffer offset:0 atIndex:1];
    
    // This is a placeholder for later implementing 3D objects
    [Encoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
    
    [Encoder endEncoding];
    
    [CommandBuffer presentDrawable:view.currentDrawable];
    
    [CommandBuffer encodeSignalEvent:VSharedEvent value:VFrameNumber];
    
    [CommandBuffer commit];
}

- (void) SetViewportSize:(simd_uint2) size
        ForRenderEncoder:(nonnull id<MTLRenderCommandEncoder>) RenderPassEncoder
{
    // Get it? GL like OpenGL?! HAHAHAHAHAHA!
    // I'm going insane can't you tell?
    // ---------------------------------------
    // Okay on a serious note this is for the Viewport we're going resize
    MTLViewport ValhallaGLory;
    ValhallaGLory.originX   = 0.0;
    ValhallaGLory.originY   = 0.0;
    ValhallaGLory.znear     = 0.0;
    ValhallaGLory.zfar      = 1.0;
    ValhallaGLory.width     = (double)size.x;   // Converting to a double
    ValhallaGLory.height    = (double)size.y;   // Converting to a double
    
    [RenderPassEncoder setViewport:ValhallaGLory];
}

@end
