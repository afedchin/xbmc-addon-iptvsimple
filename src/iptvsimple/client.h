#pragma once
/*
 *      Copyright (C) 2013 Anton Fedchin
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "libXBMC_addon.h"
#include "libXBMC_pvr.h"

extern bool                          m_bCreated;
extern std::string                   g_strUserPath;
extern std::string                   g_strClientPath;
extern ADDON::CHelper_libXBMC_addon *XBMC;
extern CHelper_libXBMC_pvr          *PVR;

extern std::string   g_strM3UPath;
extern std::string   g_strTvgPath;
extern std::string   g_strLogoPath;
extern int           g_iEPGTimeShift;
extern bool          g_bApplyTSToAll;

extern std::string PathCombine(const char* strPath, const char * strFileName);
extern std::string GetClientFilePath(const char * strFileName);
extern std::string GetUserFilePath(const char * strFileName);