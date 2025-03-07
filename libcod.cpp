#include "gsc.hpp"

cvar_t *sv_maxclients;
cvar_t *sv_allowDownload;
cvar_t *sv_pure;
cvar_t *developer;
cvar_t *rcon_password;
cvar_t *con_coloredPrints;
cvar_t *cl_allowDownload;
cvar_t *sv_timeout;
cvar_t *sv_zombietime;
cvar_t *dedicated;

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
cvar_t *sv_wwwDownload;
cvar_t *cl_wwwDownload;
#endif

cvar_t *sv_cracked;
cvar_t *sv_noauthorize;
cvar_t *g_playerCollision;
cvar_t *g_playerEject;
cvar_t *sv_allowRcon;
cvar_t *fs_library;
cvar_t *sv_downloadMessage;

#define MAX_MASTER_SERVERS 5
#define PORT_MASTER 20710
cvar_t *sv_master[MAX_MASTER_SERVERS];

void hook_sv_init(const char *format, ...)
{
	char s[COD2_MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	Q_vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_Printf("%s", s);

	/* Do stuff after sv has been initialized here */

	// Register custom cvars
	sv_cracked = Cvar_RegisterBool("sv_cracked", qfalse, CVAR_ARCHIVE);
	sv_noauthorize = Cvar_RegisterBool("sv_noauthorize", qfalse, CVAR_ARCHIVE);
	con_coloredPrints = Cvar_RegisterBool("con_coloredPrints", qtrue, CVAR_ARCHIVE);
	g_playerCollision = Cvar_RegisterBool("g_playerCollision", qtrue, CVAR_ARCHIVE);
	g_playerEject = Cvar_RegisterBool("g_playerEject", qtrue, CVAR_ARCHIVE);
	sv_allowRcon = Cvar_RegisterBool("sv_allowRcon", qtrue, CVAR_ARCHIVE);
	fs_library = Cvar_RegisterString("fs_library", "", CVAR_ARCHIVE);
	sv_downloadMessage = Cvar_RegisterString("sv_downloadMessage", "", CVAR_ARCHIVE);

	sv_master[0] = Cvar_RegisterString("sv_master1", "cod2master.activision.com", CVAR_ARCHIVE);
	sv_master[1] = Cvar_RegisterString("sv_master2", "master.cod2.ru", CVAR_ARCHIVE);
	sv_master[2] = Cvar_RegisterString("sv_master3", "", CVAR_ARCHIVE);
	sv_master[3] = Cvar_RegisterString("sv_master4", "", CVAR_ARCHIVE);
	sv_master[4] = Cvar_RegisterString("sv_master5", "", CVAR_ARCHIVE);

	// Force download on clients
	cl_allowDownload = Cvar_RegisterBool("cl_allowDownload", qtrue, CVAR_ARCHIVE | CVAR_SYSTEMINFO);

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	cl_wwwDownload = Cvar_RegisterBool("cl_wwwDownload", qtrue, CVAR_ARCHIVE | CVAR_SYSTEMINFO);
#endif

	sv_maxclients = Cvar_FindVar("sv_maxclients");
	sv_allowDownload = Cvar_FindVar("sv_allowDownload");
	sv_pure = Cvar_FindVar("sv_pure");
	developer = Cvar_FindVar("developer");
	rcon_password = Cvar_FindVar("rcon_password");
	sv_timeout = Cvar_FindVar("sv_timeout");
	sv_zombietime = Cvar_FindVar("sv_zombietime");
	dedicated = Cvar_FindVar("dedicated");

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	sv_wwwDownload = Cvar_FindVar("sv_wwwDownload");
#endif

#if COMPILE_BSP == 1
	CM_AddCommand();
#endif

}

void hook_bad_printf(const char *format, ...) {}

void hook_sv_spawnserver(const char *format, ...)
{
	char s[COD2_MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	Q_vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_Printf("%s", s);

	/* Do stuff after sv has been spawned here */

#if COMPILE_SQLITE == 1
	free_sqlite_db_stores_and_tasks();
#endif

}

#define	HEARTBEAT_MSEC	180000
#define	STATUS_MSEC		600000
void custom_SV_MasterHeartbeat(const char *game)
{
	static netadr_t	adr[MAX_MASTER_SERVERS];
	char heartbeat[32];
	int	i;

	if ( dedicated->integer != 2 )
	{
		return;	// only dedicated servers send heartbeats
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextHeartbeatTime )
	{
		svs.nextHeartbeatTime = svs.time + HEARTBEAT_MSEC;

		// send to group masters
		for ( i = 0 ; i < MAX_MASTER_SERVERS ; i++ )
		{
			if ( !sv_master[i]->string[0] )
			{
				continue;
			}

			// see if we haven't already resolved the name
			// do it when needed
			if ( sv_master[i]->modified || !adr[i].type )
			{
				sv_master[i]->modified = qfalse;

				Com_Printf( "Resolving %s\n", sv_master[i]->string );
				if ( !NET_StringToAdr( sv_master[i]->string, &adr[i] ) )
				{
					// if the address failed to resolve, clear it
					// so we don't take repeated dns hits
					Com_Printf( "Couldn't resolve address: %s\n", sv_master[i]->string );
					Cvar_SetString(sv_master[i], "");
					sv_master[i]->modified = qfalse;
					continue;
				}
				if ( !strstr( ":", sv_master[i]->string ) )
				{
					adr[i].port = BigShort( PORT_MASTER );
				}
				Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", sv_master[i]->string,
				            adr[i].ip[0], adr[i].ip[1], adr[i].ip[2], adr[i].ip[3],
				            BigShort( adr[i].port ) );
			}

			if (strlen(sv_master[i]->string))
			{
				Com_Printf( "Sending heartbeat to %s\n", sv_master[i]->string );
				sprintf(heartbeat, "heartbeat %s\n", game);
				NET_OutOfBandPrint( NS_SERVER, adr[i], heartbeat );
			}
		}
	}

	// if not time yet, don't send anything
	if ( svs.time >= svs.nextStatusResponseTime )
	{
		svs.nextStatusResponseTime = svs.time + STATUS_MSEC;

		// send to group masters
		for ( i = 0 ; i < MAX_MASTER_SERVERS ; i++ )
		{
			if ( !sv_master[i]->string[0] )
			{
				continue;
			}

			// see if we haven't already resolved the name
			// do it when needed
			if ( sv_master[i]->modified || !adr[i].type )
			{
				sv_master[i]->modified = qfalse;

				Com_Printf( "Resolving %s\n", sv_master[i]->string );
				if ( !NET_StringToAdr( sv_master[i]->string, &adr[i] ) )
				{
					// if the address failed to resolve, clear it
					// so we don't take repeated dns hits
					Com_Printf( "Couldn't resolve address: %s\n", sv_master[i]->string );
					Cvar_SetString(sv_master[i], "");
					sv_master[i]->modified = qfalse;
					continue;
				}
				if ( !strstr( ":", sv_master[i]->string ) )
				{
					adr[i].port = BigShort( PORT_MASTER );
				}
				Com_Printf( "%s resolved to %i.%i.%i.%i:%i\n", sv_master[i]->string,
				            adr[i].ip[0], adr[i].ip[1], adr[i].ip[2], adr[i].ip[3],
				            BigShort( adr[i].port ) );
			}

			if (strlen(sv_master[i]->string))
				SVC_Status(adr[i]);
		}
	}
}

int codecallback_remotecommand = 0;

int codecallback_playercommand = 0;
int codecallback_userinfochanged = 0;
int codecallback_fire_grenade = 0;
int codecallback_vid_restart = 0;

int codecallback_meleebutton = 0;
int codecallback_usebutton = 0;
int codecallback_attackbutton = 0;

cHook *hook_gametype_scripts;
int hook_codscript_gametype_scripts()
{
	hook_gametype_scripts->unhook();

	codecallback_remotecommand = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_RemoteCommand", 0);

	codecallback_playercommand = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_PlayerCommand", 0);
	codecallback_userinfochanged = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_UserInfoChanged", 0);
	codecallback_fire_grenade = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_FireGrenade", 0);
	codecallback_vid_restart = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_VidRestart", 0);

	codecallback_meleebutton = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_MeleeButton", 0);
	codecallback_usebutton = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_UseButton", 0);
	codecallback_attackbutton = Scr_GetFunctionHandle("maps/mp/gametypes/_callbacksetup", "CodeCallback_AttackButton", 0);

	int (*sig)();
	*(int *)&sig = hook_gametype_scripts->from;
	int ret = sig();
	hook_gametype_scripts->hook();

	return ret;
}

int player_disableitempickup[MAX_CLIENTS] = {0};
cHook *hook_touch_item_auto;
int touch_item_auto(gentity_t *ent, gentity_t *other, int touch)
{
	hook_touch_item_auto->unhook();

	int (*sig)(gentity_t *ent, gentity_t *other, int touch);
	*(int *)&sig = hook_touch_item_auto->from;

	int ret;

	if (!player_disableitempickup[other->s.number])
		ret = sig(ent, other, touch);
	else
		ret = 0;

	hook_touch_item_auto->hook();

	return ret;
}

cHook *hook_player_collision;
int player_collision(int a1)
{
	hook_player_collision->unhook();

	int (*sig)(int a1);
	*(int *)&sig = hook_player_collision->from;

	int ret;

	if (g_playerCollision->boolean)
		ret = sig(a1);
	else
		ret = 0;

	hook_player_collision->hook();

	return ret;
}

cHook *hook_player_eject;
int player_eject(int a1)
{
	hook_player_eject->unhook();

	int (*sig)(int a1);
	*(int *)&sig = hook_player_eject->from;

	int ret;

	if (g_playerEject->boolean)
		ret = sig(a1);
	else
		ret = 0;

	hook_player_eject->hook();

	return ret;
}

cHook *hook_fire_grenade;
gentity_t* fire_grenade(gentity_t *self, vec3_t start, vec3_t dir, int weapon, int time)
{
	hook_fire_grenade->unhook();

	gentity_t* (*sig)(gentity_t *self, vec3_t start, vec3_t dir, int weapon, int time);
	*(int *)&sig = hook_fire_grenade->from;

	gentity_t* grenade = sig(self, start, dir, weapon, time);

	hook_fire_grenade->hook();

	if (codecallback_fire_grenade)
	{
		WeaponDef_t *def = BG_WeaponDefs(weapon);
		stackPushString(def->szInternalName);
		stackPushEntity(grenade);
		short ret = Scr_ExecEntThread(self, codecallback_fire_grenade, 2);
		Scr_FreeThread(ret);
	}

	return grenade;
}

void hook_ClientCommand(int clientNum)
{
	if ( ! codecallback_playercommand)
	{
		ClientCommand(clientNum);
		return;
	}

	if (!Scr_IsSystemActive())
		return;

	stackPushArray();
	int args = Cmd_Argc();
	for (int i = 0; i < args; i++)
	{
		char tmp[COD2_MAX_STRINGLENGTH];
		SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
		if(i == 1 && tmp[0] >= 20 && tmp[0] <= 22)
		{
			char *part = strtok(tmp + 1, " ");
			while(part != NULL)
			{
				stackPushString(part);
				stackPushArrayLast();
				part = strtok(NULL, " ");
			}
		}
		else
		{
			stackPushString(tmp);
			stackPushArrayLast();
		}
	}

	short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_playercommand, 1);
	Scr_FreeThread(ret);
}

