
/* GSW.H        Graphics Server */

                /* Application Program Interface Declaration File */
                /* for C/C++ */
                /* Copyright 1989-1993 Bits Per Second Ltd */

#ifndef __GSW_H__
#define __GSW_H__

/* common parameter constants */

#define BLACK           0               /* nClr */
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define BROWN           6
#define WHITE           7
#define GREY            8
#define YELLOW          14
#define LIGHT           8

#define LSSOLID         PS_SOLID        /* nStyle */
#define LSDASH          PS_DASH
#define LSDOT           PS_DOT
#define LSDASHDOT       PS_DASHDOT
#define LSDASHDD        PS_DASHDOTDOT
#define LSNULL          PS_NULL

#define BRSOLID         0               /* nPatt */
#define BRNULL          1
#define BRHATCH         2
#define BRHATCHMAX      6
#define BRBITMAP        16
#define BRBITMAPMAX     16
#define BRTRANS         64

#define CSSYSTEM        0               /* nCSet */
#define CSUSER          1
#define CSRASTER        2

#define TXEXACT         1               /* nTMode */
#define TXLEFT          0
#define TXMID           2
#define TXRIGHT         4
#define TXBOTTOM        0
#define TXBASELINE      8
#define TXTOP           16
#define TXUP90          32
#define TXDOWN90        64
#define TXTRANS         256
#define TXFLIP          512
#define TXSLIP          1024

#define UNLOG           0               /* nUnits */
#define UNMM            1
#define UNINCH          2
#define UNDEV           3

#define NULLDATA        256             /* nAux */


#define GSSURFACENET       1    // surface chart - draw net only
#define GSSURFACEGRID      2    // surface chart - draw grid on surface
#define GSSURFACESIDES     4    // surface chart - draw side walls

#define GSPERSPECTIVE       1
#define GSCAGESIDEFLIP      2
#define GSCAGEBACKFLIP      4

/* exported function specifiers */

#ifndef GSAPIFN
#define GSAPIFN WINAPI
#endif /* NDEF GSAPIFN */

/* exported function declarations */

