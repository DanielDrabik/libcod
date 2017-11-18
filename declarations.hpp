#ifndef _DECLARATIONS_HPP_
#define _DECLARATIONS_HPP_

#define qboolean int
#define qtrue 1
#define qfalse 0

#define DotProduct(x,y)			((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c)	((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])
#define VectorAdd(a,b,c)		((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])
#define VectorCopy(a,b)			((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])
#define	VectorScale(v, s, o)	((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))
#define	VectorMA(v, s, b, o)	((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))

#define MAX_CLIENTS 64
#define PACKET_BACKUP 32
#define MAX_QPATH 64
#define MAX_OSPATH 256
#define FRAMETIME 50

typedef unsigned char byte;
typedef struct gclient_s gclient_t;
typedef struct gentity_s gentity_t;
typedef int scr_entref_t;

enum svc_ops_e
{
	svc_nop,
	svc_gamestate,
	svc_configstring,
	svc_baseline,
	svc_serverCommand,
	svc_download,
	svc_snapshot,
	svc_EOF
};

typedef enum
{
	TEAM_FREE,
	TEAM_RED,
	TEAM_BLUE,
	TEAM_SPEC,
	TEAM_NUM_TEAMS
} team_t;

typedef enum
{
	CS_FREE,
	CS_ZOMBIE,
	CS_CONNECTED,
	CS_PRIMED,
	CS_ACTIVE
} clientState_t;

typedef enum
{
	STATE_PLAYING,
	STATE_DEAD,
	STATE_SPECTATOR,
	STATE_INTERMISSION
} sessionState_t;

typedef enum
{
	CON_DISCONNECTED,
	CON_CONNECTING,
	CON_CONNECTED
} clientConnected_t;

typedef enum
{
	NA_BOT = 0,
	NA_BAD = 0,
	NA_LOOPBACK = 2,
	NA_BROADCAST = 3,
	NA_IP = 4,
	NA_IPX = 5,
	NA_BROADCAST_IPX = 6
} netadrtype_t;

typedef struct
{
	netadrtype_t type;
	byte ip[4];
	byte ipx[10];
	unsigned short port;
} netadr_t;

typedef enum
{
	NS_CLIENT,
	NS_SERVER
} netsrc_t;

typedef struct
{
	qboolean overflowed;
	byte *data;
	int maxsize;
	int cursize;
	int readcount;
	int bit;
} msg_t;

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];
typedef vec_t vec5_t[5];

typedef union
{
	int i;
	byte rgba[4];
} ucolor_t;

typedef struct cvar_s
{
	char *name;
	char *description;
	short int flags;
	byte type;
	byte modified;
	union
	{
		float floatval;
		float value;
		int integer;
		char* string;
		byte boolean;
		vec2_t vec2;
		vec3_t vec3;
		vec4_t vec4;
		ucolor_t color;
	};
	union
	{
		float latchedFloatval;
		int latchedInteger;
		char* latchedString;
		byte latchedBoolean;
		vec2_t latchedVec2;
		vec3_t latchedVec3;
		vec4_t latchedVec4;
		ucolor_t latchedColor;
	};
	union
	{
		float resetFloatval;
		int resetInteger;
		char* resetString;
		byte resetBoolean;
		vec2_t resetVec2;
		vec3_t resetVec3;
		vec4_t resetVec4;
		ucolor_t resetColor;
	};
	union
	{
		int imin;
		float fmin;
	};
	union
	{
		int imax;
		float fmax;
		const char** enumStr;
	};
	struct cvar_s *next;
	struct cvar_s *hashNext;
} cvar_t;

#define	CVAR_ARCHIVE		1
#define	CVAR_USERINFO		2
#define	CVAR_SERVERINFO		4
#define	CVAR_SYSTEMINFO		8
#define	CVAR_INIT			16
#define	CVAR_LATCH			32
#define	CVAR_ROM			64
#define CVAR_CHEAT			128
#define	CVAR_TEMP			256
#define CVAR_NORESTART		1024
#define	CVAR_USER_CREATED	16384

struct VariableStackBuffer
{
	const char *pos;
	u_int16_t size;
	u_int16_t bufLen;
	u_int16_t localId;
	char time;
	char buf[1];
};

union VariableUnion
{
	int intValue;
	float floatValue;
	unsigned int stringValue;
	const float *vectorValue;
	const char *codePosValue;
	unsigned int pointerValue;
	struct VariableStackBuffer *stackValue;
	unsigned int entityOffset;
};

union ObjectInfo_u
{
	u_int16_t size;
	u_int16_t entnum;
	u_int16_t nextEntId;
	u_int16_t self;
};

struct ObjectInfo
{
	u_int16_t refCount;
	union ObjectInfo_u u;
};

union VariableValueInternal_u
{
	u_int16_t next;
	union VariableUnion u;
	struct ObjectInfo o;
};

union VariableValueInternal_w
{
	unsigned int status;
	unsigned int type;
	unsigned int name;
	unsigned int classnum;
	unsigned int notifyName;
	unsigned int waitTime;
	unsigned int parentLocalId;
};

union VariableValueInternal_v
{
	u_int16_t next;
	u_int16_t index;
};

typedef struct
{
	union VariableUnion u;
	int type;
} VariableValue;

union Variable_u
{
	u_int16_t prev;
	u_int16_t prevSibling;
};

struct Variable
{
	u_int16_t id;
	union Variable_u u;
};

typedef struct
{
	struct Variable hash;
	union VariableValueInternal_u u;
	union VariableValueInternal_w w;
	union VariableValueInternal_v v;
	u_int16_t nextSibling;
} VariableValueInternal;

