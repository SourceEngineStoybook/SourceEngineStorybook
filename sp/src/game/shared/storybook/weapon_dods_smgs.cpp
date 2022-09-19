//=============================================================================//
//
// Purpose: DOD:S weapons based on Blixon's CSS weapon code for usage in a Half-Life 2 setting.
//
// Author: Wazanator
//
//=============================================================================//

#include "cbase.h"
#include "weapon_css_base.h"

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_C96
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_C96 : public CBase_CSS_HL2_SMG
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_C96, CBase_CSS_HL2_SMG);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_C96(void);

	virtual CSS_HL2_WeaponActClass		GetCSSWeaponActClass() { return CSSHL2_WEAPON_PISTOL; }

	virtual float GetViewKickBase() { return 9.0f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_15DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 0.08f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_C96, DT_Weapon_DODS_HL2_C96)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_c96, CWeapon_DODS_HL2_C96);
#if PRECACHE_REGISTER_CSS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_c96);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_C96)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_C96)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_C96::CWeapon_DODS_HL2_C96(void)
{
	m_bFiresUnderwater = false;
}

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_M1_Thompson
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_M1_Thompson : public CBase_CSS_HL2_SMG
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_M1_Thompson, CBase_CSS_HL2_SMG);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_M1_Thompson(void);

	virtual CSS_HL2_WeaponActClass		GetCSSWeaponActClass() { return CSSHL2_WEAPON_PISTOL; }

	virtual float GetViewKickBase() { return 5.0f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_5DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 0.08f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_M1_Thompson, DT_Weapon_DODS_HL2_M1_Thompson)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_thompson, CWeapon_DODS_HL2_M1_Thompson);
#if PRECACHE_REGISTER_CSS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_thompson);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_M1_Thompson)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_M1_Thompson)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_M1_Thompson::CWeapon_DODS_HL2_M1_Thompson(void)
{
	m_bFiresUnderwater = false;
}

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_MP40
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_MP40 : public CBase_CSS_HL2_SMG
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_MP40, CBase_CSS_HL2_SMG);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_MP40(void);

	virtual CSS_HL2_WeaponActClass		GetCSSWeaponActClass() { return CSSHL2_WEAPON_PISTOL; }

	virtual float GetViewKickBase() { return 3.0f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_4DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 0.1f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_MP40, DT_Weapon_DODS_HL2_MP40)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_mp40, CWeapon_DODS_HL2_MP40);
#if PRECACHE_REGISTER_CSS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_mp40);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_MP40)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_MP40)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_MP40::CWeapon_DODS_HL2_MP40(void)
{
	m_bFiresUnderwater = false;
}