#ifdef  __cplusplus
extern "C" {
#endif

//-------------------  Start of Version 3 GS and AG functions

int GSAPIFN GSXDataScale(
        double  fScale
);

int GSAPIFN GSSelectPalette(
        int     nMode
);

#define PALDEFAULT      0
#define PALGREYSCALE    1
#define PALPASTEL       2
#define PALRGBCMY       3
#define PALRAINBOW      4
#define PALREDSCALE     5
#define PALGREENSCALE   6
#define PALBLUESCALE    7
#define PALCYANSCALE    8
#define PALMAGENTASCALE 9
#define PALYELLOWSCALE  10
#define PALUSER         11

int GSAPIFN GSLinLog(
        double  fxOrg,
        double  fyOrg,
        double  fCycleWid,
        double  fBaseVal,
        int     nMode,
        int     nClr
);

int GSAPIFN GSErrorBar(
        int     nSel,
        int     nSymStyle,
        int     nClr,
        int     nErrSrc,
        double  fValue,
        double  fOff
);

#define EBY             0
#define EBX             1
#define EBNOPLUS        1
#define EBNOMINUS       2
#define EBNOSTEM        4
#define EBNOTICK        8
#define EBFIXED         0
#define EBPERCENT       1
#define EBSTDDEV        2
#define EBSTDERR        3
#define EBMAXMIN        4

int GSAPIFN GSDataAmpErr(
        int     nP,
        int     nG,
        const double FAR* lpfErr
);

int GSAPIFN GSDataStoreAmpErr(
        int     nPt,
        int     nGroup,
        double  fErrPlus,
        double  fErrMinus
);

int GSAPIFN GSDataGetAmpErr_(
        double FAR* lpfZ,
        int     nPt,
        int     nGroup,
        int     nMode
);

#if defined( _MSC_VER )
double GSAPIFN GSDataGetAmpErr( int nPt, int nGroup, int nMode );
#elif defined( __cplusplus )
inline double GSDataGetAmpErr( int nPt, int nGroup, int nMode )
{
    double fVal;
    return GSDataGetAmpErr_( &fVal, nPt, nGroup, nMode ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */


int GSAPIFN GSDataDistErr(
        int     nP,
        const double FAR* lpfErr
);

int GSAPIFN GSDataStoreDistErr(
        int     nPt,
        double  fErrPlus,
        double  fErrMinus
);

int GSAPIFN GSDataGetDistErr_(
        double FAR* lpfZ,
        int     nPt,
        int     nMode
);

#if defined( _MSC_VER )
double GSAPIFN GSDataGetDistErr( int nPt, int nMode );
#elif defined( __cplusplus )
inline double GSDataGetDistErr( int nPt, int nMode )
{
    double fVal;
    return GSDataGetDistErr_( &fVal, nPt, nMode ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

#define ERRPLUS         0
#define ERRMINUS        1

int GSAPIFN GSBoxWhisker(
        double  fxOrg,
        double  fyOrg,
        double  fInc,
        double  fSpace,
        int     nMode

);

#define BWPARAMETRIC    1
#define BWVARX          4
#define BWNONOTCH       8
#define BWNOWHISKER     16
#define BWNOSAMPLES     32

int GSAPIFN GSTimeGraph(
        double  fxOrg,
        double  fyOrg,
        double  fInc,
        int     nPts,
        int     nGroups,
        int     nMode
);

#define TIGMEAN         1
#define TIGSTDDEV       2

int GSAPIFN GSTimeUpdate(
        int     nMode,
        int     nGroup,
        const double FAR* lpfData
);

#define TIGUPDATE       0
#define TIGLOAD         1
#define TIGHIDE         2
#define TIGSHOW         3

int GSAPIFN AGSurfaceClr(
        int     nClrMin,
        int     nClrMax,
        int     nClrSide
);

int GSAPIFN AGDataZ(
        int     nPts,
        const double FAR* lpfZ
);

int GSAPIFN AGFFT(
        int     nPts,
        double FAR* lpfData,
        int     nMode
);
#define AGFFTREALIMAG   0
#define AGFFTAMPPHASE   1
#define AGFFTSQAMPPHASE 2
#define AGFFTINTERLEAVE 16

int GSAPIFN AGGraphBG(
        int     nMode,
        int     nClr
);

#define AGGRFRAME       1
#define AGGRFILL        2
#define AGGRDROPSHADOW  4
#define AGGRRAISED      8
#define AGGRLOWERED     16

int GSAPIFN AGTitleBG(
        int     nMode,
        int     nClr
);

#define AGTTLG          0
#define AGTTLX          1
#define AGTTLYLEFT      2
#define AGTTLYRIGHT     3
#define AGTTLUP         4
#define AGTTLDOWN       8
#define AGTTLFRAME      16
#define AGTTLFILL       32
#define AGTTLDROPSHADOW 64
#define AGTTLRAISED     128
#define AGTTLLOWERED    256

int GSAPIFN AGGridStyle(
        int     nSel,
        int     nStyleMaj,
        int     nStyleMin
);

#define AGGRIDX         0
#define AGGRIDY         1
#define AGGRIDZ         2

int GSAPIFN AGCageStyle(
        int     nMode,
        int     nClrWall,
        int     nClrEdge
);

#define AGCAGETHIN      1

int GSAPIFN AGZAxisStyle(                               // AGXAXISSTYLE
        int     nMode,
        int     nTicks,
        int     nLabEvery,
        double  fMax,
        double  fMin
);

int GSAPIFN AGLabelY(
        int     nMode,
        int     nLabs,
        LPCSTR FAR* lplpstrLabs
);

#define AGLABYLEFT      0
#define AGLABYRIGHT     1

int GSAPIFN AGLabelZ(
        int     nMode,
        int     nLabs,
        LPCSTR FAR* lplpstrLabs
);

int GSAPIFN AGLabelFormat(
        int     nSelect,
        LPCSTR  lpstrFormat
);

#define AGFORMATX       0
#define AGFORMATYL      1
#define AGFORMATYR      2
#define AGFORMATZ       3
#define AGFORMATDLABS   4

int GSAPIFN AGErrorBar(
        int     nSel,
        int     nSymStyle,
        int     nClr,
        int     nErrSrc,
        double  fValue
);

#define AGEBY           0
#define AGEBX           1
#define AGEBNOPLUS      1
#define AGEBNOMINUS     2
#define AGEBNOSTEM      4
#define AGEBNOTICK      8
#define AGEBFIXED       0
#define AGEBPERCENT     1
#define AGEBSTDDEV      2
#define AGEBSTDERR      3
#define AGEBMAXMIN      4

int GSAPIFN AGAmpError(
        int     nP,
        int     nG,
        const double FAR* lpfErr
);

int GSAPIFN AGDistError(
        int     nP,
        const double FAR* lpfErr
);

int GSAPIFN AGTimeGraph(
        int     nPts,
        int     nGroup,
        double  fDataMax,
        double  fDataMin,
        int     nStyle
);

#define AGTIGNOLABELS   1
#define AGTIGLEGCLR     2
#define AGTIGXGRID      4
#define AGTIGYGRID      8
#define AGTIGMEAN       1
#define AGTIGSTDDEV     2

int GSAPIFN AGTimeUpdate(
        int     nMode,
        int     nGroup,
        const double FAR* lpfData
);

#define AGTIGUPDATE     0
#define AGTIGLOAD       1
#define AGTIGHIDEDATA   2
#define AGTIGSHOWDATA   3

int GSAPIFN AGFGColor(
        int     nMode,
        int     nClr
);

#define AGFGALL         0
#define AGFGTITLEG      1
#define AGFGTITLEYL     2
#define AGFGTITLEYR     3
#define AGFGTITLEX      4
#define AGFGLABELS      5
#define AGFGLEGEND      6
#define AGFGAXIS        7
#define AGFGGRID        8
#define AGFGMEAN        9
#define AGFGMINMAX      10
#define AGFGSTDDEV      11
#define AGFGBESTFIT     12
#define AGFGCURVE       13
#define AGFGDATALABELS  14
#define AGFGLIMITLINES  15

int GSAPIFN AGDataLabels(
        int     nMode,
        int     nLabs,
        LPCSTR FAR* lplpstrLabs
);

#define AGDLTEXT        0
#define AGDLDATA        1
#define AGDLGROUPCLR    4

//-------------------  Start of new 3-D functions

int GSAPIFN G3SetPerspective(                        /* 3DSetPerspective */
        int     Mode,
        int  nRotation,
        int  nElevation,
        int  nPerspective
);

int GSAPIFN G3SetPerspectiveAll(                     /* 3DSetPerspectiveAll */
        int     Mode,
        double  fxAngle,
        double  fyAngle,
        double  fzAngle,
        double  fxCentre,
        double  fyCentre,
        double  fzCentre,
        double  fxEye,
        double  fyEye,
        double  fzEye
);

int GSAPIFN G3Show(                            /* 3DShow */
        int     Mode
);

int GSAPIFN GSDataZ(                            /* DataZ */
        int     nPts,
        const double FAR* lpfZ
);

int GSAPIFN GSDataStoreZ(                       /* DataStoreZ */
        int     nPt,
        double  fZ
);

int GSAPIFN GSDataGetZ_(                        /* DataGetZ */
        double FAR* lpfZ,
        int     nPt
);

#if defined( _MSC_VER )
double GSAPIFN GSDataGetZ( int nPt );
#elif defined( __cplusplus )
inline double GSDataGetZ( int nPt )
{
    double fVal;
    return GSDataGetZ_( &fVal, nPt ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN G3AreaGraph(                            /* 3D Area */
        double  fxOrg,
        double  fyOrg,
        double  fzOrg,
        double  fxInc,
        double  fzInc,
        double  fzSpace,
        double  fHt,
        int     nMode
);

int GSAPIFN G3TapeGraph(                            /* 3D Tape */
        double  fxOrg,
        double  fyOrg,
        double  fzOrg,
        double  fxInc,
        double  fzInc,
        double  fzSpace,
        int     nMode
);



int GSAPIFN G3Arrow(                            /* 3D Arrow */
        double  fxA,
        double  fyA,
        double  fzA,
        double  fxB,
        double  fyB,
        double  fHeadLen,
        int     nMode,
        int     nStyle,
        int     nClr
);

int GSAPIFN G3Axis(                             /* 3D Axis */
    int     select,
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fLen,
    double  fSection,
    double  fTickLen,
    int     nMajDivs,
    int     nMinDivs,
    int     nMode,
    int     nStyle,
    int     nClr
);

int GSAPIFN G3BarGraph(                            /* 3DBar */
        double  fxOrg,
        double  fyOrg,
        double  fzOrg,
        double  fxInc,
        double  fzInc,
        double  fxSpace,
        double  fzSpace,
        double  fStackHt,
        int     nMode
);

#define BARVARXPOS      16              /* 3DBar: nMode */
#define BARVARZPOS      64

int GSAPIFN G3Box(                            /* 3-D Box */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fWid,
    double  fHt,
    int     nPatt,
    int     nClr
);

int GSAPIFN G3BubbleChart(                      /* 3-D BubbleChart */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    int     nMode
);

int GSAPIFN G3Cage(                           /* 3-D Cage */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fxLen,
    double  fyLen,
    double  fzLen,
    double  fThick,
    int     nxGrids,
    int     nyGrids,
    int     nzGrids,
    int     nMode,
    int     nClr1,
    int     nClr2
);

int GSAPIFN G3Cube(                           /* 3-D Cube */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fxLen,
    double  fyLen,
    double  fzLen,
    int     nPatt,
    int     nClr1,
    int     nClr2,
    int     nClr3
);

int GSAPIFN G3DataLogScale(                           /* 3-D log scaling */
    int     nDataArray,
    int     fLogBase,
    double  fBaseVal,
    double  fCycleHt
);

int GSAPIFN G3DataLinScale(                           /* 3-D lin scaling */
    int     nDataArray,
    double  fFactor,
    double  fOffset
);

int GSAPIFN G3DeleteObjects( void );            /* 3-D delete objects */

int GSAPIFN G3Gantt(                            /* 3-D Gantt */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fYInc,
    double  fDepth,
    int     nMode
);

int GSAPIFN G3Grid(                             /* 3-D Grid */
    int     select,
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fAxisLen,
    double  fGridLen,
    int     nDivs,
    int     nMode,
    int     nStyle,
    int     nClr
);

int GSAPIFN G3HLC(                              /* 3-D HLC */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fXInc,
    int     nMode,
    int     nClr
);

int GSAPIFN G3Labeln(                          /* 3-D Labeln */
    int     select,
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fInc,
    double  fBaseVal,
    double  fStepVal,
    int     nPrec,
    int     nNLabs,
    int     nCSet,
    int     nMode,
    int     nClr
);


int GSAPIFN G3Label(                           /* 3-D Label */
    int     select,
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fInc,
    int     nNLabs,
    int     nCSet,
    int     nMode,
    int     nClr,
    LPCSTR FAR* lplpstrLabs
);

int GSAPIFN G3LabelTab(                                   // LABELTAB
        int     select,
        double  fxOrg,
        double  fyOrg,
        double  fzOrg,
        double  fInc,
        int     nCSet,
        int     nMode,
        int     nClr,
        LPCSTR  lpstrLabs
);


int GSAPIFN G3LineAbs(                          /* 3-D LineAbs */
    double  fxA,
    double  fyA,
    double  fzA,
    double  fxB,
    double  fyB,
    double  fzB,
    int     nMode,
    int     nStyle,
    int     nClr
);

int GSAPIFN G3LineGraph(                          /* 3-D line graph */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fXInc,
    double  fZInc,
    int     nMode,
    int     nClr
);

int GSAPIFN G3LogAxis(                          /* 3-D LogAxis */
    int     select,
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fLen,
    double  fSection,
    double  fTickLen,
    int     nCycles,
    int     nMode,
    int     nStyle,
    int     nClr
);

int GSAPIFN G3LogGrid(                          /* 3-D LogGrid */
    int     select,
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fAxisLen,
    double  fGridLen,
    int     nCycles,
    int     nMode,
    int     nStyle,
    int     nClr
);

int GSAPIFN G3SizeView(                         /* 3-D SizeView */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt
);

int GSAPIFN G3PieChart(                            /* 3-D Pie */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fRad,
    double  fDepth,
    double  fAng,
    int     nMode
);

int GSAPIFN G3SurfaceGraph(                  // 3D surface graph
        double  fxOrg,
        double  fyOrg,
        double  fzOrg,
        double  fxInc,
        double  fzInc,
        int     nMode,
        int     nColourMin,
        double  fMinValue,
        int     nColourMax,
        double  fMaxValue,
        int     nColourSide
);

int GSAPIFN G3Text(                            /* 3-D Text */
    double  fxOrg,
    double  fyOrg,
    double  fzOrg,
    double  fAngle,
    int     nCSet,
    int     nMode,
    int     nClr,
    LPCSTR  lpstrString
);


#define G3GRIDX         0
#define G3GRIDY         1
#define G3GRIDZ         2

int GSAPIFN G3CageStyle(                        /* true 3-D cage style modify */
    int select,
    int StyleMajor,
    int StyleMinor,
    int Colour);

int GSAPIFN G3SetModel(int  select);              // 3DSETMODEL

int GSAPIFN G3TestSpecial(int i, double  d,  LPCSTR lpstr);  // 3DTESTSPECIAL



//------------------ end of new 3D functions

int GSAPIFN AGOpen( void );                     /* AGOpen */

int GSAPIFN AGClose( void );                    /* AGClose */

int GSAPIFN AGReset( void );                    /* AGReset */

int GSAPIFN AGTitleG(                           /* AGTitleG */
    LPCSTR  lpstrTitle
);

int GSAPIFN AGTitleX(                           /* AGTitleX */
    LPCSTR  lpstrTitle
);

int GSAPIFN AGTitleY(                           /* AGTitleY */
    LPCSTR  lpstrTitle
);

int GSAPIFN AGTitleYR(                           /* AGTitleY Right */
    LPCSTR  lpstrTitle
);

int GSAPIFN AGLabels(                           /* AGLabels */
    int     nNLabs,
    LPCSTR FAR* lplpstrLabs
);

int GSAPIFN AGLegend(                           /* AGLegend */
    int     nNLegs,
    LPCSTR FAR* lplpstrLegs
);

int GSAPIFN AGAmp(                              /* AGAmp */
    int     nPts,
    int     nGroup,
    const double FAR* lpfAmp
);

int GSAPIFN AGDist(                             /* AGDist */
    int     nPts,
    const double FAR* lpfDist
);

int GSAPIFN AGPatt(                             /* AGPatt */
    int     nGroup,
    const int FAR* lpnPatt
);

int GSAPIFN AGSym(                              /* AGSym */
    int     nGroup,
    const int FAR* lpnSym
);

int GSAPIFN AGAux(                              /* AGAux */
    int     nSize,
    const int FAR* lpnAux
);

int GSAPIFN AGClr(                              /* AGClr */
    int     nGroup,
    const int FAR* lpnClr
);

int GSAPIFN AGDataRange(                        /* AGDataRange */
        int     nRangeMin,
        int     nRangeMax
);

int GSAPIFN AGLimitLines(                            /* AGLimit */
        int     nMode,
        int     nLinePattern,
        int     nFillPattern,
        double  fHighValue,
        double  fLowValue,
        LPCSTR  lpstrHighLabel,
        LPCSTR  lpstrLowLabel
);
#define AGLIMITHIGH     1
#define AGLIMITLOW      2
#define AGLIMITOPPSHADE 4

int GSAPIFN AGShow(                             /* AGShow */
    int     nGType,
    int     nStyle,
    int     nStats
);

#define AGPIE2D         1               /* AGShow: nGType */
#define AGPIE3D         2
#define AGBAR2D         3
#define AGBAR3D         4
#define AGGANTT         5
#define AGLINE          6
#define AGLOGLIN        7
#define AGAREA          8
#define AGSCATTER       9
#define AGPOLAR         10
#define AGHLC           11
#define AGBUBBLE        12
#define AGTAPE          13
#define AGAREA3D        14
#define AGLOGLOG        15
#define AGLINLOG        16
#define AGBOXWHISKER    17
#define AGSURFACE       128
#define AGTRUE3D        128
#define AGCOMBO         256
#define AGDUALYAXIS     512

#define AGLINENOLABELS  0x0001          /* AGShow: nStyle: line graph */
#define AGLINELEGCLR    0x0002
#define AGLINEXGRID     0x0004
#define AGLINEYGRID     0x0008
#define AGLINESYMBOLS   0x0010
#define AGLINEMONO      0x0020
#define AGLINEFORECLR   0x0040

#define AGLINESOLID     0x4000
#define AGLINETHICK     0x2000
#define AGLINEPATT      0x1000
#define AGLINESTICK     0x0800

#define AGSCATTNOLABELS 0x0001          /* AGShow: nStyle: scatter graph */
#define AGSCATTLEGCLR   0x0002
#define AGSCATTXGRID    0x0004
#define AGSCATTYGRID    0x0008
#define AGSCATTSYMBOLS  0x0010
#define AGSCATTMONO     0x0020
#define AGSCATTFORECLR  0x0040

#define AGSCATTSOLID    0x4000
#define AGSCATTTHICK    0x2000
#define AGSCATTPATT     0x1000
#define AGSCATTCURVE    0x0400
#define AGSCATTSTICK    0x0200

#define AGLOGLINNOLABS  0x0001          /* AGShow: nStyle: log/linear graph */
#define AGLOGLINLEGCLR  0x0002
#define AGLOGLINXGRID   0x0004
#define AGLOGLINYGRID   0x0008
#define AGLOGLINSYMBOLS 0x0010
#define AGLOGLINMONO    0x0020
#define AGLOGLINFORECLR 0x0040

#define AGLOGLINSOLID   0x4000
#define AGLOGLINTHICK   0x2000
#define AGLOGLINPATT    0x1000
#define AGLOGLINSTICK   0x0800

#define AGLINLOGNOLABS  0x0001          /* AGShow: nStyle: linear/log graph */
#define AGLINLOGLEGCLR  0x0002
#define AGLINLOGXGRID   0x0004
#define AGLINLOGYGRID   0x0008
#define AGLINLOGSYMBOLS 0x0010
#define AGLINLOGMONO    0x0020
#define AGLINLOGFORECLR 0x0040

#define AGLINLOGSOLID   0x4000
#define AGLINLOGTHICK   0x2000
#define AGLINLOGPATT    0x1000
#define AGLINLOGSTICK   0x0800

#define AGLOGLOGNOLABS  0x0001          /* AGShow: nStyle: log/log graph */
#define AGLOGLOGLEGCLR  0x0002
#define AGLOGLOGXGRID   0x0004
#define AGLOGLOGYGRID   0x0008
#define AGLOGLOGSYMBOLS 0x0010
#define AGLOGLOGMONO    0x0020
#define AGLOGLOGFORECLR 0x0040

#define AGLOGLOGSOLID   0x4000
#define AGLOGLOGTHICK   0x2000
#define AGLOGLOGPATT    0x1000
#define AGLOGLOGSTICK   0x0800

#define AGHLCNOLABELS   0x0001          /* AGShow: nStyle: HLC graph */
#define AGHLCXGRID      0x0004
#define AGHLCYGRID      0x0008
#define AGHLCMONO       0x0020
#define AGHLCFORECLR    0x0040
#define AGHLCOPEN       0x0080
#define AGHLCCANDLESTICK 0x0100

#define AGHLCNOCLOSE    0x4000
#define AGHLCNOBARS     0x2000
#define AGHLCTHICK      0x1000

#define AGBARNOLABELS   0x0001          /* AGShow: nStyle: bar graph */
#define AGBARLEGCLR     0x0002
#define AGBARXGRID      0x0004
#define AGBARYGRID      0x0008
#define AGBARMONO       0x0020
#define AGBARFORECLR    0x0040

#define AGBARCLUSTER    0x4000
#define AGBARSTACK      0x2000
#define AGBARSTACKPC    0x1000
#define AGBARHORIZ      0x0800
#define AGBARCLUSTZ     0x0400
#define AGBARLASTFIRST  0x0200
#define AGBARFLOAT      0x0080          // ver 4.5
#define AGBARPARETO     0x0100

#define AGGANTTNOLABELS 0x0001          /* AGShow: nStyle: gantt chart */
#define AGGANTTXGRID    0x0004
#define AGGANTTYGRID    0x0008
#define AGGANTTMONO     0x0020
#define AGGANTTFORECLR  0x0040

#define AGGANTTSPACE    0x4000
#define AGGANTTLASTFIRST 0x2000

#define AGAREANOLABELS  0x0001          /* AGShow: nStyle: area graph */
#define AGAREALEGCLR    0x0002
#define AGAREAXGRID     0x0004
#define AGAREAYGRID     0x0008
#define AGAREAMONO      0x0020
#define AGAREAFORECLR   0x0040

#define AGAREAABS       0x4000
#define AGAREAPC        0x2000

#define AGPIENOLABELS   0x0001          /* AGShow: nStyle: pie chart */
#define AGPIELEGCLR     0x0002
#define AGPIEMONO       0x0020
#define AGPIEFORECLR    0x0040

#define AGPIESMARTLABELS 0x8000
#define AGPIENOLINES    0x4000
#define AGPIESEGCLR     0x2000
#define AGPIEPERCENT    0x1000
#define AGPIEPCCHAR     0x0800
#define AGPIESAMECLR    0x0400

#define AGPOLARNOLABELS 0x0001          /* AGShow: nStyle: polar graph */
#define AGPOLARLEGCLR   0x0002
#define AGPOLARANGGRID  0x0004
#define AGPOLARRADGRID  0x0008
#define AGPOLARSYMBOL   0x0010
#define AGPOLARMONO     0x0020
#define AGPOLARFORECLR  0x0040

#define AGPOLARLINE     0x4000
#define AGPOLARTHICK    0x2000
#define AGPOLARPATT     0x1000
#define AGPOLARSTICK    0x0800

#define AGBUBNOLABELS   0x0001          /* AGShow: nStyle: bubble chart */
#define AGBUBLEGCLR     0x0002
#define AGBUBXGRID      0x0004
#define AGBUBYGRID      0x0008
#define AGBUBMONO       0x0020
#define AGBUBFORECLR    0x0040

#define AGTAPENOLABELS  0x0001          /* AGShow: nStyle: tape graph */
#define AGTAPELEGCLR    0x0002
#define AGTAPEXGRID     0x0004
#define AGTAPEYGRID     0x0008
#define AGTAPEMONO      0x0020
#define AGTAPEFORECLR   0x0040

#define AGSRFCNOLABELS  0x0001          /* AGShow: nStyle: surface graph */
#define AGSRFCXGRID     0x0004
#define AGSRFCYGRID     0x0008
#define AGSRFCMONO      0x0020
#define AGSRFCNET       0x4000
#define AGSRFCBLACKLINES 0x2000
#define AGSRFCSIDEWALL  0x1000

#define AGBWPARAMETRIC 0x4000          /* AGShow: nStyle: box whisker */
#define AGBWNONOTCH    0x2000
#define AGBWNOWHISKER  0x1000
#define AGBWNOSAMPLES  0x0800
#define AGBWNOLABELS   0x0001
#define AGBWXGRID      0x0004
#define AGBWYGRID      0x0008
#define AGBWMONO       0x0020

#define AGMEAN          0x0001          /* AGShow: nStats */
#define AGMINMAX        0x0002
#define AGSD            0x0004
#define AGLINEFIT       0x0008
#define AGCURVEFIT      0x0010
#define AGCLIPGRAPH     0x0020

int GSAPIFN AGBar2DGap(                          /* AGBar2DGap */
    int     nGap
);

int GSAPIFN AGCurveStyle(                       /* AGCurveStyle */
    int     nType,
    int     nOrder,
    int     nSteps
);

int GSAPIFN AGFontStyle(                        /* AGFontStyle */
    int     nUse,
    int     nFamily,
    int     nAttribs,
    int     nSize
);

#define AGFUSETITG      0               /* AGFontStyle: nUse */
#define AGFUSETITXY     1
#define AGFUSELABS      2
#define AGFUSELEG       3

int GSAPIFN AGLegendStyle(                      /* AGLegendStyle */
   int     nVert,
   int     nHoriz,
   int     nSize,
   int     nClr,
   int     nMode
);

#define AGLEGCENTRE     0
#define AGLEGBOTTOM     1
#define AGLEGTOP        2
#define AGLEGCENTRE     0
#define AGLEGLEFT       1
#define AGLEGRIGHT      2
#define AGLEGFRAME      1
#define AGLEGFILL       2
#define AGLEGDROPSHADOW 4
#define AGLEGRAISED     8
#define AGLEGLOWERED    16

int GSAPIFN AGXAxisStyle(                       /* AGXAxisStyle */
    int     nMode,
    int     nTicks,
    int     nLabEvery,
    double  fMax,
    double  fMin
);

int GSAPIFN AGYAxisStyle(                       /* AGYAxisStyle */
    int     nMode,
    int     nTicks,
    int     nLabEvery,
    double  fMax,
    double  fMin
);

int GSAPIFN AGYRAxisStyle(                       /* AGYRAxisStyle */
    int     nMode,
    int     nTicks,
    int     nLabEvery,
    double  fMax,
    double  fMin
);

#define AGVARORIGIN     0x0001          /* AGXAxisStyle AGYAxisStyle: nMode */
#define AGNOLABELS      0x0002
#define AGUTICKS        0x0004
#define AGUMAX          0x0008
#define AGUMIN          0x0010
#define AGLABEVERY      0x0020
#define AGNOTICKS       0x0040
#define AGTICKEVERY     0x0080
#define AGAXISLEFT      0x0100
#define AGAXISTOP       0x0100
#define AGAXISRIGHT     0x0200
#define AGAXISBOTTOM    0x0200
#define AGAXISMINORTICK  0x0400
#define AGAXISVERTLABELS 0x0800


int GSAPIFN AGHotLegend(                        /* AGHotLegend */
    int     nMode
);

int GSAPIFN AGHot(                              /* AGHot */
    int     nMode
);

int GSAPIFN AGRefresh3D(
    int     nMode               // currently none
);

int GSAPIFN AGSetPerspective(
    int     nMode,
    int     nRotation,
    int     nElevation,
    int     nEye
);

#define AG3DISO         0x0000
#define AG3DPERSPECTIVE 0x0001
#define AG3DFLIPLR      0x0002
#define AG3DFLIPFB      0x0004

int GSAPIFN AG3DStyle(
    int     nMode,
    int     nDepth,
    int     nXGap,
    int     nZGap
);

#define AG3DSETDEPTH    0x0001
#define AG3DSETXGAP     0x0002
#define AG3DSETZGAP     0x0004

int GSAPIFN AGInfo_(                            /* AGInfo */
    double FAR* lpfInfo,
    int     nIndex
);

#if defined( _MSC_VER )
double GSAPIFN AGInfo( int nIndex );
#elif defined( __cplusplus )
inline double AGInfo( int nIndex )
{
    double fInfo;
    return AGInfo_( &fInfo, nIndex ) == 0 ? fInfo : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSArea(                             /* Area */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fHt,
    int     nMode,
    int     nGroup
);

int GSAPIFN GSArea3D(                           /* Area3D */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fHt,
    double  fDepth,
    double  fAng,
    int     nMode
);

#define ARABS           1               /* Area Area3D: nMode */
#define ARVARX          2
#define ARPC            4

int GSAPIFN GSBar2D(                            /* Bar2D */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fSpace,
    double  fStackHt,
    int     nMode,
    int     nGroup
);

int GSAPIFN GSBar3D(                            /* Bar3D */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fSpace,
    double  fStackHt,
    double  fDepth,
    double  fAng,
    int     nMode,
    int     nGroup
);

#define BARSIMPLE       0               /* Bar2D Bar3D: nMode */
#define BARSTACK        1
#define BARCLUST        2
#define BARSTACKPC      3
#define BARCLUSTZ       4
#define BARFLOAT        5
#define BARHORIZ        8
#define BARVARPOS       16
#define BARHALFTONE     32
#define BARLASTFIRST    64
#define BARSYMINDEX   8192

int GSAPIFN GSBubbleChart(                      /* BubbleChart */
    double  fxOrg,
    double  fyOrg,
    int     nMode
);

int GSAPIFN GSGantt(                            /* Gantt */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    int     nMode,
    int     nGroup
);

#define GAVARY          1               /* Gantt: nMode */
#define GASPACE         2
#define GALASTFIRST     4

int GSAPIFN GSHLC(                              /* HLC */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    int     nMode,
    int     nClr
);

#define HLCVARX         1               /* HLC: nMode */
#define HLCNOCLOSE      2
#define HLCNOBARS       4
#define HLCTHICK        8
#define HLCOPEN         16
#define HLCCANDLESTICK  32

int GSAPIFN GSPie2D(                            /* Pie2D */
    double  fxOrg,
    double  fyOrg,
    double  fRad,
    int     nMode
);

int GSAPIFN GSPie3D(                            /* Pie3D */
    double  fxOrg,
    double  fyOrg,
    double  fRad,
    double  fDepth,
    double  fAng,
    int     nMode
);

#define PCNOEXPL        0               /* Pie2D Pie3D: nMode */
#define PCEXPL          1
#define PCSAMECLR       2
#define PCABSENT        4

int GSAPIFN GSPolar(                            /* Polar */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    int     nMode,
    int     nClr
);

#define POGLINE         1               /* Polar: nMode */
#define POGSYMBOL       2
#define POGSTICK        4
#define POGVARANG       8
#define POGTHICK        16
#define POGPATT         32
#define POGGROUPED      64

int GSAPIFN GSScatter(                          /* Scatter */
    double  fxOrg,
    double  fyOrg,
    int     nMode,
    int     nClr
);

int GSAPIFN GSTapeGraph(                        /* TapeGraph */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fDepth,
    double  fAng,
    int     nMode,
    int     nClr
);

#define TAPEVARX        1               /* TapeGraph: nMode */

int GSAPIFN GSLogLin(                           /* LogLin */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fCycleHt,
    double  fBaseVal,
    int     nMode,
    int     nClr
);

int GSAPIFN GSLogLog(                           /* LogLog */
    double  fxOrg,
    double  fyOrg,
    double  fCycleHt,
    double  fBaseY,
    double  fCycleWid,
    double  fBaseX,
    int     nMode,
    int     nClr
);

int GSAPIFN GSXYGraph(                          /* XYGraph */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    int     nMode,
    int     nClr
);

#define XYGSIMPLE       0               /* LogLin LogLog XYGraph: nMode */
#define XYGLINE         1
#define XYGSYMBOL       2
#define XYGSTICK        4
#define XYGVARX         8
#define XYGTHICK        16
#define XYGPATT         32
#define XYGGROUPED      64
#define XYGVARZ        128

int GSAPIFN GSOpenServer(                       /* OpenServer */
    LPCSTR  lpstrKey,
    LPCSTR  lpstrMode
);

int GSAPIFN GSCloseServer( void );              /* CloseServer */

int GSAPIFN GSOpenWin(                          /* OpenWin */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    double  fyExt,
    int     nStyle,
    int     nMode,
    LPCSTR  lpstrTitle
);

int GSAPIFN GSOpenChildWin(                     /* OpenChildWin */
    HWND    hWndParent,
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    double  fyExt,
    int     nStyle,
    int     nMode,
    LPCSTR  lpstrTitle
);

#define OWSTHICKFRAME   1               /* OpenWin OpenChildWin: nStyle */
#define OWSBORDER       2
#define OWSHSCROLL      4
#define OWSVSCROLL      8
#define OWSMAXIMIZEBOX  16
#define OWSMINIMIZEBOX  32
#define OWSSETFOCUS     64
#define OWSHIDDEN       128
#define OWSSYSMENU      256
#define OWSCAPTION      512
#define OWSCLIPCHILDREN 1024
#define OWSCLIPSIBLINGS 2048
#define OWSTRANSPARENT  4096

#define OWMFIXED        0               /* OpenWin OpenChildWin: nMode */
#define OWMFITHORZ      1
#define OWMFITVERT      2
#define OWMFITBOTH      3
#define OWMFITOPT       4
#define OWMFLEXIBLE     5
#define OWMCENTRED      256

int GSAPIFN GSCloseWin(                         /* CloseWin */
    int     nWin
);

int GSAPIFN GSOpenView(                         /* OpenView */
    int     nWin,
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    double  fyExt
);

int GSAPIFN GSCloseView(                        /* CloseView */
    int     nWin,
    int     nView,
    int     nMode
);

#define CVKEEP          0               /* CloseView: nMode */
#define CVDISCARD       1

int GSAPIFN GSUseView(                          /* UseView */
    int     nWin,
    int     nView
);

int GSAPIFN GSOnView(                           /* OnView */
    int     nWin,
    int     nView
);

int GSAPIFN GSOffView(                          /* OffView */
    int     nWin,
    int     nView
);

int GSAPIFN GSViewClip(                         /* ViewClip */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt
);

int GSAPIFN GSSetPal(                           /* SetPal */
    int     nClr,
    int     nR,
    int     nG,
    int     nB
);

int GSAPIFN GSSetBG(                            /* SetBG */
    int     nClr
);

int GSAPIFN GSGetBG( void );                    /* GetBG */

int GSAPIFN GSSetROP(                           /* SetROP */
    int     nROP
);

#define ROREPLACE       0               /* SetROP: nROP */
#define ROOR            1
#define ROXOR           2
#define RONOT           3

int GSAPIFN GSGetSXExt_(                        /* GetSXExt */
    double FAR* lpfxExt
);

#if defined( _MSC_VER )
double GSAPIFN GSGetSXExt( void );
#elif defined( __cplusplus )
inline double GSGetSXExt( void )
{
    double fxExt;
    return GSGetSXExt_( &fxExt ) == 0 ? fxExt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetSYExt_(                        /* GetSYExt */
    double FAR* lpfyExt
);

#if defined( _MSC_VER )
double GSAPIFN GSGetSYExt( void );
#elif defined( __cplusplus )
inline double GSGetSYExt( void )
{
    double fyExt;
    return GSGetSYExt_( &fyExt ) == 0 ? fyExt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetAXExt_(                        /* GetAXExt */
    double FAR* lpfxExt,
    HWND    hWnd
);

#if defined( _MSC_VER )
double GSAPIFN GSGetAXExt( HWND hWnd );
#elif defined( __cplusplus )
inline double GSGetAXExt( HWND hWnd )
{
    double fxExt;
    return GSGetAXExt_( &fxExt, hWnd ) == 0 ? fxExt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetAYExt_(                        /* GetAYExt */
    double FAR* lpfyExt,
    HWND    hWnd
);

#if defined( _MSC_VER )
double GSAPIFN GSGetAYExt( HWND hWnd );
#elif defined( __cplusplus )
inline double GSGetAYExt( HWND hWnd )
{
    double fyExt;
    return GSGetAYExt_( &fyExt, hWnd ) == 0 ? fyExt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetWXExt_(                        /* GetWXExt */
    double FAR* lpfxExt,
    int     nMode,
    int     nUnits
);

#if defined( _MSC_VER )
double GSAPIFN GSGetWXExt( int nMode, int nUnits );
#elif defined( __cplusplus )
inline double GSGetWXExt( int nMode, int nUnits )
{
    double fxExt;
    return GSGetWXExt_( &fxExt, nMode, nUnits ) == 0 ? fxExt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetWYExt_(                        /* GetWYExt */
    double FAR* lpfyExt,
    int     nMode,
    int     nUnits
);

#if defined( _MSC_VER )
double GSAPIFN GSGetWYExt( int nMode, int nUnits );
#elif defined( __cplusplus )
inline double GSGetWYExt( int nMode, int nUnits )
{
    double fyExt;
    return GSGetWYExt_( &fyExt, nMode, nUnits ) == 0 ? fyExt : -1;
}
#endif  /* _MSC_VER */

#define GWWHOLE         0               /* GetWXExt GetWYExt: nMode */
#define GWCLIPPED       1

int GSAPIFN GSGetVXExt_(                        /* GetVXExt */
    double FAR* lpfxExt
);

#if defined( _MSC_VER )
double GSAPIFN GSGetVXExt( void );
#elif defined( __cplusplus )
inline double GSGetVXExt( void )
{
    double fxExt;
    return GSGetVXExt_( &fxExt ) == 0 ? fxExt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetVYExt_(                        /* GetVYExt */
    double FAR* lpfyExt
);

#if defined( _MSC_VER )
double GSAPIFN GSGetVYExt( void );
#elif defined( __cplusplus )
inline double GSGetVYExt( void )
{
    double fyExt;
    return GSGetVYExt_( &fyExt ) == 0 ? fyExt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetSFHt_(                         /* GetSFHt */
    double FAR* lpfHt
);

#if defined( _MSC_VER )
double GSAPIFN GSGetSFHt( void );
#elif defined( __cplusplus )
inline double GSGetSFHt( void )
{
    double fHt;
    return GSGetSFHt_( &fHt ) == 0 ? fHt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetSFWid_(                        /* GetSFWid */
    double FAR* lpfWid
);

#if defined( _MSC_VER )
double GSAPIFN GSGetSFWid( void );
#elif defined( __cplusplus )
inline double GSGetSFWid( void )
{
    double fWid;
    return GSGetSFWid_( &fWid ) == 0 ? fWid : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetVer(                           /* GetVer */
    int     nMode
);

#define GVSERVER        0               /* GetVer: nMode */
#define GVDLL           1

HWND GSAPIFN GSWinHandle(                       /* WinHandle */
    int     nWin
);

int GSAPIFN GSWinNotify(                        /* WinNotify */
    HWND    hWnd,
    int     nWM,
    int     nEvents
);

#define WNPAINT         1               /* WinNotify: nEvents */
#define WNSIZE          2

int GSAPIFN GSWinPaint(                         /* WinPaint */
    int     nMode
);

#define WPAUTO          0               /* WinPaint: nMode */
#define WPMANUAL        1
#define WPNONE          2
#define WPPAINT         3
#define WPBITMAP        4
#define WPMETAFILE      5

int GSAPIFN GSAxis(                             /* Axis */
    double  fxOrg,
    double  fyOrg,
    double  fLen,
    double  fTickLen,
    int     nMajDivs,
    int     nMinDivs,
    int     nMode,
    int     nStyle,
    int     nClr
);

int GSAPIFN GSLogAxis(                          /* LogAxis */
    double  fxOrg,
    double  fyOrg,
    double  fLen,
    double  fTickLen,
    int     nCycles,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define AXTICKOUT       0               /* Axis LogAxis: nMode */
#define AXTICKIN        1
#define AXTICKTHRU      2
#define AXISX           0
#define AXISY           4

int GSAPIFN GSGrid(                             /* Grid */
    double  fxOrg,
    double  fyOrg,
    double  fAxisLen,
    double  fGridLen,
    int     nDivs,
    int     nMode,
    int     nStyle,
    int     nClr
);

int GSAPIFN GSLogGrid(                          /* LogGrid */
    double  fxOrg,
    double  fyOrg,
    double  fAxisLen,
    double  fGridLen,
    int     nCycles,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define GRX             0               /* Grid LogGrid: nMode */
#define GRY             1
#define GRNOFIRST       2
#define GRNOLAST        4

int GSAPIFN GSPolarAxes(                        /* PolarAxes */
    double  fxOrg,
    double  fyOrg,
    double  fRad,
    int     nRadDivs,
    int     nAngDivs,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define PARADGRID       1               /* PolarAxes: nMode */
#define PAANGGRID       2
#define PATHICK         4

int GSAPIFN GSCage3D(                           /* Cage3D */
    double  fxOrg,
    double  fyOrg,
    double  fxLen,
    double  fyLen,
    double  fzLen,
    double  fAng,
    double  fThick,
    int     nxGrids,
    int     nyGrids,
    int     nzGrids,
    int     nMode,
    int     nClr1,
    int     nClr2
);

#define CGGRIDX         1               /* Cage3D: nMode */
#define CGGRIDY         2
#define CGGRIDZ         4

int GSAPIFN GSStatsWin(                         /* StatsWin */
    double  fxBL,
    double  fyBL,
    double  fxTR,
    double  fyTR
);

int GSAPIFN GSMean(                             /* Mean */
    int     nStyle,
    int     nClr
);

int GSAPIFN GSSD(                               /* SD */
    int     nStyle,
    int     nClr
);

int GSAPIFN GSMinMax(                           /* MinMax */
    int     nStyle,
    int     nClr
);

int GSAPIFN GSLineFit(                          /* LineFit */
    int     nStyle,
    int     nClr
);

int GSAPIFN GSCurveFit(                         /* CurveFit */
    int     nType,
    int     nOrder,
    int     nSteps,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define CFPOLY          0               /* CurveFit: nType */
#define CFLOG           1
#define CFEXP1          2
#define CFEXP2          3
#define CFPOWER         4
#define CFINV1          5
#define CFINV2          6
#define CFINV3          7
#define CFINV4          8
#define CFINV5          9
#define CFSPLINE        10
#define CFMOVINGAVEMID  11
#define CFMOVINGAVEEND  12

int GSAPIFN GSGetCC_(                           /* GetCC */
    double FAR* lpfVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetCC( void );
#elif defined( __cplusplus )
inline double GSGetCC( void )
{
    double fVal;
    return GSGetCC_( &fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetCurveCoeff_(                   /* GetCurveCoeff */
    double FAR* lpfVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetCurveCoeff( void );
#elif defined( __cplusplus )
inline double GSGetCurveCoeff( void )
{
    double fVal;
    return GSGetCurveCoeff_( &fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetMax_(                          /* GetMax */
    double FAR* lpfVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetMax( void );
#elif defined( __cplusplus )
inline double GSGetMax( void )
{
    double fVal;
    return GSGetMax_( &fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetMean_(                         /* GetMean */
    double FAR* lpfVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetMean( void );
#elif defined( __cplusplus )
inline double GSGetMean( void )
{
    double fVal;
    return GSGetMean_( &fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetMin_(                          /* GetMin */
    double FAR* lpfVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetMin( void );
#elif defined( __cplusplus )
inline double GSGetMin( void )
{
    double fVal;
    return GSGetMin_( &fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetSD_(                           /* GetSD */
    double FAR* lpfVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetSD( void );
#elif defined( __cplusplus )
inline double GSGetSD( void )
{
    double fVal;
    return GSGetSD_( &fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSStatsArr(                         /* StatsArr */
    int     nIndex
);

int GSAPIFN GSPicRead(                          /* PicRead */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    int     nFormat,
    int     nMode,
    LPCSTR  lpstrFile
);

int GSAPIFN GSPicWrite(                         /* PicWrite */
    double  fxBL,
    double  fyBL,
    double  fWid,
    double  fHt,
    int     nFormat,
    int     nMode,
    LPCSTR  lpstrFile
);

#define PXBMP           1               /* PicRead PicWrite: nFormat */
#define PXPMF           2
#define PXWMF           3
#define PXDIB           4
#define PXPCX           5
#define PXMONO          256

#define PXCENTRE        1               /* PicRead PicWrite: nMode */
#define PXSTRETCH       2
#define PXTILE          3

int GSAPIFN GSClipRead(                         /* ClipRead */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    int     nFormat,
    int     nMode
);

int GSAPIFN GSClipWrite(                        /* ClipWrite */
    double  fxBL,
    double  fyBL,
    double  fWid,
    double  fHt,
    int     nFormat,
    int     nMode
);

#define CBBMP           1               /* ClipRead ClipWrite: nFormat */
#define CBWMF           2
#define CBDIB           4
#define CBMONO          256

#define CBCENTRE        1               /* ClipRead ClipWrite: nMode */
#define CBSTRETCH       2
#define CBTILE          3

HGLOBAL GSAPIFN GSGetMF(                        /* GetMF */
    int     nMode
);

int GSAPIFN GSLineAbs(                          /* LineAbs */
    double  fxA,
    double  fyA,
    double  fxB,
    double  fyB,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define LAUPDATE        1               /* LineAbs: nMode */
#define LATHICK         4
#define LACONT          16

int GSAPIFN GSFixPos(                           /* FixPos */
    double  fx,
    double  fy
);

int GSAPIFN GSMovePos(                          /* MovePos */
    double  fxr,
    double  fya,
    int     nMode
);

#define MPPOLAR         1               /* MovePos: nMode */

int GSAPIFN GSLineRel(                          /* LineRel */
    double  fxr,
    double  fya,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define LRNOUPDATE      1               /* LineRel: nMode */
#define LRPOLAR         2
#define LRTHICK         4

int GSAPIFN GSGetCurX_(                         /* GetCurX */
    double FAR* lpfx
);

#if defined( _MSC_VER )
double GSAPIFN GSGetCurX( void );
#elif defined( __cplusplus )
inline double GSGetCurX( void )
{
    double fx;
    return GSGetCurX_( &fx ) == 0 ? fx : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetCurY_(                         /* GetCurY */
    double FAR* lpfy
);

#if defined( _MSC_VER )
double GSAPIFN GSGetCurY( void );
#elif defined( __cplusplus )
inline double GSGetCurY( void )
{
    double fy;
    return GSGetCurY_( &fy ) == 0 ? fy : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSArc(                              /* Arc */
    double  fxOrg,
    double  fyOrg,
    double  fRadius,
    double  fAng1,
    double  fAng2,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define AARADIUS        1               /* Arc: nMode */
#define AAFILL          2
#define AATHICK         4

int GSAPIFN GSArrow(                            /* Arrow */
    double  fxA,
    double  fyA,
    double  fxB,
    double  fyB,
    double  fHeadLen,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define AWTHIN          1               /* Arrow: nMode */
#define AWMEDIUM        0
#define AWTHICK         2
#define AWFILLED        0
#define AWHOLLOW        4

int GSAPIFN GSBox2D(                            /* Box2D */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    int     nPatt,
    int     nClr
);

int GSAPIFN GSBox3D(                            /* Box3D */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    double  fDepth,
    double  fAng,
    int     nPatt,
    int     nClr1,
    int     nClr2
);

#define BXNOBOX         128             /* Box2D Box3D: nMode */
#define BXSHADOW        256
#define BXBORDER        512
#define BXRAISED        1024
#define BXLOWERED       2048

int GSAPIFN GSCircle(                           /* Circle */
    double  fxOrg,
    double  fyOrg,
    double  fRadius,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define CCFILL          2               /* Circle: nMode */
#define CCTHICK         4

int GSAPIFN GSEllipse(                          /* Ellipse */
    double  fxBL,
    double  fyBL,
    double  fxTR,
    double  fyTR,
    double  fxa,
    double  fya,
    double  fxb,
    double  fyb,
    int     nMode,
    int     nStyle,
    int     nClr
);

#define ELRADIUS        1               /* Ellipse: nMode */
#define ELFILL          2
#define ELTHICK         4

int GSAPIFN GSPolyFill(                         /* PolyFill */
    double  fxr,
    double  fya,
    int     nMode,
    double  fAng,
    int     nPatt,
    int     nClr
);

#define PFXYORG         0               /* PolyFill: nMode */
#define PFRAORG         1
#define PFXYDATA        0
#define PFRADATA        2
#define PFMIRRORV       4
#define PFMIRRORH       8

int GSAPIFN GSPolyVec(                          /* PolyVec */
    double  fxr,
    double  fya,
    int     nMode,
    double  fAng,
    int     nStyle,
    int     nClr
);

#define PVXYORG         0               /* PolyVec: nMode */
#define PVRAORG         1
#define PVXYDATA        0
#define PVRADATA        2
#define PVMIRRORV       4
#define PVMIRRORH       8
#define PVTHICK         16

int GSAPIFN GSShade(                            /* Shade */
    double  fxOrg,
    double  fyOrg,
    int     nPatt,
    int     nClr
);

int GSAPIFN GSClearView(                        /* ClearView */
    int     nMode
);

#define CLTRANSP        0               /* ClearView: nMode */
#define CLOPAQUE        1

int GSAPIFN GSDefPatt(                          /* DefPatt */
    int     nBitmap,
    const WORD FAR* lpwBitmap
);

int GSAPIFN GSDataScale(                        /* DataScale */
    double  fScale
);

int GSAPIFN GSDataRange(                        /* DataRange */
    int     nFirst,
    int     nLast
);

int GSAPIFN GSDataTrans(                        /* DataTrans */
    int     nPts,
    int     nGroup,
    const double FAR* lpfA,
    const double FAR* lpfD,
    const int FAR* lpnPatt,
    const int FAR* lpnSymbol,
    const int FAR* lpnAux,
    const int FAR* lpnClr
);

int GSAPIFN GSDataReset( void );                /* DataReset */

int GSAPIFN GSDataDim(                          /* DataDim */
    int     nPts,
    int     nGroup
);

int GSAPIFN GSDataAmp(                          /* DataAmp */
    int     nPts,
    int     nGroup,
    const double FAR* lpfA
);

int GSAPIFN GSDataDist(                         /* DataDist */
    int     nPts,
    const double FAR* lpfD
);

int GSAPIFN GSDataPatt(                         /* DataPatt */
    int     nPts,
    const int FAR* lpnPatt
);

int GSAPIFN GSDataSym(                          /* DataSym */
    int     nPts,
    const int FAR* lpnSymbol
);

int GSAPIFN GSDataAux(                          /* DataAux */
    int     nPts,
    const int FAR* lpnAux
);

int GSAPIFN GSDataClr(                          /* DataClr */
    int     nPts,
    const int FAR* lpnClr
);

int GSAPIFN GSDataStoreAmp(                     /* DataStoreAmp */
    int     nPt,
    int     nGroup,
    double  fAmp
);

int GSAPIFN GSDataStoreDist(                    /* DataStoreDist */
    int     nPt,
    double  fDist
);

int GSAPIFN GSDataStorePatt(                    /* DataStorePatt */
    int     nPt,
    int     nPatt
);

int GSAPIFN GSDataStoreSym(                     /* DataStoreSym */
    int     nPt,
    int     nSymbol
);

int GSAPIFN GSDataStoreAux(                     /* DataStoreAux */
    int     nPt,
    int     nAux
);

int GSAPIFN GSDataStoreClr(                     /* DataStoreClr */
    int     nPt,
    int     nClr
);

int GSAPIFN GSDataGetAmp_(                      /* DataGetAmp */
    double FAR* lpfAmp,
    int     nPt,
    int     nGroup
);

#if defined( _MSC_VER )
double GSAPIFN GSDataGetAmp( int nPt, int nGroup );
#elif defined( __cplusplus )
inline double GSDataGetAmp( int nPt, int nGroup )
{
    double fVal;
    return GSDataGetAmp_( &fVal, nPt, nGroup ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSDataGetDist_(                     /* DataGetDist */
    double FAR* lpfDist,
    int     nPt
);

#if defined( _MSC_VER )
double GSAPIFN GSDataGetDist( int nPt );
#elif defined( __cplusplus )
inline double GSDataGetDist( int nPt )
{
    double fVal;
    return GSDataGetDist_( &fVal, nPt ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSDataGetPatt(                      /* DataGetPatt */
    int     nPt
);

int GSAPIFN GSDataGetSym(                       /* DataGetSym */
    int     nPt
);

int GSAPIFN GSDataGetAux(                       /* DataGetAux */
    int     nPt
);

int GSAPIFN GSDataGetClr(                       /* DataGetClr */
    int     nPt
);

int GSAPIFN GSOpenPrn(                          /* OpenPrn */
    LPCSTR  lpstrDevice,
    LPCSTR  lpstrFile,
    int     nMode
);

#define PRNSETMODE      1               /* OpenPrn: nMode */
#define PRNPORTRAIT     2
#define PRNLANDSCAPE    4
#define PRNSETMODECANCEL 8

int GSAPIFN GSClosePrn( void );                 /* ClosePrn */

int GSAPIFN GSGetPrnWid_(                       /* GetPrnWid */
    double FAR* lpfWid,
    int     nUnits
);

#if defined( _MSC_VER )
double GSAPIFN GSGetPrnWid( int nUnits );
#elif defined( __cplusplus )
inline double GSGetPrnWid( int nUnits )
{
    double fWid;
    return GSGetPrnWid_( &fWid, nUnits ) == 0 ? fWid : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetPrnHt_(                        /* GetPrnHt */
    double FAR* lpfHt,
    int     nUnits
);

#if defined( _MSC_VER )
double GSAPIFN GSGetPrnHt( int nUnits );
#elif defined( __cplusplus )
inline double GSGetPrnHt( int nUnits )
{
    double fHt;
    return GSGetPrnHt_( &fHt, nUnits ) == 0 ? fHt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSPrnSetup(                         /* PrnSetup */
    double  fxBL,
    double  fyBL,
    double  fxTR,
    double  fyTR,
    int     nUnits,
    int     nMode
);

#define PRNMM       UNMM                /* GetPrnWid GetPrnHt PrnSetup: nUnits */
#define PRNINCH     UNINCH

#define PRNFIXED    OWMFIXED            /* PrnSetup: nMode */
#define PRNFITHORZ  OWMFITHORZ
#define PRNFITVERT  OWMFITVERT
#define PRNFITBOTH  OWMFITBOTH
#define PRNFITOPT   OWMFITOPT
#define PRNCENTRED  OWMCENTRED

int GSAPIFN GSPrnOut(                           /* PrnOut */
    int     nView,
    int     nCopies,
    int     nMode
);

#define PRNWINDOW       0               /* PrnOut: nMode */
#define PRNVIEW         1
#define PRNFF           2
#define PRNFRAME        4
#define PRNCANCEL               8

int GSAPIFN GSLoadRFont(                        /* LoadRFont */
    int     nFamily,
    int     nAttrib,
    int     nSize,
    int     nPitch
);

int GSAPIFN GSSetRFontFace(                     /* SetRFontFace */
    int     nFamily,
    LPCSTR  lpstrFaceName
);

int GSAPIFN GSLoadVFont(                        /* LoadVFont */
    int     nFamily,
    int     nAttrib,
    int     nPitch
);

int GSAPIFN GSSetVFontFace(                     /* SetVFontFace */
    int     nFamily,
    LPCSTR  lpstrFaceName
);

#define FOROMAN         1               /* LoadRFont LoadVFont: nFamily */
#define FOSWISS         2
#define FOMODERN        3
#define FOSCRIPT        4
#define FODECO          5

#define FOITALIC        16              /* LoadRFont LoadVFont: nAttrib */
#define FOBOLD          32
#define FOULINE         64

#define FOFIXED         128             /* LoadRFont LoadVFont: nPitch */

int GSAPIFN GSRText(                            /* RText */
    double  fxOrg,
    double  fyOrg,
    int     nCSet,
    int     nMode,
    int     nClr,
    LPCSTR  lpstrString
);

int GSAPIFN GSVText(                            /* VText */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    double  fAng,
    int     nCSet,
    int     nMode,
    int     nClr,
    LPCSTR  lpstrString
);

int GSAPIFN GSGetRTextHt_(                      /* GetRTextHt */
    double FAR* lpfHt,
    int     nCSet,
    int     nMode,
    LPCSTR  lpstrString
);

#if defined( _MSC_VER )
double GSAPIFN GSGetRTextHt( int nCSet, int nMode, LPCSTR lpstrString );
#elif defined( __cplusplus )
inline double GSGetRTextHt( int nCSet, int nMode, LPCSTR lpstrString )
{
    double fHt;
    return GSGetRTextHt_( &fHt, nCSet, nMode, lpstrString ) == 0 ? fHt : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetRTextWid_(                     /* GetRTextWid */
    double FAR* lpfWid,
    int     nCSet,
    int     nMode,
    LPCSTR  lpstrString
);

#if defined( _MSC_VER )
double GSAPIFN GSGetRTextWid( int nCSet, int nMode, LPCSTR lpstrString );
#elif defined( __cplusplus )
inline double GSGetRTextWid( int nCSet, int nMode, LPCSTR lpstrString )
{
    double fWid;
    return GSGetRTextWid_( &fWid, nCSet, nMode, lpstrString ) == 0 ? fWid : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSLabelX(                           /* LabelX */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fWid,
    double  fHt,
    int     nNLabs,
    int     nCSet,
    int     nMode,
    int     nClr,
    LPCSTR FAR* lplpstrLabs
);

int GSAPIFN GSLabelY(                           /* LabelY */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double   fWid,
    double  fHt,
    int     nNLabs,
    int     nCSet,
    int     nMode,
    int     nClr,
    LPCSTR FAR* lplpstrLabs
);

int GSAPIFN GSLabelPie(                         /* LabelPie */
    double  fxOff,
    double  fRad,
    double  fWid,
    double  fHt,
    int     nNLabs,
    int     nMode,
    int     nCSet,
    int     nTMode,
    int     nClr,
    LPCSTR FAR* lplpstrLabs
);

#define LPSEGCLR        1               /* LabelPie LabelnPie: nMode */
#define LPNOLINES       2
#define LPPC            4
#define LPPCCHAR        8
#define LPSMART         16

int GSAPIFN GSLabelnX(                          /* LabelnX */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fWid,
    double  fHt,
    double  fBaseVal,
    double  fStepVal,
    int     nPrec,
    int     nNLabs,
    int     nCSet,
    int     nMode,
    int     nClr
);

int GSAPIFN GSLabelnY(                          /* LabelnY */
    double  fxOrg,
    double  fyOrg,
    double  fInc,
    double  fWid,
    double  fHt,
    double  fBaseVal,
    double  fStepVal,
    int     nPrec,
    int     nNLabs,
    int     nCSet,
    int     nMode,
    int     nClr
);

int GSAPIFN GSLabelnPie(                        /* LabelnPie */
    double  fxOff,
    double  fRad,
    double  fWid,
    double  fHt,
    int     nPrec,
    int     nMode,
    int     nCSet,
    int     nTMode,
    int     nClr
);

int GSAPIFN GSLegend(                           /* Legend */
    double  fxOrg,
    double  fyOrg,
    double  fWid,
    double  fHt,
    int     nNLegs,
    int     nRows,
    int     nMode,
    int     nCSet,
    int     nTMode,
    int     nTClr,
    const int FAR* lpnBClr,
    const int FAR* lpnBPatt,
    LPCSTR FAR* lplpstrLabs
);

#define LGBOX           1               /* Legend: nMode */
#define LGTXCLR         2
#define LGBG            4
#define LGLINE          8
#define LGSYMBOL        16

int GSAPIFN GSDataLabels(                           /* Data labels */
    int     nMode,
    int     nPrec,
    int     nCSet,
    int     nTMode,
    int     nTClr,
    double  fDataOffset,
    int     nNLegs,
    LPCSTR FAR* lplpstrLabs
);

#define DLTEXT          0
#define DLDATA          0
#define DLGROUPCLR      4

int GSAPIFN GSSizeSymbol(                       /* SizeSymbol */
    double  fDiam
);

int GSAPIFN GSSymbol(                           /* Symbol */
    double  fxOrg,
    double  fyOrg,
    int     nSymbol,
    int     nClr
);

#define SYHOLLOWFILL    256

int GSAPIFN GSHotGraph(                         /* HotGraph */
    int     nMode
);

int GSAPIFN GSMClrRgn(                          /* MClrRgn */
    int     nRgn
);

int GSAPIFN GSMGetX_(                           /* MGetX */
    double FAR* lpfx
);

#if defined( _MSC_VER )
double GSAPIFN GSMGetX( void );
#elif defined( __cplusplus )
inline double GSMGetX( void )
{
    double fx;
    return GSMGetX_( &fx ) == 0 ? fx : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSMGetY_(                           /* MGetY */
    double FAR* lpfy
);

#if defined( _MSC_VER )
double GSAPIFN GSMGetY( void );
#elif defined( __cplusplus )
inline double GSMGetY( void )
{
    double fy;
    return GSMGetY_( &fy ) == 0 ? fy : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSMMotion( void );                  /* MMotion */

int GSAPIFN GSMNotify(                          /* MNotify */
    HWND    hWnd,
    int     nWM,
    int     nEvents
);

#define MNTRACK         1               /* MNotify: nEvents */
#define MNPRESS         2
#define MNHIT           4
#define MNHITPT         8
#define MNHITOVERLAY    16

int GSAPIFN GSMPtrOff( void );                  /* MPtrOff */

int GSAPIFN GSMPtrOn( void );                   /* MPtrOn */

int GSAPIFN GSMPtrType(                         /* MPtrType */
    int     nType
);

#define MCARROW         0               /* MPtrType: nType */
#define MCIBEAM         1
#define MCWAIT          2
#define MCCROSS         3
#define MCUPARROW       4
#define MCSIZE          5
#define MCICON          6
#define MCSIZENWSE      7
#define MCSIZENESW      8
#define MCSIZEWE        9
#define MCSIZENS        10

int GSAPIFN GSMSetRgn(                          /* MSetRgn */
    double  fxr,
    double  fya,
    int     nMode,
    double  fAng
);

int GSAPIFN GSMStatus( void );                  /* MStatus */

#define MBLEFT          1               /* MStatus: return */
#define MBMIDDLE        2
#define MBRIGHT         4

int GSAPIFN GSGetSin_(                          /* GetSin */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetSin( double fVal );
#elif defined( __cplusplus )
inline double GSGetSin( double fVal )
{
    return GSGetSin_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetCos_(                          /* GetCos */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetCos( double fVal );
#elif defined( __cplusplus )
inline double GSGetCos( double fVal )
{
    return GSGetCos_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetTan_(                          /* GetTan */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetTan( double fVal );
#elif defined( __cplusplus )
inline double GSGetTan( double fVal )
{
    return GSGetTan_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetASin_(                         /* GetASin */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetASin( double fVal );
#elif defined( __cplusplus )
inline double GSGetASin( double fVal )
{
    return GSGetASin_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetACos_(                         /* GetACos */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetACos( double fVal );
#elif defined( __cplusplus )
inline double GSGetACos( double fVal )
{
    return GSGetACos_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetATan_(                         /* GetATan */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetATan( double fVal );
#elif defined( __cplusplus )
inline double GSGetATan( double fVal )
{
    return GSGetATan_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetPi_(                           /* GetPi */
    double FAR* lpfVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetPi( void );
#elif defined( __cplusplus )
inline double GSGetPi( void )
{
    double fVal;
    return GSGetPi_( &fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetE_(                            /* GetE */
    double FAR* lpfVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetE( void );
#elif defined( __cplusplus )
inline double GSGetE( void )
{
    double fVal;
    return GSGetE_( &fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetLogE_(                         /* GetLogE */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetLogE( double fVal );
#elif defined( __cplusplus )
inline double GSGetLogE( double fVal )
{
    return GSGetLogE_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetALogE_(                        /* GetALogE */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetALogE( double fVal );
#elif defined( __cplusplus )
inline double GSGetALogE( double fVal )
{
    return GSGetALogE_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetLog10_(                        /* GetLog10 */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetLog10( double fVal );
#elif defined( __cplusplus )
inline double GSGetLog10( double fVal )
{
    return GSGetLog10_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetALog10_(                       /* GetALog10 */
    double FAR* lpfVal,
    double  fVal
);

#if defined( _MSC_VER )
double GSAPIFN GSGetALog10( double fVal );
#elif defined( __cplusplus )
inline double GSGetALog10( double fVal )
{
    return GSGetALog10_( &fVal, fVal ) == 0 ? fVal : -1;
}
#endif  /* _MSC_VER */

int GSAPIFN GSGetStatsInfo_(                       /* GetStatsInfo */
    double FAR* lpfVal,
    int nIndex
);

#ifdef  __cplusplus
}
#endif

#endif  /* __GSW_H__ */