int hook_isLanAddress(netadr_t adr)
{
	if (sv_noauthorize->boolean)
		return 1;

	return Sys_IsLANAddress(adr);
}

const char* hook_AuthorizeState(int arg)
{
	const char *s = Cmd_Argv(arg);

	if (sv_cracked->boolean && strcmp(s, "deny") == 0)
		return "accept";

	return s;
}

void hook_ClientUserinfoChanged(int clientNum)
{
	if ( ! codecallback_userinfochanged)
	{
		ClientUserinfoChanged(clientNum);
		return;
	}

	if (!Scr_IsSystemActive())
		return;

	stackPushInt(clientNum); // one parameter is required
	short ret = Scr_ExecEntThread(&g_entities[clientNum], codecallback_userinfochanged, 1);
	Scr_FreeThread(ret);
}

void custom_SV_WriteDownloadToClient(client_t *cl, msg_t *msg)
{
	int curindex;
	int iwdFile;
	char errorMessage[COD2_MAX_STRINGLENGTH];

	if (cl->state == CS_ACTIVE)
		return; // Client already in game

	if (!*cl->downloadName)
		return;	// Nothing being downloaded

	if (strlen(cl->downloadName) < 4)
		return; // File length too short

	if (strcmp(&cl->downloadName[strlen(cl->downloadName) - 4], ".iwd") != 0)
		return; // Not a iwd file

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	if ( cl->wwwDlAck )
		return; // wwwDl acknowleged
#endif

	if (strlen(sv_downloadMessage->string))
	{
		Com_sprintf(errorMessage, sizeof(errorMessage), sv_downloadMessage->string);

		MSG_WriteByte( msg, svc_download );
		MSG_WriteShort( msg, 0 ); // client is expecting block zero
		MSG_WriteLong( msg, -1 ); // illegal file size
		MSG_WriteString( msg, errorMessage );

		*cl->downloadName = 0;
		return; // Download message instead of download
	}

#if COD_VERSION == COD2_1_2 || COD_VERSION == COD2_1_3
	if (sv_wwwDownload->boolean && cl->wwwDownload)
	{
		if (!cl->wwwDl_failed)
		{
			SV_WWWRedirectClient(cl, msg);
			return; // wwwDl redirect
		}
	}
#endif

	// Hardcode client variables to make max download speed for everyone
	cl->state = CS_CONNECTED;
	cl->rate = 25000;
	cl->snapshotMsec = 50;

	if (!cl->download)
	{
		// We open the file here

		Com_Printf("clientDownload: %d : begining \"%s\"\n", cl - svs.clients, cl->downloadName);

		iwdFile = FS_iwIwd(cl->downloadName, "main");

		if ( !sv_allowDownload->integer || iwdFile || ( cl->downloadSize = FS_SV_FOpenFileRead( cl->downloadName, &cl->download ) ) <= 0 )
		{
			// cannot auto-download file
			if (iwdFile)
			{
				Com_Printf("clientDownload: %d : \"%s\" cannot download iwd files\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_CANTAUTODLGAMEIWD\x15%s", cl->downloadName);
			}
			else if ( !sv_allowDownload->boolean )
			{
				Com_Printf("clientDownload: %d : \"%s\" download disabled\n", cl - svs.clients, cl->downloadName);

				if (sv_pure->boolean)
					Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED_PURE\x15%s", cl->downloadName);
				else
					Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_SERVERDISABLED\x15%s", cl->downloadName);
			}
			else
			{
				Com_Printf("clientDownload: %d : \"%s\" file not found on server\n", cl - svs.clients, cl->downloadName);
				Com_sprintf(errorMessage, sizeof(errorMessage), "EXE_AUTODL_FILENOTONSERVER\x15%s", cl->downloadName);
			}

			MSG_WriteByte( msg, svc_download );
			MSG_WriteShort( msg, 0 ); // client is expecting block zero
			MSG_WriteLong( msg, -1 ); // illegal file size
			MSG_WriteString( msg, errorMessage );

			*cl->downloadName = 0;
			return;
		}

		// Init
		cl->downloadCurrentBlock = cl->downloadClientBlock = cl->downloadXmitBlock = 0;
		cl->downloadCount = 0;
		cl->downloadEOF = qfalse;
	}

	// Perform any reads that we need to
	while (cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW && cl->downloadSize != cl->downloadCount)
	{
		curindex = (cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW);

		if (!cl->downloadBlocks[curindex])
			cl->downloadBlocks[curindex] = (unsigned char *)Z_MallocInternal(MAX_DOWNLOAD_BLKSIZE);

		cl->downloadBlockSize[curindex] = FS_Read( cl->downloadBlocks[curindex], MAX_DOWNLOAD_BLKSIZE, cl->download );

		if (cl->downloadBlockSize[curindex] < 0)
		{
			// EOF right now
			cl->downloadCount = cl->downloadSize;
			break;
		}

		cl->downloadCount += cl->downloadBlockSize[curindex];

		// Load in next block
		cl->downloadCurrentBlock++;
	}

	// Check to see if we have eof condition and add the EOF block
	if (cl->downloadCount == cl->downloadSize && !cl->downloadEOF && cl->downloadCurrentBlock - cl->downloadClientBlock < MAX_DOWNLOAD_WINDOW)
	{
		cl->downloadBlockSize[cl->downloadCurrentBlock % MAX_DOWNLOAD_WINDOW] = 0;
		cl->downloadCurrentBlock++;
		cl->downloadEOF = qtrue;  // We have added the EOF block
	}

	// Write out the next section of the file, if we have already reached our window,
	// automatically start retransmitting

	if (cl->downloadClientBlock == cl->downloadCurrentBlock)
		return; // Nothing to transmit

	if (cl->downloadXmitBlock == cl->downloadCurrentBlock)
	{
		// We have transmitted the complete window, should we start resending?

		//FIXME:  This uses a hardcoded one second timeout for lost blocks
		//the timeout should be based on client rate somehow
		if (svs.time - cl->downloadSendTime > 1000)
			cl->downloadXmitBlock = cl->downloadClientBlock;
		else
			return;
	}

	// Send current block
	curindex = (cl->downloadXmitBlock % MAX_DOWNLOAD_WINDOW);

	MSG_WriteByte( msg, svc_download );
	MSG_WriteShort( msg, cl->downloadXmitBlock );

	// block zero is special, contains file size
	if ( cl->downloadXmitBlock == 0 )
		MSG_WriteLong( msg, cl->downloadSize );

	MSG_WriteShort( msg, cl->downloadBlockSize[curindex] );

	// Write the block
	if ( cl->downloadBlockSize[curindex] )
		MSG_WriteData( msg, cl->downloadBlocks[curindex], cl->downloadBlockSize[curindex] );

	Com_DPrintf("clientDownload: %d : writing block %d\n", cl - svs.clients, cl->downloadXmitBlock);

	// Move on to the next block
	// It will get sent with next snap shot.  The rate will keep us in line.
	cl->downloadXmitBlock++;

	cl->downloadSendTime = svs.time;
}

