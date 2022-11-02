//==================================================================================================
//
// Lightblending shader with bumpmapping to be used on the earth model. Written by White_Red_Dragons ( ShiroDkxtro2#8750 )
//
//==================================================================================================

// Includes for all shaders
#include "BaseVSShader.h"
#include "cpp_shader_constant_register_map.h"

// Includes. These are important
#include "earth_ps30.inc"
#include "earth_vs30.inc"

// Defining samplers - Maximum amount of Sampler 16. Might have to reuse some later.
const Sampler_t SAMPLER_BASETEXTURE1 = SHADER_SAMPLER0;
const Sampler_t	SAMPLER_BASETEXTURE2 = SHADER_SAMPLER1;
const Sampler_t SAMPLER_NORMALMAP = SHADER_SAMPLER2;
const Sampler_t SAMPLER_ENVMAP = SHADER_SAMPLER3;
const Sampler_t SAMPLER_SHADOWDEPTH = SHADER_SAMPLER4;
const Sampler_t SAMPLER_RANDOMROTATION = SHADER_SAMPLER5;
const Sampler_t SAMPLER_FLASHLIGHT = SHADER_SAMPLER6;
const Sampler_t SAMPLER_NORMALIZATION = SHADER_SAMPLER11;
const Sampler_t SAMPLER_LIGHTWARP = SHADER_SAMPLER12;
const Sampler_t SAMPLER_ENVMAPMASK = SHADER_SAMPLER13;
// 7-15 free

// Convars
// Enable Fullbright behavior
extern ConVar mat_fullbright;
// Disable envmapping
static ConVar mat_specular("mat_specular", "1", FCVAR_CHEAT);

// Variables for this shader
struct Earth_Vars_t
{
	Earth_Vars_t()
	{
		memset(this, 0xFF, sizeof(*this));
	}
	
	int BumpMap;

	// This is used as an alternative for $bumpmap, however its outside so that it can be used instead of bumpmap
	// That is useful if you are using a vmt from AROH_MP, but don't have model lightmapping and can use $bumpmap instead.
	int NormalTexture;
	// WRD :	I just ported some code because its faster and I'm too lazy to actually rewrite the stuff so I put this note so you can't complain
	//			about me doing some wacky stuff lol

	// This is technically a global var but we have to define it in order to use it.
	int BaseTexture;
	// BaseTextureFrame Link
	int BaseTextureFrame;
	// BaseTextureTransform Link
	int BaseTextureTransform;
	// $AlphatTestReference - Self explanatory - link
	int AlphaTestReference;
	// This is required for the flashlight
	int flashlightTexture;
	int flashlightTextureFrame;

	// Nightside texture
	int NightSideTexture;
	int LightPower;

	int envMap;
	int m_nEnvMapMask;
	int m_nEnvMapFrame;
	int m_nEnvMapTint;
	int m_nEnvMapContrast;
	int m_nEnvMapSaturation;
	
	// Needed for bumped lighting. Sort of...
	int m_nLightWarpTexture;
};

// Beginning the shader
BEGIN_VS_SHADER(Earth, "Shader for Blending based on lightlevels")

// Setting up vmt parameters
BEGIN_SHADER_PARAMS;

// This is a global var and does not require to be specified, its just here for visual sake.
//SHADER_PARAM(BASETEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "DO NOT UNCOMMENT THIS LINE");

// Nightside
SHADER_PARAM(NIGHTSIDE, SHADER_PARAM_TYPE_TEXTURE, "", "Nightside with selfillum");
SHADER_PARAM(LIGHTPOWER, SHADER_PARAM_TYPE_FLOAT, "", "Power Increase/Decrease for lightblend");

// This is here BECAUSE : Too much hassle working it out.
// AlphaTestReference, just check VDC if you are unsure what it does
SHADER_PARAM(ALPHATESTREFERENCE, SHADER_PARAM_TYPE_FLOAT, "0", "");

// Funfact, this is the same as basetexture but not quite lol
SHADER_PARAM(BUMPMAP, SHADER_PARAM_TYPE_TEXTURE, "", "Bump Mapping Texture")

