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

#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include "tinyxml/XMLUtils.h"
#include "PVRDemoData.h"

#define M3U_START_MARKER "#EXTM3U"
#define M3U_INFO_MARKER  "#EXTINF"
#define TVG_INFO_NAME_MARKER  "tvg-name=\""
#define TVG_INFO_LOGO_MARKER  "tvg-logo=\""
#define GROUP_NAME_MARKER  "group-title=\""

using namespace std;
using namespace ADDON;

PVRDemoData::PVRDemoData(void)
{
  m_iEpgStart = -1;
  m_strDefaultIcon =  "http://www.royalty-free.tv/news/wp-content/uploads/2011/06/cc-logo1.jpg";
  m_strDefaultMovie = "";
  m_strDefaultXMLTVUrl = "http://www.teleguide.info/download/new3/xmltv.xml";

  if (LoadPlayList() && m_channels.size() > 0)
  {
	  //XBMC->Log(LOG_NOTICE, "Loaded %s channels.", m_channels.size());
	  //if (LoadEPG())
	  {
			//XBMC->Log(LOG_NOTICE, "%s has loaded the data.", "LoadEPG");
	  }
  }
}

PVRDemoData::~PVRDemoData(void)
{
  m_channels.clear();
  m_groups.clear();
}

std::string PVRDemoData::GetSettingsFile() const
{
  string settingFile = g_strClientPath;
  if (settingFile.at(settingFile.size() - 1) == '\\' ||
      settingFile.at(settingFile.size() - 1) == '/')
    settingFile.append("PVRDemoAddonSettings.xml");
  else
    settingFile.append("/PVRDemoAddonSettings.xml");
  return settingFile;
}

std::string PVRDemoData::GetPlylistFile() const
{
  string settingFile = g_strClientPath;
  if (settingFile.at(settingFile.size() - 1) == '\\' ||
      settingFile.at(settingFile.size() - 1) == '/')
    settingFile.append("iptv.m3u");
  else
    settingFile.append("/iptv.m3u");
  return settingFile;
}

CStdString PVRDemoData::GetLogoFile(CStdString &pstrLogo) const
{
  CStdString logoFile = g_strClientPath;

  if (logoFile.at(logoFile.size() - 1) == '\\' ||
      logoFile.at(logoFile.size() - 1) == '/')
    logoFile.append("icons/");
  else
    logoFile.append("/icons/");

  logoFile.append(pstrLogo);
  logoFile.append(".bmp");

  return logoFile;
}

CStdString PVRDemoData::GetHttp(CStdString& url)
{
  CStdString strResult;
  void* fileHandle = XBMC->OpenFile(url.c_str(), 0);
  if (fileHandle)
  {
    char buffer[1024];
    while (int bytesRead = XBMC->ReadFile(fileHandle, buffer, 1024))
      strResult.append(buffer, bytesRead);
    XBMC->CloseFile(fileHandle);
  }
  return strResult;
}


