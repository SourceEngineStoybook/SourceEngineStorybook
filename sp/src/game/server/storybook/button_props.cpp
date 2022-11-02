//Storybook
//

#include "cbase.h"
#include "props.h"

#define SF_BUTTONBASE_SPARK 1<<10
#define SF_BUTTONBASE_LOCKED 1<<11
#define SF_BUTTONBASE_LOCKAFTERPRESS 1<<12
//Forward decls
void DoSpark(CBaseEntity* ent, const Vector& location, int nMagnitude, int nTrailLength, bool bPlaySound, const Vector& vecDir);

//Model names for bases
const char* const szBaseModelNames[]
{
	"models/props_industrial/push_button_02/push_button_02_8x3x8.mdl",
	"models/props_industrial/push_button_01/push_button_01_6x3x10.mdl",
	"models/props_industrial/rotary_switch_01/rotary_switch_01_6x6x3.mdl",
	"models/props_industrial/rotary_key_01/rotary_key_01_8x2x8.mdl",
	"models/props_industrial/lever_01/lever_01_6x3x10.mdl",
};

const char* const szButtonModelNames[]
{
	"models/props_industrial/push_button_01/push_button_01_button_4x2x4.mdl",
	"models/props_industrial/push_button_01/push_button_01_button_4x2x4.mdl",
	"models/props_industrial/rotary_switch_01/rotary_switch_01_switch_3x3x1.mdl",
	"models/props_industrial/rotary_key_01/rotary_key_01_key_3x2x3.mdl",
	"models/props_industrial/lever_01/lever_01_lever_button_4x2x4.mdl",
};



enum PushButtonTypes_t : int
{
	PBTYPE_INVALID = -1,
	PushButton,
	PushButtonWithScreen,
	RotarySwitch,
	RotaryKey,
	Lever,
	PBTYPE_LAST,
};

enum ButtonState_t
{
	IsOut,
	IsPressingIn,
	IsPressed,
	IsPressingOut,
};

enum SparkSettings_t
{
	NoSpark,
	SparkOnPressedIn,
	SparkOnPressedOut,
	SparkOnPressedBoth,
};



//The base of the switch
class CPropButtonBase : public CBaseAnimating
{
public:
	CPropButtonBase();
	DECLARE_CLASS(CPropButtonBase, CBaseAnimating);

	DECLARE_DATADESC();
	void Spawn();
	void Precache();
	bool IsValidType();
	void InputPress(inputdata_t& inputdata);
	inline int ObjectCaps() { return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS; }
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	inline void InputLock(inputdata_t& inputdata) { AddSpawnFlags(SF_BUTTONBASE_LOCKED); }
	inline void InputUnlock(inputdata_t& inputdata) { RemoveSpawnFlags(SF_BUTTONBASE_LOCKED); }
	PushButtonTypes_t m_iButtonStyle;
	//SparkSettings_t m_iSparkSettings;
	float m_flWait;
	//bool m_bLocked;
	COutputEvent m_onPressingIn;
	COutputEvent m_onPressedIn;
	COutputEvent m_onPressingOut;
	COutputEvent m_onPressedOut;


};

//The handle/button of the switch
class CPropButtonHandle : public CPropButtonBase
{
public:
	DECLARE_CLASS(CPropButtonHandle, CPropButtonBase);

	DECLARE_DATADESC();

	ButtonState_t m_buttonState;


	void Spawn();
	bool CheckInteractable();
	int ObjectCaps();
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	CPropButtonBase* GetParent();
	void PushedInThink();
	void PushedOutThink();
	void ReturnThink();
	void InputPress();

	

	static CPropButtonHandle* Create(CPropButtonBase* parent, PushButtonTypes_t buttonType)
	{
		CPropButtonHandle* pHandle = (CPropButtonHandle*)CreateEntityByName("sb_button_handle");
		pHandle->m_iButtonStyle = buttonType;
		pHandle->SetModel(szButtonModelNames[buttonType]);
		pHandle->SetParent(parent);
		pHandle->SetLocalOrigin(vec3_origin);
		pHandle->SetLocalAngles(vec3_angle);

		switch (buttonType)
		{
		case(RotarySwitch):
			pHandle->SetLocalAngles(QAngle(0, 0, 90));
			break;
		case(PushButton):
			pHandle->SetLocalOrigin(Vector(0, 0, 2));
			break;
		case(Lever):
			pHandle->SetLocalOrigin(Vector(3, 0, 0));
			break;
		}
		return pHandle;
	}

};