// This is used as an alternative for $bumpmap, however its outside so that it can be used instead of bumpmap
// That is useful if you are using a vmt from AROH_MP, but don't have model lightmapping and can use $bumpmap instead.
// This also makes it work with stock materials.
// WRD : just here because code porting stuff ( faster than writing from scratch )
SHADER_PARAM(NORMALTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "Normal texture. Basically just a bumpmap.");
SHADER_PARAM(LIGHTWARPTEXTURE, SHADER_PARAM_TYPE_TEXTURE, "", "Lightwarp Texature. Tints texel depending on their Brightness.")

SHADER_PARAM(ENVMAP, SHADER_PARAM_TYPE_ENVMAP, "", "Set the cubemap for this material.");
SHADER_PARAM(ENVMAPMASK, SHADER_PARAM_TYPE_TEXTURE, "shadertest/shadertest_envmask", "RGB Specular Texture. Will tint reflections like $envmaptint. This is useful for faking s/g PBR");
SHADER_PARAM(ENVMAPFRAME, SHADER_PARAM_TYPE_INTEGER, "0", "The frame to start an animated envmap on");

SHADER_PARAM(ENVMAPTINT, SHADER_PARAM_TYPE_COLOR, "", "envmap tint. Works like vector3 scale where each number scales the specific color by that much");

SHADER_PARAM(ENVMAPCONTRAST, SHADER_PARAM_TYPE_FLOAT, "0.0", "contrast 0 == normal 1 == color*color. Range from 0.01 to 9.99, trying any other values might affect other effects");
SHADER_PARAM(ENVMAPSATURATION, SHADER_PARAM_TYPE_FLOAT, "1.0", "saturation 0 == greyscale 1 == normal. Range from 0.01 to 9.99, trying any other values might affect other effects");
END_SHADER_PARAMS;

// Setting up variables for this shader
void SetupVars(IMaterialVar **params, Earth_Vars_t &info)
{
	// Only for sdk2013mp.
	info.BaseTexture = BASETEXTURE;
	info.NightSideTexture = NIGHTSIDE;
	info.LightPower = LIGHTPOWER;

	info.NormalTexture = NORMALTEXTURE;
	info.m_nLightWarpTexture = LIGHTWARPTEXTURE;

	info.BumpMap = BUMPMAP;
	info.BaseTextureFrame = FRAME;
	info.BaseTextureTransform = BASETEXTURETRANSFORM;
	info.AlphaTestReference = ALPHATESTREFERENCE;
	info.flashlightTexture = FLASHLIGHTTEXTURE;
	info.flashlightTextureFrame = FLASHLIGHTTEXTUREFRAME;
	info.envMap = ENVMAP;


	info.m_nEnvMapMask = ENVMAPMASK;
	info.m_nEnvMapFrame = ENVMAPFRAME;
	info.m_nEnvMapTint = ENVMAPTINT;
	info.m_nEnvMapContrast = ENVMAPCONTRAST;
	info.m_nEnvMapSaturation = ENVMAPSATURATION;

};

// Initializing parameters
SHADER_INIT_PARAMS()
{
	// We are using Normaltexture instead of bumpmap.
	// This is for backwards compatability! So don't remove it.
	if (!params[NORMALTEXTURE]->IsDefined())
	{
		if (params[BUMPMAP]->IsDefined())
			params[NORMALTEXTURE]->SetStringValue(params[BUMPMAP]->GetStringValue());
	}

	if (!params[NORMALTEXTURE]->IsDefined() && !params[BUMPMAP]->IsDefined())
	{
		params[NORMALTEXTURE]->SetStringValue("dev/flat_normal");
	}

	// Check if the hardware supports flashlight border color
	// if somehow there is an issue on RTX cards or other newer cards, remove the check and just force the new stuff.
	// This won't have ps20 shaders anyways, so it barely even matters. This is just here because I don't even know what certifies a pc support ing border colors
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
	// This is for hooking old shadernames. Can't be use with og "VertexLitGeneric" engine will throw an error telling you that you can't override base shaders
	// So this is practically useless unless you want to hook SDK_ shaders
	return 0;
};

