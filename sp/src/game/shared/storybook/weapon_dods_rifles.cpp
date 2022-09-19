//=============================================================================//
//
// Purpose: CS:S weapons recreated from scratch in Source SDK 2013 for usage in a Half-Life 2 setting.
//
// Author: Blixibon
//
//=============================================================================//

#include "cbase.h"
#include "weapon_css_base.h"

//=============================================================================
// ASSAULT RIFLES
//=============================================================================

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_BAR
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_BAR : public CBase_CSS_HL2_Rifle
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_BAR, CBase_CSS_HL2_Rifle);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_BAR(void);

	virtual CSS_HL2_WeaponActClass		GetCSSWeaponActClass() { return CSSHL2_WEAPON_AR1; }

	virtual float GetViewKickBase() { return 6.75f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_3DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 0.1f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_BAR, DT_Weapon_DODS_HL2_BAR)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_bar, CWeapon_DODS_HL2_BAR);
#if PRECACHE_REGISTER_CSS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_bar);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_BAR)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_BAR)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_BAR::CWeapon_DODS_HL2_BAR(void)
{
}

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_MP44
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_MP44 : public CBase_CSS_HL2_Rifle
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_MP44, CBase_CSS_HL2_Rifle);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_MP44(void);

	virtual CSS_HL2_WeaponActClass		GetCSSWeaponActClass() { return CSSHL2_WEAPON_AR1; }

	virtual float GetViewKickBase() { return 7.75f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_3DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 0.1f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_MP44, DT_Weapon_DODS_HL2_MP44)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_mp44, CWeapon_DODS_HL2_MP44);
#if PRECACHE_REGISTER_CSS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_mp44);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_MP44)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_MP44)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_MP44::CWeapon_DODS_HL2_MP44(void)
{
}

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_M1_Carbine
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_M1_Carbine : public CBase_CSS_HL2_SniperRifle
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_M1_Carbine, CBase_CSS_HL2_SniperRifle);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_M1_Carbine(void);

	virtual float GetViewKickBase() { return 2.0f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_3DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 0.25f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_M1_Carbine, DT_Weapon_DODS_HL2_M1_Carbine)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_m1carbine, CWeapon_DODS_HL2_M1_Carbine);
#if PRECACHE_REGISTER_CSS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_m1carbine);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_M1_Carbine)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_M1_Carbine)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_M1_Carbine::CWeapon_DODS_HL2_M1_Carbine(void)
{
	m_bCanUseScope = false;
}

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_Kar98K
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_Kar98K : public CBase_CSS_HL2_SniperRifle
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_Kar98K, CBase_CSS_HL2_SniperRifle);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_Kar98K(void);

	virtual float GetViewKickBase() { return IsWeaponZoomed() ? 3.0f : 4.0f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_1DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 1.25f; }

	virtual int		GetZoom1FOV() const { return 40; }
	virtual int		GetZoom2FOV() const { return 15; }
	virtual int		GetZoomRate() const { return 0.2f; }
	virtual int		GetUnZoomRate() const { return 0.15f; }

	// Player damage: 9 -> 30.6 (30)
	// NPC damage: 4 -> 20
	virtual float GetDamageMultiplier() const { return 3.4f; }
	virtual float GetNPCDamageMultiplier() const { return 5.0f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_Kar98K, DT_Weapon_DODS_HL2_Kar98K)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_DODS_Kar98K, CWeapon_DODS_HL2_Kar98K);
#if PRECACHE_REGISTER_DODS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_DODS_Kar98K);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_Kar98K)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_Kar98K)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_Kar98K::CWeapon_DODS_HL2_Kar98K(void)
{
}

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_M1903
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_M1903 : public CBase_CSS_HL2_SniperRifle
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_M1903, CBase_CSS_HL2_SniperRifle);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_M1903(void);

	virtual float GetViewKickBase() { return IsWeaponZoomed() ? 3.0f : 4.0f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_1DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 1.25f; }

	virtual int		GetZoom1FOV() const { return 40; }
	virtual int		GetZoom2FOV() const { return 15; }
	virtual int		GetZoomRate() const { return 0.2f; }
	virtual int		GetUnZoomRate() const { return 0.15f; }

	// Player damage: 9 -> 30.6 (30)
	// NPC damage: 4 -> 20
	virtual float GetDamageMultiplier() const { return 3.4f; }
	virtual float GetNPCDamageMultiplier() const { return 5.0f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_M1903, DT_Weapon_DODS_HL2_M1903)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_DODS_M1903, CWeapon_DODS_HL2_M1903);
#if PRECACHE_REGISTER_DODS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_DODS_M1903);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_M1903)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_M1903)
END_PREDICTION_DATA()
#endif

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_M1903::CWeapon_DODS_HL2_M1903(void)
{
}

//-----------------------------------------------------------------------------
// CWeapon_DODS_HL2_M1_Garand
//-----------------------------------------------------------------------------
class CWeapon_DODS_HL2_M1_Garand : public CBase_CSS_HL2_SniperRifle
{
public:
	DECLARE_CLASS(CWeapon_DODS_HL2_M1_Garand, CBase_CSS_HL2_SniperRifle);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_DATADESC();

	CWeapon_DODS_HL2_M1_Garand(void);
	bool Reload(void);

	virtual float GetViewKickBase() { return 2.0f; }

	virtual const Vector& GetBulletSpread(void)
	{
		static const Vector cone = VECTOR_CONE_3DEGREES;
		return cone;
	}

	virtual float GetFireRate(void) { return 0.25f; }
};

IMPLEMENT_NETWORKCLASS_DT(CWeapon_DODS_HL2_M1_Garand, DT_Weapon_DODS_HL2_M1_Garand)
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS(weapon_dods_m1_garand, CWeapon_DODS_HL2_M1_Garand);
#if PRECACHE_REGISTER_CSS_WEAPONS == 1
PRECACHE_WEAPON_REGISTER(weapon_dods_m1_garand);
#endif

BEGIN_DATADESC(CWeapon_DODS_HL2_M1_Garand)
END_DATADESC()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA(CWeapon_DODS_HL2_M1_Garand)
END_PREDICTION_DATA()
#endif

bool CWeapon_DODS_HL2_M1_Garand::Reload(void)
{
	CBaseCombatCharacter *pOwner = GetOwner();

	if (pOwner == NULL)
		return false;

	if (m_iClip1 != 0)
	{
		return false;
	}

	bool fRet = DefaultReload(GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD);
	if (fRet)
	{
		WeaponSound(RELOAD);
	}
	return fRet;
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeapon_DODS_HL2_M1_Garand::CWeapon_DODS_HL2_M1_Garand(void)
{
	m_bCanUseScope = false;
}

