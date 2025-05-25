/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/***
 *
 *	(C) 2008 Vyacheslav Dzhura
 *
 ****/
//
// hud_redraw.cpp
//
#include <cmath>

#include "hud.h"
#include "cl_util.h"
#include "triangleapi.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "event_api.h"


#if USE_VGUI
#include "vgui_TeamFortressViewport.h"
#endif

#define MAX_LOGO_FRAMES 56

extern vec3_t v_origin;
int    g_iFrameSize;

int grgLogoFrame[MAX_LOGO_FRAMES] =
{
	1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13, 13, 13, 13, 13, 12, 11, 10, 9, 8, 14, 15,
	16, 17, 18, 19, 20, 20, 20, 20, 20, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
	29, 29, 29, 29, 29, 28, 27, 26, 25, 24, 30, 31 
};

extern int g_iVisibleMouse;

float HUD_GetFOV( void );

extern cvar_t *sensitivity;

// Think
void CHud::Think( void )
{
#if USE_VGUI
	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);
#endif

	int newfov;
	HUDLIST *pList = m_pHudList;

	while( pList )
	{
		if( pList->p->m_iFlags & HUD_ACTIVE )
			pList->p->Think();
		pList = pList->pNext;
	}

	newfov = HUD_GetFOV();
	if( newfov == 0 )
	{
		m_iFOV = default_fov->value;
	}
	else
	{
		m_iFOV = newfov;
	}

	// the clients fov is actually set in the client data update section of the hud
	// Set a new sensitivity
	if( m_iFOV == default_fov->value )
	{
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = sensitivity->value * ((float)newfov / (float)default_fov->value) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}

	// think about default fov
	if( m_iFOV == 0 )
	{
		// only let players adjust up in fov,  and only if they are not overriden by something else
		m_iFOV = Q_max( default_fov->value, 90 );  
	}

	if( gEngfuncs.IsSpectateOnly() )
	{
		m_iFOV = gHUD.m_Spectator.GetFOV(); // default_fov->value;
	}
}

const char *ReturnFrameHint(int Id)
{
	switch(Id){
	case 0: return "Button";		
	case 1: return "Object";
	case 2: return "Item";
	case 3: return "File";
	case 4: return "Documents";
	case 5: return "Book";
	case 6: return "Monitor";
	case 7: return "Computer";
	case 8: return "Compact Disc";
	case 9: return "Extinguisher";
	case 10: return "Security Card";
	case 11: return "Key";
	case 12: return "Syringe";
	case 13: return "Cleansuit";
	case 14: return "Wrench";
	case 15: return "Retinal Scanner";
	case 16: return "Health Charger";
	case 17: return "HEV Charger";
	default: return "Unknown";
	}
}

void DrawFrameCorner(int x,int y,int u1,int u2,int u3,int u4,int v1,int v2,int v3,int v4)
{
	gEngfuncs.pTriAPI->TexCoord2f( u1, v1 );
	gEngfuncs.pTriAPI->Vertex3f( x, y, 0.0 );
	gEngfuncs.pTriAPI->TexCoord2f( u2, v2 );
	gEngfuncs.pTriAPI->Vertex3f( x, y+g_iFrameSize, 0.0 );
	gEngfuncs.pTriAPI->TexCoord2f( u3, v3 );
	gEngfuncs.pTriAPI->Vertex3f(  x+g_iFrameSize, y+g_iFrameSize, 0.0 );
	gEngfuncs.pTriAPI->TexCoord2f( u4, v4 );
	gEngfuncs.pTriAPI->Vertex3f(  x+g_iFrameSize, y, 0.0  );
}

