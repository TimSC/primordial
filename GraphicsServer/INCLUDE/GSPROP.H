// C/C++ prototypes for Graphics Server Property DLL

#ifndef __GSPROP_H__
#define __GSPROP_H__

typedef double FAR* LPDOUBLE;

#ifdef  __cplusplus
extern "C" {
#endif

HWND WINAPI EXPORT CreateGraph( HWND hwnd, BOOL bEnabled );
HWND WINAPI EXPORT CreateGraphEx( LPCSTR lpszWindowName,
                                  DWORD dwStyle, const RECT& rect,
                                  HWND hParentWnd, UINT Id );

int WINAPI EXPORT GetBackdrop( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetBackdrop( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetBackdropStyle( HWND hwnd );
int WINAPI EXPORT SetBackdropStyle( HWND hwnd, int BackdropStyle );
int WINAPI EXPORT GetBottomTitle( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetBottomTitle( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetCageFlip( HWND hwnd );
int WINAPI EXPORT SetCageFlip( HWND hwnd, int CageFlip );
int WINAPI EXPORT GetNumColors( HWND hwnd );
int WINAPI EXPORT GetColorData( HWND hwnd, int Index );
int WINAPI EXPORT SetColorData( HWND hwnd, LPINT ColorData );
int WINAPI EXPORT GetCurveOrder( HWND hwnd );
int WINAPI EXPORT SetCurveOrder( HWND hwnd, int CurveOrder );
int WINAPI EXPORT GetCurveSteps( HWND hwnd );
int WINAPI EXPORT SetCurveSteps( HWND hwnd, int CurveSteps );
int WINAPI EXPORT GetCurveType( HWND hwnd );
int WINAPI EXPORT SetCurveType( HWND hwnd, int CurveType );
int WINAPI EXPORT GetDataReset( HWND hwnd );
int WINAPI EXPORT SetDataReset( HWND hwnd, int DataReset );
int WINAPI EXPORT GetDrawMode( HWND hwnd );
int WINAPI EXPORT SetDrawMode( HWND hwnd, int DrawMode );
int WINAPI EXPORT GetDrawStyle( HWND hwnd );
int WINAPI EXPORT SetDrawStyle( HWND hwnd, int DrawStyle );
int WINAPI EXPORT GetElevation( HWND hwnd );
int WINAPI EXPORT SetElevation( HWND hwnd, int Elevation );
int WINAPI EXPORT GetExtraData( HWND hwnd, int ThisSet, int Index );
int WINAPI EXPORT SetExtraData( HWND hwnd, int ThisSet, LPINT ExtraData );
int WINAPI EXPORT GetFontFamily( HWND hwnd, int Index );
int WINAPI EXPORT SetFontFamily( HWND hwnd, LPINT FontFamily );
int WINAPI EXPORT GetFontName( HWND hwnd, int Index, LPSTR lpsz  );
int WINAPI EXPORT SetFontName( HWND hwnd, LPCSTR FAR *FontName );
int WINAPI EXPORT GetFontSize( HWND hwnd, int Index );
int WINAPI EXPORT SetFontSize( HWND hwnd, LPINT FontSize );
int WINAPI EXPORT GetFontStyle( HWND hwnd, int Index );
int WINAPI EXPORT SetFontStyle( HWND hwnd, LPINT FontStyle );
int WINAPI EXPORT GetGraphCaption( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetGraphCaption( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT SetGraphData( HWND hwnd, int ThisSet, LPDOUBLE GraphData );
int WINAPI EXPORT GetGraphStyle( HWND hwnd );
int WINAPI EXPORT SetGraphStyle( HWND hwnd, int GraphStyle );
int WINAPI EXPORT GetGraphTitle( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetGraphTitle( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetGraphType( HWND hwnd );
int WINAPI EXPORT SetGraphType( HWND hwnd, int GraphType );
int WINAPI EXPORT GetGridStyle( HWND hwnd );
int WINAPI EXPORT SetGridStyle( HWND hwnd, int GridStyle );
int WINAPI EXPORT GetHot( HWND hwnd );
int WINAPI EXPORT SetHot( HWND hwnd, int Hot );
int WINAPI EXPORT GetImageFile( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetImageFile( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetLabelText( HWND hwnd, int Index, LPSTR lpsz );
int WINAPI EXPORT SetLabelText( HWND hwnd, LPCSTR FAR *LabelText );
int WINAPI EXPORT GetLabelEvery( HWND hwnd );
int WINAPI EXPORT SetLabelEvery( HWND hwnd, int LabelEvery );
int WINAPI EXPORT GetLabels( HWND hwnd );
int WINAPI EXPORT SetLabels( HWND hwnd, int Labels );
int WINAPI EXPORT GetLeftTitle( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetLeftTitle( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetLegendText( HWND hwnd, int Index, LPSTR lpsz );
int WINAPI EXPORT SetLegendText( HWND hwnd, LPCSTR FAR *LegendText );
int WINAPI EXPORT GetLegendStyle( HWND hwnd );
int WINAPI EXPORT SetLegendStyle( HWND hwnd, int LegendStyle );
int WINAPI EXPORT GetLineStats( HWND hwnd );
int WINAPI EXPORT SetLineStats( HWND hwnd, int LineStats );
int WINAPI EXPORT GetMousePointer( HWND hwnd );
int WINAPI EXPORT SetMousePointer( HWND hwnd, int MousePointer );
int WINAPI EXPORT GetNumPoints( HWND hwnd );
int WINAPI EXPORT SetNumPoints( HWND hwnd, int NumPoints );
int WINAPI EXPORT GetNumSets( HWND hwnd );
int WINAPI EXPORT SetNumSets( HWND hwnd, int NumSets );
int WINAPI EXPORT GetNWin( HWND hwnd );
int WINAPI EXPORT SetNWin( HWND hwnd, int NWin );
int WINAPI EXPORT GetPalette( HWND hwnd );
int WINAPI EXPORT SetPalette( HWND hwnd, int Palette );
int WINAPI EXPORT GetNumPatterns( HWND hwnd );
int WINAPI EXPORT GetPatternData( HWND hwnd, int Index );
int WINAPI EXPORT SetPatternData( HWND hwnd, LPINT PatternData );
int WINAPI EXPORT GetPatternedLines( HWND hwnd );
int WINAPI EXPORT SetPatternedLines( HWND hwnd, int PatternedLines );
int WINAPI EXPORT GetPerspective( HWND hwnd );
int WINAPI EXPORT SetPerspective( HWND hwnd, int Perspective );
int WINAPI EXPORT GetPrintStyle( HWND hwnd );
int WINAPI EXPORT SetPrintStyle( HWND hwnd, int PrintStyle );
int WINAPI EXPORT SetPropDialog( HWND hwnd, int PropDialog );
int WINAPI EXPORT GetQuickData( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetQuickData( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetRandomData( HWND hwnd );
int WINAPI EXPORT SetRandomData( HWND hwnd, int RandomData );
int WINAPI EXPORT GetRotation( HWND hwnd );
int WINAPI EXPORT SetRotation( HWND hwnd, int Rotation );
int WINAPI EXPORT GetSDKMouse( HWND hwnd );
int WINAPI EXPORT SetSDKMouse( HWND hwnd, int SDKMouse );
int WINAPI EXPORT GetSDKPaint( HWND hwnd );
int WINAPI EXPORT SetSDKPaint( HWND hwnd, int SDKPaint );
int WINAPI EXPORT GetStatusBar( HWND hwnd );
int WINAPI EXPORT SetStatusBar( HWND hwnd, int StatusBar );
int WINAPI EXPORT GetNumSymbols( HWND hwnd );
int WINAPI EXPORT GetSymbolData( HWND hwnd, int Index );
int WINAPI EXPORT SetSymbolData( HWND hwnd, LPINT SymbolData );
int WINAPI EXPORT GetSymbolSize( HWND hwnd );
int WINAPI EXPORT SetSymbolSize( HWND hwnd, int SymbolSize );
int WINAPI EXPORT GetThickLines( HWND hwnd );
int WINAPI EXPORT SetThickLines( HWND hwnd, int ThickLines );
int WINAPI EXPORT GetTickEvery( HWND hwnd );
int WINAPI EXPORT SetTickEvery( HWND hwnd, int TickEvery );
int WINAPI EXPORT GetTicks( HWND hwnd );
int WINAPI EXPORT SetTicks( HWND hwnd, int Ticks );
int WINAPI EXPORT GetToolbar( HWND hwnd );
int WINAPI EXPORT SetToolbar( HWND hwnd, int ToolBar );
int WINAPI EXPORT GetToolStat( HWND hwnd, int Index );
int WINAPI EXPORT SetToolStat( HWND hwnd, LPINT ToolStat );
int WINAPI EXPORT GetTrue3D( HWND hwnd );
int WINAPI EXPORT SetTrue3D( HWND hwnd, int True3D );
int WINAPI EXPORT GetXAxisPos( HWND hwnd );
int WINAPI EXPORT SetXAxisPos( HWND hwnd, int XAxisPos );
int WINAPI EXPORT GetXAxisStyle( HWND hwnd );
int WINAPI EXPORT SetXAxisStyle( HWND hwnd, int XAxisStyle );
int WINAPI EXPORT GetXAxisTicks( HWND hwnd );
int WINAPI EXPORT SetXAxisTicks( HWND hwnd, int XAxisTicks );
int WINAPI EXPORT SetXPosData( HWND hwnd, int ThisSet, LPDOUBLE XPosData );

int WINAPI EXPORT SetXAxisMax( HWND hwnd, double XAxisMax );
int WINAPI EXPORT SetXAxisMin( HWND hwnd, double XAxisMin );
int WINAPI EXPORT GetBackground( HWND hwnd, int Index );
int WINAPI EXPORT SetBackground( HWND hwnd, LPINT Background );
int WINAPI EXPORT GetForeground( HWND hwnd, int Index );
int WINAPI EXPORT SetForeground( HWND hwnd, LPINT Foreground );
int WINAPI EXPORT SetYAxisMax( HWND hwnd, LPDOUBLE YAxisMax );
int WINAPI EXPORT SetYAxisMin( HWND hwnd, LPDOUBLE YAxisMin );
int WINAPI EXPORT GetYAxisPos( HWND hwnd, int Index );
int WINAPI EXPORT SetYAxisPos( HWND hwnd, LPINT YAxisPos );
int WINAPI EXPORT GetYAxisStyle( HWND hwnd, int Index );
int WINAPI EXPORT SetYAxisStyle( HWND hwnd, LPINT YAxisStyle );
int WINAPI EXPORT GetYAxisTicks( HWND hwnd, int Index );
int WINAPI EXPORT SetYAxisTicks( HWND hwnd, LPINT YAxisTicks );

int WINAPI EXPORT GetBackgroundStyle( HWND hwnd, int Index );
int WINAPI EXPORT SetBackgroundStyle( HWND hwnd, LPINT BackgroundStyle );
int WINAPI EXPORT GetCageEdgeColor( HWND hwnd );
int WINAPI EXPORT SetCageEdgeColor( HWND hwnd, int CageEdgeColor );
int WINAPI EXPORT GetCageStyle( HWND hwnd );
int WINAPI EXPORT SetCageStyle( HWND hwnd, int CageStyle );
int WINAPI EXPORT GetCageWallColor( HWND hwnd );
int WINAPI EXPORT SetCageWallColor( HWND hwnd, int CageWallColor );
int WINAPI EXPORT GetDataLabels( HWND hwnd );
int WINAPI EXPORT SetDataLabels( HWND hwnd, int DataLabels );
int WINAPI EXPORT GetDataLabelText( HWND hwnd, int ThisSet, int Index, LPSTR lpsz );
int WINAPI EXPORT SetDataLabelText( HWND hwnd, int ThisSet, LPCSTR FAR *DataLabelText );
int WINAPI EXPORT SetEBarGraphDataMinus( HWND hwnd, int ThisSet, LPDOUBLE EBarGraphDataMinus );
int WINAPI EXPORT SetEBarGraphDataPlus( HWND hwnd, int ThisSet, LPDOUBLE EBarGraphDataPlus );
int WINAPI EXPORT GetEBarSource( HWND hwnd, int Index );
int WINAPI EXPORT SetEBarSource( HWND hwnd, LPINT EBarSource );
int WINAPI EXPORT GetEBarStyle( HWND hwnd, int Index );
int WINAPI EXPORT SetEBarStyle( HWND hwnd, LPINT EBarStyle );
int WINAPI EXPORT SetEBarValue( HWND hwnd, LPDOUBLE EBarValue );
int WINAPI EXPORT SetEBarXPosDataMinus( HWND hwnd, int ThisSet, LPDOUBLE EBarXPosDataMinus );
int WINAPI EXPORT SetEBarXPosDataPlus( HWND hwnd, int ThisSet, LPDOUBLE EBarXPosDataPlus );
int WINAPI EXPORT GetGridLineStyle( HWND hwnd );
int WINAPI EXPORT SetGridLineStyle( HWND hwnd, int GridLineStyle );
int WINAPI EXPORT GetHelpFile( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetHelpFile( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetLabelStyle( HWND hwnd );
int WINAPI EXPORT SetLabelStyle( HWND hwnd, int LabelStyle );
int WINAPI EXPORT GetLeftTitleStyle( HWND hwnd );
int WINAPI EXPORT SetLeftTitleStyle( HWND hwnd, int LeftTitleStyle );
int WINAPI EXPORT GetLegendPos( HWND hwnd );
int WINAPI EXPORT SetLegendPos( HWND hwnd, int LegendPos );
int WINAPI EXPORT GetLegendSize( HWND hwnd );
int WINAPI EXPORT SetLegendSize( HWND hwnd, int LegendSize );
int WINAPI EXPORT GetOverlayGraph( HWND hwnd );
int WINAPI EXPORT SetOverlayGraph( HWND hwnd, int OverlayGraph );
int WINAPI EXPORT GetOverlayGraphStyle( HWND hwnd );
int WINAPI EXPORT SetOverlayGraphStyle( HWND hwnd, int OverlayGraphStyle );
int WINAPI EXPORT GetOverlayGraphType( HWND hwnd );
int WINAPI EXPORT SetOverlayGraphType( HWND hwnd, int OverlayGraphType );
int WINAPI EXPORT GetOverlayNumSets( HWND hwnd );
int WINAPI EXPORT SetOverlayData( HWND hwnd, int ThisSet, LPDOUBLE OverlayData );
int WINAPI EXPORT GetOverlayColor( HWND hwnd );
int WINAPI EXPORT SetOverlayColor( HWND hwnd, int OverlayColor );
int WINAPI EXPORT GetOverlayPattern( HWND hwnd );
int WINAPI EXPORT SetOverlayPattern( HWND hwnd, int OverlayPattern );
int WINAPI EXPORT GetOverlaySymbol( HWND hwnd );
int WINAPI EXPORT SetOverlaySymbol( HWND hwnd, int OverlaySymbol );
int WINAPI EXPORT GetOverlayPatternedLines( HWND hwnd );
int WINAPI EXPORT SetOverlayPatternedLines( HWND hwnd, int OverlayPatternedLines );
int WINAPI EXPORT GetOverlayThickLines( HWND hwnd );
int WINAPI EXPORT SetOverlayThickLines( HWND hwnd, int OverlayThickLines );
int WINAPI EXPORT GetPropertyCaption( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetPropertyCaption( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetPropertyPages( HWND hwnd );
int WINAPI EXPORT SetPropertyPages( HWND hwnd, int PropertyPages );
int WINAPI EXPORT GetRightTitle( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetRightTitle( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetRightTitleStyle( HWND hwnd );
int WINAPI EXPORT SetRightTitleStyle( HWND hwnd, int RightTitleStyle );
int WINAPI EXPORT GetRunTime( HWND hwnd );
int WINAPI EXPORT SetRunTime( HWND hwnd, int RunTime );
int WINAPI EXPORT GetSurfaceColorMax( HWND hwnd );
int WINAPI EXPORT SetSurfaceColorMax( HWND hwnd, int SurfaceColorMax );
int WINAPI EXPORT GetSurfaceColorMin( HWND hwnd );
int WINAPI EXPORT SetSurfaceColorMin( HWND hwnd, int SurfaceColorMin );
int WINAPI EXPORT GetSurfaceWallColor( HWND hwnd );
int WINAPI EXPORT SetSurfaceWallColor( HWND hwnd, int SurfaceWallColor );
int WINAPI EXPORT GetTrue3DDepth( HWND hwnd );
int WINAPI EXPORT SetTrue3DDepth( HWND hwnd, int True3DDepth );
int WINAPI EXPORT GetTrue3DXGap( HWND hwnd );
int WINAPI EXPORT SetTrue3DXGap( HWND hwnd, int True3DXGap );
int WINAPI EXPORT GetTrue3DZGap( HWND hwnd );
int WINAPI EXPORT SetTrue3DZGap( HWND hwnd, int True3DZGap );
int WINAPI EXPORT GetXAxisMinorTicks( HWND hwnd );
int WINAPI EXPORT SetXAxisMinorTicks( HWND hwnd, int XAxisMinorTicks );
int WINAPI EXPORT GetYAxisMinorTicks( HWND hwnd, int Index );
int WINAPI EXPORT SetYAxisMinorTicks( HWND hwnd, LPINT YAxisMinorTicks );
int WINAPI EXPORT GetYLabelText( HWND hwnd, int Axis, int Index, LPSTR lpsz );
int WINAPI EXPORT SetYLabelText( HWND hwnd, int Index, int NumLabels, LPCSTR FAR *YLabelText );
int WINAPI EXPORT GetZLabelText( HWND hwnd, int Index, LPSTR lpsz );
int WINAPI EXPORT SetZLabelText( HWND hwnd, int NumLabels, LPCSTR FAR *ZLabelText );
int WINAPI EXPORT SetZPosData( HWND hwnd, int ThisSet, LPDOUBLE ZPosData );
int WINAPI EXPORT SetZAxisMax( HWND hwnd, double ZAxisMax );
int WINAPI EXPORT SetZAxisMin( HWND hwnd, double ZAxisMin );
int WINAPI EXPORT GetZAxisStyle( HWND hwnd );
int WINAPI EXPORT SetZAxisStyle( HWND hwnd, int ZAxisStyle );
int WINAPI EXPORT GetZAxisTicks( HWND hwnd );
int WINAPI EXPORT SetZAxisTicks( HWND hwnd, int ZAxisTicks );

int WINAPI EXPORT GetGraphName( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetGraphName( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetGraphFile( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetGraphFile( HWND hwnd, LPCSTR lpsz );

int WINAPI EXPORT GetRangeMin( HWND hwnd );
int WINAPI EXPORT SetRangeMin( HWND hwnd, int RangeMin );
int WINAPI EXPORT GetRangeMax( HWND hwnd );
int WINAPI EXPORT SetRangeMax( HWND hwnd, int RangeMax );
int WINAPI EXPORT GetBar2DGap( HWND hwnd );
int WINAPI EXPORT SetBar2DGap( HWND hwnd, int Bar2DGap );
int WINAPI EXPORT GetClipGraph( HWND hwnd );
int WINAPI EXPORT SetClipGraph( HWND hwnd, int ClipGraph );
int WINAPI EXPORT GetNewLineChar( HWND hwnd );
int WINAPI EXPORT SetNewLineChar( HWND hwnd, int NewLineChar );
int WINAPI EXPORT GetSmartLabels( HWND hwnd );
int WINAPI EXPORT SetSmartLabels( HWND hwnd, int SmartLabels );
int WINAPI EXPORT GetLabelXFormat( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetLabelXFormat( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetLabelYFormat( HWND hwnd, int Index, LPSTR lpsz  );
int WINAPI EXPORT SetLabelYFormat( HWND hwnd, LPCSTR FAR *LabelYFormat );
int WINAPI EXPORT GetLabelZFormat( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetLabelZFormat( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetDataLabelFormat( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetDataLabelFormat( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetLimitLines( HWND hwnd );
int WINAPI EXPORT SetLimitLines( HWND hwnd, int nMode );
int WINAPI EXPORT GetLimitLinePattern( HWND hwnd );
int WINAPI EXPORT SetLimitLinePattern( HWND hwnd, int nPatt );
int WINAPI EXPORT GetLimitFillPattern( HWND hwnd );
int WINAPI EXPORT SetLimitFillPattern( HWND hwnd, int nPatt );
int WINAPI EXPORT SetLimitHighValue( HWND hwnd, double fVal );
int WINAPI EXPORT SetLimitLowValue( HWND hwnd, double fVal );
int WINAPI EXPORT GetLimitHighLabel( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetLimitHighLabel( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetLimitLowLabel( HWND hwnd, LPSTR lpsz );
int WINAPI EXPORT SetLimitLowLabel( HWND hwnd, LPCSTR lpsz );
int WINAPI EXPORT GetMissingData( HWND hwnd );
int WINAPI EXPORT SetMissingData( HWND hwnd, int nMode );

int WINAPI EXPORT SetBackgroundAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetColorDataAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetExtraDataAt( HWND hwnd, int ThisSet, int Index, int Value );
int WINAPI EXPORT SetFontFamilyAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetFontNameAt( HWND hwnd, int Index, LPCSTR lpsz  );
int WINAPI EXPORT SetFontSizeAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetFontStyleAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetForegroundAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetGraphDataAt( HWND hwnd, int ThisSet, int Index, double Value );
int WINAPI EXPORT SetLabelTextAt( HWND hwnd, int Index, LPCSTR lpsz  );
int WINAPI EXPORT SetLegendTextAt( HWND hwnd, int Index, LPCSTR lpsz  );
int WINAPI EXPORT SetPatternDataAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetSymbolDataAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetToolStatAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetXPosDataAt( HWND hwnd, int ThisSet, int Index, double Value );
int WINAPI EXPORT SetYAxisMaxAt( HWND hwnd, int Index, double Value );
int WINAPI EXPORT SetYAxisMinAt( HWND hwnd, int Index, double Value );
int WINAPI EXPORT SetYAxisPosAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetYAxisStyleAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetYAxisTicksAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetBackgroundStyleAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetDataLabelTextAt( HWND hwnd, int ThisSet, int Index, LPCSTR lpsz );
int WINAPI EXPORT SetEBarGraphDataMinusAt( HWND hwnd, int ThisSet, int Index, double Value );
int WINAPI EXPORT SetEBarGraphDataPlusAt( HWND hwnd, int ThisSet, int Index, double Value );
int WINAPI EXPORT SetEBarSourceAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetEBarStyleAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetEBarValueAt( HWND hwnd, int Index, double Value );
int WINAPI EXPORT SetEBarXPosDataMinusAt( HWND hwnd, int ThisSet, int Index, double Value );
int WINAPI EXPORT SetEBarXPosDataPlusAt( HWND hwnd, int ThisSet, int Index, double Value );
int WINAPI EXPORT SetOverlayDataAt( HWND hwnd, int ThisSet, int Index, double Value );
int WINAPI EXPORT SetYAxisMinorTicksAt( HWND hwnd, int Index, int Value );
int WINAPI EXPORT SetYLabelTextAt( HWND hwnd, int Axis, int Index, LPCSTR lpsz );
int WINAPI EXPORT SetZLabelTextAt( HWND hwnd, int Index, LPCSTR lpsz  );
int WINAPI EXPORT SetZPosDataAt( HWND hwnd, int ThisSet, int Index, double Value );
int WINAPI EXPORT SetLabelYFormatAt( HWND hwnd, int Index, LPCSTR lpsz  );

int WINAPI EXPORT GetPrintInfo( HWND hwnd, int Index );
int WINAPI EXPORT SetPrintInfo( HWND hwnd, LPINT PrintInfo );
int WINAPI EXPORT SetPrintInfoAt( HWND hwnd, int Index, int Value );

int WINAPI EXPORT GetGraphData_( HWND hwnd, double FAR* lpRetVal, int ThisSet, int Index );
int WINAPI EXPORT GetSDKInfo_( HWND hwnd, double FAR* lpRetVal, int Index );
int WINAPI EXPORT GetXPosData_( HWND hwnd, double FAR* lpRetVal, int ThisSet, int Index );
int WINAPI EXPORT GetXAxisMax_( HWND hwnd, double FAR* lpRetVal );
int WINAPI EXPORT GetXAxisMin_( HWND hwnd, double FAR* lpRetVal );
int WINAPI EXPORT GetYAxisMax_( HWND hwnd, double FAR* lpRetVal, int Index );
int WINAPI EXPORT GetYAxisMin_( HWND hwnd, double FAR* lpRetVal, int Index );
int WINAPI EXPORT GetEBarGraphDataMinus_( HWND hwnd, double FAR* lpRetVal, int ThisSet, int Index );
int WINAPI EXPORT GetEBarGraphDataPlus_( HWND hwnd, double FAR* lpRetVal, int ThisSet, int Index );
int WINAPI EXPORT GetEBarValue_( HWND hwnd, double FAR* lpRetVal, int Index );
int WINAPI EXPORT GetEBarXPosDataMinus_( HWND hwnd, double FAR* lpRetVal, int ThisSet, int Index );
int WINAPI EXPORT GetEBarXPosDataPlus_( HWND hwnd, double FAR* lpRetVal, int ThisSet, int Index );
int WINAPI EXPORT GetOverlayData_( HWND hwnd, double FAR* lpRetVal, int ThisSet, int Index );
int WINAPI EXPORT GetZPosData_( HWND hwnd, double FAR* lpRetVal, int ThisSet, int Index );
int WINAPI EXPORT GetZAxisMax_( HWND hwnd, double FAR* lpRetVal );
int WINAPI EXPORT GetZAxisMin_( HWND hwnd, double FAR* lpRetVal );
int WINAPI EXPORT GetLimitHighValue_( HWND hwnd, double FAR* lpRetVal );
int WINAPI EXPORT GetLimitLowValue_( HWND hwnd, double FAR* lpRetVal );

#if defined( _MSC_VER )
double WINAPI EXPORT GetGraphData( HWND hwnd, int ThisSet, int Index );
double WINAPI EXPORT GetSDKInfo( HWND hwnd, int Index );
double WINAPI EXPORT GetXPosData( HWND hwnd, int ThisSet, int Index );
double WINAPI EXPORT GetXAxisMax( HWND hwnd );
double WINAPI EXPORT GetXAxisMin( HWND hwnd );
double WINAPI EXPORT GetYAxisMax( HWND hwnd, int Index );
double WINAPI EXPORT GetYAxisMin( HWND hwnd, int Index );
double WINAPI EXPORT GetEBarGraphDataMinus( HWND hwnd, int ThisSet, int Index );
double WINAPI EXPORT GetEBarGraphDataPlus( HWND hwnd, int ThisSet, int Index );
double WINAPI EXPORT GetEBarValue( HWND hwnd, int Index );
double WINAPI EXPORT GetEBarXPosDataMinus( HWND hwnd, int ThisSet, int Index );
double WINAPI EXPORT GetEBarXPosDataPlus( HWND hwnd, int ThisSet, int Index );
double WINAPI EXPORT GetOverlayData( HWND hwnd, int ThisSet, int Index );
double WINAPI EXPORT GetZPosData( HWND hwnd, int ThisSet, int Index );
double WINAPI EXPORT GetZAxisMax( HWND hwnd );
double WINAPI EXPORT GetZAxisMin( HWND hwnd );
double WINAPI EXPORT GetLimitHighValue( HWND hwnd );
double WINAPI EXPORT GetLimitLowValue( HWND hwnd );
#elif defined( __cplusplus )
inline double WINAPI EXPORT GetGraphData( HWND hwnd, int ThisSet, int Index )
{
    double RetVal;
    return GetGraphData_( hwnd, &RetVal, ThisSet, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetSDKInfo( HWND hwnd, int Index )
{
    double RetVal;
    return GetSDKInfo_( hwnd, &RetVal, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetXPosData( HWND hwnd, int ThisSet, int Index )
{
    double RetVal;
    return GetXPosData_( hwnd, &RetVal, ThisSet, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetXAxisMax( HWND hwnd )
{
    double RetVal;
    return GetXAxisMax_( hwnd, &RetVal ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetXAxisMin( HWND hwnd )
{
    double RetVal;
    return GetXAxisMin_( hwnd, &RetVal ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetYAxisMax( HWND hwnd, int Index )
{
    double RetVal;
    return GetYAxisMax_( hwnd, &RetVal, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetYAxisMin( HWND hwnd, int Index )
{
    double RetVal;
    return GetYAxisMin_( hwnd, &RetVal, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetEBarGraphDataMinus( HWND hwnd, int ThisSet, int Index )
{
    double RetVal;
    return GetEBarGraphDataMinus_( hwnd, &RetVal, ThisSet, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetEBarGraphDataPlus( HWND hwnd, int ThisSet, int Index )
{
    double RetVal;
    return GetEBarGraphDataPlus_( hwnd, &RetVal, ThisSet, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetEBarValue( HWND hwnd, int Index )
{
    double RetVal;
    return GetEBarValue_( hwnd, &RetVal, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetEBarXPosDataMinus( HWND hwnd, int ThisSet, int Index )
{
    double RetVal;
    return GetEBarXPosDataMinus_( hwnd, &RetVal, ThisSet, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetEBarXPosDataPlus( HWND hwnd, int ThisSet, int Index )
{
    double RetVal;
    return GetEBarXPosDataPlus_( hwnd, &RetVal, ThisSet, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetOverlayData( HWND hwnd, int ThisSet, int Index )
{
    double RetVal;
    return GetOverlayData_( hwnd, &RetVal, ThisSet, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetZPosData( HWND hwnd, int ThisSet, int Index )
{
    double RetVal;
    return GetZPosData_( hwnd, &RetVal, ThisSet, Index ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetZAxisMax( HWND hwnd )
{
    double RetVal;
    return GetZAxisMax_( hwnd, &RetVal ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetZAxisMin( HWND hwnd )
{
    double RetVal;
    return GetZAxisMin_( hwnd, &RetVal ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetLimitHighValue( HWND hwnd )
{
    double RetVal;
    return GetLimitHighValue_( hwnd, &RetVal ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetLimitLowValue( HWND hwnd )
{
    double RetVal;
    return GetLimitLowValue_( hwnd, &RetVal ) ? RetVal : FALSE;
}
#endif  /* _MSC_VER */

int WINAPI EXPORT EnableEnhancedEvents( HWND hwnd, HWND hNotifyWnd, int MessageOffset);

int WINAPI EXPORT GetHitX_( HWND hwnd, double FAR* lpRetVal );
int WINAPI EXPORT GetHitY_( HWND hwnd, double FAR* lpRetVal );
int WINAPI EXPORT GetHitDataX_( HWND hwnd, double FAR* lpRetVal );
int WINAPI EXPORT GetHitDataY_( HWND hwnd, double FAR* lpRetVal );
#if defined( _MSC_VER )
double WINAPI EXPORT GetHitX( HWND hwnd );
double WINAPI EXPORT GetHitY( HWND hwnd );
double WINAPI EXPORT GetHitDataX( HWND hwnd );
double WINAPI EXPORT GetHitDataY( HWND hwnd );
#elif defined( __cplusplus )
inline double WINAPI EXPORT GetHitX( HWND hwnd )
{
    double RetVal;
    return GetHitX_( hwnd, &RetVal ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetHitY( HWND hwnd )
{
    double RetVal;
    return GetHitY_( hwnd, &RetVal ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetHitDataX( HWND hwnd )
{
    double RetVal;
    return GetHitDataX_( hwnd, &RetVal ) ? RetVal : FALSE;
}
inline double WINAPI EXPORT GetHitDataY( HWND hwnd )
{
    double RetVal;
    return GetHitDataY_( hwnd, &RetVal ) ? RetVal : FALSE;
}
#endif  /* _MSC_VER */
int WINAPI EXPORT GetHitStatus( HWND hwnd );
int WINAPI EXPORT GetHitRegion( HWND hwnd );
int WINAPI EXPORT GetHitPoint( HWND hwnd );
int WINAPI EXPORT GetHitSet( HWND hwnd );
int WINAPI EXPORT GetHotButton( HWND hwnd );

#ifdef  __cplusplus
}
#endif

#endif /* __GSPROP_H__ */
