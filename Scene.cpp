//--------------------------------------------------------------------------------------
// Scene geometry and layout preparation
// Scene rendering & update
//--------------------------------------------------------------------------------------

#include "Scene.h"
#include "Mesh.h"
#include "Model.h"
#include "Camera.h"
#include "State.h"
#include "Shader.h"
#include "Input.h"
#include "Common.h"

#include "CVector2.h" 
#include "CVector3.h" 
#include "CMatrix4x4.h"
#include "MathHelpers.h"     // Helper functions for maths
#include "GraphicsHelpers.h" // Helper functions to unclutter the code here

#include "ColourRGBA.h" 

#include <sstream>
#include <memory>

#include "Texture.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Light.h"

//--------------------------------------------------------------------------------------
// Scene Data
//--------------------------------------------------------------------------------------
// Addition of Mesh, Model and Camera classes have greatly simplified this section
// Geometry data has gone to Mesh class. Positions, rotations, matrices have gone to Model and Camera classes

// Constants controlling speed of movement/rotation (measured in units per second because we're using frame time)
const float ROTATION_SPEED = 2.0f;  // 2 radians per second for rotation
const float MOVEMENT_SPEED = 50.0f; // 50 units per second for movement (what a unit of length is depends on 3D model - i.e. an artist decision usually)
const float MAX_LIGHT_STRENGTH = 40.0f;
const float LIGHT_COLOUR_CHANGE = 0.3f;

// Meshes, models and cameras, same meaning as TL-Engine. Meshes prepared in InitGeometry function, Models & camera in InitScene
Mesh* gTeapotMesh;
Mesh* gSphereMesh;
Mesh* gCubeMesh;
Mesh* gGroundMesh;
Mesh* gLightMesh;
Mesh* gCubeTangentMesh;

Model* gTeapot;
Model* gSphere;
Model* gCube;
Model* gGround;
Model* gCube2;

Camera* gCamera;


// Store lights in an array in this exercise
const int NUM_LIGHTS = 2;
Light* gLights[NUM_LIGHTS]; 


// Additional light information
CVector3 gAmbientColour = { 0.2f, 0.2f, 0.3f }; // Background level of light (slightly bluish to match the far background, which is dark blue)
float    gSpecularPower = 256; // Specular power controls shininess - same for all models in this app

ColourRGBA gBackgroundColor = { 0.2f, 0.2f, 0.3f, 1.0f };

// Variables controlling light1's orbiting of the cube
const float gLightOrbit = 20.0f;
const float gLightOrbitSpeed = 0.7f;

// Lock FPS to monitor refresh rate, which will typically set it to 60fps. Press 'p' to toggle to full fps
bool lockFPS = true;


//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
// Variables sent over to the GPU each frame
// The structures are now in Common.h
// IMPORTANT: Any new data you add in C++ code (CPU-side) is not automatically available to the GPU
//            Anything the shaders need (per-frame or per-model) needs to be sent via a constant buffer

PerFrameConstants gPerFrameConstants;      // The constants that need to be sent to the GPU each frame (see common.h for structure)
ID3D11Buffer*     gPerFrameConstantBuffer; // The GPU buffer that will recieve the constants above

PerModelConstants gPerModelConstants;      // As above, but constant that change per-model (e.g. world matrix)
ID3D11Buffer*     gPerModelConstantBuffer; // --"--



//--------------------------------------------------------------------------------------
// Textures
//--------------------------------------------------------------------------------------

// DirectX objects controlling textures used in this lab
const int NUM_TEXTURES = 9;
Texture* gTextures[NUM_TEXTURES];


