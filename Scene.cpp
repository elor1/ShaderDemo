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
#include <vector>

#include "SceneObject.h"

//--------------------------------------------------------------------------------------
// Scene Data
//--------------------------------------------------------------------------------------
// Addition of Mesh, Model and Camera classes have greatly simplified this section
// Geometry data has gone to Mesh class. Positions, rotations, matrices have gone to Model and Camera classes

// Constants controlling speed of movement/rotation (measured in units per second because we're using frame time)
const float ROTATION_SPEED = 2.0f;  // 2 radians per second for rotation
const float MOVEMENT_SPEED = 50.0f; // 50 units per second for movement (what a unit of length is depends on 3D model - i.e. an artist decision usually)
const float BASE_LIGHT_STRENGTH = 40.0f;
const float LIGHT_COLOUR_CHANGE = 0.3f;
const float SPOTLIGHT_ANGLE = 90.0f;

// Meshes, models and cameras, same meaning as TL-Engine. Meshes prepared in InitGeometry function, Models & camera in InitScene
Mesh* gTeapotMesh;
Mesh* gSphereMesh;
Mesh* gCubeMesh;
Mesh* gGroundMesh;
Mesh* gLightMesh;
Mesh* gCubeTangentMesh;
Mesh* gDecalMesh;

std::vector<SceneObject*> gObjects;

Camera* gCamera;


// Store lights in an array in this exercise
const int NUM_LIGHTS = 2;
//Light* gLights[NUM_LIGHTS];
std::vector<Light*> gLight;


// Additional light information
CVector3 gAmbientColour = { 0.3f, 0.4f, 0.5f }; // Background level of light
float    gSpecularPower = 64; // Specular power controls shininess - same for all models in this app

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

PerFrameConstants gPerFrameConstants;      // The constants that need to be sent to the GPU each frame
ID3D11Buffer*     gPerFrameConstantBuffer; // The GPU buffer that will recieve the constants above

PerModelConstants gPerModelConstants;     
ID3D11Buffer*     gPerModelConstantBuffer;


//--------------------------------------------------------------------------------------
// Initialise scene geometry, constant buffers and states
//--------------------------------------------------------------------------------------