bool PVRDemoData::LoadEPG(void) 
{
	CStdString strXML = GetHttp(m_strDefaultXMLTVUrl);
	if (strXML.IsEmpty()) 
	{
		return false;
	}

    TiXmlDocument xmlDoc;
	if (!xmlDoc.Parse(strXML))
	{
		XBMC->Log(LOG_ERROR, "invalid demo data (no/invalid data file found at '%s')", strXML.c_str());
		return false;
	}

    TiXmlElement *pRootElement = xmlDoc.RootElement();
    if (strcmp(pRootElement->Value(), "tv") != 0)
	{
		XBMC->Log(LOG_ERROR, "invalid demo data (no <tv> tag found)");
		return false;
	}

	std::map<CStdString, int> dic;

    TiXmlNode *pChannelNode = NULL;
    while ((pChannelNode = pRootElement->IterateChildren(pChannelNode)) != NULL)
	{
		TiXmlElement *pElement = pChannelNode->ToElement();
		if (!strcmp(pChannelNode->Value(), "channel"))
		{
			CStdString strId;
			CStdString strTmp;
			CStdString strName;
	
			strId = pElement->Attribute("id");
			if (!XMLUtils::GetString(pChannelNode, "display-name", strName))
			{
				continue;
			}

			strName.Replace(' ', '_');

			PVRDemoChannel found;
			if (FindChannelByTVG(strName.c_str(), found))
			{
				dic.insert(std::make_pair(strId, found.iUniqueId));
			}
		}
		else if (!strcmp(pChannelNode->Value(), "programme") && dic.size() > 0) 
		{
			CStdString strChannel = pElement->Attribute("channel");
			map<CStdString, int>::iterator it = dic.find(strChannel);

			if (it == dic.end())
				continue;

			CStdString strTitle;
			CStdString strCategory;
			CStdString strDesc;
			int iTmpStart;
			int iTmpEnd;
			int iChannelId = it->second;

			PVRDemoChannel &channel = m_channels.at(iChannelId - 1);

			iTmpStart = ParseDateTime(pElement->Attribute("start"));
			iTmpEnd = ParseDateTime(pElement->Attribute("stop"));

			XMLUtils::GetString(pChannelNode, "title", strTitle);
			XMLUtils::GetString(pChannelNode, "category", strCategory);
			XMLUtils::GetString(pChannelNode, "desc", strDesc);

			PVRDemoEpgEntry entry;
			entry.iChannelId = channel.iUniqueId;
			entry.strTitle = strTitle;
			entry.strPlot = strTitle;
			entry.strPlotOutline = strDesc;
			entry.startTime = iTmpStart;
			entry.endTime = iTmpEnd;

			channel.epg.push_back(entry);
		}
	}

/*
	if (dic.size() == 0) 
	{
		XBMC->Log(LOG_NOTICE, "%s Unable to find channels.", __FUNCTION__);
		return false;
	}


	n = xNode.nChildNode("programme");
	for(int j = 0; j < n; j++)
	{
		CStdString strChannel;
		XMLNode xTmp = xNode.getChildNode("programme", j);

		strChannel = xTmp.getAttribute("channel");
		map<CStdString, int>::iterator it = dic.find(strChannel);

		if (it != dic.end())
		{
			CStdString strTitle;
			CStdString strCategory;
			CStdString strDesc;
			int iTmpStart;
			int iTmpEnd;
			int iChannelId = it->second;

			PVRDemoChannel &channel = m_channels.at(iChannelId - 1);

			iTmpStart = ParseDateTime(xTmp.getAttribute("start"));
			iTmpEnd = ParseDateTime(xTmp.getAttribute("stop"));

			GetString(xTmp, "title", strTitle);
			GetString(xTmp, "category", strCategory);
			GetString(xTmp, "desc", strDesc);

			PVRDemoEpgEntry entry;
			entry.iChannelId = channel.iUniqueId;
			entry.strTitle = strTitle;
			entry.strPlot = strTitle;
			entry.strPlotOutline = strDesc;
			entry.startTime = iTmpStart;
			entry.endTime = iTmpEnd;

			channel.epg.push_back(entry);
		}
	}
*/
	return true;
}

int PVRDemoData::ParseDateTime(CStdString strDate, bool iDateFormat)
{
  _tzset();

  struct tm timeinfo;
  memset(&timeinfo, 0, sizeof(tm));

  if (iDateFormat)
    sscanf(strDate, "%04d%02d%02d%02d%02d%02d", &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday, &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec);
  else
    sscanf(strDate, "%02d.%02d.%04d%02d:%02d:%02d", &timeinfo.tm_mday, &timeinfo.tm_mon, &timeinfo.tm_year, &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec);

  timeinfo.tm_mon  -= 1;
  timeinfo.tm_year -= 1900;
  timeinfo.tm_isdst = -1;

  time_t t = mktime(&timeinfo) - timezone;

  if (daylight == 0 && timeinfo.tm_isdst != 0)
        t += 3600;
    else if (daylight != 0 && timeinfo.tm_isdst == 0)
        t -= 3600;

  return t;
}

bool PVRDemoData::FindChannelByTVG(const char* strTag, PVRDemoChannel &found)
{
	for(int i = 0, max = m_channels.size(); i < max; i++)
	{
		PVRDemoChannel channel = m_channels.at(i);
		if (!strcmp(channel.strTvgName.c_str(), strTag))
		{
			found = channel;
			return true;
		}
	}

	return false;
}