//--------------------------------------------------------------------------------------
// Initialise scene geometry, constant buffers and states
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
    // Load mesh geometry data, just like TL-Engine this doesn't create anything in the scene. Create a Model for that.
    // IMPORTANT NOTE: Will only keep the first object from the mesh - multipart objects will have parts missing - see later lab for more robust loader
    try 
    {
		gTeapotMesh	  = new Mesh("Teapot.x");
		gSphereMesh	  = new Mesh("Sphere.x");
		gCubeMesh	  = new Mesh("Cube.x");
        gGroundMesh   = new Mesh("Hills.x", true);
        gLightMesh    = new Mesh("Light.x");
		gCubeTangentMesh = new Mesh("Cube.x", true);
    }
    catch (std::runtime_error e)  // Constructors cannot return error messages so use exceptions to catch mesh errors (fairly standard approach this)
    {
        gLastError = e.what(); // This picks up the error message put in the exception (see Mesh.cpp)
        return false;
    }


    // Load the shaders required for the geometry we will use (see Shader.cpp / .h)
    if (!LoadShaders())
    {
        gLastError = "Error loading shaders";
        return false;
    }


    // Create GPU-side constant buffers to receive the gPerFrameConstants and gPerModelConstants structures above
    // These allow us to pass data from CPU to shaders such as lighting information or matrices
    // See the comments above where these variable are declared and also the UpdateScene function
    gPerFrameConstantBuffer = CreateConstantBuffer(sizeof(gPerFrameConstants));
    gPerModelConstantBuffer = CreateConstantBuffer(sizeof(gPerModelConstants));
    if (gPerFrameConstantBuffer == nullptr || gPerModelConstantBuffer == nullptr)
    {
        gLastError = "Error creating constant buffers";
        return false;
    }

	gTextures[0] = new Texture("MetalDiffuseSpecular.dds");
	gTextures[1] = new Texture("tiles1.jpg");
	gTextures[2] = new Texture("brick1.jpg");
	gTextures[3] = new Texture("wood2.jpg");
	gTextures[4] = new Texture("CobbleDiffuseSpecular.dds");
	gTextures[5] = new Texture("Flare.jpg");
	gTextures[6] = new Texture("PatternDiffuseSpecular.dds");
	gTextures[7] = new Texture("PatternNormal.dds");
	gTextures[8] = new Texture("CobbleNormalHeight.dds");
	

    //// Load / prepare textures on the GPU ////

    // Load textures and create DirectX objects for them
    // The LoadTexture function requires you to pass a ID3D11Resource* (e.g. &gCubeDiffuseMap), which manages the GPU memory for the
    // texture and also a ID3D11ShaderResourceView* (e.g. &gCubeDiffuseMapSRV), which allows us to use the texture in shaders
    // The function will fill in these pointers with usable data. The variables used here are globals found near the top of the file.
	for (auto texture : gTextures)
	{
		if (!texture->Load())
		{
			return false;
		}
	}
   


  	// Create all filtering modes, blending modes etc. used by the app (see State.cpp/.h)
	if (!CreateStates())
	{
		gLastError = "Error creating states";
		return false;
	}

	return true;
}


// Prepare the scene
// Returns true on success
bool InitScene()
{
    //// Set up scene ////

	gTeapot = new Model(gTeapotMesh);
	gSphere = new Model(gSphereMesh);
	gCube   = new Model(gCubeMesh);
    gGround = new Model(gGroundMesh);
	gCube2  = new Model(gCubeTangentMesh);


	// Initial positions
	gTeapot->SetPosition({ 20.0f, 0.0f, 0.0f });
	gTeapot->SetScale(1.5f);

	gSphere->SetPosition({ 0.0f, 20.0f, 50.0f });

	gCube->SetPosition({ 50.0f, 30.0f, 10.0f });

	gCube2->SetPosition({ 60.0f, 10.0f,-20.0f });
	gCube2->SetScale(1.5f);

    // Light set-up - using an array this time
	
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
		gLights[i] = new Light();
		gLights[i]->model = new Model(gLightMesh);
    }

    gLights[0]->colour = { 0.8f, 0.8f, 1.0f };
    gLights[0]->strength = MAX_LIGHT_STRENGTH;
    gLights[0]->model->SetPosition({ 30, 20, 0 });

    gLights[1]->colour = { 1.0f, 0.8f, 0.2f };
    gLights[1]->strength = MAX_LIGHT_STRENGTH;
    gLights[1]->model->SetPosition({ -20, 50, 20 });
    gLights[1]->model->SetScale(pow(gLights[1]->strength, 0.7f)); // Convert light strength into a nice value for the scale of the light - equation is ad-hoc.

    //// Set up camera ////

    gCamera = new Camera();
    gCamera->SetPosition({ 15, 30,-70 });
    gCamera->SetRotation({ ToRadians(13), 0, 0 });

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    ReleaseStates();

	for (auto texture : gTextures)
	{
		if (texture)
		{
			texture->diffuseSpecularMap->Release();
			texture->diffuseSpecularMapSRV->Release();
		}
		delete texture;    texture = nullptr;
	}

    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();

    ReleaseShaders();

    // See note in InitGeometry about why we're not using unique_ptr and having to manually delete
	
   /* for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        delete gLights[i]->model;  gLights[i]->model = nullptr;
    }*/

	for (auto light : gLights)
	{
		delete light;	light = nullptr;
	}
	
    delete gCamera;    gCamera    = nullptr;
    delete gGround;    gGround    = nullptr;
	delete gTeapot;	   gTeapot    = nullptr;
	delete gSphere;	   gSphere    = nullptr;
	delete gCube;	   gCube	  = nullptr;
	delete gCube2;	   gCube2	  = nullptr;

    delete gLightMesh;     gLightMesh     = nullptr;
    delete gGroundMesh;    gGroundMesh    = nullptr;
	delete gTeapotMesh;    gTeapotMesh    = nullptr;
	delete gSphereMesh;	   gSphere		  = nullptr;
	delete gCubeMesh;	   gCube		  = nullptr;
	delete gCubeTangentMesh;	   gCube2		  = nullptr;
}