void CPropButtonBase::InputPress(inputdata_t& inputdata)
{
	CPropButtonHandle* pHandle = dynamic_cast<CPropButtonHandle*>(FirstMoveChild());
	if (pHandle)
		pHandle->InputPress();
	else
		Warning("sb_button_prop %s with no handle!!", GetEntityName());
	
}
CPropButtonBase::CPropButtonBase()
{
	m_iButtonStyle = PBTYPE_INVALID;
}
void CPropButtonBase::Precache()
{
	PrecacheScriptSound("DoSpark");
	PrecacheScriptSound("Buttons.snd21");
}
void CPropButtonBase::Spawn()
{
	Precache();
	for (int i = 0; i < sizeof(szBaseModelNames) / sizeof(char*); i++)
		if (strcmp(GetModelName().ToCStr(), szBaseModelNames[i]) == 0)
		{
			m_iButtonStyle = (PushButtonTypes_t)i;
			break;
		}
	
	if (!IsValidType()) // You chose an invalid type
	{
		Warning("Invalid sb_button_prop type %i\n", m_iButtonStyle);
		m_iButtonStyle = PushButton;

	}

	PrecacheModel(szBaseModelNames[m_iButtonStyle]);
	PrecacheModel(szButtonModelNames[m_iButtonStyle]);
	SetModel(szBaseModelNames[m_iButtonStyle]);
	SetSolid(SOLID_VPHYSICS);
	CPropButtonHandle::Create(this, m_iButtonStyle);


	BaseClass::Spawn();
}

void CPropButtonBase::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	CPropButtonHandle* pHandle = (CPropButtonHandle*)FirstMoveChild();
	pHandle->InputPress();
}
bool CPropButtonBase::IsValidType()
{
	if (m_iButtonStyle <= PBTYPE_INVALID || m_iButtonStyle >= PBTYPE_LAST)
		return false;
	else return true;
}



int CPropButtonHandle::ObjectCaps()
{
	return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS;
}
bool CPropButtonHandle::CheckInteractable()
{
	if (GetParent()->HasSpawnFlags(SF_BUTTONBASE_LOCKED) || m_buttonState == IsPressingIn || m_buttonState == IsPressingOut)
		return false;
	else
		return true;
}
CPropButtonBase* CPropButtonHandle::GetParent()
{
	CPropButtonBase* pBase = dynamic_cast<CPropButtonBase*>(BaseClass::GetParent());
	if (!pBase)
	{
		Warning("sb_button_handle without sb_button_prop parent!!\n");
		UTIL_Remove(this);
	}
	 return pBase;
}


void CPropButtonHandle::InputPress()
{
	CPropButtonBase* pParent = GetParent();
	if (CheckInteractable() && pParent)
	{
		if (m_buttonState == IsOut)
		{
				m_buttonState = IsPressingIn;
				pParent->m_onPressingIn.FireOutput(this, this);
					SetThink(&CPropButtonHandle::PushedOutThink);

				SetThink(&CPropButtonHandle::PushedInThink);
				if (m_iButtonStyle <= PushButtonWithScreen)
					SetLocalVelocity(Vector(-4, 0, 0));
				else if (m_iButtonStyle == Lever)
				{
					SetLocalAngularVelocity(QAngle(360, 0, 0));
					//SetNextThink(gpGlobals->curtime + 0.5f);
				}
				else
					SetLocalAngularVelocity(QAngle(0, 0 , -180));
			
		}
		else
		{
			if (pParent->HasSpawnFlags(SF_BUTTONBASE_SPARK))
				DoSpark(this, GetAbsOrigin(), 1, 1, true, vec3_origin);
				m_buttonState = IsPressingOut;
				pParent->m_onPressingOut.FireOutput(this, this);
				SetThink(&CPropButtonHandle::PushedOutThink);
				if (m_iButtonStyle <= PushButtonWithScreen)
					SetLocalVelocity(Vector(4, 0, 0));
				else if(m_iButtonStyle == Lever)
					SetLocalAngularVelocity(QAngle(-360, 0, 0));
				else
					SetLocalAngularVelocity(QAngle(0, 0, 180));
			
		}
		if (m_iButtonStyle == Lever)
		{
			EmitSound_t ep;
			ep.m_nChannel = CHAN_VOICE;
			ep.m_pSoundName = "Buttons.snd21";
			ep.m_flVolume = 1;
			ep.m_SoundLevel = SNDLVL_NORM;
			EmitSound(CPASAttenuationFilter(this), entindex(), ep);
		}
		SetMoveType(MOVETYPE_NOCLIP);
		if(m_iButtonStyle <= PushButtonWithScreen)
			SetNextThink(gpGlobals->curtime + 0.25f);
		else
			SetNextThink(gpGlobals->curtime + 0.5f);
	}
}
void CPropButtonHandle::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	InputPress();
}
void CPropButtonHandle::Spawn()
{

	if (!IsValidType())
	{
		Warning("Invalid sb_button_handle type %i\n", m_iButtonStyle);
		m_iButtonStyle = PushButton;
	}

	
	SetSolid(SOLID_VPHYSICS);
	PrecacheModel(szButtonModelNames[m_iButtonStyle]);
	//SetModel(szButtonModelNames[m_iButtonStyle]);
	//SetUse(&CPropButtonHandle::ButtonUse);

}