typedef struct
{
	const char *fieldBuffer;
	u_int16_t canonicalStrCount;
	byte developer;
	byte developer_script;
	byte evaluate;
	byte pad[3];
	const char *error_message;
	int error_index;
	unsigned int time;
	unsigned int timeArrayId;
	unsigned int pauseArrayId;
	unsigned int levelId;
	unsigned int gameId;
	unsigned int animId;
	unsigned int freeEntList;
	unsigned int tempVariable;
	byte bInited;
	byte pad2;
	u_int16_t savecount;
	unsigned int checksum;
	unsigned int entId;
	unsigned int entFieldName;
	struct HunkUser *programHunkUser;
	const char *programBuffer;
	const char *endScriptBuffer;
	u_int16_t saveIdMap[24574];
	u_int16_t saveIdMapRev[24574];
} scrVarPub_t;

struct function_stack_t
{
	const char *pos;
	unsigned int localId;
	unsigned int localVarCount;
	VariableValue *top;
	VariableValue *startTop;
};

struct function_frame_t
{
	struct function_stack_t fs;
	int topType;
};

typedef struct
{
	unsigned int *localVars;
	VariableValue *maxstack;
	int function_count;
	struct function_frame_t *function_frame;
	VariableValue *top;
	byte debugCode;
	byte abort_on_error;
	byte terminal_error;
	byte pad;
	unsigned int inparamcount;
	unsigned int outparamcount;
	struct function_frame_t function_frame_start[32];
	VariableValue stack[2048];
} scrVmPub_t;

typedef int	fileHandle_t;
typedef void (*xfunction_t)();
typedef void (*xmethod_t)(scr_entref_t);

typedef struct scr_function_s
{
	const char      *name;
	xfunction_t     call;
	qboolean        developer;
} scr_function_t;

typedef struct scr_method_s
{
	const char     *name;
	xmethod_t      call;
	qboolean       developer;
} scr_method_t;

