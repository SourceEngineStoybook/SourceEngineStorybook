//=============================================================================//
//
// Purpose: DOD:S weapons based on Blixon's CSS weapon code for usage in a Half-Life 2 setting.
//
// Author: Wazanator
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "weapon_css_base.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "gamestats.h"
#ifndef CLIENT_DLL
#include "basecombatcharacter.h"
#include "ai_basenpc.h"
#include "player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//extern ConVar pistol_use_new_accuracy;

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_Colt
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_Colt : public CBase_CSS_HL2_Pistol
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_Colt, CBase_CSS_HL2_Pistol);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_Colt(void);

	virtual float GetViewKickBase() { return 1.5f; }

	virtual float GetShotPenaltyTime() { return 0.4f; }

	virtual const Vector& GetBulletSpread(void)
	{
		// Handle NPCs first
		static Vector npcCone = VECTOR_CONE_3DEGREES;
		if (GetOwner() && GetOwner()->IsNPC())
			return npcCone;

		static Vector cone;

		float ramp = RemapValClamped(GetAccuracyPenalty(),
			0.0f,
			1.5f,
			0.0f,
			1.0f);

		// We lerp from very accurate to inaccurate over time
		VectorLerp(VECTOR_CONE_1DEGREES, VECTOR_CONE_5DEGREES, ramp, cone);

		return cone;
	}

	virtual float GetFireRate(void) { return 0.5f; }
	virtual float GetRefireRate(void) { return 0.15f; }
	virtual float GetDryRefireRate(void) { return 0.25f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_Colt, DT_Weapon_DODS_HL2_Colt)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_colt, CWeapon_DODS_HL2_Colt);
#if PRECACHE_REGISTER_DODS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_colt);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_Colt)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_Colt)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_Colt::CWeapon_DODS_HL2_Colt(void)
{
}

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_P38
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_P38 : public CBase_CSS_HL2_Pistol
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_P38, CBase_CSS_HL2_Pistol);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_P38(void);

	virtual float GetViewKickBase() { return 1.5f; }

	virtual float GetShotPenaltyTime() { return 0.4f; }

	virtual const Vector& GetBulletSpread(void)
	{
		// Handle NPCs first
		static Vector npcCone = VECTOR_CONE_3DEGREES;
		if (GetOwner() && GetOwner()->IsNPC())
			return npcCone;

		static Vector cone;

		float ramp = RemapValClamped(GetAccuracyPenalty(),
			0.0f,
			1.5f,
			0.0f,
			1.0f);

		// We lerp from very accurate to inaccurate over time
		VectorLerp(VECTOR_CONE_1DEGREES, VECTOR_CONE_5DEGREES, ramp, cone);

		return cone;
	}

	virtual float GetFireRate(void) { return 0.5f; }
	virtual float GetRefireRate(void) { return 0.15f; }
	virtual float GetDryRefireRate(void) { return 0.25f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_P38, DT_Weapon_DODS_HL2_P38)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_p38, CWeapon_DODS_HL2_P38);
#if PRECACHE_REGISTER_DODS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_p38);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_P38)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_P38)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_P38::CWeapon_DODS_HL2_P38(void)
{
}