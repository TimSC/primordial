/*
------------------------------------------------------------------------------

rand.cpp: By Bob Jenkins.  My random number generator, ISAAC.
          Adopted and modified by Jason Spofford

------------------------------------------------------------------------------
*/
#include "Rand.h"
#include <QDateTime>

using namespace rapidjson;

/*
------------------------------------------------------------------------------

 If (seed==TRUE), then use the contents of randrsl[0..255] as the seed.

------------------------------------------------------------------------------
*/


typedef  unsigned       char ub1;
#define UB1MAXVAL 0xff

#define UB4MAXVAL 0xffffffff

#define ind(mm,x)  (*(ub4 *)((ub1 *)(mm) + ((x) & ((RANDSIZ-1)<<2))))

/* if (seed==TRUE), then use the contents of randrsl[] to initialize mm[]. */
#define mix(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}

#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (a^(mix)) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}


////////////////////////////////////////////////////////////////////////////////
// Randomizer
//
//
//
ub4 Randomizer::aa = 0;
ub4 Randomizer::bb = 0;
ub4 Randomizer::cc = 0;

ub4 Randomizer::randrsl[RANDSIZ];
ub4 Randomizer::randcnt;
ub4 Randomizer::mm[RANDSIZ];

Randomizer::Randomizer(void)
{
  static int isSeeded = 0;

  if (!isSeeded)
  {
    RandSeed(QDateTime::currentMSecsSinceEpoch());
    isSeeded = 1;
  }
}


///////////////////////////////////////////////////////
// RandSeed
//
// Takes one seed and expands it into the array
//
void Randomizer::RandSeed(int seed)
{
	randrsl[0] = seed;
	for (int i = 1; i < RANDSIZ; i++)
		randrsl[i] = (randrsl[i - 1] + 1) * randrsl[i - 1];

    RandInit(true);
}


///////////////////////////////////////////////////////
// RandInit
//
// Initializes the seed array
//
void Randomizer::RandInit(int seed)
{
	int i;
	ub4 a,b,c,d,e,f,g,h;

	aa=bb=cc=0;
	a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

	for (i=0; i<4; ++i)          /* scramble it */
	{
		mix(a,b,c,d,e,f,g,h);
	}

	for (i=0; i<RANDSIZ; i+=8)   /* fill in mm[] with messy stuff */
	{
		if (seed)                  /* use all the information in the seed */
		{
			a+=randrsl[i  ]; b+=randrsl[i+1]; c+=randrsl[i+2]; d+=randrsl[i+3];
			e+=randrsl[i+4]; f+=randrsl[i+5]; g+=randrsl[i+6]; h+=randrsl[i+7];
		}
		mix(a,b,c,d,e,f,g,h);
		mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
		mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
	}

	if (seed) 
	{        /* do a second pass to make all of the seed affect all of mm */
		for (i=0; i<RANDSIZ; i+=8)
		{
			a+=mm[i  ]; b+=mm[i+1]; c+=mm[i+2]; d+=mm[i+3];
			e+=mm[i+4]; f+=mm[i+5]; g+=mm[i+6]; h+=mm[i+7];
			mix(a,b,c,d,e,f,g,h);
			mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
			mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
		}
	}

	Isaac();            /* fill in the first set of results */
	randcnt=RANDSIZ;    /* prepare to use the first set of results */
}


///////////////////////////////////////////////////////
// Isaac
//
// Calculates the next value
//
void Randomizer::Isaac()
{
    ub4 a,b,x,y,*m,*m2,*r,*mend;

	m=mm; r=randrsl;
	a = aa; b = bb + (++cc);

	for (m = mm, mend = m2 = m+(RANDSIZ/2); m<mend; )
	{
		rngstep( a<<13, a, b, mm, m, m2, r, x);
		rngstep( a>>6 , a, b, mm, m, m2, r, x);
		rngstep( a<<2 , a, b, mm, m, m2, r, x);
		rngstep( a>>16, a, b, mm, m, m2, r, x);
	}

	for (m2 = mm; m2<mend; )
	{
		rngstep( a<<13, a, b, mm, m, m2, r, x);
		rngstep( a>>6 , a, b, mm, m, m2, r, x);
		rngstep( a<<2 , a, b, mm, m, m2, r, x);
		rngstep( a>>16, a, b, mm, m, m2, r, x);
	}

	bb = b; aa = a;
}


///////////////////////////////////////////////////////////////
// Serialize
//
// Takes or sets the state of the randmizer
//