typedef enum
{
	EV_NONE = 0,
	EV_FOOTSTEP_RUN_DEFAULT,
	EV_FOOTSTEP_RUN_BARK,
	EV_FOOTSTEP_RUN_BRICK,
	EV_FOOTSTEP_RUN_CARPET,
	EV_FOOTSTEP_RUN_CLOTH,
	EV_FOOTSTEP_RUN_CONCRETE,
	EV_FOOTSTEP_RUN_DIRT,
	EV_FOOTSTEP_RUN_FLESH,
	EV_FOOTSTEP_RUN_FOLIAGE,
	EV_FOOTSTEP_RUN_GLASS,
	EV_FOOTSTEP_RUN_GRASS,
	EV_FOOTSTEP_RUN_GRAVEL,
	EV_FOOTSTEP_RUN_ICE,
	EV_FOOTSTEP_RUN_METAL,
	EV_FOOTSTEP_RUN_MUD,
	EV_FOOTSTEP_RUN_PAPER,
	EV_FOOTSTEP_RUN_PLASTER,
	EV_FOOTSTEP_RUN_ROCK,
	EV_FOOTSTEP_RUN_SAND,
	EV_FOOTSTEP_RUN_SNOW,
	EV_FOOTSTEP_RUN_WATER,
	EV_FOOTSTEP_RUN_WOOD,
	EV_FOOTSTEP_RUN_ASPHALT,
	EV_FOOTSTEP_WALK_DEFAULT,
	EV_FOOTSTEP_WALK_BARK,
	EV_FOOTSTEP_WALK_BRICK,
	EV_FOOTSTEP_WALK_CARPET,
	EV_FOOTSTEP_WALK_CLOTH,
	EV_FOOTSTEP_WALK_CONCRETE,
	EV_FOOTSTEP_WALK_DIRT,
	EV_FOOTSTEP_WALK_FLESH,
	EV_FOOTSTEP_WALK_FOLIAGE,
	EV_FOOTSTEP_WALK_GLASS,
	EV_FOOTSTEP_WALK_GRASS,
	EV_FOOTSTEP_WALK_GRAVEL,
	EV_FOOTSTEP_WALK_ICE,
	EV_FOOTSTEP_WALK_METAL,
	EV_FOOTSTEP_WALK_MUD,
	EV_FOOTSTEP_WALK_PAPER,
	EV_FOOTSTEP_WALK_PLASTER,
	EV_FOOTSTEP_WALK_ROCK,
	EV_FOOTSTEP_WALK_SAND,
	EV_FOOTSTEP_WALK_SNOW,
	EV_FOOTSTEP_WALK_WATER,
	EV_FOOTSTEP_WALK_WOOD,
	EV_FOOTSTEP_WALK_ASPHALT,
	EV_FOOTSTEP_PRONE_DEFAULT,
	EV_FOOTSTEP_PRONE_BARK,
	EV_FOOTSTEP_PRONE_BRICK,
	EV_FOOTSTEP_PRONE_CARPET,
	EV_FOOTSTEP_PRONE_CLOTH,
	EV_FOOTSTEP_PRONE_CONCRETE,
	EV_FOOTSTEP_PRONE_DIRT,
	EV_FOOTSTEP_PRONE_FLESH,
	EV_FOOTSTEP_PRONE_FOLIAGE,
	EV_FOOTSTEP_PRONE_GLASS,
	EV_FOOTSTEP_PRONE_GRASS,
	EV_FOOTSTEP_PRONE_GRAVEL,
	EV_FOOTSTEP_PRONE_ICE,
	EV_FOOTSTEP_PRONE_METAL,
	EV_FOOTSTEP_PRONE_MUD,
	EV_FOOTSTEP_PRONE_PAPER,
	EV_FOOTSTEP_PRONE_PLASTER,
	EV_FOOTSTEP_PRONE_ROCK,
	EV_FOOTSTEP_PRONE_SAND,
	EV_FOOTSTEP_PRONE_SNOW,
	EV_FOOTSTEP_PRONE_WATER,
	EV_FOOTSTEP_PRONE_WOOD,
	EV_FOOTSTEP_PRONE_ASPHALT,
	EV_JUMP_DEFAULT,
	EV_JUMP_BARK,
	EV_JUMP_BRICK,
	EV_JUMP_CARPET,
	EV_JUMP_CLOTH,
	EV_JUMP_CONCRETE,
	EV_JUMP_DIRT,
	EV_JUMP_FLESH,
	EV_JUMP_FOLIAGE,
	EV_JUMP_GLASS,
	EV_JUMP_GRASS,
	EV_JUMP_GRAVEL,
	EV_JUMP_ICE,
	EV_JUMP_METAL,
	EV_JUMP_MUD,
	EV_JUMP_PAPER,
	EV_JUMP_PLASTER,
	EV_JUMP_ROCK,
	EV_JUMP_SAND,
	EV_JUMP_SNOW,
	EV_JUMP_WATER,
	EV_JUMP_WOOD,
	EV_JUMP_ASPHALT,
	EV_LANDING_DEFAULT,
	EV_LANDING_BARK,
	EV_LANDING_BRICK,
	EV_LANDING_CARPET,
	EV_LANDING_CLOTH,
	EV_LANDING_CONCRETE,
	EV_LANDING_DIRT,
	EV_LANDING_FLESH,
	EV_LANDING_FOLIAGE,
	EV_LANDING_GLASS,
	EV_LANDING_GRASS,
	EV_LANDING_GRAVEL,
	EV_LANDING_ICE,
	EV_LANDING_METAL,
	EV_LANDING_MUD,
	EV_LANDING_PAPER,
	EV_LANDING_PLASTER,
	EV_LANDING_ROCK,
	EV_LANDING_SAND,
	EV_LANDING_SNOW,
	EV_LANDING_WATER,
	EV_LANDING_WOOD,
	EV_LANDING_ASPHALT,
	EV_LANDING_PAIN_DEFAULT,
	EV_LANDING_PAIN_BARK,
	EV_LANDING_PAIN_BRICK,
	EV_LANDING_PAIN_CARPET,
	EV_LANDING_PAIN_CLOTH,
	EV_LANDING_PAIN_CONCRETE,
	EV_LANDING_PAIN_DIRT,
	EV_LANDING_PAIN_FLESH,
	EV_LANDING_PAIN_FOLIAGE,
	EV_LANDING_PAIN_GLASS,
	EV_LANDING_PAIN_GRASS,
	EV_LANDING_PAIN_GRAVEL,
	EV_LANDING_PAIN_ICE,
	EV_LANDING_PAIN_METAL,
	EV_LANDING_PAIN_MUD,
	EV_LANDING_PAIN_PAPER,
	EV_LANDING_PAIN_PLASTER,
	EV_LANDING_PAIN_ROCK,
	EV_LANDING_PAIN_SAND,
	EV_LANDING_PAIN_SNOW,
	EV_LANDING_PAIN_WATER,
	EV_LANDING_PAIN_WOOD,
	EV_LANDING_PAIN_ASPHALT,
	EV_FOLIAGE_SOUND,
	EV_STANCE_FORCE_STAND,
	EV_STANCE_FORCE_CROUCH,
	EV_STANCE_FORCE_PRONE,
	EV_STEP_VIEW,
	EV_ITEM_PICKUP,
	EV_AMMO_PICKUP,
	EV_NOAMMO,
	EV_EMPTYCLIP,
	EV_EMPTY_OFFHAND,
	EV_RESET_ADS,
	EV_RELOAD,
	EV_RELOAD_FROM_EMPTY,
	EV_RELOAD_START,
	EV_RELOAD_END,
	EV_RAISE_WEAPON,
	EV_PUTAWAY_WEAPON,
	EV_WEAPON_ALT,
	EV_PULLBACK_WEAPON,
	EV_FIRE_WEAPON,
	EV_FIRE_WEAPONB,
	EV_FIRE_WEAPON_LASTSHOT,
	EV_RECHAMBER_WEAPON,
	EV_EJECT_BRASS,
	EV_MELEE_SWIPE,
	EV_FIRE_MELEE,
	EV_PREP_OFFHAND,
	EV_USE_OFFHAND,
	EV_SWITCH_OFFHAND,
	EV_BINOCULAR_ENTER,
	EV_BINOCULAR_EXIT,
	EV_BINOCULAR_FIRE,
	EV_BINOCULAR_RELEASE,
	EV_BINOCULAR_DROP,
	EV_MELEE_HIT,
	EV_MELEE_MISS,
	EV_FIRE_WEAPON_MG42,
	EV_FIRE_QUADBARREL_1,
	EV_FIRE_QUADBARREL_2,
	EV_BULLET_TRACER,
	EV_SOUND_ALIAS,
	EV_SOUND_ALIAS_AS_MASTER,
	EV_BULLET_HIT_SMALL,
	EV_BULLET_HIT_LARGE,
	EV_SHOTGUN_HIT,
	EV_BULLET_HIT_AP,
	EV_BULLET_HIT_CLIENT_SMALL,
	EV_BULLET_HIT_CLIENT_LARGE,
	EV_GRENADE_BOUNCE,
	EV_GRENADE_EXPLODE,
	EV_ROCKET_EXPLODE,
	EV_ROCKET_EXPLODE_NOMARKS,
	EV_CUSTOM_EXPLODE,
	EV_CUSTOM_EXPLODE_NOMARKS,
	EV_BULLET,
	EV_PLAY_FX,
	EV_PLAY_FX_ON_TAG,
	EV_EARTHQUAKE,
	EV_GRENADE_SUICIDE,
	EV_OBITUARY
} entity_event_t;

