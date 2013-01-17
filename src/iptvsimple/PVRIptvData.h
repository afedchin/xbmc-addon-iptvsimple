#pragma once
/*
 *      Copyright (C) 2011 Pulse-Eight
 *      http://www.pulse-eight.com/
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

#include <vector>
#include "platform/util/StdString.h"
#include "client.h"
#include "platform/threads/threads.h"

struct PVRIptvEpgEntry
{
  int         iBroadcastId;
  std::string strTitle;
  int         iChannelId;
  time_t      startTime;
  time_t      endTime;
  std::string strPlotOutline;
  std::string strPlot;
  std::string strIconPath;
  int         iGenreType;
  int         iGenreSubType;
//  time_t      firstAired;
//  int         iParentalRating;
//  int         iStarRating;
//  bool        bNotify;
//  int         iSeriesNumber;
//  int         iEpisodeNumber;
//  int         iEpisodePartNumber;
//  std::string strEpisodeName;
};

struct PVRIptvEpgChannel
{
  int         iId;
  std::string strName;
  std::vector<PVRIptvEpgEntry> epg;
};

struct PVRIptvChannel
{
  bool                    bRadio;
  int                     iUniqueId;
  int                     iChannelNumber;
  int                     iEncryptionSystem;
  std::string             strChannelName;
  std::string             strIconPath;
  std::string             strStreamURL;

  std::vector<PVRIptvEpgEntry> epg;

  int                     iTvgId;
  std::string             strTvgName;
};

struct PVRIptvRecording
{
  int         iDuration;
  int         iGenreType;
  int         iGenreSubType;
  std::string strChannelName;
  std::string strPlotOutline;
  std::string strPlot;
  std::string strRecordingId;
  std::string strStreamURL;
  std::string strTitle;
  time_t      recordingTime;
};

struct PVRIptvChannelGroup
{
  bool             bRadio;
  int              iGroupId;
  std::string      strGroupName;
  std::vector<int> members;
};

class PVRIptvData : public PLATFORM::CThread
{
public:
  PVRIptvData(void);
  virtual ~PVRIptvData(void);

  virtual int GetChannelsAmount(void);
  virtual PVR_ERROR GetChannels(ADDON_HANDLE handle, bool bRadio);
  virtual bool GetChannel(const PVR_CHANNEL &channel, PVRIptvChannel &myChannel);

  virtual int GetChannelGroupsAmount(void);
  virtual PVR_ERROR GetChannelGroups(ADDON_HANDLE handle, bool bRadio);
  virtual PVR_ERROR GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP &group);

  virtual PVR_ERROR GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL &channel, time_t iStart, time_t iEnd);

  virtual int GetRecordingsAmount(void);
  virtual PVR_ERROR GetRecordings(ADDON_HANDLE handle);

  virtual std::string GetSettingsFile() const;
  virtual std::string GetPlylistFile() const;
  virtual CStdString GetLogoFile(CStdString &pstrLogo) const;

protected:
  virtual bool LoadPlayList(void);
  virtual bool LoadEPG(void);
  virtual CStdString GetFileContents(CStdString& url);
  virtual PVRIptvEpgChannel * FindEgpChannelById(int iId);
  virtual PVRIptvEpgChannel * FindEgpChannelByPvrChannel(PVRIptvChannel &pvrChannel);
  virtual int ParseDateTime(CStdString strDate, bool iDateFormat = true);

  virtual bool gzipInflate( const std::string& compressedBytes, std::string& uncompressedBytes);

  virtual CStdString GetCachedFileContents(const char * strCachedName, const char * strFilePath);

protected:
  virtual void *Process(void);

private:
  std::vector<PVRIptvEpgChannel>	m_egpChannels;
  std::vector<PVRIptvChannelGroup>	m_groups;
  std::vector<PVRIptvChannel>		m_channels;
  std::vector<PVRIptvRecording>		m_recordings;
  bool								m_bEGPLoaded;
  time_t							m_iEpgStart;
  CStdString						m_strDefaultIcon;
  CStdString						m_strXMLTVUrl;
  CStdString						m_strM3uUrl;
};
