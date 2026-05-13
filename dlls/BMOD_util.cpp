// ---------------------------------------------------------------
// BubbleMod
// 
// AUTHOR
//        Tyler Lund <halflife@bubblemod.org>
//
// LICENSE                                                            
//                                                                    
//        Permission is granted to anyone to use this  software  for  
//        any purpose on any computer system, and to redistribute it  
//        in any way, subject to the following restrictions:          
//                                                                    
//        1. The author is not responsible for the  consequences  of  
//           use of this software, no matter how awful, even if they  
//           arise from defects in it.                                
//        2. The origin of this software must not be misrepresented,  
//           either by explicit claim or by omission.                 
//        3. Altered  versions  must  be plainly marked as such, and  
//           must  not  be  misrepresented  (by  explicit  claim  or  
//           omission) as being the original software.                
//        3a. It would be nice if I got  a  copy  of  your  improved  
//            version  sent to halflife@bubblemod.org. 
//        4. This notice must not be removed or altered.              
//                                                                    
// ---------------------------------------------------------------
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include <time.h>
#include "shake.h"
#include "decals.h"
#include "player.h"
#include "weapons.h"
#include "gamerules.h"
#include <string.h>
#include <time.h>

// Cut BMOD_util.cpp version for BetterDM

BOOL UTIL_OBB_LineTest(Vector vecSrc, Vector vecDst, Vector boxP, Vector boxE)
{
	// Unit vector in direction of line.
	Vector l = vecDst - vecSrc;

	// Half of line segment length
	float hl = fabs(l.Length() / 2);
	
	l = l.Normalize();

	// midpoint of line segemnt.
	Vector mid = (vecDst + vecSrc) / 2;
	
	// The separating axis.
	Vector T = boxP - mid;
/*
	char sPlayers[256] = "";
	sprintf(sPlayers, "Start: %f, %f, %f\nEnd: %f, %f, %f\nhl: %f\nunit: %f, %f, %f\n\n", vecSrc.x, vecSrc.y, vecSrc.z,
			vecDst.x, vecDst.y, vecDst.z, hl, l.x, l.y, l.z);
	g_engfuncs.pfnServerPrint( sPlayers );


	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
                WRITE_BYTE( TE_BEAMPOINTS );
                WRITE_COORD(mid.x);
                WRITE_COORD(mid.y);
                WRITE_COORD(mid.z);
                WRITE_COORD(mid.x + (hl * l.x));
                WRITE_COORD(mid.y + (hl * l.y));
                WRITE_COORD(mid.z + (hl * l.z));
                WRITE_SHORT( g_sModelIndexLaser );
                WRITE_BYTE( 0 ); // Starting frame
                WRITE_BYTE( 0  ); // framerate * 0.1
                WRITE_BYTE( 255 ); // life * 0.1
                WRITE_BYTE( 10 ); // width
                WRITE_BYTE( 0 ); // noise
                WRITE_BYTE( 0 ); // color r,g,b
                WRITE_BYTE( 0 ); // color r,g,b
                WRITE_BYTE( 255 ); // color r,g,b
                WRITE_BYTE( 255 ); // brightness
                WRITE_BYTE( 0 ); // scroll speed
    MESSAGE_END();
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
                WRITE_BYTE( TE_BEAMPOINTS );
                WRITE_COORD(mid.x);
                WRITE_COORD(mid.y);
                WRITE_COORD(mid.z);
                WRITE_COORD(mid.x - (hl * l.x));
                WRITE_COORD(mid.y - (hl * l.y));
                WRITE_COORD(mid.z - (hl * l.z));
                WRITE_SHORT( g_sModelIndexLaser );
                WRITE_BYTE( 0 ); // Starting frame
                WRITE_BYTE( 0  ); // framerate * 0.1
                WRITE_BYTE( 255 ); // life * 0.1
                WRITE_BYTE( 10 ); // width
                WRITE_BYTE( 0 ); // noise
                WRITE_BYTE( 255 ); // color r,g,b
                WRITE_BYTE( 0 ); // color r,g,b
                WRITE_BYTE( 0 ); // color r,g,b
                WRITE_BYTE( 255 ); // brightness
                WRITE_BYTE( 0 ); // scroll speed
    MESSAGE_END();
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
                WRITE_BYTE( TE_BEAMPOINTS );
                WRITE_COORD(mid.x);
                WRITE_COORD(mid.y);
                WRITE_COORD(mid.z);
                WRITE_COORD(boxP.x);
                WRITE_COORD(boxP.y);
                WRITE_COORD(boxP.z);
                WRITE_SHORT( g_sModelIndexLaser );
                WRITE_BYTE( 0 ); // Starting frame
                WRITE_BYTE( 0  ); // framerate * 0.1
                WRITE_BYTE( 255 ); // life * 0.1
                WRITE_BYTE( 10 ); // width
                WRITE_BYTE( 0 ); // noise
                WRITE_BYTE( 255 ); // color r,g,b
                WRITE_BYTE( 255 ); // color r,g,b
                WRITE_BYTE( 255 ); // color r,g,b
                WRITE_BYTE( 255 ); // brightness
                WRITE_BYTE( 0 ); // scroll speed
    MESSAGE_END();
*/
	// The box is assumed to be axis aligned to the world, so we start
	// with those trivial cases.
	if( ( fabs(T.x) > boxE.x + hl * fabs(l.x) ) ||
		( fabs(T.y) > boxE.y + hl * fabs(l.y) ) ||
		( fabs(T.z) > boxE.z + hl * fabs(l.z) ) )
		return FALSE;

	// Now we check the box transformed into the line space.
	if ( ( fabs(T.y*l.z - T.z*l.y) > boxE.y*fabs(l.z) + boxE.z*fabs(l.y) ) ||
		 ( fabs(T.z*l.x - T.x*l.z) > boxE.z*fabs(l.x) + boxE.x*fabs(l.z) ) ||
		 ( fabs(T.x*l.y - T.y*l.x) > boxE.x*fabs(l.y) + boxE.y*fabs(l.x) ) )
		 return FALSE;

	return TRUE;
}