// I wish we could do this after the Shader_Draw because then we wouldn't need to check if we can load a texture
// we are doing the same check afterwards for the bools... Mh maybe we can throw the bools in here but declare them earlier?
SHADER_INIT
{
	Earth_Vars_t info;
	SetupVars(params, info);

	//	No need for Assert
	//	Assert(info.flashlightTexture >= 0);
	// Load the FlashlightTexture
	LoadTexture(info.flashlightTexture, TEXTUREFLAGS_SRGB);

	// Only do Env Map Stuff if Envmap Is enabled
	if (params[info.envMap]->IsDefined())
	{
		//	No need for Assert
		//	Assert(info.envMap >= 0);
		int envMapFlags = g_pHardwareConfig->GetHDRType() == HDR_TYPE_NONE ? TEXTUREFLAGS_SRGB : 0;
		envMapFlags |= TEXTUREFLAGS_ALL_MIPS;
		LoadCubeMap(info.envMap, envMapFlags);
	}

	// Load the BaseTexture
	// Don't remove the check, this is for any defined one. Undefined one will mean it would load nothing
	if (params[info.BaseTexture]->IsDefined())
	{
		LoadTexture(info.BaseTexture, TEXTUREFLAGS_SRGB);
	}

	if (params[info.NightSideTexture]->IsDefined())
	{
		LoadTexture(info.NightSideTexture, TEXTUREFLAGS_SRGB);
	}

	// Load EnvMapMask
	if (params[info.m_nEnvMapMask]->IsDefined())
	{
		LoadTexture(info.m_nEnvMapMask);
	}

	// This forces $Model 1
	SET_FLAGS(MATERIAL_VAR_MODEL);
	// All the stuff for models
	if ((info.NormalTexture != -1) && params[info.NormalTexture]->IsDefined())
	{
		//	// Load the Bumpmap Texture, this is here because why check for the same thing twice?
		LoadBumpMap(info.NormalTexture);

		//	// This param breaks Vertex Lighting but we need it??? I can't really confirm this. I mentioned I had issues with black models when forcing static lighting off
			SET_FLAGS2(MATERIAL_VAR2_DIFFUSE_BUMPMAPPED_MODEL);         // Required for dynamic lighting
	}

	SET_FLAGS2(MATERIAL_VAR2_NEEDS_TANGENT_SPACES);             // Required for dynamic lighting
	SET_FLAGS2(MATERIAL_VAR2_USE_FIXED_FUNCTION_BAKED_LIGHTING);
	SET_FLAGS2(MATERIAL_VAR2_SUPPORTS_HW_SKINNING);             // Required for skinning
	SET_FLAGS2(MATERIAL_VAR2_LIGHTING_VERTEX_LIT);              // Required for dynamic lighting... Right... What...?
	SET_FLAGS2(MATERIAL_VAR2_NEEDS_BAKED_LIGHTING_SNAPSHOTS);   // Required for ambient cube
	SET_FLAGS2(MATERIAL_VAR2_SUPPORTS_FLASHLIGHT);              // Required for flashlight
	SET_FLAGS2(MATERIAL_VAR2_USE_FLASHLIGHT);                   // Required for flashlight

};

