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
#import "../Shaders/Metal/ValhallaShaderTypes.h"

@implementation MetalBackend
{
    id<MTLDevice>        v_device;
    id<MTLCommandQueue>  v_command_queue;
    id<MTLCommandBuffer> v_command_buffer;
    
    
    uint32_t FrameNumber;
}

-(void)RenderFrameToView:(nonnull MTKView*)view
{
    uint32_t FrameIndex = FrameNumber % MaxFramesInFlight;
    FrameNumber++;  // Increment this value so we properally count the frames
    
    // Now from documentation and basic knowledge in Graphic APIs
    // We create a new command buffer for this current frame
    v_command_buffer = [v_command_queue commandBuffer];
    
    if ([view.device supportsFamily:MTLGPUFamilyMetal4])
    {
        
    }
    else if ([view.device supportsFamily:MTLGPUFamilyMetal3])
    {
        
    }
}

@end

@implementation Metal4Backend
{
    id<MTL4CommandQueue>    v_command_queue;
    
    id<MTL4CommandBuffer>   v_command_buffer;
    
    // This little gremlin needs to be a pointer
    // you can thank me later from saving you from
    // the psychological effects of war with him
    NSArray<id<MTL4CommandAllocator>>* v_command_alloc;
    
    id<MTL4ArgumentTable>   v_arg_table;
    
    id<MTLResidencySet>     v_resident_set;
    
    id<MTLSharedEvent>      v_shared_event;
    
    uint64_t FrameNumber;
    
    simd_uint2 ViewportSize;
    
    id<MTLBuffer> v_viewport_size_buffer;
    
    NSArray<id<MTLBuffer>>* v_vertices_buffers;
    
    id<MTLRenderPipelineState> v_render_pipeline_state;
    
    SceneData* WorldData;
    
    SceneLightingData* WorldLightingData;
    
    id<MTLBuffer> VertexBuffer;
    
    id<MTLBuffer> LightBuffer;
    
    id<MTLBuffer> SceneBuffer;
    
    id<MTLBuffer> SceneLightingBuffer;
}
-(nonnull instancetype) initWithMetalKitView:(MTKView *)view
{
    self = [super init];
    
    if (nil == self)
    {
        return nil;
    }
    
    _v_device = view.device;
    
    v_command_queue = [self.v_device newMTL4CommandQueue];
    
    v_command_buffer = [self.v_device newCommandBuffer];
    
    _v_lib = [self.v_device newDefaultLibrary];
    
    v_arg_table = [self makeArgumentTable];
    
    v_viewport_size_buffer = [self.v_device newBufferWithLength:sizeof(ViewportSize)options:MTLResourceStorageModeShared];
    
    // Compile the render pipeline state for the viewport
    v_render_pipeline_state = [self compileRenderPipeline:view.colorPixelFormat];
    
    // Set the first frame number to zero. Because we are initialzing the GPU so we haven't render anything
    // Trust me it makes complete fucking sense! once you realize it
    FrameNumber = 0;
    
    v_shared_event = [self.v_device newSharedEvent];
    v_shared_event.signaledValue = FrameNumber;
    
    [v_resident_set addAllocation:v_viewport_size_buffer];
    
    // Add the buffers that store the triangle vertex data to the residency set
    for (id<MTLBuffer> vertices_buffer in v_vertices_buffers)
    {
        [v_resident_set addAllocation:vertices_buffer];
    }
    
    [v_resident_set commit];
    
    [v_command_queue addResidencySet:v_resident_set];
    
    [v_command_queue addResidencySet:((CAMetalLayer*)view.layer).residencySet];
    
    // Allocate buffers so we don't have these performace drops
    // because of these fucking apes that haven't ascended to
    // planet of the apes levels of intelligence
    VertexBuffer = [self.v_device newBufferWithLength:sizeof(VertexData) options:MTLResourceStorageModeShared];
    
    SceneBuffer = [self.v_device newBufferWithLength:sizeof(SceneData) options:MTLResourceStorageModeShared];
    
    SceneLightingBuffer = [self.v_device newBufferWithLength:sizeof(SceneLightingData) options:MTLResourceStorageModeShared];
    
    LightBuffer = [self.v_device newBufferWithLength:sizeof(LightData) * MaxLightsAllowed options:MTLResourceStorageModeShared];
    
    
    [self updateViewportSize:view.drawableSize];
    
    return self;
}