//--------------------------------------------------------------------------------------
// Scene Rendering
//--------------------------------------------------------------------------------------

// Render everything in the scene from the given camera
void RenderSceneFromCamera(Camera* camera)
{
    // Set camera matrices in the constant buffer and send over to GPU
    gPerFrameConstants.viewMatrix           = camera->ViewMatrix();
    gPerFrameConstants.projectionMatrix     = camera->ProjectionMatrix();
    gPerFrameConstants.viewProjectionMatrix = camera->ViewProjectionMatrix();
    UpdateConstantBuffer(gPerFrameConstantBuffer, gPerFrameConstants);

    // Indicate that the constant buffer we just updated is for use in the vertex shader (VS) and pixel shader (PS)
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer); // First parameter must match constant buffer number in the shader 
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);


    //// Render lit models ////

    // Select which shaders to use next
    gD3DContext->VSSetShader(gPixelLightingVertexShader, nullptr, 0);
    gD3DContext->PSSetShader(gPixelLightingPixelShader,  nullptr, 0);
    
    // States - no blending, normal depth buffer and culling
    gD3DContext->OMSetBlendState(gNoBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gUseDepthBufferState, 0);
    gD3DContext->RSSetState(gCullBackState);

    // Select the approriate textures and sampler to use in the pixel shader
	gD3DContext->PSSetShaderResources(0, 1, &gTextures[0]->diffuseSpecularMapSRV);
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // Render model - it will update the model's world matrix and send it to the GPU in a constant buffer, then it will call
    // the Mesh render function, which will set up vertex & index buffer before finally calling Draw on the GPU

	gTeapot->Render();
    

    // Render other lit models, only change textures for each one
	gD3DContext->VSSetShader(gNormalMappingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gParallaxMappingPixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &gTextures[4]->diffuseSpecularMapSRV); // First parameter must match texture slot number in the shader
	gD3DContext->PSSetShaderResources(1, 1, &gTextures[8]->diffuseSpecularMapSRV);
	gGround->Render();

	gD3DContext->PSSetShader(gFadeTexturePixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &gTextures[2]->diffuseSpecularMapSRV);
	gD3DContext->PSSetShaderResources(1, 1, &gTextures[3]->diffuseSpecularMapSRV);
	gCube->Render();

	// Select which shaders to use next
	gD3DContext->VSSetShader(gWiggleVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gSpherePixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &gTextures[1]->diffuseSpecularMapSRV);
	gSphere->Render();

	// Select which shaders to use next
	gD3DContext->VSSetShader(gNormalMappingVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gNormalMappingPixelShader, nullptr, 0);
	gD3DContext->PSSetShaderResources(0, 1, &gTextures[6]->diffuseSpecularMapSRV); // First parameter must match texture slot number in the shaer
	gD3DContext->PSSetShaderResources(1, 1, &gTextures[7]->diffuseSpecularMapSRV);
	gCube2->Render();

    //// Render lights ////

    // Select which shaders to use next
	gD3DContext->VSSetShader(gBasicTransformVertexShader, nullptr, 0);
	gD3DContext->PSSetShader(gLightModelPixelShader, nullptr, 0);

    // Select the texture and sampler to use in the pixel shader
    gD3DContext->PSSetShaderResources(0, 1, &gTextures[5]->diffuseSpecularMapSRV); // First parameter must match texture slot number in the shaer
    gD3DContext->PSSetSamplers(0, 1, &gAnisotropic4xSampler);

    // States - additive blending, read-only depth buffer and no culling (standard set-up for blending
    gD3DContext->OMSetBlendState(gAdditiveBlendingState, nullptr, 0xffffff);
    gD3DContext->OMSetDepthStencilState(gDepthReadOnlyState, 0);
    gD3DContext->RSSetState(gCullNoneState);

    // Render all the lights in the array
    for (int i = 0; i < NUM_LIGHTS; ++i)
    {
        gPerModelConstants.objectColour = gLights[i]->colour; // Set any per-model constants apart from the world matrix just before calling render (light colour here)
        gLights[i]->model->Render();
    }
}




