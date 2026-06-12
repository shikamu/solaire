#pragma once

enum PropertyTypeID
{
	PROP_NONE,
	PROP_SHIELD,
	PROP_SHIELD_INTEGRITY,
	PROP_ARMOUR,
	PROP_MODULE_INTEGRITY,

	PROP_SENSOR_RANGE,
	PROP_SENSOR_SPEED,
	PROP_SENSOR_ACCURACY,
	PROP_SENSOR_SHIELDING,
	PROP_SENSOR_SIGNATURE,
	PROP_COMM_RANGE,
	PROP_MAX_TARGETS_SOFT,
	PROP_MAX_TARGETS_HARD, 

	PROP_THRUST, // 1 = Full speed fast, 0 = stationary
	PROP_MANEUVERABILITY, // 5 = VERY maneuverable, 0 = cannot move
	PROP_STABILITY,
	PROP_MASS
};

enum ModuleType
{
	MT_NONE,
	MT_OFFENSIVE,
	MT_DEFENSIVE, 
	MT_ELECTRONIC,
	MT_UTILITY
};

enum ModuleSize
{
	MS_ZERO, 
	MS_SMALL,
	MS_MEDIUM,
	MS_LARGE 
};

//enum ActivationType
//{
//	ACT_NONE,
//	ACT_PRIMARY,
//	ACT_SECONDARY,
//	ACT_TERTIARY,
//	ACT_COLLISION,
//	ACT_OTHER
//};

enum LOCK_TYPE
{
	LOCK_NONE,
	LOCK_SOFT,
	LOCK_HARD
};

enum AGENT_TYPE
{
	AGENT_LOCAL,
	AGENT_REMOTE, 
	AGENT_AI_LOCAL,
	AGENT_AI_REMOTE
};


const int LIGHT_ID = 99;
const int SHIP_OUTLINE_ID = 666;


// Mask Constants
/////////////////////////

const unsigned int MASK_RESERVED = 1;
const unsigned int MASK_PROJECTILE = 1 << 1;
const unsigned int MASK_WARHEAD = 1 << 2;
const unsigned int MASK_SHIP = 1 << 3;
const unsigned int MASK_STATIC = 1 << 4;
const unsigned int MASK_INANIMATE = 1 << 5;



const unsigned int MASK_GROUP_1 = 1 << 16;
const unsigned int MASK_GROUP_2 = 1 << 17;
const unsigned int MASK_GROUP_3 = 1 << 18;
const unsigned int MASK_GROUP_4 = 1 << 19;

const unsigned int GROUP_FILTER = MASK_GROUP_1 | MASK_GROUP_2 | MASK_GROUP_3 | MASK_GROUP_4; 

// Activation Mask
//////////////////////////

const unsigned short ACT_RESERVED = 1;
const unsigned short ACT_PRIMARY = 1 << 1;
const unsigned short ACT_SECONDARY = 1 << 2;
const unsigned short ACT_TERTIARY = 1 << 3;
const unsigned short ACT_COLLISION = 1 << 4;

// Input Mask
//////////////////////////

const unsigned int INPUT_PRIMARY = 1;
const unsigned int INPUT_SECONDARY = 1 << 1;
const unsigned int INPUT_TERTIARY = 1 << 2; 
const unsigned int INPUT_NEXT = 1 << 3; 
const unsigned int INPUT_PREV = 1 << 4; 
const unsigned int INPUT_FRONT = 1 << 5; 
const unsigned int INPUT_SCORES = 1 << 6; 
const unsigned int INPUT_PAUSE = 1 << 7;

// Target Type Constants
//////////////////////////

const char TARGET_NONE = 0; 
const char TARGET_SOFT = 1;  
const char TARGET_HARD = 2;


enum MaskComparison
{
	MASK_IGNORE,
	MASK_COLLISION,
	MASK_LOGIC, 
	MASK_COLLISION_LOGIC
};