// Redraw
// step through the local data,  placing the appropriate graphics & text as appropriate
// returns 1 if they've changed, 0 otherwise
int CHud::Redraw( float flTime, int intermission )
{
	m_fOldTime = m_flTime;	// save time of previous redraw
	m_flTime = flTime;
	m_flTimeDelta = (double)( m_flTime - m_fOldTime );
	static float m_flShotTime = 0;

	// Clock was reset, reset delta
	if( m_flTimeDelta < 0 )
		m_flTimeDelta = 0;

#if USE_VGUI
	// Bring up the scoreboard during intermission
	if (gViewPort)
	{
		if( m_iIntermission && !intermission )
		{
			// Have to do this here so the scoreboard goes away
			m_iIntermission = intermission;
			gViewPort->HideCommandMenu();
			gViewPort->HideScoreBoard();
			gViewPort->UpdateSpectatorPanel();
		}
		else if( !m_iIntermission && intermission )
		{
			m_iIntermission = intermission;
			gViewPort->HideCommandMenu();
			gViewPort->HideVGUIMenu();
#if !USE_NOVGUI_SCOREBOARD
			gViewPort->ShowScoreBoard();
#endif
			gViewPort->UpdateSpectatorPanel();
			// Take a screenshot if the client's got the cvar set
			if( CVAR_GET_FLOAT( "hud_takesshots" ) != 0 )
				m_flShotTime = flTime + 1.0;	// Take a screenshot in a second
		}
	}
#else
	if( !m_iIntermission && intermission )
	{
		// Take a screenshot if the client's got the cvar set
		if( CVAR_GET_FLOAT( "hud_takesshots" ) != 0 )
			m_flShotTime = flTime + 1.0f;	// Take a screenshot in a second
	}
#endif
	if( m_flShotTime && m_flShotTime < flTime )
	{
		gEngfuncs.pfnClientCmd( "snapshot\n" );
		m_flShotTime = 0;
	}

	m_iIntermission = intermission;

	// if no redrawing is necessary
	// return 0;

	m_iHudNumbersYOffset = IsHL25() ? m_iFontHeight * 0.2 : 0;

	if( m_pCvarDraw->value )
	{
		HUDLIST *pList = m_pHudList;

		while( pList )
		{
			if ( !m_bAlienMode )
			{
				if ( !intermission )
				{
					if ( (pList->p->m_iFlags & HUD_ACTIVE) && !(m_iHideHUDDisplay & HIDEHUD_ALL) )
						pList->p->Draw(flTime);
				}
				else
				{  // it's an intermission,  so only draw hud elements that are set to draw during intermissions
					if ( pList->p->m_iFlags & HUD_INTERMISSION )
						pList->p->Draw( flTime );
				}
			} else
			{  // alien mode!!!
				if ( pList->p->m_iFlags & HUD_ALIEN )
					if ( (pList->p->m_iFlags & HUD_ACTIVE) && !(m_iHideHUDDisplay & HIDEHUD_ALL) )
						pList->p->Draw( flTime );
			}

			pList = pList->pNext;
		}
	}

	// are we in demo mode? do we need to draw the logo in the top corner?
	if( m_iLogo )
	{
		int x, y, i;

		if( m_hsprLogo == 0 )
			m_hsprLogo = LoadSprite( "sprites/%d_logo.spr" );

		SPR_Set( m_hsprLogo, 250, 250, 250 );

		x = SPR_Width( m_hsprLogo, 0 );
		x = ScreenWidth - x;
		y = SPR_Height( m_hsprLogo, 0 ) / 2;

		// Draw the logo at 20 fps
		int iFrame = (int)( flTime * 20 ) % MAX_LOGO_FRAMES;
		i = grgLogoFrame[iFrame] - 1;

		SPR_DrawAdditive( i, x, y, NULL );
	}

	// draw selection frame around entity
	if (m_iFrameIndex !=0)
	{
        vec3_t v_center, v_maxs;
		
		gEngfuncs.pTriAPI->WorldToScreen(m_vAimFrameCoords,v_center);
		gEngfuncs.pTriAPI->WorldToScreen(m_vAimFrameMaxs,v_maxs);
		
		v_center[0] = XPROJECT(v_center[0]);
		v_center[1] = YPROJECT(v_center[1]);
		v_center[2] = 0.0f;
		v_maxs[0] = XPROJECT(v_maxs[0]);
		v_maxs[1] = YPROJECT(v_maxs[1]);
		v_maxs[2] = 0.0f;

		g_iFrameSize = m_iFrameSize;

		gEngfuncs.pTriAPI->SpriteTexture( (struct model_s *)pFrameTexture, 0 );
		gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd );
        gEngfuncs.pTriAPI->Begin( TRI_QUADS ); 
		
		if (v_center[0]>v_maxs[0])
		{
			//right bottom
			DrawFrameCorner(v_center[0],v_center[1],1,1,0,0,1,0,0,1);
			//left bottom
			DrawFrameCorner(v_center[0]-(v_center[0]-v_maxs[0]),v_center[1],1,0,0,1,0,0,1,1);
			//left up
			DrawFrameCorner(v_maxs[0], v_maxs[1],0,0,1,1,0,1,1,0);
			//left right
			DrawFrameCorner(v_maxs[0]+(v_center[0]-v_maxs[0]),v_maxs[1],0,1,1,0,1,1,0,0);
		}
		else
		{
			//right bottom
			DrawFrameCorner(v_center[0],v_center[1],1,0,0,1,0,0,1,1);
			//left bottom
			DrawFrameCorner(v_center[0]-(v_center[0]-v_maxs[0]),v_center[1],1,1,0,0,1,0,0,1);
			//left up
			DrawFrameCorner(v_maxs[0],v_maxs[1],0,1,1,0,1,1,0,0);
			//left right
			DrawFrameCorner(v_maxs[0]+(v_center[0]-v_maxs[0]),v_maxs[1],0,0,1,1,0,1,1,0);
		}
		gEngfuncs.pTriAPI->End();
		gEngfuncs.pTriAPI->RenderMode( kRenderNormal );

		//to make that working, modify in decay.dll player.cpp blank message value of frame kind to -1
		//if (!m_iFrameKind==-1)
		if (m_iFrameKind != -1)
		{
			const char* pszMes = ReturnFrameHint(m_iFrameKind);
			gHUD.DrawHudString( v_maxs[0]+5, v_maxs[1]+4, ScreenWidth, pszMes, 255, 180, 0 );
		}
	}

	/*
	char szMes[255];
	if ( m_bAlienMode )
	  sprintf( szMes, "Alien slave (vortigaunt) mode on" ); 
	else
	  sprintf( szMes, "Normal mode" );
  */

	//ReturnFrameHint(szMes,m_iFrameKind);
