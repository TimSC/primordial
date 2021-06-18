// SoundRegistry.cpp
//
/////////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include "SoundRegistry.h"

//-------------------------------CSoundRegistry-------------------------


////////////////////////////////////////////////////////////////////////
// CSoundRegistry::GetPath
//
// Play a specific sound event.  Transmitter is silent
//
std::string CSoundRegistry::GetPath(const std::string &szEvent)
{
    assert(&szEvent);

    //CRegistry sound;

    std::string sEvent = "AppEvents\\Schemes\\Apps\\";
	sEvent += m_sScheme;
	sEvent += '\\';
	sEvent += szEvent;
	sEvent += "\\.current";

    /*if (sound.OpenKey(sEvent, HKEY_CURRENT_USER))
	{
		sEvent.Empty();
		sound.GetString(NULL, sEvent);
		return sEvent;
//		sndPlaySound(sEvent, SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
    }
    sEvent.Empty();*/
	return sEvent;
}


///////////////////////////////////////////////////////
// CSoundRegistry::CSoundRegistry
//
//
CSoundRegistry::CSoundRegistry(const std::string &szScheme, const std::string &szSchemeName)
{
	SetScheme(szScheme, szSchemeName);
}


///////////////////////////////////////////////////////
// CSoundRegistry::SetScheme
//
//
void CSoundRegistry::SetScheme(const std::string &szScheme, const std::string &szSchemeName)
{
	m_sScheme     = szScheme;
	m_sSchemeName = szSchemeName;
    m_bSetScheme  = false;
}


///////////////////////////////////////////////////////
// CSoundRegistry::SoundEvent
//
//
void CSoundRegistry::SoundEvent(const std::string &szEvent, const std::string &szEventName, const std::string &szPath)
{
    /*CRegistry sound;
    std::string sEvent("AppEvents\\EventLabels\\");

	sEvent += szEvent;

	sound.CreateKey(sEvent, HKEY_CURRENT_USER);
	sound.SetString(NULL, szEventName);

	sEvent  = "AppEvents\\Schemes\\Apps\\";
	sEvent  += m_sScheme;

	if (!m_bSetScheme)
	{
		m_bSetScheme = TRUE;
		sound.CreateKey(sEvent, HKEY_CURRENT_USER);
		sound.SetString(NULL, m_sSchemeName);
	}

	sEvent += '\\';
	sEvent += szEvent;

	// If the key doesn't exist
	if (!sound.OpenKey(sEvent, HKEY_CURRENT_USER))
	{
		// Create the key with the right name
		sound.CreateKey(sEvent, HKEY_CURRENT_USER);
		sound.SetString(NULL, szEventName);

		// Set the default sound
		sEvent += "\\.current";
		sound.CreateKey(sEvent, HKEY_CURRENT_USER);
		sEvent  = szPath;
		sEvent += szEventName;
		sEvent += ".wav";
        sound.SetString(NULL, sEvent);
    }*/
}