void CPropButtonHandle::PushedInThink()
{
	CPropButtonBase* pParent = GetParent();
	SetLocalVelocity(vec3_origin);
	SetLocalAngularVelocity(vec3_angle);
	pParent->m_onPressedIn.FireOutput(this,this);
	m_buttonState = IsPressed;
	// if (pParent->m_iSparkSettings == SparkOnPressedIn || pParent->m_iSparkSettings == SparkOnPressedBoth)
	if (pParent->HasSpawnFlags(SF_BUTTONBASE_LOCKAFTERPRESS))
		pParent->AddSpawnFlags(SF_BUTTONBASE_LOCKED);	
	if(pParent->HasSpawnFlags(SF_BUTTONBASE_SPARK))
		DoSpark(this, GetAbsOrigin(), 1, 1, true, vec3_origin);
	if (pParent->m_flWait != -1)
	{
		SetThink(&CPropButtonHandle::ReturnThink);
		SetNextThink(gpGlobals->curtime + pParent->m_flWait);
	}
	switch (m_iButtonStyle)
	{
	case(RotarySwitch):
		SetLocalAngles(vec3_angle);
		break;
	case(RotaryKey):
		SetLocalAngles(QAngle(0, 0, -90));
		break;
	case(Lever):
		SetLocalAngles(QAngle(180, 0, 0));
	}
}

void CPropButtonHandle::ReturnThink()
{

		InputPress();
}
void CPropButtonHandle::PushedOutThink()
{
	CPropButtonBase* pParent = GetParent();
	SetLocalVelocity(vec3_origin);
	SetLocalAngularVelocity(vec3_angle);
	pParent->m_onPressedOut.FireOutput(this, this);
	m_buttonState = IsOut;

//	if (pParent->m_iSparkSettings == SparkOnPressedIn || pParent->m_iSparkSettings == SparkOnPressedBoth)
	//	DoSpark(this, GetAbsOrigin(), 1, 1, true, vec3_origin);
	SetNextThink(TICK_NEVER_THINK);

}

LINK_ENTITY_TO_CLASS(sb_button_prop, CPropButtonBase);
BEGIN_DATADESC(CPropButtonBase)
	DEFINE_KEYFIELD(m_iButtonStyle, FIELD_INTEGER, "buttonType"),
	//DEFINE_KEYFIELD(m_bLocked, FIELD_BOOLEAN, "locked"),
	DEFINE_KEYFIELD(m_flWait, FIELD_FLOAT, "wait"),
	//DEFINE_KEYFIELD(m_iSparkSettings, FIELD_INTEGER, "sparkSettings"),
	DEFINE_INPUTFUNC(FIELD_VOID,"Press", InputPress),
	DEFINE_INPUTFUNC(FIELD_VOID,"Lock", InputLock),
	DEFINE_INPUTFUNC(FIELD_VOID,"Unlock", InputUnlock),
	DEFINE_OUTPUT(m_onPressingIn, "OnPressingIn"),
	DEFINE_OUTPUT(m_onPressedIn, "OnPressedIn"),
	DEFINE_OUTPUT(m_onPressingOut, "OnPressingOut"),
	DEFINE_OUTPUT(m_onPressedOut, "OnPressedOut"),