typedef enum
{
	TRACE_HITTYPE_NONE = 0x0,
	TRACE_HITTYPE_ENTITY = 0x1,
	TRACE_HITTYPE_DYNENT_MODEL = 0x2,
	TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
	TRACE_HITTYPE_GLASS = 0x4
} TraceHitType;

typedef struct trace_s
{
	float fraction;
	vec3_t normal;
	int surfaceFlags;
	int contents;
	const char *material;
	TraceHitType hitType;
	u_int16_t hitId;
	u_int16_t modelIndex;
	u_int16_t partName;
	u_int16_t partGroup;
	byte allsolid;
	byte startsolid;
	byte walkable;
	byte padding;
} trace_t;

typedef struct leakyBucket_s leakyBucket_t;
struct leakyBucket_s
{
	netadrtype_t type;
	unsigned char adr[4];
	int	lastTime;
	signed char	burst;
	long hash;
	leakyBucket_t *prev, *next;
};

typedef struct usercmd_s
{
	int serverTime;
	int buttons;
	byte weapon;
	byte offHandIndex;
	int angles[3];
	char forwardmove;
	char rightmove;
} usercmd_t;

#if COD_VERSION == COD2_1_0 || COD_VERSION == COD2_1_2
#define MAX_MSGLEN 0x4000
#elif COD_VERSION == COD2_1_3
#define MAX_MSGLEN 0x20000
#endif

typedef void netProfileInfo_t;

typedef struct
{
	int			outgoingSequence;
	netsrc_t	sock;
	int			dropped;
	int			incomingSequence;
	netadr_t	remoteAddress;
	int 		qport;
	int			fragmentSequence;
	int			fragmentLength;
	byte		fragmentBuffer[MAX_MSGLEN];
	qboolean	unsentFragments;
	int			unsentFragmentStart;
	int			unsentLength;
	byte		unsentBuffer[MAX_MSGLEN];
	netProfileInfo_t *netProfile;
} netchan_t;

typedef struct
{
	char command[1024];
	int cmdTime;
	int cmdType;
} reliableCommands_t;

typedef struct
{
	netadr_t adr;
	int challenge;
	int time;
	int pingTime;
	int firstTime;
	int firstPing;
	qboolean connected;
	int guid;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	char pbguid[64];
	int ipAuthorize;
#endif
} challenge_t; // verified for 1.0, guessed for 1.2 and 1.3

typedef enum
{
	TR_STATIONARY,
	TR_INTERPOLATE,
	TR_LINEAR,
	TR_LINEAR_STOP,
	TR_SINE,
	TR_GRAVITY
} trType_t;

typedef struct
{
	trType_t	trType;
	int			trTime;
	int			trDuration;
	vec3_t		trBase;
	vec3_t		trDelta;
} trajectory_t;

typedef struct entityState_s
{
	int number;
	int eType;
	int eFlags;
	trajectory_t pos;
	trajectory_t apos;
	int time;
	int time2;
	vec3_t origin;
	vec3_t angles;
	int otherEntityNum;
	int attackerEntityNum;
	int groundEntityNum;
	int constantLight;
	int loopSound;
	int surfaceFlags;
	int modelindex;
	int clientNum;
	int	iHeadIcon;
	int	iHeadIconTeam;
	int	solid;
	int	eventParm;
	int	eventSequence;
	vec4_t events;
	vec4_t eventParms;
	int weapon;
	int legsAnim;
	int torsoAnim;
	int stage;
	int loopfxid;
	int hintstring;
	int animMovetype;
	vec3_t unkAngles;
} entityState_t; // verified

typedef struct
{
	byte		linked;
	byte		bmodel;
	byte		svFlags;
	byte		pad1;
	int			clientMask[2];
	byte		inuse;
	byte		pad2[3];
	int			broadcastTime;
	vec3_t		mins, maxs;
	int			contents;
	vec3_t		absmin, absmax;
	vec3_t		currentOrigin;
	vec3_t		currentAngles;
	u_int16_t	ownerNum;
	u_int16_t	pad3;
	int			eventTime;
} entityShared_t; // verified

typedef enum
{
	STAT_HEALTH,
	STAT_DEAD_YAW,
	STAT_MAX_HEALTH,
	STAT_FRIENDLY_LOOKAT_CLIENTNUM,
	STAT_FRIENDLY_LOOKAT_HEALTH,
	STAT_SPAWN_COUNT
} statIndex_t;

typedef enum
{
	PLAYERVIEWLOCK_NONE = 0x0,
	PLAYERVIEWLOCK_FULL = 0x1,
	PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
	PLAYERVIEWLOCKCOUNT = 0x3,
} ViewLockTypes_t;

typedef enum
{
	OBJST_EMPTY = 0x0,
	OBJST_ACTIVE = 0x1,
	OBJST_INVISIBLE = 0x2,
	OBJST_DONE = 0x3,
	OBJST_CURRENT = 0x4,
	OBJST_FAILED = 0x5,
	OBJST_NUMSTATES = 0x6,
} objectiveState_t;

typedef struct objective_s
{
	objectiveState_t state;
	vec3_t origin;
	int entNum;
	int teamNum;
	int icon;
} objective_t;

