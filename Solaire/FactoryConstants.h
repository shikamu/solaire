#pragma once
#include <path.h>


enum SHADER_TYPE
{
	SHADER_SHIP,
	SHADER_SHIELD,
	SHADER_BULLET,
	SHADER_HUD
};

enum MODULE_ID
{
	MOD_NONE,
	MOD_GUN,
	MOD_WARHEAD_LAUNCHER,
	MOD_STATIC_LAUNCHER,
	MOD_SHIELD_REGEN,
	MOD_CLOAK,
	MOD_SENSOR_JAMMER,
	MOD_ENGINE_JAMMER,
	MOD_SHIELD_JAMMER,
	MOD_TURRET_GUN,
};

enum PROJECTILE_TYPE
{
	PROJECTILE_BULLET,
	PROJECTILE_EMP,
	PROJECTILE_SLUG,
	PROJECTILE_FLECHETTE
};

enum WARHEAD_TYPE
{
	WARHEAD_ROCKET,
	WARHEAD_MISSILE,
	WARHEAD_TORPEDO,
	WARHEAD_EMP,
};

enum STATIC_TYPE
{
	STATIC_TURRET,
	STATIC_HEAL_BEACON,
	STATIC_MINE,
	STATIC_EMP_MINE
};

enum ACTUATOR_TYPE
{
	ACT_NONE,
	ACT_AI_BASIC,
	ACT_AI_ADV,
	ACT_LOCAL, 
	ACT_NET_CLIENT,
	ACT_NET_SERVER,
};

// Model Constants

const irr::io::path 	FILE_PROJECTILE_BULLET = "Models/Bullet.obj";
const irr::io::path 	FILE_PROJECTILE_EMP = "Models/Bullet.obj";
const irr::io::path 	FILE_PROJECTILE_SLUG = "Models/Slug.obj";
const irr::io::path 	FILE_PROJECTILE_FLECHETTE = "Models/Flechette.obj";

const irr::io::path 	FILE_WARHEAD_ROCKET = "Models/Rocket.obj";
const irr::io::path 	FILE_WARHEAD_MISSILE = "Models/Missile.obj";
const irr::io::path 	FILE_WARHEAD_TORPEDO = "Models/Torpedo.obj";
const irr::io::path 	FILE_WARHEAD_EMP = "Models/Rocket.obj";

const irr::io::path 	FILE_STATIC_TURRET = "Models/Turret.obj";
const irr::io::path 	FILE_STATIC_HEAL_BEACON = "Models/Beacon.obj";
const irr::io::path 	FILE_STATIC_MINE = "Models/Mine.obj";
const irr::io::path 	FILE_STATIC_EMP_MINE = "Models/Mine.obj";


const irr::io::path 	FILE_SHIP_ORION_HI = "Models/OrionHi.obj";
const irr::io::path 	FILE_SHIP_ORION_LO = "Models/Orion.obj";

const irr::io::path 	FILE_SHIP_SIRIUS_HI = "Models/SiriusHi.obj";
const irr::io::path 	FILE_SHIP_SIRIUS_LO = "Models/Sirius.obj";

const irr::io::path 	FILE_SHIP_SCOPRIUS_HI = "Models/ScorpiusHi.obj";
const irr::io::path 	FILE_SHIP_SCOPRIUS_LO = "Models/Scorpius.obj";