// Segfault fix
int hook_BG_IsWeaponValid(int a1, int a2)
{
	if ( !(unsigned char)sub_80E9758(a2) )
		return 0;

	if ( !(unsigned char)sub_80D9E84(a1 + 1348, a2) )
		return 0;

	int weapon = (int)BG_WeaponDefs(a2);

	if ( !weapon )
		return 0;

	if ( !*(long *)(weapon + 132) && *(char *)(a1 + 1365) != a2 && *(char *)(a1 + 1366) != a2 && *(long *)(weapon + 876) != a2 )
		return 0;

	return 1;
}

char *custom_va(const char *format, ...)
{
	struct va_info_t *info;
	int index;
	va_list va;

	info = (va_info_t*)Sys_GetValue(1);
	index = info->index;
	info->index = (info->index + 1) % MAX_VASTRINGS;

	va_start(va, format);
	Q_vsnprintf(info->va_string[index], sizeof(info->va_string[0]), format, va);
	va_end(va);

	info->va_string[index][1023] = 0;

	return info->va_string[index];
}

void hook_SV_VerifyIwds_f(client_t *cl)
{
	if (sv_pure->boolean)
		cl->pureAuthentic = 1;
}

void hook_SV_ResetPureClient_f(client_t *cl)
{
	cl->pureAuthentic = 0;

	if (codecallback_vid_restart)
	{
		if (!Scr_IsSystemActive())
			return;

		if (cl->gentity == NULL)
			return;

		stackPushInt(cl - svs.clients);
		short ret = Scr_ExecEntThread(cl->gentity, codecallback_vid_restart, 1);
		Scr_FreeThread(ret);
	}
}