END_DATADESC()

LINK_ENTITY_TO_CLASS(sb_button_handle, CPropButtonHandle);
BEGIN_DATADESC(CPropButtonHandle)
	DEFINE_FIELD(m_iButtonStyle, FIELD_INTEGER),
	DEFINE_FIELD(m_buttonState, FIELD_INTEGER),
	//DEFINE_FUNCTION(ButtonUse),
	DEFINE_THINKFUNC(PushedInThink	),
	DEFINE_THINKFUNC(PushedOutThink	),
	DEFINE_THINKFUNC(ReturnThink	),
END_DATADESC()

//
//Sockets
//
#define SOCKETMODEL "models/props_industrial/powersocket_01/powersocket_01_6x3x6.mdl"
#define SOCKETCAPMODEL "models/props_industrial/powersocket_01/powersocket_01_cover_4x2x6.mdl"
#define PLUGMODEL "models/props_industrial/powersocket_01/powersocket_01_plug_4x6x4.mdl"
#define WAITTIME 0.5f
enum socketState_t
{
	closed,
	open,
	occupied,
};
	

class CElectricalSocket : public CBaseAnimating
{
	DECLARE_CLASS(CElectricalSocket, CBaseAnimating);
public:
	DECLARE_DATADESC();
	void Precache();
	void Spawn();
	void SocketThink();
	inline int ObjectCaps() { return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS; }
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	socketState_t m_bSocketState; //false for closed,  true for open
	COutputEvent m_onConnected;
	COutputEvent m_onDisconnected;
};
LINK_ENTITY_TO_CLASS(sb_socket, CElectricalSocket);
BEGIN_DATADESC(CElectricalSocket)
	DEFINE_OUTPUT(m_onConnected,"OnConnected"),
	DEFINE_OUTPUT(m_onDisconnected,"OnDisconnected"),
	DEFINE_THINKFUNC(SocketThink),
	DEFINE_FIELD(m_bSocketState,FIELD_INTEGER),
END_DATADESC()


class CElectricalPlug : public CPhysicsProp
{
	DECLARE_CLASS(CElectricalPlug, CPhysicsProp);
public:
	DECLARE_DATADESC();
	void Spawn();
	int ObjectCaps();
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	bool m_bState; //false unplugged, true for plugged
};
LINK_ENTITY_TO_CLASS(sb_socket_plug, CElectricalPlug);
BEGIN_DATADESC(CElectricalPlug)
	DEFINE_FIELD(m_bState,FIELD_BOOLEAN),
END_DATADESC()

class CElectricalSocketCap : public CBaseAnimating
{
	DECLARE_CLASS(CElectricalSocketCap, CBaseAnimating);
public:
	DECLARE_DATADESC();
	bool m_bIsMoving;
	int ObjectCaps();
	void Spawn();
	void OpenThink();
	void CloseThink();
	CElectricalSocket* GetParent();
	CPointEntity* GetPivot();
	
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	static CElectricalSocketCap* Create(CElectricalSocket* pParent)
	{
		CElectricalSocketCap* pCap = (CElectricalSocketCap*)CreateEntityByName("sb_socket_cap");
		CPointEntity* pPivot = (CPointEntity*)CreateEntityByName("sb_socket_cap_pivot");
		pPivot->SetParent(pParent);
		pPivot->SetLocalOrigin(Vector(2, 0, -3));
		pPivot->SetLocalAngles(vec3_angle);
		//NDebugOverlay::Cross3D(pPivot->GetAbsOrigin(), 5, 255, 0, 0, false, 255);
		pCap->SetParent(pPivot);
		pCap->SetModel(SOCKETCAPMODEL);
		pCap->SetLocalOrigin(Vector(0, 0, 3));
		pCap->SetLocalAngles(vec3_angle);
		return pCap;
	}
};

LINK_ENTITY_TO_CLASS(sb_socket_cap, CElectricalSocketCap);
BEGIN_DATADESC(CElectricalSocketCap)
	DEFINE_THINKFUNC(OpenThink),
	DEFINE_THINKFUNC(CloseThink),
	DEFINE_FIELD(m_bIsMoving,FIELD_BOOLEAN),
END_DATADESC()


LINK_ENTITY_TO_CLASS(sb_socket_cap_pivot, CPointEntity);