bool PVRDemoData::LoadPlayList(void) 
{
	CStdString strPlaylistFile = GetPlylistFile();
	CStdString strPlayList = GetHttp(strPlaylistFile);
	if (strPlayList.IsEmpty()) 
	{
		return false;
	}

	_stat64 buffer;
	XBMC->StatFile(strPlaylistFile, &buffer);
	time_t tmp = buffer.st_mtime;
	tmp += 60 * 4;
	tm timeinfo = *localtime(&tmp);

	

	//time_t tmp = (time_t)(buffer.st_mtime);

	char szLine[4096];
	CStdString strLine;
	CStdString strInfo = "";

  /* load channels */
  int iUniqueChannelId = 0;
  int iUniqueGroupId = 0;
  int iChannelNum = 0;
  bool isfirst = true;

  PVRDemoChannel tmpChannel;
  stringstream stream(strPlayList);

  //while(file.getline(szLine, 1024)) 
  while(stream.getline(szLine, 1024))
  {
	  strLine = szLine;
	  strLine.TrimRight(" \t\r\n");
      strLine.TrimLeft(" \t");

	  if (isfirst) 
	  {
		  isfirst = false;

		  if (strLine.Left(3) == "\xEF\xBB\xBF")
		  {
			  strLine.Delete(0, 3);
		  }

		  if (strLine.Left((int)strlen(M3U_START_MARKER)) == M3U_START_MARKER) 
		  {
			  continue;
		  }
		  else
		  {
			  // bad file
			  return false;
		  }
	  }

	  CStdString strChnlName;
	  CStdString strTvgName;
	  CStdString strTvgLogo;
	  CStdString strGroupName;

	  strLine = XBMC->UnknownToUTF8(strLine);

	  if (strLine.Left((int)strlen(M3U_INFO_MARKER)) == M3U_INFO_MARKER) 
	  {
		  // parse info
		  int iColon = (int)strLine.find(":");
		  int iComma = (int)strLine.find(",");
		  if (iColon >= 0 && iComma >= 0 && iComma > iColon) 
		  {
			  // read name
			  iComma++;
			  strChnlName = strLine.Right((int)strLine.size() - iComma);
			  tmpChannel.strChannelName = strChnlName;

			  // read info
			  iColon++;
			  CStdString strLength = strLine.Mid(iColon, iComma - iColon);

			  int iTvgName = (int)strLength.find(TVG_INFO_NAME_MARKER);
			  int iTvgLogo = (int)strLength.find(TVG_INFO_LOGO_MARKER);
			  int iGroupName = (int)strLength.find(GROUP_NAME_MARKER);

			  if (iTvgName >= 0) 
			  {
				  iTvgName += sizeof(TVG_INFO_NAME_MARKER) - 1;
				  int iTvgNameEnd = (int)strLength.Find('"', iTvgName);
				  
				  if (iTvgNameEnd >= 0)
				  {
					  strTvgName = strLength.substr(iTvgName, iTvgNameEnd - iTvgName);
					  tmpChannel.strTvgName = strTvgName;
				  }
			  }

			  if (iTvgLogo >= 0) 
			  {
				  iTvgLogo += sizeof(TVG_INFO_LOGO_MARKER) - 1;
				  int iTvgLogoEnd = (int)strLength.Find('"', iTvgLogo);
				  
				  if (iTvgLogoEnd >= 0)
				  {
					  strTvgLogo = strLength.substr(iTvgLogo, iTvgLogoEnd - iTvgLogo);
					  if (!strTvgLogo.IsEmpty())
						  tmpChannel.strIconPath = GetLogoFile(strTvgLogo);
				  }
			  }

			  if (iGroupName >= 0) 
			  {
				  iGroupName += sizeof(GROUP_NAME_MARKER) - 1;
				  int iGroupNameEnd = (int)strLength.Find('"', iGroupName);
				  
				  if (iGroupNameEnd >= 0)
				  {
					  strGroupName = strLength.substr(iGroupName, iGroupNameEnd - iGroupName);

					  PVRDemoChannelGroup group;
					  group.strGroupName = strGroupName;
					  group.iGroupId = ++iUniqueGroupId;
					  group.bRadio = false;

					  m_groups.push_back(group);
				  }
			  }
		  }
	  } 
	  else 
	  {
		  strChnlName = tmpChannel.strChannelName;
		  if (strChnlName.IsEmpty())
		  {
			  strChnlName = strLine;
		  }

		  if (!strChnlName.IsEmpty() && !strLine.IsEmpty())
		  {
			  PVRDemoChannel channel;

			  channel.strChannelName = tmpChannel.strChannelName;
			  channel.strTvgName = tmpChannel.strTvgName;
			  channel.strIconPath = tmpChannel.strIconPath;
			  channel.strStreamURL = strLine;
			  channel.iUniqueId = ++iUniqueChannelId;
			  channel.iChannelNumber = ++iChannelNum;

			  channel.iEncryptionSystem = 0;
			  channel.bRadio = false;

			  m_channels.push_back(channel);

			  if (iUniqueGroupId > 0) 
			  {
				  m_groups.at(iUniqueGroupId - 1).members.push_back(channel.iChannelNumber);
			  }
		  }

		  tmpChannel.strChannelName = "";
	  }
  }
  
  //file.close();
  return true;
}

