//==================================================================================================
//
// Fake Atmosphere shader with consideration for lightsources. Written by White_Red_Dragons ( ShiroDkxtro2#8750 )
// To be used with $additive. Do not shine a flashlight on a model using this. In my experience that will just crash the game for some reason.
//	I did not have the time to debug that and its unnecessary. Who has a flashlight in their 3d skybox anyways?
//
//==================================================================================================

// Includes for all shaders
#include "BaseVSShader.h"
#include "cpp_shader_constant_register_map.h"

// Includes for PS30
#include "atmosphere_vs30.inc"
#include "atmosphere_ps30.inc"

// This is not technically required, but makes it easier to understand later
const Sampler_t SAMPLER_BASETEXTURE = SHADER_SAMPLER0;
const Sampler_t SAMPLER_SHADOWDEPTH = SHADER_SAMPLER4;
const Sampler_t SAMPLER_RANDOMROTATION = SHADER_SAMPLER5;
const Sampler_t SAMPLER_FLASHLIGHT = SHADER_SAMPLER6;

// Convars
extern ConVar mat_fullbright;

// Variables for this shader
struct Atmosphere_Vars_t
{
	Atmosphere_Vars_t()
	{
		memset(this, 0xFF, sizeof(*this));
	}

	// The usual Stuff
	int baseTexture;
	int TransparencyFactor;
	int GradientFactor;

	int flashlightTexture;
	int flashlightTextureFrame;
	int alphaTestReference;
};

// Beginning the shader
BEGIN_VS_SHADER(ATMOSPHERE, "Fake Atmosphere shader")

// Setting up vmt parameters
BEGIN_SHADER_PARAMS;
// WRD: Basetexture is a global param, it does not have to be defined

SHADER_PARAM(TRANSPARENCYFACTOR, SHADER_PARAM_TYPE_FLOAT, "", "1 more, 0 less");
SHADER_PARAM(GRADIENTFACTOR, SHADER_PARAM_TYPE_FLOAT, "", "1 more, 0 less");
SHADER_PARAM(ALPHATESTREFERENCE, SHADER_PARAM_TYPE_FLOAT, "", "Hah get it? Because its a reference?")

END_SHADER_PARAMS;

// Setting up variables for this shader
void SetupVars(IMaterialVar **params, Atmosphere_Vars_t &info)
{
	info.baseTexture = BASETEXTURE;
	info.TransparencyFactor = TRANSPARENCYFACTOR;
	info.GradientFactor = GRADIENTFACTOR;
	info.alphaTestReference = ALPHATESTREFERENCE;
};

// Initializing parameters
SHADER_INIT_PARAMS()
{
	// Check if the hardware supports flashlight border color
	if (g_pHardwareConfig->SupportsBorderColor())
	{
		params[FLASHLIGHTTEXTURE]->SetStringValue("effects/flashlight_border");
	}
	else
	{
		params[FLASHLIGHTTEXTURE]->SetStringValue("effects/flashlight001");
	}
};

// Define shader fallback
SHADER_FALLBACK
{
	return 0;
};

SHADER_INIT
{
	Atmosphere_Vars_t info;
	SetupVars(params, info);

	Assert(info.flashlightTexture >= 0);
	LoadTexture(info.flashlightTexture, TEXTUREFLAGS_SRGB);

	if (params[info.baseTexture]->IsDefined())
	{
		LoadTexture(info.baseTexture, TEXTUREFLAGS_SRGB);
	}

	SET_FLAGS(MATERIAL_VAR_MODEL);

		SET_FLAGS2(MATERIAL_VAR2_SUPPORTS_HW_SKINNING);             // Required for skinning
		SET_FLAGS2(MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL);         // Required for dynamic lighting
		SET_FLAGS2(MATERIAL_VAR2_NEEDS_TANGENT_SPACES);             // Required for dynamic lighting
		SET_FLAGS2(MATERIAL_VAR2_LIGHTING_VERTEX_LIT);              // Required for dynamic lighting
		SET_FLAGS2(MATERIAL_VAR2_NEEDS_BAKED_LIGHTING_SNAPSHOTS);   // Required for ambient cube
		SET_FLAGS2(MATERIAL_VAR2_SUPPORTS_FLASHLIGHT);              // Required for flashlight
		SET_FLAGS2(MATERIAL_VAR2_USE_FLASHLIGHT);                   // Required for flashlight
};