- (void) waitOnSharedEvent:(nonnull id<MTLSharedEvent>) SharedEvent forEarlierFrame:(uint64_t) EarlierFrameNumber
{
    // This is for 10 milliseconds, for your information
    const uint64_t WaitTimeLimit = 10;
    
    BOOL BeforeTimeout = [v_shared_event waitUntilSignaledValue:EarlierFrameNumber timeoutMS:WaitTimeLimit];
    
    if (false == BeforeTimeout)
    {
        NSLog(@"No signal given from frame %llu to shared event after %llums", EarlierFrameNumber, WaitTimeLimit);
    }
}

- (void) renderFrameToView:(nonnull MTKView*) view
{
    if ([self isMissingRequirementsFromView:view])
    {
        return;
    }
    
    @autoreleasepool
    {
        const uint32_t FrameIndex = FrameNumber % MaxFramesInFlight;
        NSString* FrameLabel = [NSString stringWithFormat:@"GPU Frame: %llu", FrameNumber];
        MTL4RenderPassDescriptor* RenderPassDesc = view.currentMTL4RenderPassDescriptor;
        
        if (FrameNumber > MaxFramesInFlight)
        {
            [self waitOnSharedEvent:v_shared_event forEarlierFrame:MaxFramesInFlight];
        }
        
        id<MTL4CommandAllocator> FrameAllocator = v_command_alloc[FrameIndex];
        
        [FrameAllocator reset];
        
        [v_command_buffer beginCommandBufferWithAllocator:FrameAllocator];
        v_command_buffer.label = @"Valhalla Metal 4 Command Buffer";
        
        id<MTL4RenderCommandEncoder> RenderPassEncoder;
        RenderPassEncoder = [v_command_buffer renderCommandEncoderWithDescriptor:RenderPassDesc];
        
        RenderPassEncoder.label = @"Valhalla Metal 4 Command Encoder";
        
        [RenderPassEncoder setRenderPipelineState:v_render_pipeline_state];
        [self setViewportSize:ViewportSize forRenderEncoder:RenderPassEncoder];
        [self setRenderPassArguments:RenderPassEncoder forFrame:FrameNumber with:v_arg_table vertexBuffer:VertexBuffer viewportSize:v_viewport_size_buffer];
        
        // Implement draw 3D scene with models
        // also make sure we have models or render nothing
        // I'm just assuming
        
        
    }
}

- (BOOL) isMissingRequirementsFromView:(nonnull MTKView*) view
{
    BOOL DrawableMissing                = false;
    BOOL RenderPassDescriptorMissing    = false;
    
    if (nil == view.currentDrawable)
    {
        NSLog(@"The Metal 4 View does not have a current drawable instance!");
        DrawableMissing = true;
    }
    
    if (nil == view.currentMTL4RenderPassDescriptor)
    {
        NSLog(@"The Metal 4 View does not have a Render Pass Descriptor for the Valhalla Metal 4 Pipeline!");
        RenderPassDescriptorMissing = true;
    }
    
    return DrawableMissing || RenderPassDescriptorMissing;
}

- (nullable id<MTL4ArgumentTable>) makeArgumentTable
{
    NSError* error = nil;
    
    // We're going to configure the settings for a new argument table with two buffer bindings
    MTL4ArgumentTableDescriptor* ArgTableDesc;
    ArgTableDesc = [MTL4ArgumentTableDescriptor new];
    ArgTableDesc.maxBufferBindCount = 2;
    
    id<MTL4ArgumentTable> ArgTable;
    ArgTable = [self.v_device newArgumentTableWithDescriptor:ArgTableDesc error:&error];
    
    [self check:ArgTable name:@"argument table" number:-1 error:error];
    return ArgTable;
}

- (nullable id<MTLResidencySet>) makeResidencySet
{
    NSError* error = nil;
    
    // I am so freaking hilarious!!!!
    // This is the Residency Set Descriptor okay, don't get lost
    // in the mutants eyes
    MTLResidencySetDescriptor* ResidentEvilDesc;
    ResidentEvilDesc = [MTLResidencySetDescriptor new];
    
    id<MTLResidencySet> ResidencySet;
    ResidencySet = [self.v_device newResidencySetWithDescriptor:ResidentEvilDesc error:&error];
    
    [self check:ResidencySet name:@"residency set" number:-1 error:error];
    
    return ResidencySet;
}

