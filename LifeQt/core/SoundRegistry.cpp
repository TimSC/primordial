// SoundRegistry.cpp
//
// ///////////////////////////////////////////////////////////////////////////

#include <assert.h>
#include <QDir>
#include <QFileInfo>
#include "SoundRegistry.h"

//-------------------------------CSoundRegistry-------------------------

CSoundRegistry::CSoundRegistry()
{
    m_bSetScheme = false;
    m_sScheme = "default";
    m_sSchemeName = "default";
}

CSoundRegistry::~CSoundRegistry()
{

}

CSoundRegistry::CSoundRegistry(const CSoundRegistry &obj)
{
    *this = obj;
}

CSoundRegistry &CSoundRegistry::operator=(const CSoundRegistry& obj)
{
    m_sScheme     = obj.m_sScheme;
    m_sSchemeName = obj.m_sSchemeName;
    m_bSetScheme  = obj.m_bSetScheme;
    return *this;
}

// //////////////////////////////////////////////////////////////////////
// CSoundRegistry::GetPath
//
// Get a specific sound event file path.
//
std::string CSoundRegistry::GetPath(const std::string &szEvent)
{
    assert(&szEvent);

    /*PL.Extinction
    PL.Start
    PL.TooOld
    PL.Eaten
    PL.NoEnergy
    PL.Birth
    PL.Mate
    PL.Terminate
    PL.Edit
    PL.Feed*/

    QString fina = "426888__thisusernameis__beep4.mp3";

    //Try up one folder level to find audio folder
    QDir dir1 = QDir(QDir::currentPath());
    dir1.cd("../audio/");
    QDir dir2 = QDir(QDir::currentPath()); //Also check current folder for audio
    dir2.cd("./audio/");

    QString path = dir1.filePath(fina);
    if(!QFileInfo::exists(path))
        path = dir2.filePath(fina);

    if(QFileInfo::exists(path))
        return path.toStdString();

    return ""; //Can't find audio file
}

// /////////////////////////////////////////////////////
// CSoundRegistry::CSoundRegistry
//
//
CSoundRegistry::CSoundRegistry(const std::string &szScheme, const std::string &szSchemeName)
{
	SetScheme(szScheme, szSchemeName);
}


// /////////////////////////////////////////////////////
// CSoundRegistry::SetScheme
//
//
void CSoundRegistry::SetScheme(const std::string &szScheme, const std::string &szSchemeName)
{
	m_sScheme     = szScheme;
	m_sSchemeName = szSchemeName;
    m_bSetScheme  = false;
}