// Adds bot pings and removes spam on 1.2 and 1.3
void custom_SV_CalcPings( void )
{
	int i, j;
	client_t *cl;
	int total, count;
	int delta;

	for ( i = 0 ; i < sv_maxclients->integer ; i++ )
	{
		cl = &svs.clients[i];

		if ( cl->state != CS_ACTIVE )
		{
			cl->ping = -1;
			continue;
		}

		if ( !cl->gentity )
		{
			cl->ping = -1;
			continue;
		}

		if ( cl->netchan.remoteAddress.type == NA_BOT )
		{
			cl->ping = 0;
			cl->lastPacketTime = svs.time;
			continue;
		}

		total = 0;
		count = 0;

		for ( j = 0 ; j < PACKET_BACKUP ; j++ )
		{
			if ( cl->frames[j].messageAcked == 0xFFFFFFFF )
				continue;

			delta = cl->frames[j].messageAcked - cl->frames[j].messageSent;
			count++;
			total += delta;
		}

		if ( !count )
			cl->ping = 999;
		else
		{
			cl->ping = total / count;

			if ( cl->ping > 999 )
				cl->ping = 999;
		}
	}
}

void custom_SV_CheckTimeouts( void )
{
	int	i;
	client_t *cl;
	int	droppoint;
	int	zombiepoint;

	droppoint = svs.time - 1000 * sv_timeout->integer;
	zombiepoint = svs.time - 1000 * sv_zombietime->integer;

	for ( i = 0, cl = svs.clients; i < sv_maxclients->integer; i++, cl++ )
	{
		// message times may be wrong across a changelevel
		if (cl->lastPacketTime > svs.time)
			cl->lastPacketTime = svs.time;

		if (cl->state == CS_ZOMBIE && cl->lastPacketTime < zombiepoint)
		{
			cl->state = CS_FREE; // can now be reused
			continue;
		}

		if (cl->state >= CS_CONNECTED && cl->lastPacketTime < droppoint)
		{
			// wait several frames so a debugger session doesn't
			// cause a timeout
			if ( ++cl->timeoutCount > 5 )
			{
				SV_DropClient(cl, "EXE_TIMEDOUT");
				cl->state = CS_FREE; // don't bother with zombie state
			}
		}
		else
			cl->timeoutCount = 0;
	}
}

#if COMPILE_BOTS == 1
int bot_buttons[MAX_CLIENTS] = {0};
int bot_weapon[MAX_CLIENTS] = {0};
char bot_forwardmove[MAX_CLIENTS] = {0};
char bot_rightmove[MAX_CLIENTS] = {0};
void custom_SV_BotUserMove(client_t *client)
{
	int num;
	usercmd_t ucmd = {0};

	if (client->gentity == NULL)
		return;

	num = client - svs.clients;
	ucmd.serverTime = svs.time;

	playerState_t *ps = SV_GameClientNum(num);
	gentity_t *ent = SV_GentityNum(num);

	if (bot_weapon[num])
		ucmd.weapon = (byte)(bot_weapon[num] & 0xFF);
	else
		ucmd.weapon = (byte)(ps->weapon & 0xFF);

	if (ent->client == NULL)
		return;

	if (ent->client->sess.archiveTime == 0)
	{
		ucmd.buttons = bot_buttons[num];

		ucmd.forwardmove = bot_forwardmove[num];
		ucmd.rightmove = bot_rightmove[num];

		VectorCopy(ent->client->sess.cmd.angles, ucmd.angles);
	}

	client->deltaMessage = client->netchan.outgoingSequence - 1;
	SV_ClientThink(client, &ucmd);
}
#endif

void hook_scriptError(int a1, int a2, int a3, void *a4)
{
	if (developer->integer == 2)
		runtimeError(0, a1, a2, a3);
	else
		scriptError(a1, a2, a3, a4);
}

#if COMPILE_PLAYER == 1
int gamestate_size[MAX_CLIENTS] = {0};
void hook_gamestate_info(const char *format, ...)
{
	char s[COD2_MAX_STRINGLENGTH];
	va_list va;

	va_start(va, format);
	Q_vsnprintf(s, sizeof(s), format, va);
	va_end(va);

	Com_DPrintf("%s", s);

	char *tok;
	int gamestate = 0;
	int clientnum = 0;
	tok = strtok(s, " ");

	for (int i = 0; tok != NULL; i++)
	{
		if (i == 1)
			gamestate = atoi(tok);
		if (i == 7)
			clientnum = atoi(tok);
		tok = strtok(NULL, " ");
	}

	gamestate_size[clientnum] = gamestate;
}

int clientfps[MAX_CLIENTS] = {0};
int clientframes[MAX_CLIENTS] = {0};
uint64_t clientframetime[MAX_CLIENTS] = {0};
int previousbuttons[MAX_CLIENTS] = {0};

cHook *hook_play_movement;
int play_movement(client_t *cl, usercmd_t *ucmd)
{
	hook_play_movement->unhook();

	int (*sig)(client_t *cl, usercmd_t *ucmd);
	*(int *)&sig = hook_play_movement->from;

	int ret = sig(cl, ucmd);

	hook_play_movement->hook();

	int clientnum = cl - svs.clients;

	clientframes[clientnum]++;

	if (Sys_Milliseconds64() - clientframetime[clientnum] >= 1000)
	{
		if (clientframes[clientnum] > 1000)
			clientframes[clientnum] = 1000;

		clientfps[clientnum] = clientframes[clientnum];
		clientframetime[clientnum] = Sys_Milliseconds64();
		clientframes[clientnum] = 0;
	}

	if (ucmd->buttons & KEY_MASK_MELEE && !(previousbuttons[clientnum] & KEY_MASK_MELEE))
	{
		if(codecallback_meleebutton)
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_meleebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	if (ucmd->buttons & KEY_MASK_USE && !(previousbuttons[clientnum] & KEY_MASK_USE))
	{
		if(codecallback_usebutton)
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_usebutton, 0);
			Scr_FreeThread(ret);
		}
	}

	if (ucmd->buttons & KEY_MASK_FIRE && !(previousbuttons[clientnum] & KEY_MASK_FIRE))
	{
		if(codecallback_attackbutton)
		{
			short ret = Scr_ExecEntThread(cl->gentity, codecallback_attackbutton, 0);
			Scr_FreeThread(ret);
		}
	}

	previousbuttons[clientnum] = ucmd->buttons;
	return ret;
}

