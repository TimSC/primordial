//////////////////////////////////////////////////////////
// BoolArchive is a little helper class to save or load
// booleans in the archive
//
#pragma once

class BoolArchive
{
public:
	static void Store(CArchive& ar, bool by)
	{
		BYTE temp = BYTE(by?1:0);
		ar << temp;
	}

	static void Load(CArchive& ar, bool& by)
	{
		BYTE temp;
		ar >> temp;
		by = (temp?true:false);
	}
};