bool PVRDemoData::LoadDemoData(void)
{
  TiXmlDocument xmlDoc;
  string strSettingsFile = GetSettingsFile();

  if (!xmlDoc.LoadFile(strSettingsFile))
  {
    XBMC->Log(LOG_ERROR, "invalid demo data (no/invalid data file found at '%s')", strSettingsFile.c_str());
    return false;
  }

  TiXmlElement *pRootElement = xmlDoc.RootElement();
  if (strcmp(pRootElement->Value(), "demo") != 0)
  {
    XBMC->Log(LOG_ERROR, "invalid demo data (no <demo> tag found)");
    return false;
  }

  /* load channels */
  int iUniqueChannelId = 0;
  TiXmlElement *pElement = pRootElement->FirstChildElement("channels");
  if (pElement)
  {
    TiXmlNode *pChannelNode = NULL;
    while ((pChannelNode = pElement->IterateChildren(pChannelNode)) != NULL)
    {
      CStdString strTmp;
      PVRDemoChannel channel;
      channel.iUniqueId = ++iUniqueChannelId;

      /* channel name */
      if (!XMLUtils::GetString(pChannelNode, "name", strTmp))
        continue;
      channel.strChannelName = strTmp;

      /* radio/TV */
      XMLUtils::GetBoolean(pChannelNode, "radio", channel.bRadio);

      /* channel number */
      if (!XMLUtils::GetInt(pChannelNode, "number", channel.iChannelNumber))
        channel.iChannelNumber = iUniqueChannelId;

      /* CAID */
      if (!XMLUtils::GetInt(pChannelNode, "encryption", channel.iEncryptionSystem))
        channel.iEncryptionSystem = 0;

      /* icon path */
      if (!XMLUtils::GetString(pChannelNode, "icon", strTmp))
        channel.strIconPath = m_strDefaultIcon;
      else
        channel.strIconPath = strTmp;

      /* stream url */
      if (!XMLUtils::GetString(pChannelNode, "stream", strTmp))
        channel.strStreamURL = m_strDefaultMovie;
      else
        channel.strStreamURL = strTmp;

      m_channels.push_back(channel);
    }
  }

  /* load channel groups */
  int iUniqueGroupId = 0;
  pElement = pRootElement->FirstChildElement("channelgroups");
  if (pElement)
  {
    TiXmlNode *pGroupNode = NULL;
    while ((pGroupNode = pElement->IterateChildren(pGroupNode)) != NULL)
    {
      CStdString strTmp;
      PVRDemoChannelGroup group;
      group.iGroupId = ++iUniqueGroupId;

      /* group name */
      if (!XMLUtils::GetString(pGroupNode, "name", strTmp))
        continue;
      group.strGroupName = strTmp;

      /* radio/TV */
      XMLUtils::GetBoolean(pGroupNode, "radio", group.bRadio);

      /* members */
      TiXmlNode* pMembers = pGroupNode->FirstChild("members");
      TiXmlNode *pMemberNode = NULL;
      while (pMembers != NULL && (pMemberNode = pMembers->IterateChildren(pMemberNode)) != NULL)
      {
        int iChannelId = atoi(pMemberNode->FirstChild()->Value());
        if (iChannelId > -1)
          group.members.push_back(iChannelId);
      }

      m_groups.push_back(group);
    }
  }

  /* load EPG entries */
  pElement = pRootElement->FirstChildElement("epg");
  if (pElement)
  {
    TiXmlNode *pEpgNode = NULL;
    while ((pEpgNode = pElement->IterateChildren(pEpgNode)) != NULL)
    {
      CStdString strTmp;
      int iTmp;
      PVRDemoEpgEntry entry;

      /* broadcast id */
      if (!XMLUtils::GetInt(pEpgNode, "broadcastid", entry.iBroadcastId))
        continue;

      /* channel id */
      if (!XMLUtils::GetInt(pEpgNode, "channelid", iTmp))
        continue;
      PVRDemoChannel &channel = m_channels.at(iTmp - 1);
      entry.iChannelId = channel.iUniqueId;

      /* title */
      if (!XMLUtils::GetString(pEpgNode, "title", strTmp))
        continue;
      entry.strTitle = strTmp;

      /* start */
      if (!XMLUtils::GetInt(pEpgNode, "start", iTmp))
        continue;
      entry.startTime = iTmp;

      /* end */
      if (!XMLUtils::GetInt(pEpgNode, "end", iTmp))
        continue;
      entry.endTime = iTmp;

      /* plot */
      if (XMLUtils::GetString(pEpgNode, "plot", strTmp))
        entry.strPlot = strTmp;

      /* plot outline */
      if (XMLUtils::GetString(pEpgNode, "plotoutline", strTmp))
        entry.strPlotOutline = strTmp;

      /* icon path */
      if (XMLUtils::GetString(pEpgNode, "icon", strTmp))
        entry.strIconPath = strTmp;

      /* genre type */
      XMLUtils::GetInt(pEpgNode, "genretype", entry.iGenreType);

      /* genre subtype */
      XMLUtils::GetInt(pEpgNode, "genresubtype", entry.iGenreSubType);

      XBMC->Log(LOG_DEBUG, "loaded EPG entry '%s' channel '%d' start '%d' end '%d'", entry.strTitle.c_str(), entry.iChannelId, entry.startTime, entry.endTime);
      channel.epg.push_back(entry);
    }
  }

  /* load recordings */
  iUniqueGroupId = 0; // reset unique ids
  pElement = pRootElement->FirstChildElement("recordings");
  if (pElement)
  {
    TiXmlNode *pRecordingNode = NULL;
    while ((pRecordingNode = pElement->IterateChildren(pRecordingNode)) != NULL)
    {
      CStdString strTmp;
      PVRDemoRecording recording;

      /* recording title */
      if (!XMLUtils::GetString(pRecordingNode, "title", strTmp))
        continue;
      recording.strTitle = strTmp;

      /* recording url */
      if (!XMLUtils::GetString(pRecordingNode, "url", strTmp))
        recording.strStreamURL = m_strDefaultMovie;
      else
        recording.strStreamURL = strTmp;

      iUniqueGroupId++;
      strTmp.Format("%d", iUniqueGroupId);
      recording.strRecordingId = strTmp;

      /* channel name */
      if (XMLUtils::GetString(pRecordingNode, "channelname", strTmp))
        recording.strChannelName = strTmp;

      /* plot */
      if (XMLUtils::GetString(pRecordingNode, "plot", strTmp))
        recording.strPlot = strTmp;

      /* plot outline */
      if (XMLUtils::GetString(pRecordingNode, "plotoutline", strTmp))
        recording.strPlotOutline = strTmp;

      /* genre type */
      XMLUtils::GetInt(pRecordingNode, "genretype", recording.iGenreType);

      /* genre subtype */
      XMLUtils::GetInt(pRecordingNode, "genresubtype", recording.iGenreSubType);

      /* duration */
      XMLUtils::GetInt(pRecordingNode, "duration", recording.iDuration);

      /* recording time */
      if (XMLUtils::GetString(pRecordingNode, "time", strTmp))
      {
        time_t timeNow = time(NULL);
        struct tm* now = localtime(&timeNow);

        int delim = strTmp.Find(':');
        if (delim != CStdString::npos)
        {
          now->tm_hour = (int)strtol(strTmp.Left(delim), NULL, 0);
          now->tm_min  = (int)strtol(strTmp.Mid(delim + 1), NULL, 0);
          now->tm_mday--; // yesterday

          recording.recordingTime = mktime(now);
        }
      }

      m_recordings.push_back(recording);
    }
  }

  return true;
}