//	gHUD.DrawHudString( 10, 10, 512, szMes, 255, 180, 0 );

	//draw sun
    if (m_iLensIndex !=0)
	{
		vec3_t screen,ors;
		float tN[9];
		tN[0]=1.0;
		tN[1]=0.8;
		tN[2]=0.7;
		tN[3]=0.5;
		tN[4]=0.4;
        tN[5]=0.25;
		tN[6]=0.1;
		tN[7]=-0.1;
		tN[8]=-0.2;
		
		cl_entity_t *ent = gEngfuncs.GetEntityByIndex(m_iLensIndex);
		if (ent)
		{
			vec3_t  forward, right, up;
	        AngleVectors ( ent->angles, forward, right, up );//get f/r/u vectors

			pmtrace_t tr;
			gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
			gEngfuncs.pEventAPI->EV_PlayerTrace( (float *)&v_origin, v_origin + (forward+up)/2 * 8192, PM_GLASS_IGNORE, -1, &tr );
			//pmtrace_t tr = *(gEngfuncs.PM_TraceLine( (float *)&v_origin, v_origin + (forward+up)/2 * 8192, 0, 2, -1 )); // PM_GLASS_IGNORE
			if (gEngfuncs.PM_PointContents( tr.endpos, NULL )!=CONTENTS_SKY)
			{
				return 0;
			}

			if (!gEngfuncs.pTriAPI->WorldToScreen(tr.endpos,ors)) 
			{
				int LWidth = 0, LHeight = 0;
				
				ors[0] = XPROJECT(ors[0]);
				ors[1] = YPROJECT(ors[1]);
				ors[2] = 0.0f;
				
				if (ors[0]<0 || ors[0]>ScreenWidth) return 0;
				if (ors[1]<0 || ors[1]>ScreenHeight) return 0;
				
				for (int i=0;i<9;i++)
				{
					if (i!=0) 
						SPR_Set(m_hsprLens[i], 50, 50, 50); //was 250 
					else 
						SPR_Set(m_hsprLens[i], 100, 100, 100); //make sun brighter then other lens
					LWidth = SPR_Width(m_hsprLens[i],0);
					LHeight = SPR_Height(m_hsprLens[i],0);
					
					screen=ors;
					screen[0] = ScreenWidth/2+(screen[0]-ScreenWidth/2)*tN[i];
					screen[1] = ScreenHeight/2+(screen[1]-ScreenHeight/2)*tN[i];
					screen[0] = screen[0]-(LWidth/2);
					screen[1] = screen[1]-(LHeight/2);
					
					SPR_DrawAdditive(0, screen[0],screen[1], NULL);
				}
			}
		}
	}

	/*
	if( g_iVisibleMouse )
	{
		void IN_GetMousePos( int *mx, int *my );
		int mx, my;

		IN_GetMousePos( &mx, &my );

		if( m_hsprCursor == 0 )
		{
			m_hsprCursor = SPR_Load( "sprites/cursor.spr" );
		}

		SPR_Set( m_hsprCursor, 250, 250, 250 );

		// Draw the logo at 20 fps
		SPR_DrawAdditive( 0, mx, my, NULL );
	}
	*/

	return 1;
}