// Drawing the shader
SHADER_DRAW
{
	Atmosphere_Vars_t info;
	SetupVars(params, info);

	// Setting up booleans
	bool bHasBaseTexture = (info.baseTexture != -1) && params[info.baseTexture]->IsTexture();
	bool bHasFlashlight = UsingFlashlight(params);
	bool bHasTransparencyFactor = (info.TransparencyFactor != -1) && params[info.TransparencyFactor]->GetFloatValue() != 0.0f;
	bool bHasGradientFactor = (info.GradientFactor != -1) && params[info.GradientFactor]->GetFloatValue() != 0.0f;
	bool bIsAlphaTested = IS_FLAG_SET(MATERIAL_VAR_ALPHATEST) != 0;

	// Determining whether we're dealing with a fully opaque material
	BlendType_t nBlendType = EvaluateBlendRequirements(info.baseTexture, true);
	bool bFullyOpaque = (nBlendType != BT_BLENDADD) && (nBlendType != BT_BLEND) && !bIsAlphaTested;

	if (info.alphaTestReference != -1 && params[info.alphaTestReference]->GetFloatValue() > 0.0f)
	{
		pShaderShadow->AlphaFunc(SHADER_ALPHAFUNC_GEQUAL, params[info.alphaTestReference]->GetFloatValue());
	}

	// Basically : Not in Hammer(?)
	if (IsSnapshotting())
	{
		// If alphatest is on, enable it. Well it always is... so yeah haha
		pShaderShadow->EnableAlphaTest(bIsAlphaTested);

		if (bHasFlashlight)
		{
			pShaderShadow->EnableBlending(true);
			pShaderShadow->BlendFunc(SHADER_BLEND_ONE, SHADER_BLEND_ONE); // Additive blending
		}
		else
		{
			SetDefaultBlendingShadowState(info.baseTexture, true);
		}

//		int nShadowFilterMode = bHasFlashlight ? g_pHardwareConfig->GetShadowFilterMode() : 0;

		// Setting up samplers
		pShaderShadow->EnableTexture(SAMPLER_BASETEXTURE, true);    // Basetexture
		pShaderShadow->EnableSRGBRead(SAMPLER_BASETEXTURE, true);   // Basetexture is sRGB

		// If the flashlight is on, set up its textures
		if (bHasFlashlight)
		{
			pShaderShadow->EnableTexture(SAMPLER_SHADOWDEPTH, true);        // Shadow depth map
			pShaderShadow->SetShadowDepthFiltering(SAMPLER_SHADOWDEPTH);
			pShaderShadow->EnableSRGBRead(SAMPLER_SHADOWDEPTH, false);
			pShaderShadow->EnableTexture(SAMPLER_RANDOMROTATION, true);     // Noise map
			pShaderShadow->EnableTexture(SAMPLER_FLASHLIGHT, true);         // Flashlight cookie
			pShaderShadow->EnableSRGBRead(SAMPLER_FLASHLIGHT, true);
		}

		// Enabling sRGB writing
		// See common_ps_fxc.h line 349
		// PS2b shaders and up write sRGB
		pShaderShadow->EnableSRGBWrite(true);

		// This only works on model
			// We only need the position and surface normal
			unsigned int flags = VERTEX_POSITION | VERTEX_NORMAL | VERTEX_FORMAT_COMPRESSED;
			// We need three texcoords, all in the default float2 size
			pShaderShadow->VertexShaderVertexFormat(flags, 1, 0, 0);

		// We don't do linux
		// Setting up static vertex shader
		DECLARE_STATIC_VERTEX_SHADER(atmosphere_vs30);
		SET_STATIC_VERTEX_SHADER(atmosphere_vs30);

		// Setting up static pixel shader
		DECLARE_STATIC_PIXEL_SHADER(atmosphere_ps30);
		SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHT, 0);
		SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHTDEPTHFILTERMODE, 0);
		SET_STATIC_PIXEL_SHADER(atmosphere_ps30);

		// Setting up fog
		DefaultFog();

		// HACK HACK HACK - enable alpha writes all the time so that we have them for underwater stuff
		// WRD: Do not do this if you want it to render in the 3d skybox, I experienced issues with it.
		// pShaderShadow->EnableAlphaWrites(bFullyOpaque);
	}
	else // Not snapshotting -- begin dynamic state
	{
		bool bLightingOnly = mat_fullbright.GetInt() == 2 && !IS_FLAG_SET(MATERIAL_VAR_NO_DEBUG_OVERRIDE);

		// Setting up albedo texture
		if (bHasBaseTexture)
		{
			// Last number here is the frame to start on.
			BindTexture(SAMPLER_BASETEXTURE, info.baseTexture, 0);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_BASETEXTURE, TEXTURE_GREY);
		}

		// Getting the light state
		// WRD : What this does is get the lightstate. Yeah right I know thats what It just said. More specifically it gets whether to use following lightstates
		// lightState.m_nNumLights, lightState.m_bAmbientLight, lightState.m_bStaticLightTexel, lightState.m_bStaticLightVertex
		// There is also a dynamic one, anyways although technically overridable, not advised since it can drastically fuck up the lighting
		// Also StaticLightTexel, StaticLightVertex, NumLights appear to not exist in sdk2013sp.
		LightState_t lightState;
		pShaderAPI->GetDX9LightState(&lightState);

		// Setting up the flashlight related textures and variables
		bool bFlashlightShadows = false;
		if (bHasFlashlight)
		{
			Assert(info.flashlightTexture >= 0 && info.flashlightTextureFrame >= 0);
			Assert(params[info.flashlightTexture]->IsTexture());
			BindTexture(SAMPLER_FLASHLIGHT, info.flashlightTexture, info.flashlightTextureFrame);
			VMatrix worldToTexture;
			ITexture *pFlashlightDepthTexture;
			FlashlightState_t state = pShaderAPI->GetFlashlightStateEx(worldToTexture, &pFlashlightDepthTexture);
			bFlashlightShadows = state.m_bEnableShadows && (pFlashlightDepthTexture != NULL);

			SetFlashLightColorFromState(state, pShaderAPI, PSREG_FLASHLIGHT_COLOR);

			if (pFlashlightDepthTexture && g_pConfig->ShadowDepthTexture() && state.m_bEnableShadows)
			{
				BindTexture(SAMPLER_SHADOWDEPTH, pFlashlightDepthTexture, 0);
				pShaderAPI->BindStandardTexture(SAMPLER_RANDOMROTATION, TEXTURE_SHADOW_NOISE_2D);
			}
		}

		// Getting fog info
		MaterialFogMode_t fogType = pShaderAPI->GetSceneFogMode();
		int fogIndex = (fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z) ? 1 : 0;

		// Getting skinning info
		int numBones = pShaderAPI->GetCurrentNumBones();

		// Some debugging stuff
		bool bWriteDepthToAlpha = false;
		bool bWriteWaterFogToAlpha = false;
		if (bFullyOpaque)
		{
			bWriteDepthToAlpha = pShaderAPI->ShouldWriteDepthToDestAlpha();
			bWriteWaterFogToAlpha = (fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z);
			AssertMsg(!(bWriteDepthToAlpha && bWriteWaterFogToAlpha),
				"Can't write two values to alpha at the same time.");
		}

		float vEyePos_SpecExponent[4];
		pShaderAPI->GetWorldSpaceCameraPosition(vEyePos_SpecExponent);

		// Setting up dynamic vertex shader
		DECLARE_DYNAMIC_VERTEX_SHADER(atmosphere_vs30);
		SET_DYNAMIC_VERTEX_SHADER_COMBO(DYNAMIC_LIGHT, lightState.HasDynamicLight());
		SET_DYNAMIC_VERTEX_SHADER_COMBO(STATIC_LIGHT_VERTEX, 1); // HackHack - This Lightstate doesn't exist in SP so I just forced it to the default value
		SET_DYNAMIC_VERTEX_SHADER_COMBO(DOWATERFOG, fogIndex);
		SET_DYNAMIC_VERTEX_SHADER_COMBO(SKINNING, numBones > 0);
		SET_DYNAMIC_VERTEX_SHADER_COMBO(LIGHTING_PREVIEW, pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_ENABLE_FIXED_LIGHTING) != 0);
		SET_DYNAMIC_VERTEX_SHADER_COMBO(COMPRESSED_VERTS, (int)vertexCompression);
		SET_DYNAMIC_VERTEX_SHADER_COMBO(NUM_LIGHTS, 4); // HackHack - This Lightstate doesn't exist in SP so I just forced it to the default value
		SET_DYNAMIC_VERTEX_SHADER(atmosphere_vs30);

		// Setting up dynamic pixel shader
		DECLARE_DYNAMIC_PIXEL_SHADER(atmosphere_ps30);
		//SET_DYNAMIC_PIXEL_SHADER_COMBO(NUM_LIGHTS, lightState.m_nNumLights); HackHack - This Lightstate doesn't exist in SP so I just forced it to the default value
		SET_DYNAMIC_PIXEL_SHADER_COMBO(WRITEWATERFOGTODESTALPHA, bWriteWaterFogToAlpha);
		SET_DYNAMIC_PIXEL_SHADER_COMBO(WRITE_DEPTH_TO_DESTALPHA, bWriteDepthToAlpha);
		SET_DYNAMIC_PIXEL_SHADER_COMBO(PIXELFOGTYPE, pShaderAPI->GetPixelFogCombo());
		//SET_DYNAMIC_PIXEL_SHADER_COMBO(FLASHLIGHTSHADOWS, bFlashlightShadows);
		SET_DYNAMIC_PIXEL_SHADER(atmosphere_ps30);

		// What ColorSpace to use
		SetModulationPixelShaderDynamicState_LinearColorSpace(PSREG_DIFFUSE_MODULATION);

		// Send lighting array to the pixel shader
		pShaderAPI->CommitPixelShaderLighting(PSREG_LIGHT_INFO_ARRAY);

		// Handle mat_fullbright 2 (diffuse lighting only)
		if (bLightingOnly)
		{
			pShaderAPI->BindStandardTexture(SAMPLER_BASETEXTURE, TEXTURE_GREY); // Basecolor
		}

		// Sending fog info to the pixel shader
		pShaderAPI->SetPixelShaderFogParams(PSREG_FOG_PARAMS);

		// More flashlight related stuff
		if (bHasFlashlight)
		{
			VMatrix worldToTexture;
			float atten[4], pos[4], tweaks[4];

			const FlashlightState_t &flashlightState = pShaderAPI->GetFlashlightState(worldToTexture);
			SetFlashLightColorFromState(flashlightState, pShaderAPI, PSREG_FLASHLIGHT_COLOR);

			BindTexture(SAMPLER_FLASHLIGHT, flashlightState.m_pSpotlightTexture, flashlightState.m_nSpotlightTextureFrame);

			// Set the flashlight attenuation factors
			atten[0] = flashlightState.m_fConstantAtten;
			atten[1] = flashlightState.m_fLinearAtten;
			atten[2] = flashlightState.m_fQuadraticAtten;
			atten[3] = flashlightState.m_FarZ;
			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_ATTENUATION, atten, 1);

			// Set the flashlight origin
			pos[0] = flashlightState.m_vecLightOrigin[0];
			pos[1] = flashlightState.m_vecLightOrigin[1];
			pos[2] = flashlightState.m_vecLightOrigin[2];
			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_POSITION_RIM_BOOST, pos, 1);

			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_TO_WORLD_TEXTURE, worldToTexture.Base(), 4);

			// Tweaks associated with a given flashlight
			tweaks[0] = ShadowFilterFromState(flashlightState);
			tweaks[1] = ShadowAttenFromState(flashlightState);
			HashShadow2DJitter(flashlightState.m_flShadowJitterSeed, &tweaks[2], &tweaks[3]);
			pShaderAPI->SetPixelShaderConstant(PSREG_ENVMAP_TINT__SHADOW_TWEAKS, tweaks, 1);
		}

		float AtmosphereControl[4] = {1, 0, 0, 1};

		if (bHasTransparencyFactor)
		{
			AtmosphereControl[0] = params[info.TransparencyFactor]->GetFloatValue();
		}

		if (bHasGradientFactor)
		{
			AtmosphereControl[3] = params[info.GradientFactor]->GetFloatValue();
		}
		

		pShaderAPI->SetPixelShaderConstant(27, AtmosphereControl, 1);
		pShaderAPI->SetPixelShaderConstant(5, vEyePos_SpecExponent, 1);

	} // WRD : IMPORTANT!!!!! This must be above Draw. I f'd it up once, never again, therefore this note is here lol
	// Actually draw the shader
	Draw();
};

// Closing it off
END_SHADER;