int PVRDemoData::GetChannelsAmount(void)
{
  return m_channels.size();
}

PVR_ERROR PVRDemoData::GetChannels(ADDON_HANDLE handle, bool bRadio)
{
  for (unsigned int iChannelPtr = 0; iChannelPtr < m_channels.size(); iChannelPtr++)
  {
    PVRDemoChannel &channel = m_channels.at(iChannelPtr);
    if (channel.bRadio == bRadio)
    {
      PVR_CHANNEL xbmcChannel;
      memset(&xbmcChannel, 0, sizeof(PVR_CHANNEL));

      xbmcChannel.iUniqueId         = channel.iUniqueId;
      xbmcChannel.bIsRadio          = channel.bRadio;
      xbmcChannel.iChannelNumber    = channel.iChannelNumber;
      strncpy(xbmcChannel.strChannelName, channel.strChannelName.c_str(), sizeof(xbmcChannel.strChannelName) - 1);
      strncpy(xbmcChannel.strStreamURL, channel.strStreamURL.c_str(), sizeof(xbmcChannel.strStreamURL) - 1);
      xbmcChannel.iEncryptionSystem = channel.iEncryptionSystem;
      strncpy(xbmcChannel.strIconPath, channel.strIconPath.c_str(), sizeof(xbmcChannel.strIconPath) - 1);
      xbmcChannel.bIsHidden         = false;

      PVR->TransferChannelEntry(handle, &xbmcChannel);
    }
  }

  return PVR_ERROR_NO_ERROR;
}