- (nonnull NSArray<id<MTL4CommandAllocator>> *) makeCommandAllocators:(NSUInteger) count
{
    NSMutableArray<id<MTL4CommandAllocator>>* AllocArray;
    AllocArray = [[NSMutableArray alloc] initWithCapacity:count];
    
    for (uint i = 0; i < count; i+=1)
    {
        id<MTL4CommandAllocator> allocator;
        allocator = [self.v_device newCommandAllocator];
        [self check:allocator name:@"command allocator" number:AllocArray.count error:nil];
        
        [AllocArray addObject:allocator];
    }
    
    return AllocArray;
}

- (void) updateViewportSize:(CGSize)size
{
    ViewportSize.x = size.width;
    ViewportSize.y = size.height;
    
    // Now copy the new size to memory
    memcpy(v_viewport_size_buffer.contents, &ViewportSize, sizeof(ViewportSize));
}

- (void) setRenderPassArguments:(nonnull id<MTL4RenderCommandEncoder>) RenderPassEncoder
                    forFrame:(NSUInteger) FrameNumber
                        with:(nonnull id<MTL4ArgumentTable>) ArgumentTable
                vertexBuffer:(nonnull id<MTLBuffer>) VertexBuffer
                viewportSize:(nonnull id<MTLBuffer>) ViewportSizeBuffer
{
    // Configure buffer here
    
    [ArgumentTable setAddress:VertexBuffer.gpuAddress atIndex:SBI_VertexData];
    
    [ArgumentTable setAddress:ViewportSizeBuffer.gpuAddress atIndex:SBI_ViewportSize];
    
    [RenderPassEncoder setArgumentTable:ArgumentTable atStages:MTLRenderStageVertex];
}

- (nonnull id<MTLRenderPipelineState>) compileRenderPipeline:(MTLPixelFormat) ColorPixelFormat
{
    
    /// This is a Metal 4 compiler instance with a default compiler configuration
    id<MTL4Compiler> compiler = [self createDefaultMetalCompiler];
    
    /// A configuration used for the render pipeline and its method that will compile
    MTL4RenderPipelineDescriptor* RendererPipelineDesc;
    RendererPipelineDesc = [self configureRenderPipeline: ColorPixelFormat];
    
    MTL4CompilerTaskOptions* CompilerTaskOptions;
    CompilerTaskOptions = [self configureCompilerTaskOptions];
    
    NSError* error = nil;
    
    id<MTLRenderPipelineState> RenderPipelineState;
    RenderPipelineState = [compiler newRenderPipelineStateWithDescriptor:RendererPipelineDesc compilerTaskOptions:CompilerTaskOptions error:&error];
    
    NSAssert(nil != RenderPipelineState,
             @"The compiler cannot create a Metal 4 Pipeline state due to: %@\n%@", error,
             @"Check the Metal 4 Render Pipeline descriptor's configuration and or turn on Metal API validation for more information");
    
    return RenderPipelineState;
}

- (void) setViewportSize:(simd_uint2) size
        forRenderEncoder:(nonnull id<MTL4RenderCommandEncoder>) RenderPassEncoder
{
    MTLViewport Viewport;
    Viewport.originX    = 0.0;
    Viewport.originY    = 0.0;
    Viewport.znear      = 0.0;
    Viewport.zfar       = 1.0;
    Viewport.width      = (double)size.x;
    Viewport.height     = (double)size.y;
    
    [RenderPassEncoder setViewport:Viewport];
}

- (nullable MTL4CompilerTaskOptions*) configureCompilerTaskOptions
{
    NSURL* ArchiveURL = [[NSBundle mainBundle] URLForResource:@"archive" withExtension:@"metallib"];
    
    if (nil == ArchiveURL)
    {
        return nil;
    }
    
    NSError* error = nil;
    id<MTL4Archive> DefaultArchive = [self.v_device newArchiveWithURL:ArchiveURL error:&error];
    
    if (nil == DefaultArchive)
    {
        NSString* ErrorString;
        ErrorString = @"The Metal 4 GPU Device cannot create a new archive from a URL: ";
        ErrorString = [ErrorString stringByAppendingString: nil == error ? @"." : @"\n Error: %@\n"];
        
        NSLog(ErrorString, ArchiveURL, error);
        return nil;
    }
    
    MTL4CompilerTaskOptions* CompilerTaskOptions = [MTL4CompilerTaskOptions new];
    CompilerTaskOptions.lookupArchives = @[DefaultArchive];
    
    return CompilerTaskOptions;
}