int CElectricalSocketCap::ObjectCaps()
{
	return GetParent() && GetParent()->m_bSocketState == occupied ? BaseClass::ObjectCaps() : BaseClass::ObjectCaps() | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS;
}
void CElectricalSocketCap::Spawn()
{
	PrecacheModel(SOCKETCAPMODEL);
	SetModel(SOCKETCAPMODEL);
	SetSolid(SOLID_VPHYSICS);
}
void CElectricalSocketCap::OpenThink()
{
	CElectricalSocket* pParent = GetParent();
	CPointEntity* pPivot = GetPivot();
	pPivot->SetLocalAngularVelocity(vec3_angle);
	//SetLocalVelocity(vec3_origin);
	pParent->m_bSocketState = open;
	SetNextThink(TICK_NEVER_THINK);
	GetParent()->SetNextThink(0.2f);
	m_bIsMoving = false;
}
void CElectricalSocketCap::CloseThink()
{
	CElectricalSocket* pParent = GetParent();
	CPointEntity* pPivot = GetPivot();
	pPivot->SetLocalAngularVelocity(vec3_angle);
	//SetLocalVelocity(vec3_origin);
	pPivot->SetLocalAngles(vec3_angle);
	//SetLocalOrigin(Vector(2, 0, 0));
	SetNextThink(TICK_NEVER_THINK);
	pParent->m_bSocketState = closed;
	GetParent()->SetNextThink(TICK_NEVER_THINK);
	m_bIsMoving = false;
}
CPointEntity* CElectricalSocketCap::GetPivot()
{
	CPointEntity* pSocket = dynamic_cast<CPointEntity*>(BaseClass::GetParent()); //get the parent of the pivot
	if (!pSocket)
	{
		Warning("Cap with no pivot !!\n");
		UTIL_Remove(this);
	}
	return pSocket;
}
CElectricalSocket* CElectricalSocketCap::GetParent()
{
	CElectricalSocket* pSocket = dynamic_cast<CElectricalSocket*>(BaseClass::GetParent()->GetParent()); //get the parent of the pivot
	if (!pSocket)
	{
		Warning("Cap with no socket !!\n");
		//UTIL_Remove(this);
	}
	return pSocket;
}
void CElectricalSocketCap::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	if (!m_bIsMoving)
	{
		CElectricalSocket* pParent = GetParent();
		CPointEntity* pPivot = GetPivot();
		pPivot->SetMoveType(MOVETYPE_NOCLIP);
		SetNextThink(gpGlobals->curtime + 0.25f);
		m_bIsMoving = true;
		if (pParent->m_bSocketState == closed)
		{
			pPivot->SetLocalAngularVelocity(QAngle(450, 0, 0));
			//SetLocalVelocity(Vector(12, 0, -12));// Temporary
			SetThink(&CElectricalSocketCap::OpenThink);

		}
		else
		{
			pPivot->SetLocalAngularVelocity(QAngle(-450, 0, 0));
			//SetLocalVelocity(Vector(-12, 0, 12));// Temporary
			SetThink(&CElectricalSocketCap::CloseThink);
		}
	}
}
void CElectricalSocket::Precache()
{
	PrecacheScriptSound("DoSpark");
}
void CElectricalSocket::Spawn()
{
	Precache();
	PrecacheModel(SOCKETMODEL);
	PrecacheModel(SOCKETCAPMODEL);
	
	SetModel(SOCKETMODEL);
	SetSolid(SOLID_VPHYSICS);
	CElectricalSocketCap::Create(this);
	//CreateVPhysics();
	VPhysicsInitStatic();
	SetThink(&CElectricalSocket::SocketThink);
	

}