void ScaleColors( int &r, int &g, int &b, int a )
{
	float x = (float)a / 255;
	r = (int)( r * x );
	g = (int)( g * x );
	b = (int)( b * x );
}

const unsigned char colors[8][3] =
{
{127, 127, 127}, // additive cannot be black
{255,   0,   0},
{  0, 255,   0},
{255, 255,   0},
{  0,   0, 255},
{  0, 255, 255},
{255,   0, 255},
{240, 180,  24}
};

int CHud::DrawHudString( int xpos, int ypos, int iMaxX, const char *szIt, int r, int g, int b )
{
	if( hud_textmode->value == 2 )
	{
		gEngfuncs.pfnDrawSetTextColor( r / 255.0, g / 255.0, b / 255.0 );
		return gEngfuncs.pfnDrawConsoleString( xpos, ypos, (char*) szIt );
	}

	// xash3d: reset unicode state
	TextMessageDrawChar( 0, 0, 0, 0, 0, 0 );

	// draw the string until we hit the null character or a newline character
	for( ; *szIt != 0 && *szIt != '\n'; szIt++ )
	{
		int w = gHUD.m_scrinfo.charWidths['M'];
		if( xpos + w  > iMaxX )
			return xpos;
		if( ( *szIt == '^' ) && ( *( szIt + 1 ) >= '0') && ( *( szIt + 1 ) <= '7') )
		{
			szIt++;
			r = colors[*szIt - '0'][0];
			g = colors[*szIt - '0'][1];
			b = colors[*szIt - '0'][2];
			if( !*(++szIt) )
				return xpos;
		}
		int c = (unsigned int)(unsigned char)*szIt;

		xpos += TextMessageDrawChar( xpos, ypos, c, r, g, b );
	}

	return xpos;
}

int DrawUtfString( int xpos, int ypos, int iMaxX, const char *szIt, int r, int g, int b )
{
	if (IsXashFWGS())
	{
		// xash3d: reset unicode state
		gEngfuncs.pfnVGUI2DrawCharacterAdditive( 0, 0, 0, 0, 0, 0, 0 );

		// draw the string until we hit the null character or a newline character
		for( ; *szIt != 0 && *szIt != '\n'; szIt++ )
		{
			int w = gHUD.m_scrinfo.charWidths['M'];
			if( xpos + w  > iMaxX )
				return xpos;
			if( ( *szIt == '^' ) && ( *( szIt + 1 ) >= '0') && ( *( szIt + 1 ) <= '7') )
			{
				szIt++;
				r = colors[*szIt - '0'][0];
				g = colors[*szIt - '0'][1];
				b = colors[*szIt - '0'][2];
				if( !*(++szIt) )
					return xpos;
			}
			int c = (unsigned int)(unsigned char)*szIt;
			xpos += gEngfuncs.pfnVGUI2DrawCharacterAdditive( xpos, ypos, c, r, g, b, 0 );
		}
		return xpos;
	}
	else
	{
		return gHUD.DrawHudString(xpos, ypos, iMaxX, szIt, r, g, b);
	}
}