- (nonnull MTL4LibraryFunctionDescriptor*) makeVertexShaderConfiguration:(nonnull NSString*) VertexShaderFuncName
{
    MTL4LibraryFunctionDescriptor* VertexFunction;
    VertexFunction = [MTL4LibraryFunctionDescriptor new];
    VertexFunction.library = self.v_lib;
    VertexFunction.name = VertexShaderFuncName;
    
    return VertexFunction;
}

- (nonnull MTL4LibraryFunctionDescriptor*) makeFragmentShaderConfiguration:(nonnull NSString*) FragmentShaderFuncName
{
    MTL4LibraryFunctionDescriptor* FragmentFunction;
    FragmentFunction = [MTL4LibraryFunctionDescriptor new];
    FragmentFunction.library = self.v_lib;
    FragmentFunction.name = FragmentShaderFuncName;
    
    return FragmentFunction;
}

- (nullable id<MTL4Compiler>) createDefaultMetalCompiler
{
    NSError* error = nil;
    
    id<MTL4Compiler> compiler;
    compiler = [self.v_device newCompilerWithDescriptor:[MTL4CompilerDescriptor new] error:&error];
    
    if (nil == compiler)
    {
        NSString* ErrorString;
        ErrorString = @"The Metal GPU Device cannot create a compiler";
        ErrorString = [ErrorString stringByAppendingString:nil == error ? @"." : @": %@\n"];
        
        NSAssert(false, ErrorString, error);
    }
    
    return compiler;
}

- (nonnull MTL4RenderPipelineDescriptor*) configureRenderPipeline:(MTLPixelFormat) ColorPixelFormat
{
    /// This creates the Metal 4 Render Pipeline Descriptor.
    /// Think of this as a "recipe" for the GPU should process shit like vertices, fragments, blending, and other shit.
    /// ------------------------------------------------------------------------------
    /// How many times have I've said shit? Who the hell keeps count?!
    MTL4RenderPipelineDescriptor* RendererPipelineDesc;
    RendererPipelineDesc = [MTL4RenderPipelineDescriptor new];
    RendererPipelineDesc.label = @"Valhalla Engine Metal 4 Renderer Pipeline"; // Every egotistic I know
    
    NSString* SceneShaderVSFuncName = @"ValhallaSceneVS";
    NSString* SceneShaderPSFuncName = @"ValhallaScenePS";
    
    /// Then setup the pixel format, for the vertex shader, and fragment (pixel) shader for this pipeline configuration
    /// -------------------------------------------------------------------------------
    RendererPipelineDesc.colorAttachments[0].pixelFormat    = ColorPixelFormat;
    RendererPipelineDesc.vertexFunctionDescriptor           = [self makeVertexShaderConfiguration:SceneShaderVSFuncName];
    RendererPipelineDesc.fragmentFunctionDescriptor         = [self makeFragmentShaderConfiguration:SceneShaderPSFuncName];
    
    return RendererPipelineDesc;
}

- (void) check:(nullable id) resource name:(nonnull NSString*) name number:(long) number error:(nullable NSError*) error
{
    if (nil != resource)
    {
        return;
    }
    
    NSMutableString* ErrorMSG;
    
    ErrorMSG = [NSMutableString stringWithString:@"Valhalla Metal 4 device cannot create"];
    [ErrorMSG appendFormat: @" %@", name];
    
    // If number is greater than or equal to
    if (number >= 0)
    {
        [ErrorMSG appendFormat:@ "%ld", number];
    }
    
    if (error != nil)
    {
        [ErrorMSG appendFormat:@": %@\n", error];
    }
    else
    {
        [ErrorMSG appendString:@"."];
    }
    
    NSAssert(false, ErrorMSG);
}

@end