// Drawing the shader
SHADER_DRAW
{
	Earth_Vars_t info;
	SetupVars(params, info);

	// **Heavyily breaths in**
	// "THE BOOL LIST"

	// ->IsTexture Booleans

	// These two have to be up here for BaseAlpha and NormalAlpha Masking bools to work
	bool bHasBaseTexture1 = (info.BaseTexture != -1) && params[info.BaseTexture]->IsTexture();
	bool bHasBaseTexture2 = (info.NightSideTexture != -1) && params[info.NightSideTexture]->IsTexture();
	bool bHasNormalTexture = (info.NormalTexture != -1) && params[info.NormalTexture]->IsTexture();
	
	bool bHasEnvTexture = (info.envMap != -1) && params[info.envMap]->IsTexture();
	bool bHasEnvMapMask = (info.m_nEnvMapMask != -1) && params[info.m_nEnvMapMask]->IsTexture();
	bool bHasLightWarp = (info.m_nLightWarpTexture != -1) && params[info.m_nLightWarpTexture]->IsTexture();
	
	// These are global vars. Therefore we have to check if they are set, like so :
	// Luckily they don't need an info. link for loading the texture, so we can simply check if they are used and go from there
//	bool bHasSelfIllum = IS_FLAG_SET(MATERIAL_VAR_SELFILLUM) != 0; // unequal to 0 so you can have things beyond 1
	bool bIsAlphaTested = IS_FLAG_SET(MATERIAL_VAR_ALPHATEST) != 0; // unequal to 0 so you can have things beyond 1
//	bool bHasBaseAlphaEnvmapMask = IS_FLAG_SET(MATERIAL_VAR_BASEALPHAENVMAPMASK) != 0 && bHasBaseTexture1; // only makes sense if there is a basetexture
	bool bHalfLambert = IS_FLAG_SET(MATERIAL_VAR_HALFLAMBERT);

	//==================================================================================================
	//	Envmap Texture Parameters
	//
	// We default some values and only look up if we even have related textures.
	// This way we just do one check instead of checking for all bools even when there is no texture. Adds an additional check when you have one sadly
	//==================================================================================================
	// EnvMap related
	bool bHasEnvMapTint = false;
	bool bHasEnvMapContrast = false;
	bool bHasEnvMapSaturation = false;

	// We use all of these later
	float Float4EnvMapTintNoEMM[4] = { 1, 1, 1, 0 };
	float Float4EnvMapContrastSaturation[4] = { 0, 1, 0, 0 };

	if (bHasEnvTexture)
	{
//		Msg("bHasEnvTexture = True. \n\n");

		params[info.m_nEnvMapTint]->GetVecValue(Float4EnvMapTintNoEMM, 3); // ->IsDefined() doesn't work so this is required.
		bHasEnvMapTint = (Float4EnvMapTintNoEMM[0] != 1.000000 || Float4EnvMapTintNoEMM[1] != 1.000000 || Float4EnvMapTintNoEMM[2] != 1.000000); // Questionable, but works

		Float4EnvMapContrastSaturation[0] = params[info.m_nEnvMapContrast]->GetFloatValue();
		bHasEnvMapContrast = (Float4EnvMapContrastSaturation[0] != 0.00); // lets pray the default value of 0.0 works

		Float4EnvMapContrastSaturation[1] = params[info.m_nEnvMapSaturation]->GetFloatValue();
		bHasEnvMapSaturation = (Float4EnvMapContrastSaturation[1] != 1.00);

		// If we have no masking at all, simply display the envmap. Look in the shader under Envmapping and you'll understand
	}

	bool bHasLightPower = (params[info.LightPower]->GetFloatValue() != 0.000);

	// Rest is free
	float LightPower[4] = { 1, 0, 0.1, 0 };
	if (bHasLightPower)
	{
		LightPower[0] = params[info.LightPower]->GetFloatValue();
	}

	//	int ShaderComboIndex = 0; // required for later checks
	// Only mess with this bool if you know what you are doing. Its necessary for the second render pass
	bool bHasFlashlight = UsingFlashlight(params);

	// Determining whether we're dealing with a fully opaque material
	BlendType_t nBlendType = EvaluateBlendRequirements(info.BaseTexture, true);
	bool bFullyOpaque = (nBlendType != BT_BLENDADD) && (nBlendType != BT_BLEND) && !bIsAlphaTested;

	// Important stuff
	if (IsSnapshotting())
	{
		// If alphatest is on, enable it
		pShaderShadow->EnableAlphaTest(bIsAlphaTested);

		if (info.AlphaTestReference != -1 && params[info.AlphaTestReference]->GetFloatValue() > 0.0f)
		{
			pShaderShadow->AlphaFunc(SHADER_ALPHAFUNC_GEQUAL, params[info.AlphaTestReference]->GetFloatValue());
		}

		if (bHasFlashlight)
		{
			pShaderShadow->EnableBlending(true);
			pShaderShadow->BlendFunc(SHADER_BLEND_ONE, SHADER_BLEND_ONE); // Additive blending
		}
		else
		{
			SetDefaultBlendingShadowState(info.BaseTexture, true);
		}

		int nShadowFilterMode = bHasFlashlight ? g_pHardwareConfig->GetShadowFilterMode() : 0;

		// Setting up samplers. We always set them up because otherwise its a mess checking if statements here
		pShaderShadow->EnableTexture(SHADER_SAMPLER0, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER1, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER2, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER3, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER4, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER5, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER6, true);

		pShaderShadow->EnableTexture(SHADER_SAMPLER11, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER12, true);
		pShaderShadow->EnableTexture(SHADER_SAMPLER13, true);


		pShaderShadow->EnableSRGBRead(SAMPLER_BASETEXTURE1, true);   // Basecolor is sRGB
		pShaderShadow->EnableSRGBRead(SAMPLER_BASETEXTURE2, true);
		pShaderShadow->EnableSRGBRead(SAMPLER_NORMALMAP, false);       // Normal map are not sRGB. Thank god for that
		// Default VLG does not declare srgb or anything. For both Normalization OR EnvMapMask OR Lightwarp

		// if any of them are set, enable their respective samplers
		if (g_pHardwareConfig->GetHDRType() == HDR_TYPE_NONE)
		{
			pShaderShadow->EnableSRGBRead(SAMPLER_ENVMAP, true); // Envmap is only sRGB with HDR disabled?
		}

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

		// We only need the position and surface normal
		//  | VERTEX_SPECULAR <- enable vertex lighting behaviour?
		unsigned int flags = VERTEX_POSITION | VERTEX_NORMAL | VERTEX_FORMAT_COMPRESSED;
		// We need three texcoords, all in the default float2 size
		// NOTE : flags, TEXCOORD AMOUNT, texcoord res, userdata thing
		pShaderShadow->VertexShaderVertexFormat(flags, 1, 0, 0);

					DECLARE_STATIC_VERTEX_SHADER(earth_vs30);
					SET_STATIC_VERTEX_SHADER(earth_vs30);

					//Shader Declaration
					//Pixel Shader
					DECLARE_STATIC_PIXEL_SHADER(earth_ps30);
					// Required for Flashlight renderpass
					SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHT, bHasFlashlight);
					SET_STATIC_PIXEL_SHADER_COMBO(FLASHLIGHTDEPTHFILTERMODE, nShadowFilterMode);
					// Put anything else below
					SET_STATIC_PIXEL_SHADER_COMBO(ENVIRONMENTMAPPING, bHasEnvTexture); // bHasEnvTexture
					SET_STATIC_PIXEL_SHADER_COMBO(HALFLAMBERT, bHalfLambert);
					SET_STATIC_PIXEL_SHADER_COMBO(LIGHTWARP, bHasLightWarp);
					SET_STATIC_PIXEL_SHADER(earth_ps30);

		DefaultFog(); // Setting up fog

		// This will destroy 3d skybox stuff?
		//	pShaderShadow->EnableAlphaWrites(bFullyOpaque);
	}
	else // Not snapshotting -- begin dynamic state
	{
		bool bLightingOnly = mat_fullbright.GetInt() == 2 && !IS_FLAG_SET(MATERIAL_VAR_NO_DEBUG_OVERRIDE);

		// NOTE : We set all standard textures, because it would be more work to check whether or not to do so.
		// they get overriden afterwards if there is something to use, sadly this is rather unperformant when compared to not setting them at all

		pShaderAPI->BindStandardTexture(SHADER_SAMPLER2, TEXTURE_BLACK);
		pShaderAPI->BindStandardTexture(SHADER_SAMPLER4, TEXTURE_BLACK);
		pShaderAPI->BindStandardTexture(SHADER_SAMPLER5, TEXTURE_BLACK);
		pShaderAPI->BindStandardTexture(SHADER_SAMPLER6, TEXTURE_BLACK);
		pShaderAPI->BindStandardTexture(SHADER_SAMPLER13, TEXTURE_BLACK);

		if (bHasNormalTexture || bHasLightWarp)
		{
			pShaderAPI->BindStandardTexture(SHADER_SAMPLER11, TEXTURE_NORMALIZATION_CUBEMAP_SIGNED); // "Normalization Cubemap Sampler" Technically unused
		}
		//pShaderAPI->BindStandardTexture(SHADER_SAMPLER12, TEXTURE_BLACK); // Lightwarp is disabled when not used so no need for StandardTexture

		// Setting up albedo texture
		if (bHasBaseTexture1)
		{
			BindTexture(SAMPLER_BASETEXTURE1, info.BaseTexture, info.BaseTextureFrame);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_BASETEXTURE1, TEXTURE_GREY);
		}

		if (bHasBaseTexture2)
		{
			BindTexture(SAMPLER_BASETEXTURE2, info.NightSideTexture, info.BaseTextureFrame);
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_BASETEXTURE2, TEXTURE_BLACK);
		}


		// Default Fallback for all Shaders.

		// Setting up normal map
		if (bHasLightWarp)
		{
			BindTexture(SAMPLER_LIGHTWARP, info.m_nLightWarpTexture, -1);
		}

		if (bHasNormalTexture)
		{
			BindTexture(SAMPLER_NORMALMAP, info.NormalTexture, 0); // If lightwarp yes && Bumps -> default normal map -> actual normal map
		}
		else
		{
			pShaderAPI->BindStandardTexture(SAMPLER_NORMALMAP, TEXTURE_NORMALMAP_FLAT);
		}

		if (bHasEnvMapMask)
		{
						// SAMPLER13
			BindTexture(SAMPLER_ENVMAPMASK, info.m_nEnvMapMask, 0);
		}


		// Getting the light states, this means what kind of lighting should be received. However it appears we have to force it regardless.
		// I debugged this crap for 8 hours just to find you need to force ambient lighting if you want vertexlighting. Its dumb
		LightState_t lightState;
		pShaderAPI->GetDX9LightState(&lightState);

		// Setting up the flashlight related textures and variables. Practically Stock code so ignore
		bool bFlashlightShadows = false;
		if (bHasFlashlight)
		{
			BindTexture(SAMPLER_FLASHLIGHT, info.flashlightTexture, info.flashlightTextureFrame);
			VMatrix worldToTexture;
			ITexture *pFlashlightDepthTexture;
			FlashlightState_t state = pShaderAPI->GetFlashlightStateEx(worldToTexture, &pFlashlightDepthTexture);
			bFlashlightShadows = state.m_bEnableShadows && (pFlashlightDepthTexture != NULL);
			SetFlashLightColorFromState(state, pShaderAPI, PSREG_FLASHLIGHT_COLOR);  // c28
			if (pFlashlightDepthTexture && g_pConfig->ShadowDepthTexture() && state.m_bEnableShadows)
			{
				BindTexture(SAMPLER_SHADOWDEPTH, pFlashlightDepthTexture, 0);
				pShaderAPI->BindStandardTexture(SAMPLER_RANDOMROTATION, TEXTURE_SHADOW_NOISE_2D);
			}

		}

		// We are still preparing some things for Dynamic Combos.

		// Getting fog info
		MaterialFogMode_t fogType = pShaderAPI->GetSceneFogMode();
		int fogIndex = (fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z) ? 1 : 0;

		// Necessary for Skinning - Make sure he doesn't Get YOUR current bones
		int numBones = pShaderAPI->GetCurrentNumBones();

		// Some debugging stuff
		// Can possibly be overwritten
		bool bWriteDepthToAlpha = false;
		bool bWriteWaterFogToAlpha = false;
		if (bFullyOpaque) // Usually the case, why not inverse it? Set a default value unless !bFullyOpaque
		{
			bWriteDepthToAlpha = pShaderAPI->ShouldWriteDepthToDestAlpha();
			bWriteWaterFogToAlpha = (fogType == MATERIAL_FOG_LINEAR_BELOW_FOG_Z);
			AssertMsg(!(bWriteDepthToAlpha && bWriteWaterFogToAlpha),
				"Can't write two values to alpha at the same time.");
		}

		if (bHasEnvTexture)
		{
			float vEyePos_SpecExponent[4]; // Prepare a float
			pShaderAPI->GetWorldSpaceCameraPosition(vEyePos_SpecExponent); // Grab the Camera Position of the Player and throw it in our new float
			int iEnvMapLOD = 6;
			auto envTexture = params[info.envMap]->GetTextureValue(); 		// Determine the detaillevel used for the cubemap
			if (envTexture)
			{	// Get power of 2 of texture width
				int width = envTexture->GetMappingWidth();
				int mips = 0;
				while (width >>= 1)
					++mips;
				// Cubemap has 4 sides so 2 mips less
				iEnvMapLOD = mips;
			}
			// Dealing with very high and low resolution cubemaps
			if (iEnvMapLOD > 12)	iEnvMapLOD = 12;
			if (iEnvMapLOD < 4)		iEnvMapLOD = 4;
			// .w is unused
			vEyePos_SpecExponent[3] = iEnvMapLOD;
			pShaderAPI->SetPixelShaderConstant(11, vEyePos_SpecExponent, 1); // c11 - previously called as PSREG_EYEPOS_SPEC_EXPONENT
		}
					//			Warning("material %s Using Normalmap and more.\n\n", "Earth");
					DECLARE_DYNAMIC_VERTEX_SHADER(earth_vs30);
					SET_DYNAMIC_VERTEX_SHADER_COMBO(DYNAMIC_LIGHT, lightState.HasDynamicLight());
					SET_DYNAMIC_VERTEX_SHADER_COMBO(STATIC_LIGHT_VERTEX, 1);
					SET_DYNAMIC_VERTEX_SHADER_COMBO(DOWATERFOG, fogIndex);
					SET_DYNAMIC_VERTEX_SHADER_COMBO(SKINNING, numBones > 0);
					SET_DYNAMIC_VERTEX_SHADER_COMBO(LIGHTING_PREVIEW, pShaderAPI->GetIntRenderingParameter(INT_RENDERPARM_ENABLE_FIXED_LIGHTING) != 0);
					SET_DYNAMIC_VERTEX_SHADER_COMBO(COMPRESSED_VERTS, (int)vertexCompression);
					SET_DYNAMIC_VERTEX_SHADER_COMBO(NUM_LIGHTS, 4);
					SET_DYNAMIC_VERTEX_SHADER(earth_vs30);

					DECLARE_DYNAMIC_PIXEL_SHADER(earth_ps30);
					SET_DYNAMIC_PIXEL_SHADER_COMBO(WRITEWATERFOGTODESTALPHA, bWriteWaterFogToAlpha);
					SET_DYNAMIC_PIXEL_SHADER_COMBO(WRITE_DEPTH_TO_DESTALPHA, bWriteDepthToAlpha);
					SET_DYNAMIC_PIXEL_SHADER_COMBO(PIXELFOGTYPE, pShaderAPI->GetPixelFogCombo());
					SET_DYNAMIC_PIXEL_SHADER_COMBO(NUM_LIGHTS, 4);
					SET_DYNAMIC_PIXEL_SHADER_COMBO(AMBIENT_LIGHT, lightState.m_bAmbientLight ? 1 : 0);
					SET_DYNAMIC_PIXEL_SHADER(earth_ps30);

		// BaseTextureTransform, all shaders have it
		SetVertexShaderTextureTransform(VERTEX_SHADER_SHADER_SPECIFIC_CONST_0, info.BaseTextureTransform);

		// c1 - previously PSREG_DIFFUSE_MODULATION
		SetModulationPixelShaderDynamicState_LinearColorSpace(1);

		// Send ambient cube to the pixel shader, force to black if not available
		// c4-9
		pShaderAPI->SetPixelShaderStateAmbientLightCube(PSREG_AMBIENT_CUBE, !lightState.m_bAmbientLight);

		// Send lighting array to the pixel shader
		// c20-25
		pShaderAPI->CommitPixelShaderLighting(PSREG_LIGHT_INFO_ARRAY);

		// Handle mat_fullbright 2 (diffuse lighting only)
		if (bLightingOnly)
		{
			pShaderAPI->BindStandardTexture(SAMPLER_BASETEXTURE1, TEXTURE_GREY); // Consider moving this upwards
			pShaderAPI->BindStandardTexture(SAMPLER_BASETEXTURE2, TEXTURE_GREY); // Consider moving this upwards
		}

		// Handle mat_specular 0 (no envmap reflections)
		if (!mat_specular.GetBool())
		{
			pShaderAPI->BindStandardTexture(SAMPLER_ENVMAP, TEXTURE_BLACK); // Setting the black texture twice now :(
		}

		// Sending fog info to the pixel shader
		// c12
		pShaderAPI->SetPixelShaderFogParams(12);

		// Bind textures for all shader instances of this
			if (bHasEnvTexture)
			{
				BindTexture(SAMPLER_ENVMAP, info.envMap, 0);
			}

			if (bHasEnvMapTint)
			{
				pShaderAPI->SetPixelShaderConstant(32, Float4EnvMapTintNoEMM);
			}

			if (bHasEnvMapContrast == true || bHasEnvMapSaturation == true)
			{
				pShaderAPI->SetPixelShaderConstant(33, Float4EnvMapContrastSaturation);
			}

		// More flashlight related stuff
		// Flashlight is a second renderpass. It uses c2, c13, c14, c28. Which means that they are free otherwise.
		if (bHasFlashlight)
		{
			VMatrix worldToTexture;
			float atten[4], pos[4], tweaks[4];

			const FlashlightState_t &flashlightState = pShaderAPI->GetFlashlightState(worldToTexture);
			SetFlashLightColorFromState(flashlightState, pShaderAPI, PSREG_FLASHLIGHT_COLOR);  // c28 - ONLY USED WITH THE FLASHLIGHT

			BindTexture(SAMPLER_FLASHLIGHT, flashlightState.m_pSpotlightTexture, flashlightState.m_nSpotlightTextureFrame);

			// Set the flashlight attenuation factors
			atten[0] = flashlightState.m_fConstantAtten;
			atten[1] = flashlightState.m_fLinearAtten;
			atten[2] = flashlightState.m_fQuadraticAtten;
			atten[3] = flashlightState.m_FarZ;
			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_ATTENUATION, atten, 1);  // c13 - ONLY USED WITH THE FLASHLIGHT

			// Set the flashlight origin
			pos[0] = flashlightState.m_vecLightOrigin[0];
			pos[1] = flashlightState.m_vecLightOrigin[1];
			pos[2] = flashlightState.m_vecLightOrigin[2];
			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_POSITION_RIM_BOOST, pos, 1); // c14 - ONLY USED WITH THE FLASHLIGHT

			pShaderAPI->SetPixelShaderConstant(PSREG_FLASHLIGHT_TO_WORLD_TEXTURE, worldToTexture.Base(), 4); // c15 - ONLY USED WITH THE FLASHLIGHT

			// Tweaks associated with a given flashlight
			tweaks[0] = ShadowFilterFromState(flashlightState);
			tweaks[1] = ShadowAttenFromState(flashlightState);
			HashShadow2DJitter(flashlightState.m_flShadowJitterSeed, &tweaks[2], &tweaks[3]);
			pShaderAPI->SetPixelShaderConstant(PSREG_ENVMAP_TINT__SHADOW_TWEAKS, tweaks, 1); // c2 - ONLY USED WITH THE FLASHLIGHT
		}

		pShaderAPI->SetPixelShaderConstant(27, LightPower, 1);


	}	// Important!!! This bracket HAS TO BE above Draw(), I fucked this up before and so will you
	// Actually draw the shader
	Draw();
};

// Closing it off
END_SHADER;