int CHud::DrawHudStringLen( const char *szIt )
{
	int l = 0;
	for( ; *szIt != 0 && *szIt != '\n'; szIt++ )
	{
		l += gHUD.m_scrinfo.charWidths[(unsigned char)*szIt];
	}
	return l;
}

int CHud::DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b )
{
	char szString[32];
	sprintf( szString, "%d", iNumber );
	return DrawHudStringReverse( xpos, ypos, iMinX, szString, r, g, b );
}

// draws a string from right to left (right-aligned)
int CHud::DrawHudStringReverse( int xpos, int ypos, int iMinX, const char *szString, int r, int g, int b )
{
	// find the end of the string
	for( const char *szIt = szString; *szIt != 0; szIt++ )
		xpos -= gHUD.m_scrinfo.charWidths[(unsigned char)*szIt];
	if( xpos < iMinX )
		xpos = iMinX;
	DrawHudString( xpos, ypos, gHUD.m_scrinfo.iWidth, szString, r, g, b );
	return xpos;
}

int CHud::DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b )
{
	int iWidth = GetSpriteRect( m_HUD_number_0 ).right - GetSpriteRect( m_HUD_number_0 ).left;
	int k;
	
	if( iNumber > 0 )
	{
		// SPR_Draw 100's
		if( iNumber >= 100 )
		{
			k = iNumber / 100;
			SPR_Set( GetSprite( m_HUD_number_0 + k ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect( m_HUD_number_0 + k ) );
			x += iWidth;
		}
		else if( iFlags & ( DHN_3DIGITS ) )
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw 10's
		if( iNumber >= 10 )
		{
			k = ( iNumber % 100 ) / 10;
			SPR_Set( GetSprite( m_HUD_number_0 + k ), r, g, b );
			SPR_DrawAdditive( 0, x, y, &GetSpriteRect( m_HUD_number_0 + k ) );
			x += iWidth;
		}
		else if( iFlags & ( DHN_3DIGITS | DHN_2DIGITS ) )
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		k = iNumber % 10;
		SPR_Set( GetSprite( m_HUD_number_0 + k ), r, g, b );
		SPR_DrawAdditive( 0,  x, y, &GetSpriteRect( m_HUD_number_0 + k ) );
		x += iWidth;
	}
	else if( iFlags & DHN_DRAWZERO )
	{
		SPR_Set( GetSprite( m_HUD_number_0 ), r, g, b );

		// SPR_Draw 100's
		if( iFlags & ( DHN_3DIGITS ) )
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		if( iFlags & ( DHN_3DIGITS | DHN_2DIGITS ) )
		{
			//SPR_DrawAdditive( 0, x, y, &rc );
			x += iWidth;
		}

		// SPR_Draw ones
		SPR_DrawAdditive( 0,  x, y, &GetSpriteRect( m_HUD_number_0 ) );
		x += iWidth;
	}

	return x;
}

int CHud::GetNumWidth( int iNumber, int iFlags )
{
	if( iFlags & ( DHN_3DIGITS ) )
		return 3;

	if( iFlags & ( DHN_2DIGITS ) )
		return 2;

	if( iNumber <= 0 )
	{
		if( iFlags & ( DHN_DRAWZERO ) )
			return 1;
		else
			return 0;
	}

	if( iNumber < 10 )
		return 1;

	if( iNumber < 100 )
		return 2;

	return 3;
}	

void CHud::DrawDarkRectangle( int x, int y, int wide, int tall )
{
	//gEngfuncs.pTriAPI->RenderMode( kRenderTransTexture );
	gEngfuncs.pfnFillRGBABlend( x, y, wide, tall, 0, 0, 0, 255 * 0.6 );
	FillRGBA( x + 1, y, wide - 1, 1, 255, 140, 0, 255 );
	FillRGBA( x, y, 1, tall - 1, 255, 140, 0, 255 );
	FillRGBA( x + wide - 1, y + 1, 1, tall - 1, 255, 140, 0, 255 );
	FillRGBA( x, y + tall - 1, wide - 1, 1, 255, 140, 0, 255 );
}