// Prepare the geometry required for the scene
// Returns true on success
bool InitGeometry()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
    // Load mesh geometry data
    try 
    {
		gTeapotMesh	     = new Mesh("Teapot.x");
		gSphereMesh	     = new Mesh("Sphere.x");
		gCubeMesh	     = new Mesh("Cube.x");
        gGroundMesh      = new Mesh("Hills.x", true);
        gLightMesh       = new Mesh("Light.x");
		gCubeTangentMesh = new Mesh("Cube.x", true);
		gDecalMesh       = new Mesh("Decal.x");
    }
    catch (std::runtime_error e)
    {
        gLastError = e.what();
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
    gPerFrameConstantBuffer = CreateConstantBuffer(sizeof(gPerFrameConstants));
    gPerModelConstantBuffer = CreateConstantBuffer(sizeof(gPerModelConstants));
    if (gPerFrameConstantBuffer == nullptr || gPerModelConstantBuffer == nullptr)
    {
        gLastError = "Error creating constant buffers";
        return false;
    }

	
  	// Create all filtering modes, blending modes etc.
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
	//// Set up models ////
	//Cubes
	gObjects.push_back(new SceneObject(new Model(gCubeMesh), new Texture("brick1.jpg"), gPixelLightingVertexShader,
	                                   gFadeTexturePixelShader, gNoBlendingState, gCullBackState, gUseDepthBufferState,
	                                   gAnisotropic4xSampler));
	gObjects.back()->textures.push_back(new Texture("wood2.jpg"));
	gObjects.back()->model->SetPosition({ 50.0f, 10.0f, -40.0f });

	gObjects.push_back(new SceneObject(new Model(gCubeMesh), new Texture("StoneDiffuseSpecular.dds"),
	                                   gPixelLightingVertexShader, gPixelLightingPixelShader, gNoBlendingState,
	                                   gCullBackState, gUseDepthBufferState, gAnisotropic4xSampler));
	gObjects.back()->model->SetPosition({ -10.0f, 30.0f, 40.0f });

	gObjects.push_back(new SceneObject(new Model(gCubeTangentMesh), new Texture("PatternDiffuseSpecular.dds"),
	                                   gNormalMappingVertexShader, gNormalMappingPixelShader, gNoBlendingState,
	                                   gCullBackState, gUseDepthBufferState, gAnisotropic4xSampler));
	gObjects.back()->textures.push_back(new Texture("PatternNormal.dds"));
	gObjects.back()->model->SetPosition({ 50.0f, 10.0f,40.0f });
	gObjects.back()->model->SetRotation({ 0.0f, 45.0f, 0.0f });
	gObjects.back()->model->SetScale(1.5f);

	//Decals
	gObjects.push_back(new SceneObject(new Model(gDecalMesh), new Texture("Moogle.png"), gPixelLightingVertexShader,
	                                   gTextureAlphaPixelShader, gMultiplicativeBlendingState, gCullBackState,
	                                   gUseDepthBufferState, gAnisotropic4xSampler));
	gObjects.back()->model->SetPosition({ -10.0f, 30.0f, 39.9f });

	gObjects.push_back(new SceneObject(new Model(gDecalMesh), new Texture("Cloud.png"), gPixelLightingVertexShader,
	                                   gFadeTexturePixelShader, gAdditiveBlendingState, gCullBackState,
	                                   gUseDepthBufferState, gAnisotropic4xSampler));
	gObjects.back()->textures.push_back(new Texture("Cloud.png"));
	gObjects.back()->model->SetPosition({ 50.0f, 10.0f, -40.1f });

	//Teapot
	gObjects.push_back(new SceneObject(new Model(gTeapotMesh), new Texture("MetalDiffuseSpecular.dds"),
	                                   gPixelLightingVertexShader, gPixelLightingPixelShader, gNoBlendingState,
	                                   gCullBackState, gUseDepthBufferState, gAnisotropic4xSampler));
	gObjects.back()->model->SetPosition({ 20.0f, 0.0f, 0.0f });
	gObjects.back()->model->SetScale(1.5f);

	//Sphere
	gObjects.push_back(new SceneObject(new Model(gSphereMesh), new Texture("tiles1.jpg"), gWiggleVertexShader,
	                                   gTextureScrollPixelShader, gNoBlendingState, gCullBackState,
	                                   gUseDepthBufferState, gAnisotropic4xSampler));
	gObjects.back()->model->SetPosition({ 15.0f, 20.0f, 50.0f });

	//Ground
	gObjects.push_back(new SceneObject(new Model(gGroundMesh), new Texture("CobbleDiffuseSpecular.dds"),
	                                   gNormalMappingVertexShader, gParallaxMappingPixelShader, gNoBlendingState,
	                                   gCullBackState, gUseDepthBufferState, gAnisotropic4xSampler));
	gObjects.back()->textures.push_back(new Texture("CobbleNormalHeight.dds"));

	gObjects.push_back(new SceneObject(new Model(gCubeMesh), new Texture("Skybox.dds"),
		gSkyboxVertexShader, gSkyboxPixelShader, gNoBlendingState,
		gCullFrontState, gSkyboxDepthBufferState, gTrilinearSampler));
	gObjects.back()->model->SetScale(25.0f);
	
	for (auto object : gObjects)
	{
		for (auto texture : object->textures)
		{
			if (!texture->Load())
			{
				return false;
			}
		}
	}
	
	//Light set up
	for (int i = 0 ; i < NUM_LIGHTS; i++)
	{
		gLight.push_back(new Light(new Model(gLightMesh), new Texture("Flare.jpg"), gBasicTransformVertexShader, gLightModelPixelShader, gAdditiveBlendingState, gCullNoneState, gDepthReadOnlyState, gAnisotropic4xSampler, BASE_LIGHT_STRENGTH, { 0.8f, 0.8f, 1.0f }));
	}
	gLight[0]->model->SetPosition({ 30, 20, 0 });
	gLight[1]->colour = { 1.0f, 0.8f, 0.2f };
	gLight[1]->model->SetPosition({ -80, 50, 40 });
	gLight[1]->model->SetRotation({ 0.0f, 1.7f, 0.0f });
	gLight[1]->strength *= 3;

	for (auto light : gLight)
	{
		light->model->SetScale(pow(light->strength, 0.7f)); // Convert light strength into a nice value for the scale of the light
		
		for (auto texture : light->textures)
		{
			if (!texture->Load())
			{
				return false;
			}
		}
	}
	
    //// Set up camera ////
    gCamera = new Camera();
    gCamera->SetPosition({ 15, 50,-120 });
    gCamera->SetRotation({ ToRadians(13), 0, 0 });

    return true;
}


