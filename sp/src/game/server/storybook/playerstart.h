#pragma once
//=========Source Engine Storybook Dev Team===========
// 
// Purpose: Extension of info_player_start
//
//====================================================

#define MAX_NUM_PLAYERMODELS 4

#include "cbase.h"

class CPlayerStart : public CPointEntity //LYCHY: Adding ability to choose playermodels based on info_player_start keyvalues
{
public:
	DECLARE_CLASS(CPlayerStart, CPointEntity);
	DECLARE_DATADESC();

	string_t m_szPlayerModel[MAX_NUM_PLAYERMODELS];

	void Spawn() override;
};