bool PVRDemoData::GetChannel(const PVR_CHANNEL &channel, PVRDemoChannel &myChannel)
{
  for (unsigned int iChannelPtr = 0; iChannelPtr < m_channels.size(); iChannelPtr++)
  {
    PVRDemoChannel &thisChannel = m_channels.at(iChannelPtr);
    if (thisChannel.iUniqueId == (int) channel.iUniqueId)
    {
      myChannel.iUniqueId         = thisChannel.iUniqueId;
      myChannel.bRadio            = thisChannel.bRadio;
      myChannel.iChannelNumber    = thisChannel.iChannelNumber;
      myChannel.iEncryptionSystem = thisChannel.iEncryptionSystem;
      myChannel.strChannelName    = thisChannel.strChannelName;
      myChannel.strIconPath       = thisChannel.strIconPath;
      myChannel.strStreamURL      = thisChannel.strStreamURL;

      return true;
    }
  }

  return false;
}

int PVRDemoData::GetChannelGroupsAmount(void)
{
  return m_groups.size();
}

PVR_ERROR PVRDemoData::GetChannelGroups(ADDON_HANDLE handle, bool bRadio)
{
  for (unsigned int iGroupPtr = 0; iGroupPtr < m_groups.size(); iGroupPtr++)
  {
    PVRDemoChannelGroup &group = m_groups.at(iGroupPtr);
    if (group.bRadio == bRadio)
    {
      PVR_CHANNEL_GROUP xbmcGroup;
      memset(&xbmcGroup, 0, sizeof(PVR_CHANNEL_GROUP));

      xbmcGroup.bIsRadio = bRadio;
      strncpy(xbmcGroup.strGroupName, group.strGroupName.c_str(), sizeof(xbmcGroup.strGroupName) - 1);

      PVR->TransferChannelGroup(handle, &xbmcGroup);
    }
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRDemoData::GetChannelGroupMembers(ADDON_HANDLE handle, const PVR_CHANNEL_GROUP &group)
{
  for (unsigned int iGroupPtr = 0; iGroupPtr < m_groups.size(); iGroupPtr++)
  {
    PVRDemoChannelGroup &myGroup = m_groups.at(iGroupPtr);
    if (!strcmp(myGroup.strGroupName.c_str(),group.strGroupName))
    {
      for (unsigned int iChannelPtr = 0; iChannelPtr < myGroup.members.size(); iChannelPtr++)
      {
        int iId = myGroup.members.at(iChannelPtr) - 1;
        if (iId < 0 || iId > (int)m_channels.size() - 1)
          continue;
        PVRDemoChannel &channel = m_channels.at(iId);
        PVR_CHANNEL_GROUP_MEMBER xbmcGroupMember;
        memset(&xbmcGroupMember, 0, sizeof(PVR_CHANNEL_GROUP_MEMBER));

        strncpy(xbmcGroupMember.strGroupName, group.strGroupName, sizeof(xbmcGroupMember.strGroupName) - 1);
        xbmcGroupMember.iChannelUniqueId = channel.iUniqueId;
        xbmcGroupMember.iChannelNumber   = channel.iChannelNumber;

        PVR->TransferChannelGroupMember(handle, &xbmcGroupMember);
      }
    }
  }

  return PVR_ERROR_NO_ERROR;
}

PVR_ERROR PVRDemoData::GetEPGForChannel(ADDON_HANDLE handle, const PVR_CHANNEL &channel, time_t iStart, time_t iEnd)
{
  if (m_iEpgStart == -1)
    m_iEpgStart = iStart;

  time_t iLastEndTime = m_iEpgStart + 1;
  int iAddBroadcastId = 0;

  for (unsigned int iChannelPtr = 0; iChannelPtr < m_channels.size(); iChannelPtr++)
  {
    PVRDemoChannel &myChannel = m_channels.at(iChannelPtr);
    if (myChannel.iUniqueId != (int) channel.iUniqueId)
      continue;

    while (iLastEndTime < iEnd && myChannel.epg.size() > 0)
    {
      time_t iLastEndTimeTmp = 0;
      for (unsigned int iEntryPtr = 0; iEntryPtr < myChannel.epg.size(); iEntryPtr++)
      {
        PVRDemoEpgEntry &myTag = myChannel.epg.at(iEntryPtr);

        EPG_TAG tag;
        memset(&tag, 0, sizeof(EPG_TAG));

        tag.iUniqueBroadcastId = myTag.iBroadcastId + iAddBroadcastId;
        tag.strTitle           = myTag.strTitle.c_str();
        tag.iChannelNumber     = myTag.iChannelId;
        tag.startTime          = myTag.startTime + iLastEndTime;
        tag.endTime            = myTag.endTime + iLastEndTime;
        tag.strPlotOutline     = myTag.strPlotOutline.c_str();
        tag.strPlot            = myTag.strPlot.c_str();
        tag.strIconPath        = myTag.strIconPath.c_str();
        tag.iGenreType         = myTag.iGenreType;
        tag.iGenreSubType      = myTag.iGenreSubType;

        iLastEndTimeTmp = tag.endTime;

        PVR->TransferEpgEntry(handle, &tag);
      }

      iLastEndTime = iLastEndTimeTmp;
      iAddBroadcastId += myChannel.epg.size();
    }
  }

  return PVR_ERROR_NO_ERROR;
}

int PVRDemoData::GetRecordingsAmount(void)
{
  return m_recordings.size();
}

PVR_ERROR PVRDemoData::GetRecordings(ADDON_HANDLE handle)
{
  for (std::vector<PVRDemoRecording>::iterator it = m_recordings.begin() ; it != m_recordings.end() ; it++)
  {
    PVRDemoRecording &recording = *it;

    PVR_RECORDING xbmcRecording;

    xbmcRecording.iDuration     = recording.iDuration;
    xbmcRecording.iGenreType    = recording.iGenreType;
    xbmcRecording.iGenreSubType = recording.iGenreSubType;
    xbmcRecording.recordingTime = recording.recordingTime;

    strncpy(xbmcRecording.strChannelName, recording.strChannelName.c_str(), sizeof(xbmcRecording.strChannelName) - 1);
    strncpy(xbmcRecording.strPlotOutline, recording.strPlotOutline.c_str(), sizeof(xbmcRecording.strPlotOutline) - 1);
    strncpy(xbmcRecording.strPlot,        recording.strPlot.c_str(),        sizeof(xbmcRecording.strPlot) - 1);
    strncpy(xbmcRecording.strRecordingId, recording.strRecordingId.c_str(), sizeof(xbmcRecording.strRecordingId) - 1);
    strncpy(xbmcRecording.strTitle,       recording.strTitle.c_str(),       sizeof(xbmcRecording.strTitle) - 1);
    strncpy(xbmcRecording.strStreamURL,   recording.strStreamURL.c_str(),   sizeof(xbmcRecording.strStreamURL) - 1);

    PVR->TransferRecordingEntry(handle, &xbmcRecording);
  }

  return PVR_ERROR_NO_ERROR;
}