// Rendering the scene
void RenderScene()
{
    //// Common settings ////

    // Set up the light information in the constant buffer
    // Don't send to the GPU yet, the function RenderSceneFromCamera will do that
    gPerFrameConstants.light1Colour   = gLights[0]->colour * gLights[0]->strength;
    gPerFrameConstants.light1Position = gLights[0]->model->Position();
    gPerFrameConstants.light2Colour   = gLights[1]->colour * gLights[1]->strength;
    gPerFrameConstants.light2Position = gLights[1]->model->Position();

    gPerFrameConstants.ambientColour  = gAmbientColour;
    gPerFrameConstants.specularPower  = gSpecularPower;
    gPerFrameConstants.cameraPosition = gCamera->Position();



    //// Main scene rendering ////

    // Set the back buffer as the target for rendering and select the main depth buffer.
    // When finished the back buffer is sent to the "front buffer" - which is the monitor.
    gD3DContext->OMSetRenderTargets(1, &gBackBufferRenderTarget, gDepthStencil);

    // Clear the back buffer to a fixed colour and the depth buffer to the far distance
    gD3DContext->ClearRenderTargetView(gBackBufferRenderTarget, &gBackgroundColor.r);
    gD3DContext->ClearDepthStencilView(gDepthStencil, D3D11_CLEAR_DEPTH, 1.0f, 0);

    // Setup the viewport to the size of the main window
    D3D11_VIEWPORT vp;
    vp.Width  = static_cast<FLOAT>(gViewportWidth);
    vp.Height = static_cast<FLOAT>(gViewportHeight);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    gD3DContext->RSSetViewports(1, &vp);

    // Render the scene from the main camera
    RenderSceneFromCamera(gCamera);


    //// Scene completion ////

    // When drawing to the off-screen back buffer is complete, we "present" the image to the front buffer (the screen)
    // Set first parameter to 1 to lock to vsync (typically 60fps)
    gSwapChain->Present(lockFPS ? 1 : 0, 0);
}


//--------------------------------------------------------------------------------------
// Scene Update
//--------------------------------------------------------------------------------------

// Update models and camera. frameTime is the time passed since the last frame
void UpdateScene(float frameTime)
{
	gPerFrameConstants.gTime += frameTime;
	
	// Control sphere (will update its world matrix)
	gTeapot->Control(0, frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma );
	
    // Orbit the light - a bit of a cheat with the static variable [ask the tutor if you want to know what this is]
	/*static float rotate = 0.0f;
    static bool go = true;
	gLights[0].model->SetPosition( gTeapot->Position() + CVector3{ cos(rotate) * gLightOrbit, 10, sin(rotate) * gLightOrbit } );
    if (go)  rotate -= gLightOrbitSpeed * frameTime;
    if (KeyHit(Key_1))  go = !go;*/

	//Update 1st light's strength
	gLights[0]->strength = abs(sin(gPerFrameConstants.gTime)) * MAX_LIGHT_STRENGTH;
	gLights[0]->model->SetScale(pow(gLights[0]->strength, 0.7f));

	//Update 2nd light's colour
	CVector3 HSLColour = RGBToHSL(gLights[1]->colour);
	HSLColour.x += LIGHT_COLOUR_CHANGE;
	if (HSLColour.x >= 360.0f)
	{
		HSLColour.x = 0.0f;
	}
	gLights[1]->colour = HSLToRGB(HSLColour);
	
	// Control camera (will update its view matrix)
	gCamera->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D );


    // Toggle FPS limiting
    if (KeyHit(Key_P))  lockFPS = !lockFPS;

    // Show frame time / FPS in the window title //
    const float fpsUpdateTime = 0.5f; // How long between updates (in seconds)
    static float totalFrameTime = 0;
    static int frameCount = 0;
    totalFrameTime += frameTime;
    ++frameCount;
    if (totalFrameTime > fpsUpdateTime)
    {
        // Displays FPS rounded to nearest int, and frame time (more useful for developers) in milliseconds to 2 decimal places
        float avgFrameTime = totalFrameTime / frameCount;
        std::ostringstream frameTimeMs;
        frameTimeMs.precision(2);
        frameTimeMs << std::fixed << avgFrameTime * 1000;
        std::string windowTitle = "CO2409 Assignment - Frame Time: " + frameTimeMs.str() +
                                  "ms, FPS: " + std::to_string(static_cast<int>(1 / avgFrameTime + 0.5f));
        SetWindowTextA(gHWnd, windowTitle.c_str());
        totalFrameTime = 0;
        frameCount = 0;
    }
}