typedef enum
{
	HE_TYPE_FREE = 0x0,
	HE_TYPE_TEXT = 0x1,
	HE_TYPE_VALUE = 0x2,
	HE_TYPE_PLAYERNAME = 0x3,
	HE_TYPE_MAPNAME = 0x4,
	HE_TYPE_GAMETYPE = 0x5,
	HE_TYPE_MATERIAL = 0x6,
	HE_TYPE_TIMER_DOWN = 0x7,
	HE_TYPE_TIMER_UP = 0x8,
	HE_TYPE_TENTHS_TIMER_DOWN = 0x9,
	HE_TYPE_TENTHS_TIMER_UP = 0xA,
	HE_TYPE_CLOCK_DOWN = 0xB,
	HE_TYPE_CLOCK_UP = 0xC,
	HE_TYPE_WAYPOINT = 0xD,
	HE_TYPE_COUNT = 0xE,
} he_type_t;

typedef struct
{
	char r;
	char g;
	char b;
	char a;
} hudelem_colorsplit_t;

typedef union
{
	hudelem_colorsplit_t split;
	int rgba;
} hudelem_color_t;

typedef struct hudelem_s
{
	he_type_t type;
	float x;
	float y;
	float z;
	float fontScale;
	int font;
	int alignOrg;
	int alignScreen;
	hudelem_color_t color;
	hudelem_color_t fromColor;
	int fadeStartTime;
	int fadeTime;
	int label;
	int width;
	int height;
	int materialIndex;
	int fromWidth;
	int fromHeight;
	int scaleStartTime;
	int scaleTime;
	float fromX;
	float fromY;
	int fromAlignOrg;
	int fromAlignScreen;
	int moveStartTime;
	int moveTime;
	int time;
	int duration;
	float value;
	int text;
	float sort;
	hudelem_color_t glowColor;
} hudelem_t; // verified

typedef struct hudElemState_s
{
	hudelem_t current[31];
	hudelem_t archival[31];
} hudElemState_t;

typedef struct
{
	float	yaw;
	int	timer;
	int	transIndex;
	int	flags;
} mantleState_t;

typedef struct playerState_s
{
	int	commandTime;
	int pm_type;
	int bobCycle;
	int pm_flags;
	int pm_time;
	vec3_t origin;
	vec3_t velocity;
	vec2_t oldVelocity; // 48
	int	weaponTime;
	int weaponDelay;
	int	grenadeTimeLeft;
	int	throwBackGrenadeOwner;
	int	throwBackGrenadeTimeLeft;
	int	gravity;
	float leanf;
	int speed;
	vec3_t delta_angles;
	int groundEntityNum;
	vec3_t vLadderVec;
	int jumpTime;
	float jumpOriginZ;
	int legsTime;
	int legsAnim;
	int torsoTime;
	int torsoAnim;
	int	legsAnimDuration;
	int	torsoAnimDuration;
	int	damageTimer;
	int	damageDuration;
	int	flinchYawAnim;
	int	movementDir;
	int	eFlags;
	int	eventSequence;
	int events[4];
	unsigned int eventParms[4];
	int	oldEventSequence;
	int	clientNum;
	int	offHandIndex;
	unsigned int weapon;
	int	weaponstate;
	float fWeaponPosFrac;
	int adsDelayTime;
	int	viewmodelIndex;
	vec3_t viewangles;
	int viewHeightTarget;
	float viewHeightCurrent;
	int viewHeightLerpTime;
	int viewHeightLerpTarget;
	int viewHeightLerpDown;
	int unknown[5];
	int	damageEvent;
	int	damageYaw;
	int	damagePitch;
	int	damageCount;
	int	stats[6];
	int	ammo[128];
	int	ammoclip[128]; // 836
	unsigned int weapons[4]; // 1348
	unsigned int weaponsold[4];
	unsigned int weaponrechamber[2]; // ?
	vec3_t mins;
	vec3_t maxs;
	float proneDirection;
	float proneDirectionPitch;
	float proneTorsoPitch;
	ViewLockTypes_t viewlocked;
	int viewlocked_entNum;
	int	cursorHint;
	int	cursorHintString;
	int	cursorHintEntIndex;
	int unknown1;
	vec3_t unkAngles;
	float holdBreathScale;
	int holdBreathTimer;
	mantleState_t mantleState;
	int entityEventSequence;
	int	weapAnim;
	float aimSpreadScale;
	int	shellshockIndex;
	int	shellshockTime;
	int	shellshockDuration;
	objective_t objective[16];
	int archiveTime;
	hudElemState_t hud;
} playerState_t;

typedef enum
{
	TEAM_NONE,
	TEAM_AXIS,
	TEAM_ALLIES,
	TEAM_SPECTATOR
} sessionTeam_t;

typedef struct
{
	sessionState_t state;
	int forceSpectatorClient;
	int statusIcon;
	int archiveTime;
	int	score;
	int deaths;
	u_int16_t scriptPersId;
	byte pad2;
	byte pad;
	clientConnected_t connected;
	usercmd_t cmd;
	usercmd_t oldcmd;
	qboolean localClient;
	qboolean predictItemPickup;
	char name[32];
	int maxHealth;
	int enterTime;
	int voteCount;
	int teamVoteCount;
	float moveSpeedScaleMultiplier; // ?
	int viewmodelIndex;
	qboolean noSpectate;
	int teamInfo;
	int clientId;
	sessionTeam_t team;
	int model;
	int attachedModels[6];
	int attachedModelsTags[6];
	char manualModeName[32];
	int psOffsetTime;
} clientSession_t; // verified