void CElectricalSocket::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	CElectricalSocketCap* pCap = dynamic_cast<CElectricalSocketCap*>(FirstMoveChild()->FirstMoveChild());
	if (pCap)
		pCap->Use(pActivator, pCaller, useType, value);
	else
		DevMsg("Socket could not pass use function onto cap!\n");
}
void CElectricalSocket::SocketThink()
{


	CElectricalSocketCap* pCap = dynamic_cast<CElectricalSocketCap*>(FirstMoveChild()->FirstMoveChild());
	if (!pCap)
		pCap = dynamic_cast<CElectricalSocketCap*>(NextMovePeer());
	if (!pCap)
	{
		//DevMsg("No socket cap found!\n");
		return;
	}
	
	CElectricalPlug* pPlug = NULL;
	CBaseEntity* pEntity;
	//NDebugOverlay::Sphere(GetAbsOrigin(), 5, 255, 0, 0, false, 1);
	for (CEntitySphereQuery pSphere(GetAbsOrigin(), 4); (pEntity = pSphere.GetCurrentEntity()) != NULL; pSphere.NextEntity())
	{
		if (pPlug = dynamic_cast<CElectricalPlug*>(pEntity))
		{
			m_bSocketState = occupied;
			pPlug->SetParent(this);
			pPlug->SetLocalOrigin(Vector(2, 0, 0));
			pPlug->SetLocalAngles(QAngle(0, 0, 0));
			//pPlug->VPhysicsDestroyObject();
			pPlug->VPhysicsGetObject()->EnableMotion(false);
			//pPlug->VPhysicsInitStatic();
			pPlug->m_bState = true;
			pPlug->SetMoveType(MOVETYPE_NONE);
			m_onConnected.FireOutput(this, this);
			SetNextThink(TICK_NEVER_THINK);
			if (HasSpawnFlags(SF_BUTTONBASE_SPARK))
				DoSpark(pPlug, pPlug->GetAbsOrigin(), 1, 1, true, vec3_origin);
			break;

		}

	}
	SetNextThink(gpGlobals->curtime + WAITTIME);
}
void CElectricalPlug::Spawn()
{
	PrecacheModel(PLUGMODEL);
	SetModel(PLUGMODEL);
	SetSolid(SOLID_VPHYSICS);
	SetMoveType(MOVETYPE_VPHYSICS);
	CreateVPhysics();

	
}
int CElectricalPlug::ObjectCaps()
{
	return BaseClass::ObjectCaps() | FCAP_IMPULSE_USE | FCAP_USE_IN_RADIUS;
} 
	
void CElectricalPlug::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	if (m_bState) // plugged in
	{
		CElectricalSocket* pSocket = dynamic_cast<CElectricalSocket*>(GetParent());
		if (!pSocket)
		{
			Assert(0);// plug is plugged in but no socket?
			return;
		}
		pSocket->m_onDisconnected.FireOutput(this,this);
		pSocket->SetNextThink(gpGlobals->curtime + WAITTIME);
		if (pSocket->HasSpawnFlags(SF_BUTTONBASE_SPARK))
			DoSpark(this, GetAbsOrigin(), 1, 1, true, vec3_origin);
		VPhysicsGetObject()->EnableMotion(true);
		SetMoveType(MOVETYPE_VPHYSICS);
		SetParent(NULL);
		//CreateVPhysics();
			
		m_bState = false;

	}
	BaseClass::Use(pActivator, pCaller, useType, value);
}



//Momentary rotary switch

#define ROTARYBASEMODEL "models/props_industrial/rotary_crank_01/rotary_crank_01_5x8x5.mdl"
#define ROTARYCRANKMODEL "models/props_industrial/rotary_crank_01/rotary_crank_01_crank_2x14x14.mdl"

class CRotaryCrankBase : public CBaseAnimating
{
	DECLARE_CLASS(CRotaryCrankBase, CBaseAnimating);
public:
	DECLARE_DATADESC();
	void Spawn();
	void Precache();
	float m_flMaxDist;
	inline void InputLock(inputdata_t& inputdata) { AddSpawnFlags(SF_BUTTONBASE_LOCKED); }
	inline void InputUnlock(inputdata_t& inputdata) { RemoveSpawnFlags(SF_BUTTONBASE_LOCKED); }
	COutputFloat m_outPosition;
	COutputEvent m_atZero;
	COutputEvent m_atFull;

};
LINK_ENTITY_TO_CLASS(sb_rotary_crank, CRotaryCrankBase);
BEGIN_DATADESC(CRotaryCrankBase)
	DEFINE_OUTPUT(m_outPosition,"Position"),
	DEFINE_OUTPUT(m_atZero,"OnFullyClosed"),
	DEFINE_OUTPUT(m_atFull,"OnFullyOpen"),
	DEFINE_KEYFIELD(m_flMaxDist,FIELD_FLOAT,"maxdistance"),
END_DATADESC()