int player_g_speed[MAX_CLIENTS] = {0};
int player_g_gravity[MAX_CLIENTS] = {0};
cHook *hook_play_endframe;
int play_endframe(gentity_t *ent)
{
	hook_play_endframe->unhook();

	int (*sig)(gentity_t *ent);
	*(int *)&sig = hook_play_endframe->from;

	int ret = sig(ent);

	hook_play_endframe->hook();

	if (ent->client->sess.state == STATE_PLAYING)
	{
		int num = ent - g_entities;

		if (player_g_speed[num] > 0)
			ent->client->ps.speed = player_g_speed[num];

		if (player_g_gravity[num] > 0)
			ent->client->ps.gravity = player_g_gravity[num];
	}

	return ret;
}

int custom_animation[MAX_CLIENTS] = {0};
cHook *hook_set_anim;
int set_anim(playerState_t *ps, int animNum, animBodyPart_t bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force)
{
	hook_set_anim->unhook();

	int (*sig)(playerState_t *ps, int animNum, animBodyPart_t bodyPart, int forceDuration, qboolean setTimer, qboolean isContinue, qboolean force);
	*(int *)&sig = hook_set_anim->from;

	int ret;

	if (!custom_animation[ps->clientNum])
		ret = sig(ps, animNum, bodyPart, forceDuration, setTimer, isContinue, force);
	else
		ret = sig(ps, custom_animation[ps->clientNum], bodyPart, forceDuration, qtrue, isContinue, qtrue);

	hook_set_anim->hook();

	return ret;
}
#endif

#if COMPILE_RATELIMITER == 1
// ioquake3 rate limit connectionless requests
// https://github.com/ioquake/ioq3/blob/master/code/server/sv_main.c

// This is deliberately quite large to make it more of an effort to DoS
#define MAX_BUCKETS	16384
#define MAX_HASHES 1024

static leakyBucket_t buckets[ MAX_BUCKETS ];
static leakyBucket_t* bucketHashes[ MAX_HASHES ];
leakyBucket_t outboundLeakyBucket;

static long SVC_HashForAddress( netadr_t address )
{
	unsigned char *ip = address.ip;
	int	i;
	long hash = 0;

	for ( i = 0; i < 4; i++ )
	{
		hash += (long)( ip[ i ] ) * ( i + 119 );
	}

	hash = ( hash ^ ( hash >> 10 ) ^ ( hash >> 20 ) );
	hash &= ( MAX_HASHES - 1 );

	return hash;
}

static leakyBucket_t *SVC_BucketForAddress( netadr_t address, int burst, int period )
{
	leakyBucket_t *bucket = NULL;
	int	i;
	long hash = SVC_HashForAddress( address );
	uint64_t now = Sys_Milliseconds64();

	for ( bucket = bucketHashes[ hash ]; bucket; bucket = bucket->next )
	{
		if ( memcmp( bucket->adr, address.ip, 4 ) == 0 )
		{
			return bucket;
		}
	}

	for ( i = 0; i < MAX_BUCKETS; i++ )
	{
		int interval;

		bucket = &buckets[ i ];
		interval = now - bucket->lastTime;

		// Reclaim expired buckets
		if ( bucket->lastTime > 0 && ( interval > ( burst * period ) ||
		                               interval < 0 ) )
		{
			if ( bucket->prev != NULL )
			{
				bucket->prev->next = bucket->next;
			}
			else
			{
				bucketHashes[ bucket->hash ] = bucket->next;
			}

			if ( bucket->next != NULL )
			{
				bucket->next->prev = bucket->prev;
			}

			memset( bucket, 0, sizeof( leakyBucket_t ) );
		}

		if ( bucket->type == 0 )
		{
			bucket->type = address.type;
			memcpy( bucket->adr, address.ip, 4 );

			bucket->lastTime = now;
			bucket->burst = 0;
			bucket->hash = hash;

			// Add to the head of the relevant hash chain
			bucket->next = bucketHashes[ hash ];
			if ( bucketHashes[ hash ] != NULL )
			{
				bucketHashes[ hash ]->prev = bucket;
			}

			bucket->prev = NULL;
			bucketHashes[ hash ] = bucket;

			return bucket;
		}
	}

	// Couldn't allocate a bucket for this address
	return NULL;
}

bool SVC_RateLimit( leakyBucket_t *bucket, int burst, int period )
{
	if ( bucket != NULL )
	{
		uint64_t now = Sys_Milliseconds64();
		int interval = now - bucket->lastTime;
		int expired = interval / period;
		int expiredRemainder = interval % period;

		if ( expired > bucket->burst || interval < 0 )
		{
			bucket->burst = 0;
			bucket->lastTime = now;
		}
		else
		{
			bucket->burst -= expired;
			bucket->lastTime = now - expiredRemainder;
		}

		if ( bucket->burst < burst )
		{
			bucket->burst++;

			return false;
		}
	}

	return true;
}

bool SVC_RateLimitAddress( netadr_t from, int burst, int period )
{
	leakyBucket_t *bucket = SVC_BucketForAddress( from, burst, period );

	return SVC_RateLimit( bucket, burst, period );
}

bool isRconCommandWithForwardedOutput(const char* command)
{
	return (strcmp(command, "map") == 0 || strcmp(command, "map_restart") == 0 || strcmp(command, "fast_restart") == 0 || strcmp(command, "devmap") == 0);
}

void hook_SVC_RemoteCommand(netadr_t from, msg_t *msg)
{
	if (!sv_allowRcon->boolean)
		return;

	// Prevent using rcon as an amplifier and make dictionary attacks impractical
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SVC_RemoteCommand: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	bool badRconPassword = !strlen( rcon_password->string ) || strcmp(Cmd_Argv(1), rcon_password->string) != 0;
	if (badRconPassword)
	{
		static leakyBucket_t bucket;

		// Make DoS via rcon impractical
		if ( SVC_RateLimit( &bucket, 10, 1000 ) )
		{
			Com_DPrintf( "SVC_RemoteCommand: rate limit exceeded, dropping request\n" );
			return;
		}
	}

	if (!codecallback_remotecommand || badRconPassword || !Scr_IsSystemActive() || isRconCommandWithForwardedOutput(Cmd_Argv(2)))
	{
#if COD_VERSION == COD2_1_0
		int lasttime_offset = 0x0848B674;
#elif COD_VERSION == COD2_1_2
		int lasttime_offset = 0x0849EB74;
#elif COD_VERSION == COD2_1_3
		int lasttime_offset = 0x0849FBF4;
#endif

		*(int *)lasttime_offset = 0;

		SVC_RemoteCommand(from, msg);
	}
	else
	{
		stackPushInt((int)msg);

		stackPushArray();
		int args = Cmd_Argc();
		for (int i = 2; i < args; i++)
		{
			char tmp[COD2_MAX_STRINGLENGTH];
			SV_Cmd_ArgvBuffer(i, tmp, sizeof(tmp));
			stackPushString(tmp);
			stackPushArrayLast();
		}

		stackPushString(NET_AdrToString(from));

		short ret = Scr_ExecThread(codecallback_remotecommand, 3);
		Scr_FreeThread(ret);
	}
}