struct gclient_s
{
	playerState_t ps;
	clientSession_t sess;
	int spectatorClient;
	qboolean noclip;
	qboolean ufo;
	qboolean bFrozen;
	int lastCmdTime;
	int buttons;
	int oldbuttons;
	int latched_buttons;
	int buttonsSinceLastFrame;
	vec3_t oldOrigin;
	float fGunPitch;
	float fGunYaw;
	int damage_blood;
	vec3_t damage_from;
	qboolean damage_fromWorld;
	int accurateCount; // N/A
	int accuracy_shots; // N/A
	int accuracy_hits; // N/A
	int inactivityTime;
	qboolean inactivityWarning;
	int playerTalkTime;
	int rewardTime; // N/A
	float antilagShootTime; // 10256
	int unknown_space[2];
	int unknownClientEndFrameVar;
	int unknown_space2[3];
	gentity_t *lookAtEntity; // needs a NULL check, otherwise crash.
	int activateEntNumber;
	int activateTime;
	int nonPVSFriendlyEntNum;
	int pingPlayerTime;
	int damageFeedBackTime;
	vec2_t damageFeedBackDir;
	vec3_t swayViewAngles; // 10316
	vec3_t swayOffset;
	vec3_t swayAngles;
	int unknown_space3[7];
	float weaponRecoil; // 10380
	int unknown_space4[3];
	int lastServerTime;
	int lastActivateTime;
}; // verified

typedef int turretInfo_s;

struct gentity_s
{
	entityState_t s;
	entityShared_t r;
	struct gclient_s *client;
	turretInfo_s *pTurretInfo;
	byte physicsObject; // 352
	byte takedamage;
	byte active;
	byte nopickup;
	byte model;
	byte team;
	byte handler;
	byte pad;
	u_int16_t classname; // 360
	u_int16_t target;
	u_int16_t targetname;
	int spawnflags;
	int flags;
	int eventTime;
	qboolean freeAfterEvent; // 380
	qboolean unlinkAfterEvent;
	int clipmask;
	int realClipmask;
	int realContents; // 396
	int nextthink; // 400
	int healthPoints;
	int unknown;
	int damage; // 412
	int unknown2;
	int unknown3;
	float physicsBounce; // 424
	u_int16_t item; // 428 This is item id, not item pointer!
	int hurtTouchTime;
	int useSharedNum;
	int attackerWeapon; // 440 ?
	int unknown11[11];
	int playerStatePrediction; // ?
	vec3_t lerpOrigin; // 492
	vec3_t lerpAngles; // 504
	vec3_t moverOrigin; // 516
	byte attachedModels[6]; // 528
	u_int16_t attachedTagName; // 536 ?
	u_int16_t numAttachedModels; // 538 ?
	int animTree; // 540 ?
	vec4_t color; // ?
}; // verified

#define MAX_DOWNLOAD_BLKSIZE 1024
#define MAX_DOWNLOAD_WINDOW 8

typedef struct
{
	playerState_t ps;
	int	num_entities;
	int	num_clients;
	int	first_entity;
	int	first_client;
	unsigned int messageSent;
	unsigned int messageAcked;
	int	messageSize;
} clientSnapshot_t;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
	char num;
	char data[256];
	int dataLen;
} voices_t;
#pragma pack(pop)

typedef struct client_s
{
	clientState_t	state;
	int				unksnapshotvar;
	int				unksnapshotvar2;
	char			userinfo[1024];
	reliableCommands_t	reliableCommands[128];
	int				reliableSequence;
	int				reliableAcknowledge;
	int				reliableSent;
	int				messageAcknowledge;
	int				gamestateMessageNum;
	int				challenge;
	usercmd_t  	 	lastUsercmd;
	int				lastClientCommand;
	char			lastClientCommandString[1024];
	gentity_t 		*gentity;
	char 			name[32];
	char			downloadName[MAX_QPATH];
	fileHandle_t	download;
	int				downloadSize;
	int				downloadCount;
	int				downloadClientBlock;
	int				downloadCurrentBlock;
	int				downloadXmitBlock;
	unsigned char	*downloadBlocks[MAX_DOWNLOAD_WINDOW];
	int				downloadBlockSize[MAX_DOWNLOAD_WINDOW];
	qboolean		downloadEOF;
	int				downloadSendTime;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	char			wwwDownloadURL[MAX_OSPATH];
	qboolean		wwwDownload;
	qboolean		wwwDownloadStarted;
	qboolean		wwwDlAck;
	qboolean		wwwDl_failed;
#endif
	int				deltaMessage;
	int				floodprotect;
	int				lastPacketTime;
	int				lastConnectTime;
	int				nextSnapshotTime;
	qboolean		rateDelayed;
	int				timeoutCount;
	clientSnapshot_t frames[PACKET_BACKUP];
	int				ping;
	int				rate;
	int				snapshotMsec;
	int				pureAuthentic;
	netchan_t		netchan;
	int 			guid;
	short			clscriptid;
	int				bot;
	int				serverId;
	voices_t		voicedata[40];
	int				unsentVoiceData;
	byte			mutedClients[MAX_CLIENTS];
	byte			hasVoip;
#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	char 			pbguid[64];
#endif
} client_t;

typedef struct archivedSnapshot_s
{
	int start;
	int size;
} archivedSnapshot_t;

typedef struct cachedClient_s
{
	int playerStateExists;
	clientState_t *cs;
	playerState_t *ps;
} cachedClient_t;

typedef struct
{
	qboolean	initialized;
	int			time;
	int			snapFlagServerBit;
	client_t	*clients;
	int			numSnapshotEntities;
	int			numSnapshotClients;
	int			nextSnapshotEntities;
	int			nextSnapshotClients;
	entityState_t *snapshotEntities;
	clientState_t *snapshotClients;
	int 		archivedSnapshotEnabled;
	int 		nextArchivedSnapshotFrames;
	archivedSnapshot_t *archivedSnapshotFrames;
	int 		*archivedSnapshotBuffer;
	int 		nextArchivedSnapshotBuffer;
	int			nextCachedSnapshotEntities;
	int 		nextCachedSnapshotClients;
	int 		nextCachedSnapshotFrames;
	cachedClient_t cachedSnapshotClients;
	int			nextHeartbeatTime;
	int 		nextStatusResponseTime;
	challenge_t	challenges[1024];
	netadr_t	redirectAddress;
	netadr_t	authorizeAddress;
	char 		netProfilingBuf[1504];
} serverStatic_t; // verified

