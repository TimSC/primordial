// Registry.h : header file
// $Revision$
//
/////////////////////////////////////////////////////////////////////////////

#ifndef KeyRegistry_h
#define KeyRegistry_h

#include <string>

//CRC////////////////////////////////////////////////////////////////////////
//
// CSoundRegistry
//
// Provides a means to set registry sounds or read them
//
class CSoundRegistry
{
public:
	CSoundRegistry(){};
    CSoundRegistry(const std::string &szScheme, const std::string &szSchemeName);
    void SetScheme(const std::string &szScheme, const std::string &szSchemeName);
    void SoundEvent(const std::string &szEvent, const std::string &szEventName, const std::string &szPath);
    std::string GetPath(const std::string &szEvent);

private:
    bool    m_bSetScheme;
    std::string m_sScheme;
    std::string m_sSchemeName;
};



#endif
