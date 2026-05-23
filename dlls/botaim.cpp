#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "botaim.h"

namespace
{
struct bot_aim_t
{
	edict_t* pEdict;
	Vector v_angle;
	Vector ideal_angles;
	Vector randomized_ideal_angles;
	Vector angular_deviation;
	Vector aim_speed;
};

cvar_t botaim_spring_stiffness_x = { "botaim_spring_stiffness_x", "13.0", FCVAR_EXTDLL };
cvar_t botaim_spring_stiffness_y = { "botaim_spring_stiffness_y", "13.0", FCVAR_EXTDLL };
cvar_t botaim_damper_coefficient_x = { "botaim_damper_coefficient_x", "0.22", FCVAR_EXTDLL };
cvar_t botaim_damper_coefficient_y = { "botaim_damper_coefficient_y", "0.22", FCVAR_EXTDLL };
cvar_t botaim_influence_x_on_y = { "botaim_influence_x_on_y", "0.25", FCVAR_EXTDLL };
cvar_t botaim_influence_y_on_x = { "botaim_influence_y_on_x", "0.17", FCVAR_EXTDLL };
cvar_t botaim_fix = { "botaim_fix", "1", FCVAR_EXTDLL };

bool cvars_registered = false;
bot_aim_t bots[32];

float WrapAngle(float angle)
{
	if (angle >= 180) angle -= 360 * ((int)(angle / 360) + 1);
	if (angle < -180) angle += 360 * ((int)(angle / 360) + 1);
	return angle;
}

Vector WrapAngles(Vector angles)
{
	angles.x = WrapAngle(angles.x);
	angles.y = WrapAngle(angles.y);
	angles.z = WrapAngle(angles.z);
	return angles;
}

void BotPointGun(bot_aim_t* pBot)
{
	const bool aim_fix = (CVAR_GET_FLOAT("botaim_fix") > 0.0f);
	Vector spring_stiffness(CVAR_GET_FLOAT("botaim_spring_stiffness_x"), CVAR_GET_FLOAT("botaim_spring_stiffness_y"), 0);
	Vector damper_coefficient(CVAR_GET_FLOAT("botaim_damper_coefficient_x"), CVAR_GET_FLOAT("botaim_damper_coefficient_y"), 0);
	Vector influence(CVAR_GET_FLOAT("botaim_influence_y_on_x"), CVAR_GET_FLOAT("botaim_influence_x_on_y"), 0);

	if (aim_fix)
		pBot->ideal_angles = Vector(-pBot->pEdict->v.idealpitch, pBot->pEdict->v.ideal_yaw, 0);
	else
		pBot->ideal_angles = Vector(pBot->pEdict->v.idealpitch, pBot->pEdict->v.ideal_yaw, 0);

	pBot->angular_deviation = WrapAngles(pBot->ideal_angles - pBot->v_angle);
	pBot->aim_speed.x = (spring_stiffness.x * pBot->angular_deviation.x) - (damper_coefficient.x * pBot->aim_speed.x);
	pBot->aim_speed.y = (spring_stiffness.y * pBot->angular_deviation.y) - (damper_coefficient.y * pBot->aim_speed.y);
	pBot->aim_speed.x += pBot->aim_speed.y * influence.x;
	pBot->aim_speed.y += pBot->aim_speed.x * influence.y;

	pBot->pEdict->v.v_angle = WrapAngles(pBot->v_angle + gpGlobals->frametime * Vector(pBot->aim_speed.x, pBot->aim_speed.y, 0));
	pBot->pEdict->v.angles.x = -pBot->pEdict->v.v_angle.x / 3;
	pBot->pEdict->v.angles.y = pBot->pEdict->v.v_angle.y;
	pBot->pEdict->v.angles.z = 0;
	pBot->v_angle = pBot->pEdict->v.v_angle;
}
}

void BotAimStartFrame()
{
	if (!cvars_registered)
	{
		CVAR_REGISTER(&botaim_spring_stiffness_x);
		CVAR_REGISTER(&botaim_spring_stiffness_y);
		CVAR_REGISTER(&botaim_damper_coefficient_x);
		CVAR_REGISTER(&botaim_damper_coefficient_y);
		CVAR_REGISTER(&botaim_influence_x_on_y);
		CVAR_REGISTER(&botaim_influence_y_on_x);
		CVAR_REGISTER(&botaim_fix);
		cvars_registered = true;
	}

	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{
		edict_t* pPlayer = INDEXENT(i);
		if (FNullEnt(pPlayer) || !(pPlayer->v.flags & FL_FAKECLIENT) || pPlayer->v.deadflag == DEAD_DEAD)
			continue;

		bot_aim_t* pBot = &bots[i - 1];
		pBot->pEdict = pPlayer;
		pBot->v_angle = WrapAngles(pBot->v_angle);
		BotPointGun(pBot);
	}
}