void hook_SV_GetChallenge(netadr_t from)
{
	// Prevent using getchallenge as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SV_GetChallenge: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Allow getchallenge to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		Com_DPrintf( "SV_GetChallenge: rate limit exceeded, dropping request\n" );
		return;
	}

	SV_GetChallenge(from);
}

void hook_SV_DirectConnect(netadr_t from)
{
	// Prevent using connect as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SV_DirectConnect: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Allow connect to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		Com_DPrintf( "SV_DirectConnect: rate limit exceeded, dropping request\n" );
		return;
	}

	SV_DirectConnect(from);
}

void hook_SVC_Info(netadr_t from)
{
	// Prevent using getinfo as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SVC_Info: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Allow getinfo to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		Com_DPrintf( "SVC_Info: rate limit exceeded, dropping request\n" );
		return;
	}

	SVC_Info(from);
}

void hook_SVC_Status(netadr_t from)
{
	// Prevent using getstatus as an amplifier
	if ( SVC_RateLimitAddress( from, 10, 1000 ) )
	{
		Com_DPrintf( "SVC_Status: rate limit from %s exceeded, dropping request\n", NET_AdrToString( from ) );
		return;
	}

	// Allow getstatus to be DoSed relatively easily, but prevent
	// excess outbound bandwidth usage when being flooded inbound
	if ( SVC_RateLimit( &outboundLeakyBucket, 10, 100 ) )
	{
		Com_DPrintf( "SVC_Status: rate limit exceeded, dropping request\n" );
		return;
	}

	SVC_Status(from);
}
#endif

void manymaps_prepare(const char *mapname, int read)
{
	char map_check[MAX_OSPATH];
	char library_path[MAX_OSPATH];

	cvar_t *fs_homepath = Cvar_FindVar("fs_homepath");
	cvar_t *fs_game = Cvar_FindVar("fs_game");
	cvar_t *map = Cvar_FindVar("mapname");

	if (strlen(fs_library->string))
		strncpy(library_path, fs_library->string, sizeof(library_path));
	else
		snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->string, fs_game->string);

	Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);

#if COD_VERSION == COD2_1_0
	const char *stock_maps[] = { "mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation" };
#else
	const char *stock_maps[] = { "mp_breakout", "mp_brecourt", "mp_burgundy", "mp_carentan", "mp_dawnville", "mp_decoy", "mp_downtown", "mp_farmhouse", "mp_leningrad", "mp_matmata", "mp_railyard", "mp_toujane", "mp_trainstation", "mp_rhine", "mp_harbor" };
#endif

	bool map_found = false, from_stock_map = false;

	for (int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++)
	{
		if (strcmp(map->string, stock_maps[i]) == 0)
		{
			from_stock_map = true;
			break;
		}
	}

	for (int i = 0; i < int( sizeof(stock_maps) / sizeof(stock_maps[0]) ); i++)
	{
		if (strcmp(mapname, stock_maps[i]) == 0)
		{
			map_found = true;

			if (from_stock_map) // When changing from stock map to stock map do not trigger manymap
				return;
			else
				break;
		}
	}

	int map_exists = access(map_check, F_OK) != -1;

	if (!map_exists && !map_found)
		return;

	DIR *dir;
	struct dirent *dir_ent;

	dir = opendir(library_path);

	if (!dir)
		return;

	while ((dir_ent = readdir(dir)) != NULL)
	{
		if (strcmp(dir_ent->d_name, ".") == 0 || strcmp(dir_ent->d_name, "..") == 0)
			continue;

		char fileDelete[MAX_OSPATH];
		Com_sprintf(fileDelete, MAX_OSPATH, "%s/%s/%s", fs_homepath->string, fs_game->string, dir_ent->d_name);

		if (access(fileDelete, F_OK) != -1)
		{
			int unlink_success = unlink(fileDelete) == 0;
			printf("manymaps> REMOVED OLD LINK: %s result of unlink: %s\n", fileDelete, unlink_success?"success":"failed");
		}
	}

	closedir(dir);

	if (map_exists)
	{
		char src[MAX_OSPATH];
		char dst[MAX_OSPATH];

		Com_sprintf(src, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);
		Com_sprintf(dst, MAX_OSPATH, "%s/%s/%s.iwd", fs_homepath->string, fs_game->string, mapname);

		if (access(src, F_OK) != -1)
		{
			int link_success = symlink(src, dst) == 0;
			printf("manymaps> NEW LINK: src=%s dst=%s result of link: %s\n", src, dst, link_success?"success":"failed");

			if (link_success && read == -1) // FS_LoadDir is needed when empty.iwd is missing (then .d3dbsp isn't referenced anywhere)
				FS_LoadDir(fs_homepath->string, fs_game->string);
		}
	}
}

int hook_findMap(const char *qpath, void **buffer)
{
	int read = FS_ReadFile(qpath, buffer);
	manymaps_prepare(Cmd_Argv(1), read);

	if (read != -1)
		return read;
	else
		return FS_ReadFile(qpath, buffer);
}

bool hook_SV_MapExists(const char *mapname)
{
	bool map_exists = SV_MapExists(mapname);

	if (map_exists)
	{
		return map_exists;
	}
	else
	{
		char map_check[MAX_OSPATH];
		char library_path[MAX_OSPATH];

		cvar_t *fs_homepath = Cvar_FindVar("fs_homepath");
		cvar_t *fs_game = Cvar_FindVar("fs_game");
		if (strlen(fs_library->string))
			strncpy(library_path, fs_library->string, sizeof(library_path));
		else
			snprintf(library_path, sizeof(library_path), "%s/%s/Library", fs_homepath->string, fs_game->string);

		Com_sprintf(map_check, MAX_OSPATH, "%s/%s.iwd", library_path, mapname);

		return access(map_check, F_OK) != -1;
	}
}