void  Randomizer::SerializeJson(rapidjson::Document &d, rapidjson::Value &v)
{
    Document::AllocatorType& allocator = d.GetAllocator();

    Value randrslArr(kArrayType);
    Value mmArr(kArrayType);
    for (int i = 0; i < RANDSIZ; i++)
    {
        randrslArr.PushBack(Value(randrsl[i]), allocator);
        mmArr.PushBack(Value(mm[i]), allocator);
    }
    v.AddMember("randrsl", randrslArr, allocator);
    v.AddMember("mm", mmArr, allocator);

    v.AddMember("randcnt", Value(randcnt), allocator);
    v.AddMember("aa", Value(aa), allocator);
    v.AddMember("bb", Value(bb), allocator);
    v.AddMember("cc", Value(cc), allocator);
}

void Randomizer::SerializeJsonLoad(const rapidjson::Value& v)
{
    const Value &randrslArr = v["randrsl"];
    for (int i = 0; i < randrslArr.Size(); i++)
        randrsl[i] = randrslArr[i].GetUint();

    const Value &mmArr = v["mm"];
    for (int i = 0; i < mmArr.Size(); i++)
        mm[i] = mmArr[i].GetUint();

    randcnt = v["randcnt"].GetUint();
    aa = v["aa"].GetUint();
    bb = v["bb"].GetUint();
    cc = v["cc"].GetUint();

}

short Randomizer::Short(int max)
{
	return (short) (rand() % max);
}


////////////////////////////////////////////////////////////////////////////////
// RandomKey
//
//
//
ub4 RandomKey::aa = 0;
ub4 RandomKey::bb = 0;
ub4 RandomKey::cc = 0;

ub4 RandomKey::randrsl[RANDSIZ];
ub4 RandomKey::randcnt;
ub4 RandomKey::mm[RANDSIZ];


///////////////////////////////////////////////////////
// RandSeed
//
// Takes one seed and expands it into the array
//
void RandomKey::RandSeed(int seed)
{
	randrsl[0] = seed;
	for (int i = 1; i < RANDSIZ; i++)
		randrsl[i] = (randrsl[i - 1] + 1) * randrsl[i - 1];

    RandInit(true);
}


///////////////////////////////////////////////////////
// RandInit
//
// Initializes the seed array
//
void RandomKey::RandInit(int seed)
{
	int i;
	ub4 a,b,c,d,e,f,g,h;

	aa=bb=cc=0;
	a=b=c=d=e=f=g=h=0x9e3779b9;  /* the golden ratio */

	for (i=0; i<4; ++i)          /* scramble it */
	{
		mix(a,b,c,d,e,f,g,h);
	}

	for (i=0; i<RANDSIZ; i+=8)   /* fill in mm[] with messy stuff */
	{
		if (seed)                  /* use all the information in the seed */
		{
			a+=randrsl[i  ]; b+=randrsl[i+1]; c+=randrsl[i+2]; d+=randrsl[i+3];
			e+=randrsl[i+4]; f+=randrsl[i+5]; g+=randrsl[i+6]; h+=randrsl[i+7];
		}
		mix(a,b,c,d,e,f,g,h);
		mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
		mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
	}

	if (seed) 
	{        /* do a second pass to make all of the seed affect all of mm */
		for (i=0; i<RANDSIZ; i+=8)
		{
			a+=mm[i  ]; b+=mm[i+1]; c+=mm[i+2]; d+=mm[i+3];
			e+=mm[i+4]; f+=mm[i+5]; g+=mm[i+6]; h+=mm[i+7];
			mix(a,b,c,d,e,f,g,h);
			mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
			mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
		}
	}

	Isaac();            /* fill in the first set of results */
	randcnt=RANDSIZ;    /* prepare to use the first set of results */
}


///////////////////////////////////////////////////////
// Isaac
//
// Calculates the next value
//
void RandomKey::Isaac()
{
    ub4 a,b,x,y,*m,*m2,*r,*mend;

	m=mm; r=randrsl;
	a = aa; b = bb + (++cc);

	for (m = mm, mend = m2 = m+(RANDSIZ/2); m<mend; )
	{
		rngstep( a<<13, a, b, mm, m, m2, r, x);
		rngstep( a>>6 , a, b, mm, m, m2, r, x);
		rngstep( a<<2 , a, b, mm, m, m2, r, x);
		rngstep( a>>16, a, b, mm, m, m2, r, x);
	}

	for (m2 = mm; m2<mend; )
	{
		rngstep( a<<13, a, b, mm, m, m2, r, x);
		rngstep( a>>6 , a, b, mm, m, m2, r, x);
		rngstep( a<<2 , a, b, mm, m, m2, r, x);
		rngstep( a>>16, a, b, mm, m, m2, r, x);
	}

	bb = b; aa = a;
}

bool RandomKey::CheckKey(long lKey)
{
	RandSeed(lKey & 0x0000FFFF);
	return ((short) (lKey >> 16) == (short) (rand() & 0x0000FFFF));
}

void RandomKey::SetKey(long& lKey)
{
	RandSeed(lKey & 0x0000FFFF);

	long l = (rand() & 0x0000FFFF);
	lKey |= (l << 16);
}
