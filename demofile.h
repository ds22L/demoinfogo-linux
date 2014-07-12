//====== Copyright � 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TOOLDEMOFILE_H
#define TOOLDEMOFILE_H

#include <string>
#include <stdint.h>

#define DEMO_HEADER_ID		"HL2DEMO"
#define DEMO_PROTOCOL		4

#if !defined( MAX_OSPATH )
#define	MAX_OSPATH		260			// max length of a filesystem pathname
#endif

typedef int32_t	 	int32;
typedef uint32_t	uint32;

// Demo messages
enum
{
	// it's a startup message, process as fast as possible
	dem_signon	= 1,
	// it's a normal network packet that we stored off
	dem_packet,
	// sync client clock to demo tick
	dem_synctick,
	// console command
	dem_consolecmd,
	// user input command
	dem_usercmd,
	// network data tables
	dem_datatables,
	// end of time.
	dem_stop,
	// a blob of binary data understood by a callback function
	dem_customdata,

	dem_stringtables,

	// Last command
	dem_lastcmd		= dem_stringtables
};

struct demoheader_t
{
	char	demofilestamp[8];				// Should be HL2DEMO
	int32	demoprotocol;					// Should be DEMO_PROTOCOL
	int32	networkprotocol;				// Should be PROTOCOL_VERSION
	char	servername[ MAX_OSPATH ];		// Name of server
	char	clientname[ MAX_OSPATH ];		// Name of client who recorded the game
	char	mapname[ MAX_OSPATH ];			// Name of map
	char	gamedirectory[ MAX_OSPATH ];	// Name of game directory (com_gamedir)
	float	playback_time;					// Time of track
	int32   playback_ticks;					// # of ticks in track
	int32   playback_frames;				// # of frames in track
	int32	signonlength;					// length of sigondata in bytes
};

#define FDEMO_NORMAL		0
#define FDEMO_USE_ORIGIN2	(1<<0)
#define FDEMO_USE_ANGLES2	(1<<1)
#define FDEMO_NOINTERP		(1<<2)	// don't interpolate between this an last view

#define MAX_SPLITSCREEN_CLIENTS	2

struct QAngle
{
	float x, y, z;
	void Init()
	{
		x = y = z = 0.0f;
	}
};
struct Vector
{
	float x, y, z;
	void Init()
	{
		x = y = z = 0.0f;
	}
};

struct democmdinfo_t
{
	// Default constructor
	democmdinfo_t() {}

	struct Split_t
	{
		Split_t()
		{
			flags = FDEMO_NORMAL;
			viewOrigin.Init();
			viewAngles.Init();
			localViewAngles.Init();

			// Resampled origin/angles
			viewOrigin2.Init();
			viewAngles2.Init();
			localViewAngles2.Init();
		}

		Split_t&	operator=(const Split_t& src )
		{
			if ( this == &src )
				return *this;

			flags = src.flags;
			viewOrigin = src.viewOrigin;
			viewAngles = src.viewAngles;
			localViewAngles = src.localViewAngles;
			viewOrigin2 = src.viewOrigin2;
			viewAngles2 = src.viewAngles2;
			localViewAngles2 = src.localViewAngles2;

			return *this;
		}

		const Vector& GetViewOrigin()
		{
			if ( flags & FDEMO_USE_ORIGIN2 )
			{
				return viewOrigin2;
			}
			return viewOrigin;
		}

		const QAngle& GetViewAngles()
		{
			if ( flags & FDEMO_USE_ANGLES2 )
			{
				return viewAngles2;
			}
			return viewAngles;
		}
		const QAngle& GetLocalViewAngles()
		{
			if ( flags & FDEMO_USE_ANGLES2 )
			{
				return localViewAngles2;
			}
			return localViewAngles;
		}

		void Reset( void )
		{
			flags = 0;
			viewOrigin2 = viewOrigin;
			viewAngles2 = viewAngles;
			localViewAngles2 = localViewAngles;
		}

		int32		flags;

		// original origin/viewangles
		Vector		viewOrigin;
		QAngle		viewAngles;
		QAngle		localViewAngles;

		// Resampled origin/viewangles
		Vector		viewOrigin2;
		QAngle		viewAngles2;
		QAngle		localViewAngles2;
	};

	void Reset( void )
	{
		for ( int i = 0; i < MAX_SPLITSCREEN_CLIENTS; ++i )
		{
			u[ i ].Reset();
		}
	}

	Split_t			u[ MAX_SPLITSCREEN_CLIENTS ];
};

class CDemoFile  
{
public:
	CDemoFile();
	virtual ~CDemoFile();

	bool	Open( const char *name );
	void	Close();

	int32	ReadRawData( char *buffer, int32 length );

	void	ReadSequenceInfo( int32 &nSeqNrIn, int32 &nSeqNrOutAck );

	void	ReadCmdInfo( democmdinfo_t& info );

	void	ReadCmdHeader( unsigned char &cmd, int32 &tick, unsigned char& playerSlot );
	
	int32	ReadUserCmd( char *buffer, int32 &size );

	demoheader_t *ReadDemoHeader();

public:
	demoheader_t    m_DemoHeader;  //general demo info

	std::string m_szFileName;

	size_t m_fileBufferPos;
	std::string m_fileBuffer;
};

uint32 ReadVarInt32( const char* buf, size_t bufLength, size_t& index );

#endif // TOOLDEMOFILE_H