// Release the geometry and scene resources created above
void ReleaseResources()
{
    ReleaseStates();

    if (gPerModelConstantBuffer)  gPerModelConstantBuffer->Release();
    if (gPerFrameConstantBuffer)  gPerFrameConstantBuffer->Release();

    ReleaseShaders();
	
	for (auto light : gLight)
	{
		delete light;	light = nullptr;
	}

	for (auto object : gObjects)
	{
		delete object;	object = nullptr;
	}

	delete gCamera;			 gCamera		  = nullptr;

    delete gLightMesh;       gLightMesh       = nullptr;
    delete gGroundMesh;      gGroundMesh      = nullptr;
	delete gTeapotMesh;      gTeapotMesh      = nullptr;
	delete gSphereMesh;	     gSphereMesh	  = nullptr;
	delete gCubeMesh;	     gCubeMesh		  = nullptr;
	delete gCubeTangentMesh; gCubeTangentMesh = nullptr;
	delete gDecalMesh;	     gDecalMesh		  = nullptr;
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
    gD3DContext->VSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);
    gD3DContext->PSSetConstantBuffers(0, 1, &gPerFrameConstantBuffer);

	for (auto object : gObjects)
	{
		object->Render();
	}
	
    for (auto light : gLight)
    {
		gPerModelConstants.objectColour = light->colour;
		light->Render();
    }
}




// Rendering the scene
void RenderScene()
{
    //// Common settings ////

    // Set up the light information in the constant buffer
    // Don't send to the GPU yet, the function RenderSceneFromCamera will do that
    gPerFrameConstants.light1Colour   = gLight[0]->colour * gLight[0]->strength;
    gPerFrameConstants.light1Position = gLight[0]->model->Position();
	
    gPerFrameConstants.light2Colour   = gLight[1]->colour * gLight[1]->strength;
    gPerFrameConstants.light2Position = gLight[1]->model->Position();
	gPerFrameConstants.light2Facing	  = Normalise(gLight[1]->model->WorldMatrix().GetZAxis());
	gPerFrameConstants.light2CosHalfAngle = cos(ToRadians(SPOTLIGHT_ANGLE / 2));

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
	
	// Controls
	gObjects[5]->model->Control(0, frameTime, Key_I, Key_K, Key_J, Key_L, Key_U, Key_O, Key_Period, Key_Comma );
	gCamera->Control(frameTime, Key_Up, Key_Down, Key_Left, Key_Right, Key_W, Key_S, Key_A, Key_D);
	
    // Orbit the light
	static float rotate = 0.0f;
    static bool go = true;
	gLight[0]->model->SetPosition(gObjects[5]->model->Position() + CVector3{ cos(rotate) * gLightOrbit, 10, sin(rotate) * gLightOrbit } );
    if (go)  rotate -= gLightOrbitSpeed * frameTime;
    if (KeyHit(Key_1))  go = !go;

	//Update 1st light's strength
	gLight[0]->strength = abs(sin(gPerFrameConstants.gTime)) * BASE_LIGHT_STRENGTH;
	gLight[0]->model->SetScale(pow(gLight[0]->strength, 0.7f));

	//Update 2nd light's colour
	CVector3 HSLColour = RGBToHSL(gLight[1]->colour);
	HSLColour.x += LIGHT_COLOUR_CHANGE;
	if (HSLColour.x >= 360.0f)
	{
		HSLColour.x = 0.0f;
	}
	gLight[1]->colour = HSLToRGB(HSLColour);

	//Skybox follows camera;
	gObjects.back()->model->SetPosition(gCamera->Position());

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