class CRotaryCrankHandle : public CBaseAnimating
{
	DECLARE_CLASS(CRotaryCrankHandle, CBaseAnimating);
public:
	DECLARE_DATADESC();
	//void Spawn();
	//float m_flAngleCranked;
	CRotaryCrankBase* GetParent();
	float m_flLastUseTime;
	bool m_bBackwards; //false is forward, true is backwards
	void Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value);
	void StopMovingThink();
	inline int ObjectCaps() { return BaseClass::ObjectCaps() | FCAP_CONTINUOUS_USE | FCAP_USE_IN_RADIUS; }
	static CRotaryCrankHandle* Create(CBaseEntity* pOther)
	{
		CRotaryCrankHandle* pHandle = (CRotaryCrankHandle*)CreateEntityByName("sb_rotary_crank_handle");
		pHandle->SetModel(ROTARYCRANKMODEL);
		pHandle->SetParent(pOther);
		pHandle->SetSolid(SOLID_VPHYSICS);	
		pHandle->SetLocalOrigin(vec3_origin);
		pHandle->SetLocalAngles(vec3_angle);
		return pHandle;
	}
};
LINK_ENTITY_TO_CLASS(sb_rotary_crank_handle, CRotaryCrankHandle);
BEGIN_DATADESC(CRotaryCrankHandle)
	DEFINE_THINKFUNC(StopMovingThink),
END_DATADESC()

void CRotaryCrankBase::Precache()
{
	PrecacheScriptSound("Buttons.snd24");
}
void CRotaryCrankBase::Spawn()
{
	Precache();
	if (m_flMaxDist == .0f)
		m_flMaxDist = 360.0f;
	PrecacheModel(ROTARYBASEMODEL);
	PrecacheModel(ROTARYCRANKMODEL);
	SetModel(ROTARYBASEMODEL);
	CRotaryCrankHandle::Create(this);
	
}

void CRotaryCrankHandle::StopMovingThink()
{
		SetLocalAngularVelocity(vec3_angle);
		SetNextThink(TICK_NEVER_THINK);
		if (m_bBackwards)
			m_bBackwards = false;
		else
			m_bBackwards = true;

}
void CRotaryCrankHandle::Use(CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value)
{
	CRotaryCrankBase* pBase = GetParent();
	if (!pBase->HasSpawnFlags(SF_BUTTONBASE_LOCKED))
	{
		float flMaxDist = GetParent()->m_flMaxDist;
		float flAngleCranked = GetLocalAngles().z * -1;
		if (GetNextThink() == -1)
		{
			EmitSound_t ep;
			ep.m_nChannel = CHAN_VOICE;
			ep.m_pSoundName = "Buttons.snd24";
			ep.m_flVolume = 1;
			ep.m_SoundLevel = SNDLVL_NORM;
			EmitSound(CPASAttenuationFilter(this), entindex(), ep);
		}
		SetMoveType(MOVETYPE_NOCLIP);
		m_flLastUseTime = gpGlobals->curtime;
		SetThink(&CRotaryCrankHandle::StopMovingThink);

		if (m_bBackwards)
		{
			if (flAngleCranked < .0f)
			{
				pBase->m_atZero.FireOutput(this, this);
				SetLocalAngularVelocity(vec3_angle);
			}
			else
			{
				//Backwards
				DevMsg("Backwards: %f\n", flAngleCranked);
				SetLocalAngularVelocity(QAngle(0, 0, 90));
			}

		}
		else if (!m_bBackwards)
		{
			if (flAngleCranked >= flMaxDist)
			{
				pBase->m_atFull.FireOutput(this, this);
				SetLocalAngularVelocity(vec3_angle);
			}
			else
			{
				//Forwards
				DevMsg("Forwards: %f\n", flAngleCranked);
				SetLocalAngularVelocity(QAngle(0, 0, -90));
			}
		}
		
		pBase->m_outPosition.Set(flAngleCranked / flMaxDist, this, this);
		SetNextThink(gpGlobals->curtime + 0.05f);
	}



}


CRotaryCrankBase* CRotaryCrankHandle::GetParent()
{
	CRotaryCrankBase* pBase = dynamic_cast<CRotaryCrankBase*>(BaseClass::GetParent());
	if (!pBase)
	{
		Warning("sb_button_handle without sb_button_prop parent!!\n");
		UTIL_Remove(this);
	}
	return pBase;
}
	