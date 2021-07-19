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

    // https://freesound.org/people/thisusernameis/sounds/426888/
    QString fina = "426888__thisusernameis__beep4.mp3";

    if(szEvent == "PL.Start")
    {
        // https://freesound.org/people/plasterbrain/sounds/243020/
        fina = "243020__plasterbrain__game-start.mp3";
    }
    else if(szEvent == "PL.Birth")
    {
        //https://freesound.org/people/pablopiccox/sounds/104478/
        fina = "104478__pablopiccox__cat-birth-mp3-01.mp3";
    }
    else if(szEvent == "PL.TooOld")
    {
        //https://freesound.org/people/Albasty/sounds/559621/
        fina = "559621__albasty__monster-sound-medium-death.mp3";
    }
    else if(szEvent == "PL.Eaten")
    {
        //https://freesound.org/people/princessemilu/sounds/457475/
        fina = "104478__pablopiccox__cat-birth-mp3-01.mp3";
    }
    else if(szEvent == "PL.NoEnergy")
    {
        //https://freesound.org/people/threadzz/sounds/384992/
        fina = "384992__threadzz__woman-sigh.mp3";
    }
    else if(szEvent == "PL.Mate")
    {
        //https://freesound.org/people/egomassive/sounds/536765/
        fina = "536765__egomassive__squish.mp3";
    }
    else if(szEvent == "PL.Terminate")
    {
        //https://freesound.org/people/okieactor/sounds/415912/
        fina = "415912__okieactor__heathers-gunshot-effect2.mp3";
    }
    else if(szEvent == "PL.Edit")
    {

    }
    else if(szEvent == "PL.Feed")
    {
        //https://freesound.org/people/Rhidor/sounds/346322/
        fina = "346322__rhidor__filling-cereal-into-a-bowl.mp3";
    }
    else if(szEvent == "PL.Extinction")
    {
        //https://freesound.org/people/greenvwbeetle/sounds/240837/
        fina = "240837__greenvwbeetle__creepy-sound.mp3";
    }

    //Try up one folder level to find audio folder
    QDir dir1 = QDir(QDir::currentPath());
    dir1.cd("../audio/");
    QDir dir2 = QDir(QDir::currentPath()); //Also check current folder for audio
    dir2.cd("./audio/");
    QDir dir3 = QDir("/usr/share/primordial-life/audio/"); //Check default linux path

    QString path = dir1.filePath(fina);
    if(!QFileInfo::exists(path))
        path = dir2.filePath(fina);
    if(!QFileInfo::exists(path))
        path = dir3.filePath(fina);

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