class cCallOfDuty2Pro
{
public:
	cCallOfDuty2Pro()
	{
		// dont inherit lib of parent
		unsetenv("LD_PRELOAD");

		// otherwise the printf()'s are printed at crash/end on older os/compiler versions
		setbuf(stdout, NULL);

#if COD_VERSION == COD2_1_0
		printf("> [LIBCOD] Compiled for: CoD2 1.0\n");
#elif COD_VERSION == COD2_1_2
		printf("> [LIBCOD] Compiled for: CoD2 1.2\n");
#elif COD_VERSION == COD2_1_3
		printf("> [LIBCOD] Compiled for: CoD2 1.3\n");
#endif

		printf("> [LIBCOD] Compiled %s %s using GCC %s\n", __DATE__, __TIME__, __VERSION__);

		// allow to write in executable memory
		mprotect((void *)0x08048000, 0x135000, PROT_READ | PROT_WRITE | PROT_EXEC);

#if COD_VERSION == COD2_1_0
		cracking_hook_call(0x08061FE7, (int)hook_sv_init);
		cracking_hook_call(0x08091D0C, (int)hook_sv_spawnserver);
		cracking_hook_call(0x0808F281, (int)hook_ClientCommand);
		cracking_hook_call(0x0808C8C0, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808BFCA, (int)hook_isLanAddress);
		cracking_hook_call(0x0808AD00, (int)hook_findMap);
		cracking_hook_call(0x08114655, (int)hook_SV_MapExists); // GSC map function
		cracking_hook_call(0x08114940, (int)hook_SV_MapExists); // GSC mapexists function
		cracking_hook_call(0x0808F134, (int)hook_ClientUserinfoChanged);
		cracking_hook_call(0x0807059F, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x080707C3, (int)Scr_GetCustomMethod);
		cracking_hook_call(0x0810E507, (int)Com_DPrintf);
		cracking_hook_call(0x08081CFE, (int)hook_scriptError);

#if COMPILE_PLAYER == 1
		cracking_hook_call(0x0808E18F, (int)hook_gamestate_info);
#endif

		hook_gametype_scripts = new cHook(0x0810DDEE, (int)hook_codscript_gametype_scripts);
		hook_gametype_scripts->hook();

		hook_player_collision = new cHook(0x080F2F2E, (int)player_collision);
		hook_player_collision->hook();
		hook_player_eject = new cHook(0x080F474A, (int)player_eject);
		hook_player_eject->hook();
		hook_fire_grenade = new cHook(0x0810C1F6, (int)fire_grenade);
		hook_fire_grenade->hook();

#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x0808F488, (int)play_movement);
		hook_play_movement->hook();
		hook_play_endframe = new cHook(0x080F4DBE, (int)play_endframe);
		hook_play_endframe->hook();
		hook_set_anim = new cHook(0x080D69B2, (int)set_anim);
		hook_set_anim->hook();
		hook_touch_item_auto = new cHook(0x081037F0, (int)touch_item_auto);
		hook_touch_item_auto->hook();
#endif

		cracking_hook_function(0x080E97F0, (int)hook_BG_IsWeaponValid);
		cracking_hook_function(0x0808E544, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B59CE, (int)custom_va);
		cracking_hook_function(0x0808EC66, (int)hook_SV_VerifyIwds_f);
		cracking_hook_function(0x0808EEEC, (int)hook_SV_ResetPureClient_f);
		cracking_hook_function(0x0809443E, (int)custom_SV_CalcPings);
		cracking_hook_function(0x080945AC, (int)custom_SV_CheckTimeouts);
		cracking_hook_function(0x08094F02, (int)custom_SV_MasterHeartbeat);

#if COMPILE_JUMP == 1
		cracking_hook_function(0x080D9FF4, (int)Jump_ClearState);
		cracking_hook_function(0x080DA1A6, (int)Jump_ReduceFriction);
		cracking_hook_function(0x080DA238, (int)Jump_ClampVelocity);
		cracking_hook_function(0x080DA0A4, (int)Jump_IsPlayerAboveMax);
		cracking_hook_function(0x080DA016, (int)Jump_GetStepHeight);
		cracking_hook_function(0x080DA584, (int)Jump_Check);
		cracking_hook_function(0x080DA0F4, (int)Jump_ApplySlowdown);
		cracking_hook_function(0x080DA0CA, (int)Jump_ActivateSlowdown);
		cracking_hook_function(0x080D9EE8, (int)Jump_RegisterDvars);
#endif

#if COMPILE_BOTS == 1
		cracking_hook_function(0x0809479A, (int)custom_SV_BotUserMove);
#endif

#if COMPILE_RATELIMITER == 1
		cracking_hook_call(0x08094081, (int)hook_SVC_Info);
		cracking_hook_call(0x0809403E, (int)hook_SVC_Status);
		cracking_hook_call(0x08094191, (int)hook_SVC_RemoteCommand);
		cracking_hook_call(0x080940C4, (int)hook_SV_GetChallenge);
		cracking_hook_call(0x08094107, (int)hook_SV_DirectConnect);
#endif

#elif COD_VERSION == COD2_1_2
		cracking_hook_call(0x08062301, (int)hook_sv_init);
		cracking_hook_call(0x08093572, (int)hook_sv_spawnserver);
		cracking_hook_call(0x08090B0C, (int)hook_ClientCommand);
		cracking_hook_call(0x0808DA52, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808D22E, (int)hook_isLanAddress);
		cracking_hook_call(0x0808BCFC, (int)hook_findMap);
		cracking_hook_call(0x08116981, (int)hook_SV_MapExists); // GSC map function
		cracking_hook_call(0x08116C6C, (int)hook_SV_MapExists); // GSC mapexists function
		cracking_hook_call(0x080909BE, (int)hook_ClientUserinfoChanged);
		cracking_hook_call(0x08070B1B, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x08070D3F, (int)Scr_GetCustomMethod);
		cracking_hook_call(0x08110832, (int)Com_DPrintf);
		cracking_hook_call(0x0808227A, (int)hook_scriptError);
		cracking_hook_call(0x0808FCBE, (int)hook_bad_printf);

#if COMPILE_PLAYER == 1
		cracking_hook_call(0x0808F533, (int)hook_gamestate_info);
#endif

		hook_gametype_scripts = new cHook(0x0811012A, (int)hook_codscript_gametype_scripts);
		hook_gametype_scripts->hook();

		hook_player_collision = new cHook(0x080F553E, (int)player_collision);
		hook_player_collision->hook();
		hook_player_eject = new cHook(0x080F6D5A, (int)player_eject);
		hook_player_eject->hook();
		hook_fire_grenade = new cHook(0x0810E532, (int)fire_grenade);
		hook_fire_grenade->hook();

#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x08090D18, (int)play_movement);
		hook_play_movement->hook();
		hook_play_endframe = new cHook(0x080F73D2, (int)play_endframe);
		hook_play_endframe->hook();
		hook_set_anim = new cHook(0x080D8F92, (int)set_anim);
		hook_set_anim->hook();
		hook_touch_item_auto = new cHook(0x08105B24, (int)touch_item_auto);
		hook_touch_item_auto->hook();
#endif

		cracking_hook_function(0x080EBDE0, (int)hook_BG_IsWeaponValid);
		cracking_hook_function(0x0808FD2E, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B7E62, (int)custom_va);
		cracking_hook_function(0x080904A0, (int)hook_SV_VerifyIwds_f);
		cracking_hook_function(0x08090726, (int)hook_SV_ResetPureClient_f);
		cracking_hook_function(0x0809630E, (int)custom_SV_CalcPings);
		cracking_hook_function(0x080964C4, (int)custom_SV_CheckTimeouts);
		cracking_hook_function(0x08096E1A, (int)custom_SV_MasterHeartbeat);

#if COMPILE_JUMP == 1
		cracking_hook_function(0x080DC5D4, (int)Jump_ClearState);
		cracking_hook_function(0x080DC786, (int)Jump_ReduceFriction);
		cracking_hook_function(0x080DC818, (int)Jump_ClampVelocity);
		cracking_hook_function(0x080DC684, (int)Jump_IsPlayerAboveMax);
		cracking_hook_function(0x080DC5F6, (int)Jump_GetStepHeight);
		cracking_hook_function(0x080DCB64, (int)Jump_Check);
		cracking_hook_function(0x080DC6D4, (int)Jump_ApplySlowdown);
		cracking_hook_function(0x080DC6AA, (int)Jump_ActivateSlowdown);
		cracking_hook_function(0x080DC4C8, (int)Jump_RegisterDvars);
#endif

#if COMPILE_BOTS == 1
		cracking_hook_function(0x080966B2, (int)custom_SV_BotUserMove);
#endif

#if COMPILE_RATELIMITER == 1
		cracking_hook_call(0x08095B8E, (int)hook_SVC_Info);
		cracking_hook_call(0x08095ADA, (int)hook_SVC_Status);
		cracking_hook_call(0x08095D63, (int)hook_SVC_RemoteCommand);
		cracking_hook_call(0x08095BF8, (int)hook_SV_GetChallenge);
		cracking_hook_call(0x08095CB2, (int)hook_SV_DirectConnect);
#endif

#elif COD_VERSION == COD2_1_3
		cracking_hook_call(0x080622F9, (int)hook_sv_init);
		cracking_hook_call(0x0809362A, (int)hook_sv_spawnserver);
		cracking_hook_call(0x08090BA0, (int)hook_ClientCommand);
		cracking_hook_call(0x0808DB12, (int)hook_AuthorizeState);
		cracking_hook_call(0x0808D2FA, (int)hook_isLanAddress);
		cracking_hook_call(0x0808BDC8, (int)hook_findMap);
		cracking_hook_call(0x08116ADD, (int)hook_SV_MapExists); // GSC map function
		cracking_hook_call(0x08116DC8, (int)hook_SV_MapExists); // GSC mapexists function
		cracking_hook_call(0x08090A52, (int)hook_ClientUserinfoChanged);
		cracking_hook_call(0x08070BE7, (int)Scr_GetCustomFunction);
		cracking_hook_call(0x08070E0B, (int)Scr_GetCustomMethod);
		cracking_hook_call(0x0811098E, (int)Com_DPrintf);
		cracking_hook_call(0x08082346, (int)hook_scriptError);
		cracking_hook_call(0x0808FD52, (int)hook_bad_printf);

#if COMPILE_PLAYER == 1
		cracking_hook_call(0x0808F5C7, (int)hook_gamestate_info);
#endif

		hook_gametype_scripts = new cHook(0x08110286, (int)hook_codscript_gametype_scripts);
		hook_gametype_scripts->hook();

		hook_player_collision = new cHook(0x080F5682, (int)player_collision);
		hook_player_collision->hook();
		hook_player_eject = new cHook(0x080F6E9E, (int)player_eject);
		hook_player_eject->hook();
		hook_fire_grenade = new cHook(0x0810E68E, (int)fire_grenade);
		hook_fire_grenade->hook();

#if COMPILE_PLAYER == 1
		hook_play_movement = new cHook(0x08090DAC, (int)play_movement);
		hook_play_movement->hook();
		hook_play_endframe = new cHook(0x080F7516, (int)play_endframe);
		hook_play_endframe->hook();
		hook_set_anim = new cHook(0x080D90D6, (int)set_anim);
		hook_set_anim->hook();
		hook_touch_item_auto = new cHook(0x08105C80, (int)touch_item_auto);
		hook_touch_item_auto->hook();
#endif

		cracking_hook_function(0x080EBF24, (int)hook_BG_IsWeaponValid);
		cracking_hook_function(0x0808FDC2, (int)custom_SV_WriteDownloadToClient);
		cracking_hook_function(0x080B7FA6, (int)custom_va);
		cracking_hook_function(0x08090534, (int)hook_SV_VerifyIwds_f);
		cracking_hook_function(0x080907BA, (int)hook_SV_ResetPureClient_f);
		cracking_hook_function(0x080963C8, (int)custom_SV_CalcPings);
		cracking_hook_function(0x0809657E, (int)custom_SV_CheckTimeouts);
		cracking_hook_function(0x08096ED6, (int)custom_SV_MasterHeartbeat);

#if COMPILE_JUMP == 1
		cracking_hook_function(0x080DC718, (int)Jump_ClearState);
		cracking_hook_function(0x080DC8CA, (int)Jump_ReduceFriction);
		cracking_hook_function(0x080DC95C, (int)Jump_ClampVelocity);
		cracking_hook_function(0x080DC7C8, (int)Jump_IsPlayerAboveMax);
		cracking_hook_function(0x080DC73A, (int)Jump_GetStepHeight);
		cracking_hook_function(0x080DCCA8, (int)Jump_Check);
		cracking_hook_function(0x080DC818, (int)Jump_ApplySlowdown);
		cracking_hook_function(0x080DC7EE, (int)Jump_ActivateSlowdown);
		cracking_hook_function(0x080DC60C, (int)Jump_RegisterDvars);
#endif

#if COMPILE_BOTS == 1
		cracking_hook_function(0x0809676C, (int)custom_SV_BotUserMove);
#endif

#if COMPILE_RATELIMITER == 1
		cracking_hook_call(0x08095C48, (int)hook_SVC_Info);
		cracking_hook_call(0x08095B94, (int)hook_SVC_Status);
		cracking_hook_call(0x08095E1D, (int)hook_SVC_RemoteCommand);
		cracking_hook_call(0x08095CB2, (int)hook_SV_GetChallenge);
		cracking_hook_call(0x08095D6C, (int)hook_SV_DirectConnect);
#endif

#endif

		printf("> [PLUGIN LOADED]\n");
	}

	~cCallOfDuty2Pro()
	{
		printf("> [PLUGIN UNLOADED]\n");
	}
};

cCallOfDuty2Pro *pro;

// lol, single again: because it got loaded two times now
// both now: able to load with wrapper AND directly
// IMPORTANT: file needs "lib" infront of name, otherwise it wont be loaded
// will be called when LD_PRELOAD is referencing this .so

void __attribute__ ((constructor)) lib_load(void)
{
	pro = new cCallOfDuty2Pro;
}

void __attribute__ ((destructor)) lib_unload(void)
{
	delete pro;
}