typedef struct
{
	const char *key;
	const char *value;
} keyValueStr_t;

typedef struct
{
	byte spawnVarsValid;
	int numSpawnVars;
	keyValueStr_t spawnVars[64];
	int numSpawnVarChars;
	char spawnVarChars[2048];
} SpawnVar;

typedef struct
{
	u_int16_t entnum;
	u_int16_t otherEntnum;
	int useCount;
	int otherUseCount;
} trigger_info_t;

typedef struct
{
	struct gclient_s *clients;
	struct gentity_s *gentities;
	int gentitySize;
	int num_entities;
	struct gentity_s *firstFreeEnt;
	struct gentity_s *lastFreeEnt;
	fileHandle_t logFile;
	int initializing;
	int clientIsSpawning;
	objective_t objectives[16];
	int maxclients;
	int framenum;
	int time;
	int previousTime;
	int frameTime;
	int startTime;
	int teamScores[TEAM_NUM_TEAMS];
	int lastTeammateHealthTime;
	qboolean bUpdateScoresForIntermission;
	int manualNameChange;
	int numConnectedClients;
	int sortedClients[MAX_CLIENTS];
	char voteString[1024];
	char voteDisplayString[1024];
	int voteTime; // 711
	int voteExecuteTime;
	int voteYes;
	int voteNo;
	int numVotingClients;
	byte gap[2072];
	SpawnVar spawnVars;
	int savePersist;
	struct gentity_s *droppedWeaponCue[32];
	float fFogOpaqueDist;
	float fFogOpaqueDistSqrd;
	int remapCount;
	int currentPlayerClone;
	trigger_info_t pendingTriggerList[256];
	trigger_info_t currentTriggerList[256];
	int pendingTriggerListSize;
	int currentTriggerListSize;
	int finished;
	int bPlayerIgnoreRadiusDamage;
	int bPlayerIgnoreRadiusDamageLatched;
	int registerWeapons;
	int bRegisterItems;
	int currentEntityThink;
	void *openScriptIOFileHandles[1];
	char *openScriptIOFileBuffers[1];
} level_locals_t; // possibly more stuff here

typedef enum
{
	SS_DEAD,
	SS_LOADING,
	SS_GAME
} serverState_t;

#define MAX_CONFIGSTRINGS   2048
#define MAX_MODELS          256
#define GENTITYNUM_BITS     10
#define MAX_GENTITIES       ( 1 << GENTITYNUM_BITS )
#define MAX_ENT_CLUSTERS    16
#define MAX_BPS_WINDOW 		20

typedef struct
{
	int cluster;
	int area;
	int firstLeafBrush;
	int numLeafBrushes;
	int firstLeafSurface;
	int numLeafSurfaces;
} cLeaf_t;

typedef struct cmodel_s
{
	vec3_t mins, maxs;
	cLeaf_t leaf;
} cmodel_t;

typedef struct
{
	int svFlags;
	int clientMask[2];
	vec3_t absmin;
	vec3_t absmax;
} archivedEntityShared_t;

typedef struct archivedEntity_s
{
	entityState_t s;
	archivedEntityShared_t r;
} archivedEntity_t;

typedef struct svEntity_s
{
	u_int16_t worldSector;
	u_int16_t nextEntityInWorldSector;
	archivedEntity_t baseline;
	int numClusters;
	int clusternums[MAX_ENT_CLUSTERS];
	int	lastCluster;
	int	linkcontents;
	float linkmin[2];
	float linkmax[2];
} svEntity_t;

typedef struct
{
	serverState_t state;
	qboolean restarting;
	int start_frameTime;
	int	checksumFeed;
	int timeResidual;
	int unk; // ?
	struct cmodel_s *models[MAX_MODELS]; // ?
	char *configstrings[MAX_CONFIGSTRINGS];
	svEntity_t svEntities[MAX_GENTITIES];
	char *entityParsePoint;
	gentity_t *gentities;
	int gentitySize;
	int	num_entities;
	playerState_t *gameClients;
	int gameClientSize;
	int	skelTimeStamp;
	int	skelMemPos;
	int	bpsWindow[MAX_BPS_WINDOW];
	int	bpsWindowSteps;
	int	bpsTotalBytes;
	int	bpsMaxBytes;
	int	ubpsWindow[MAX_BPS_WINDOW];
	int	ubpsTotalBytes;
	int	ubpsMaxBytes;
	float ucompAve;
	int	ucompNum;
	char gametype[MAX_QPATH];
} server_t; // verified

typedef struct WeaponDef_t
{
	const char *szInternalName;
	const char *szDisplayName;
} WeaponDef_t; // Way too many stuff here

typedef enum
{
	ANIM_BP_UNUSED,
	ANIM_BP_LEGS,
	ANIM_BP_TORSO,
	ANIM_BP_BOTH,
	NUM_ANIM_BODYPARTS
} animBodyPart_t;

// Idk real types but weapon seems correct
typedef enum
{
	IT_BAD,
	IT_WEAPON,
	IT_AMMO,
	IT_ARMOR,
	IT_HEALTH,
	IT_HOLDABLE,
} itemType_t;

typedef struct gitem_s
{
	char *classname; // ??? They don't set classname or what? Other fields work fine.
	char *pickup_sound;
	char *world_model;
	int giTag; // ?
	char *icon;
	char *display_name; // Weapon string e.g WEAPON_STEN
	int quantity; // ammo for weapons
	itemType_t giType;
	int giAmmoIndex;
	int giClipIndex;
	int giSharedAmmoCapIndex; // guessed
} gitem_t;

#define KEY_MASK_NONE        	0

#define KEY_MASK_FORWARD        127
#define KEY_MASK_BACK           -127
#define KEY_MASK_MOVERIGHT      127
#define KEY_MASK_MOVELEFT       -127

#define KEY_MASK_FIRE           1
#define KEY_MASK_MELEE          4
#define KEY_MASK_USE            8
#define KEY_MASK_RELOAD         16
#define KEY_MASK_LEANLEFT       64
#define KEY_MASK_LEANRIGHT      128
#define KEY_MASK_PRONE          256
#define KEY_MASK_CROUCH         512
#define KEY_MASK_JUMP           1024
#define KEY_MASK_ADS_MODE       4096
#define KEY_MASK_MELEE_BREATH   32772
#define KEY_MASK_HOLDBREATH     32768
#define KEY_MASK_FRAG           65536
#define KEY_MASK_SMOKE          131072

#define EF_VOTED 0x00100000
#define EF_TALK 0x00200000
#define EF_TAUNT 0x00400000
#define EF_FIRING 0x00000020
#define EF_MANTLE 0x00004000
#define EF_CROUCHING 0x00000004
#define EF_PRONE 0x00000008
#define EF_DEAD 0x00020000
#define EF_USETURRET 0x00000300
#define EF_AIMDOWNSIGHT 0x00040000

#define PMF_LADDER 32

#if COD_VERSION == COD2_1_0
static const int gentities_offset = 0x08665480;
#elif COD_VERSION == COD2_1_2
static const int gentities_offset = 0x08679380;
#elif COD_VERSION == COD2_1_3
static const int gentities_offset = 0x08716400;
#endif

#if COD_VERSION == COD2_1_0
static const int gclients_offset = 0x086F1480;
#elif COD_VERSION == COD2_1_2
static const int gclients_offset = 0x08705480;
#elif COD_VERSION == COD2_1_3
static const int gclients_offset = 0x087A2500;
#endif

#define g_entities ((gentity_t*)(gentities_offset))
#define g_clients ((gclient_t*)(gclients_offset))

#if COD_VERSION == COD2_1_0
static const int varpub_offset = 0x08394000;
#elif COD_VERSION == COD2_1_2
static const int varpub_offset = 0x08396480;
#elif COD_VERSION == COD2_1_3
static const int varpub_offset = 0x08397500;
#endif

#if COD_VERSION == COD2_1_0
static const int varglob_offset = 0x08294000;
#elif COD_VERSION == COD2_1_2
static const int varglob_offset = 0x08296480;
#elif COD_VERSION == COD2_1_3
static const int varglob_offset = 0x08297500;
#endif

#if COD_VERSION == COD2_1_0
static const int vmpub_offset = 0x083D7600;
#elif COD_VERSION == COD2_1_2
static const int vmpub_offset = 0x083D7A00;
#elif COD_VERSION == COD2_1_3
static const int vmpub_offset = 0x083D8A80;
#endif

#if COD_VERSION == COD2_1_0
static const int sv_offset = 0x0842BC80;
#elif COD_VERSION == COD2_1_2
static const int sv_offset = 0x0843F180;
#elif COD_VERSION == COD2_1_3
static const int sv_offset = 0x08440200;
#endif

#if COD_VERSION == COD2_1_0
static const int svs_offset = 0x0841FB00;
#elif COD_VERSION == COD2_1_2
static const int svs_offset = 0x08422000;
#elif COD_VERSION == COD2_1_3
static const int svs_offset = 0x08423080;
#endif

#if COD_VERSION == COD2_1_0
static const int level_offset = 0x0859B400;
#elif COD_VERSION == COD2_1_2
static const int level_offset = 0x085AF300;
#elif COD_VERSION == COD2_1_3
static const int level_offset = 0x0864C380;
#endif

#if COD_VERSION == COD2_1_0
static const int itemlist_offset = 0x08164C20;
#elif COD_VERSION == COD2_1_2
static const int itemlist_offset = 0x081840A0;
#elif COD_VERSION == COD2_1_3
static const int itemlist_offset = 0x081850C0;
#endif

#define scrVarPub (*((scrVarPub_t*)( varpub_offset )))
#define scrVmPub (*((scrVmPub_t*)( vmpub_offset )))
#define scrVarGlob (((VariableValueInternal*)( varglob_offset )))
#define scrVarGlob_high (((VariableValueInternal*)( varglob_offset + 16 * 32770 )))
#define sv (*((server_t*)( sv_offset )))
#define svs (*((serverStatic_t*)( svs_offset )))
#define level (*((level_locals_t*)( level_offset )))
#define bg_itemlist (((gitem_t*)( itemlist_offset )))

// Check for critical structure sizes and fail if not match
#if COD_VERSION == COD2_1_0
static_assert((sizeof(client_t) == 0x78F14), "ERROR: client_t size is invalid!");
#elif COD_VERSION == COD2_1_2
static_assert((sizeof(client_t) == 0x79064), "ERROR: client_t size is invalid!");
#elif COD_VERSION == COD2_1_3
static_assert((sizeof(client_t) == 0xB1064), "ERROR: client_t size is invalid!");
#endif

static_assert((sizeof(gentity_t) == 560), "ERROR: gentity_t size is invalid!");
static_assert((sizeof(gclient_t) == 0x28A4), "ERROR: gclient_t size is invalid!");
static_assert((sizeof(gitem_t) == 44), "ERROR: gitem_t size is invalid!");

#endif
