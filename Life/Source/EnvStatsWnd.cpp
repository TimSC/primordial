// EnvStatsWnd.cpp : implementation file
//

#include "stdafx.h"
#include "Primordial Life.h"
#include "EnvStatsWnd.h"
#include "Environ.h"
#include "Biots.h"
//#include "GsProp.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CEnvStatsWnd
//
//
IMPLEMENT_DYNCREATE(CEnvStatsWnd, CMiniFrameWnd)

CEnvStatsWnd::CEnvStatsWnd(CEnvStatsWnd** pExternalRef)
{
	m_pEnv         = NULL;
	m_pExternalRef = pExternalRef;
	m_toolbarHeight = 0;
	m_choice = AfxUserRegistry().GetValue("EnvStats.choice", ID_POPULATION);
	m_bLogging = false;
}

CEnvStatsWnd::~CEnvStatsWnd()
{
}


void CEnvStatsWnd::OnSize(UINT nType, int cx, int cy) 
{
	CMiniFrameWnd::OnSize(nType, cx, cy);

	if (nType == SIZE_RESTORED)
	{
		cy -= m_toolbarHeight;

		if (m_graph.GetSafeHwnd() != NULL)
			m_graph.MoveWindow(0, 0, cx, cy, FALSE);

		if (m_breedView.GetSafeHwnd() != NULL)
			m_breedView.MoveWindow(0, 0, cx, cy, FALSE);

		Invalidate();

		PostMessage(WM_COMMAND, m_choice, 0);
	}
}


/*static*/
BOOL CEnvStatsWnd::CreateWnd(Environment* pEnv, CWnd* pParentWnd, CEnvStatsWnd** pExternalRef)
{
	CEnvStatsWnd* pWnd = new CEnvStatsWnd(pExternalRef);

	if (!pWnd)
		return FALSE;

	pWnd->m_pEnv       = pEnv;
	pWnd->m_pParentWnd = pParentWnd;

	CWinRect rect;

	rect.FromRegistry("EnvStats", 100, 100, 500, 400);

	BOOL bSuccess = pWnd->Create(NULL, "", WS_SYSMENU | WS_VISIBLE | 
		MFS_SYNCACTIVE | MFS_THICKFRAME, rect, pParentWnd);
	
	if (!bSuccess)
	{
		delete pWnd;
		return NULL;
	}

	CRect cRect;
	pWnd->GetClientRect(&cRect);

	// Subtract off the toolbar area
	CRect cToolbar;
	pWnd->m_wndToolBar.GetWindowRect(cToolbar);
	pWnd->m_toolbarHeight = (cToolbar.bottom - cToolbar.top);
		
	cRect.bottom -= pWnd->m_toolbarHeight;

	bSuccess = pWnd->m_graph.Create(NULL, NULL, WS_VISIBLE | 
		WS_CHILD, cRect, pWnd, ID_GRAPH, NULL);
	if (!bSuccess)
	{
		pWnd->DestroyWindow();
		AfxMessageBox("Unable to display graph object.\nYou may need to re-install.");
		return NULL;
	}

	((CView*)&pWnd->m_breedView)->Create(NULL, NULL, 
		AFX_WS_DEFAULT_VIEW, cRect, pWnd, 101, NULL);
	pWnd->m_breedView.ShowWindow(SW_HIDE);

	switch (pWnd->m_choice)
	{
		default:
		case ID_POPULATION:
			pWnd->OnPopulation();
			break;

		case ID_ENERGY_SPREAD:
			pWnd->OnEnergySpread();
			break;

		case ID_LINE_TYPES:
			pWnd->OnLineTypes();
			break;

		case ID_AGE_SPREAD:
			pWnd->OnAgeSpread();
			break;
		case ID_EVOLUTION:
			pWnd->OnEvolution();
			break;
		case ID_CPUTIME:
			pWnd->OnCPUTime();
		case ID_SPECIES:
			pWnd->OnSpecies();
	}

	if (pExternalRef)
		*pExternalRef = pWnd;

	return bSuccess;
}


// Call OnClose to close
BEGIN_MESSAGE_MAP(CEnvStatsWnd, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CEnvStatsWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_AGE_SPREAD, OnAgeSpread)
	ON_UPDATE_COMMAND_UI(ID_AGE_SPREAD, OnUpdateAgeSpread)
	ON_COMMAND(ID_LINE_TYPES, OnLineTypes)
	ON_UPDATE_COMMAND_UI(ID_LINE_TYPES, OnUpdateLineTypes)
	ON_COMMAND(ID_POPULATION, OnPopulation)
	ON_UPDATE_COMMAND_UI(ID_POPULATION, OnUpdatePopulation)
	ON_COMMAND(ID_ENERGY_SPREAD, OnEnergySpread)
	ON_UPDATE_COMMAND_UI(ID_ENERGY_SPREAD, OnUpdateEnergySpread)
	ON_COMMAND(IDC_STATS_LOG, OnStatsLog)
	ON_UPDATE_COMMAND_UI(IDC_STATS_LOG, OnUpdateStatsLog)
	ON_COMMAND(ID_EVOLUTION, OnEvolution)
	ON_UPDATE_COMMAND_UI(ID_EVOLUTION, OnUpdateEvolution)
	ON_COMMAND(ID_CPUTIME, OnCPUTime)
	ON_UPDATE_COMMAND_UI(ID_CPUTIME, OnUpdateCPUTime)
	ON_COMMAND(ID_SPECIES, OnSpecies)
	ON_UPDATE_COMMAND_UI(ID_SPECIES, OnUpdateSpecies)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CEnvStatsWnd message handlers
//
void CEnvStatsWnd::OnPaint() 
{
	CPaintDC dc(this);

/*	CRect rect;
	GetClientRect(rect);
	rect.bottom -= m_toolbarHeight;

	dc.FillSolidRect(rect, RGB(0, 0, 0));

	dc.SetTextColor(RGB(255,255,255));
	dc.SetBkColor(RGB(0,0,0));
	dc.TextOut(10, 10, sText);*/
}


void CEnvStatsWnd::PaintNow()
{
	switch(m_choice)
	{  
		default:
		case ID_POPULATION:
			SetPopulation();
			break;

		case ID_ENERGY_SPREAD:
			SetEnergySpread();
			break;

		case ID_AGE_SPREAD:
			SetAgeSpread();
			break;

		case ID_LINE_TYPES:
			SetLineTypes();
			break;
		case ID_EVOLUTION:
			SetEvolution();
			break;
		case ID_CPUTIME:
			SetCPUTime();
			break;
		case ID_SPECIES:
			SetSpecies();
			break;
	}
}


BOOL CEnvStatsWnd::DestroyWindow() 
{
	CWinRect rect;
	GetWindowRect(rect);
	rect.ToRegistry("EnvStats");

	AfxUserRegistry().SetValue("EnvStats.choice", m_choice);

	if (m_graph.GetSafeHwnd() != NULL)
		m_graph.DestroyWindow();

	if (m_breedView.GetSafeHwnd() != NULL)
		m_breedView.DestroyWindow();

	return CMiniFrameWnd::DestroyWindow();
}


// Do NOT REMOVE!! - normally this deletes the object!!
void CEnvStatsWnd::PostNcDestroy() 
{
	if (m_pExternalRef != NULL)
		*m_pExternalRef = NULL;

	CMiniFrameWnd::PostNcDestroy();
}


BOOL CEnvStatsWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_TOOLWINDOW;	
	return CMiniFrameWnd::PreCreateWindow(cs);
}


void CEnvStatsWnd::SetTitle(bool bChange)
{
	LPCTSTR pszTitle = NULL;

	switch (m_choice)
	{
		case ID_POPULATION:
			pszTitle = "Population over the Last 5 Days";
			break;

		case ID_ENERGY_SPREAD:
			pszTitle = "Biots per Energy Percentage";
			break;

		case ID_AGE_SPREAD:
			pszTitle = "Biots per Age Group";
			break;

		case ID_LINE_TYPES:
			pszTitle = "Line Type Percentages over the Last 5 Days";
			break;
		case ID_EVOLUTION:
			pszTitle = "Measures of Adaptation and Diversity";
			break;
		case ID_CPUTIME:
			pszTitle = "Milliseconds of CPU Time per Biot";
			break;
		case ID_SPECIES:
			pszTitle = "Population By Species";
			break;
	}

	CString sTitle;
	GetWindowText(sTitle);
	if (sTitle != pszTitle)
		SetWindowText(pszTitle);
}


//////////////////////////////////////////////////////////
// POPULATION METHODS
//
//


/*void CEnvStatsWnd::OnBreeding() 
{
	m_choice = ID_BREEDING;
	m_graph.ShowWindow(SW_HIDE);

	SetTitle(TRUE);
	m_breedView.UpdateData(FALSE);
	SetBreeding();
	m_breedView.ShowWindow(SW_SHOWNORMAL);
}*/


void CEnvStatsWnd::SetEnergySpread(bool bDraw) 
{
	CEnvStatsList& list = m_pEnv->m_statsList;

 	m_graph.SetDataReset(9);
	m_graph.SetGraphType(4);
	m_graph.SetGraphStyle(2);
	m_graph.SetBackground(0);
	m_graph.SetPatternedLines(1);
	m_graph.SetPatternData(0);
	m_graph.SetRandomData(0);

	// Only ticks on both axis
	m_graph.SetTicks(3);

	m_graph.SetNumPoints((short) CEnvStats::ENERGY_LEVELS);
	m_graph.SetNumSets(1);

	m_graph.SetAutoInc(1);
	m_graph.SetColorData(10);	// Green

	if (list.GetCount() == 0)
	{
		m_graph.SetDrawMode(3);
		return;
	}

	CEnvStats& stats = list.GetTail();
	m_graph.SetAutoInc(1);

	for (int i = 0; i < CEnvStats::ENERGY_LEVELS; i++)
		m_graph.SetGraphData((float) stats.m_energy[i]);

	m_graph.SetAutoInc(1);
	CString sString;
	for (i = 0; i < CEnvStats::ENERGY_LEVELS; i++)
	{
		sString.Format("%d%%", (i+1) * (100 / CEnvStats::ENERGY_LEVELS));
		m_graph.SetLabelText( sString );
	}

	m_graph.SetDrawMode(3);
}


int CEnvStatsWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMiniFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM
		| CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_ENVSTATS))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	m_menu.LoadMenu (IDR_STATS_MENU);
	SetMenu(NULL);
	SetMenu(&m_menu);
	
	return 0;
}


void CEnvStatsWnd::OnAgeSpread() 
{
	m_choice = ID_AGE_SPREAD;
	SetTitle(true);

	m_breedView.ShowWindow(SW_HIDE);
	m_graph.ShowWindow(SW_SHOWNORMAL);

	SetAgeSpread();
}


void CEnvStatsWnd::SetAgeSpread(bool bDraw) 
{
	CEnvStatsList& list = m_pEnv->m_statsList;

 	m_graph.SetDataReset(9);
	m_graph.SetGraphType(4);
	m_graph.SetGraphStyle(2);
	m_graph.SetBackground(0);
	m_graph.SetPatternedLines(1);
	m_graph.SetPatternData(0);
	m_graph.SetRandomData(0);

	// Only ticks on both axis
	m_graph.SetTicks(3);
//	m_graph.SetTickEvery(20);

	m_graph.SetNumPoints((short) CEnvStats::INTERVALS);
	m_graph.SetNumSets(1);

	m_graph.SetAutoInc(1);
	m_graph.SetColorData(3);	// Cyan

	if (list.GetCount() == 0)
	{
		m_graph.SetDrawMode(3);
		return;
	}

	CEnvStats& stats = list.GetTail();
	m_graph.SetAutoInc(1);

	for (int i = 0; i < (int) stats.m_ageIntervals; i++)
		m_graph.SetGraphData((float) stats.m_ages[i]);

	m_graph.SetAutoInc(1);
	for (i = 0; i < (int)stats.m_ageIntervals; i++)
		m_graph.SetLabelText( stats.ToDays( (stats.m_ageRange * (i + 1)) / stats.m_ageIntervals ) );

	m_graph.SetDrawMode(3);
}


void CEnvStatsWnd::OnUpdateAgeSpread(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_choice == ID_AGE_SPREAD);
	pCmdUI->Enable(TRUE);
}

void CEnvStatsWnd::OnLineTypes() 
{
	m_choice = ID_LINE_TYPES;
	SetTitle(true);

	m_breedView.ShowWindow(SW_HIDE);
	m_graph.ShowWindow(SW_SHOWNORMAL);

	SetLineTypes();
}

void CEnvStatsWnd::OnUpdateLineTypes(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_choice == ID_LINE_TYPES);
	pCmdUI->Enable(TRUE);
}

void CEnvStatsWnd::SetLineTypes(bool bDraw)
{
	CEnvStatsList& list = m_pEnv->m_statsList;

 	m_graph.SetDataReset(9);
	m_graph.SetGraphType(6);
	m_graph.SetGraphStyle(4);
	m_graph.SetBackground(0);
	m_graph.SetPatternedLines(1);
	m_graph.SetPatternData(6);
	m_graph.SetRandomData(0);

	// Only ticks on both axis
	m_graph.SetTicks(1);
	m_graph.SetTickEvery(20);

	m_graph.SetNumPoints(CEnvStats::SAMPLES);
	m_graph.SetNumSets(5);

	m_graph.SetAutoInc(1);
	m_graph.SetColorData(10);	// Green
	m_graph.SetColorData(9);	// Light Blue
	m_graph.SetColorData(12);	// Light Red
	m_graph.SetColorData(11);	// Light Cyan
	m_graph.SetColorData(15); 	// White

	m_graph.SetAutoInc(1);
	
/*	if (m_pEnv->m_stats.m_peakPopulation > 50)
		m_graph.SetYAxisMax((float) (m_pEnv->m_stats.m_peakPopulation));
	else
		m_graph.SetYAxisMax((float) 50);*/

	int i;
	POSITION pos;
	float fltLastPerGreen, fltLastPerRed, fltLastPerBlue, fltLastPerEyes, fltLastPerWhite;
	m_graph.SetAutoInc(1);

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		m_graph.SetGraphData((float) stats.m_perGreen);
		i++;
		fltLastPerGreen = stats.m_perGreen;
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		m_graph.SetGraphData((float) stats.m_perBlue);
		i++;
		fltLastPerBlue = stats.m_perBlue;
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		m_graph.SetGraphData((float) stats.m_perRed);
		i++;
		fltLastPerRed = stats.m_perRed;
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		m_graph.SetGraphData((float) stats.m_perLtBlue);
		i++;
		fltLastPerEyes = stats.m_perLtBlue;		
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		m_graph.SetGraphData((float) stats.m_perWhite);
		i++;
		fltLastPerWhite = stats.m_perWhite;			
	}

	CString strLegend;
	strLegend.Format(_T("Chloroplasts  %3.2f%%"), fltLastPerGreen);
	m_graph.SetLegendText(strLegend);
	strLegend.Format(_T("Shields  %3.2f%%"), fltLastPerBlue);
	m_graph.SetLegendText(strLegend);
	strLegend.Format(_T("Teeth  %3.2f%%"), fltLastPerRed);
	m_graph.SetLegendText(strLegend);
	strLegend.Format(_T("Eyes  %3.2f%%"), fltLastPerEyes);
	m_graph.SetLegendText(strLegend);
	strLegend.Format(_T("Injectors  %3.2f%%"), fltLastPerWhite);
	m_graph.SetLegendText(strLegend);

	m_graph.SetDrawMode(3);
}


void CEnvStatsWnd::OnPopulation() 
{
	m_choice = ID_POPULATION;
	SetTitle(true);

	m_breedView.ShowWindow(SW_HIDE);
	m_graph.ShowWindow(SW_SHOWNORMAL);

	SetPopulation();
}


void CEnvStatsWnd::SetPopulation(bool bDraw)
{
	CEnvStatsList& list = m_pEnv->m_statsList;

 	m_graph.SetDataReset(9);
	m_graph.SetGraphType(6);
	m_graph.SetGraphStyle(4);
	m_graph.SetBackground(0);
	m_graph.SetPatternedLines(1);
	m_graph.SetPatternData(6);
	m_graph.SetRandomData(0);

	// Only ticks on both axis
	m_graph.SetTicks(1);
	m_graph.SetTickEvery(20);

	m_graph.SetNumPoints(CEnvStats::SAMPLES);
	m_graph.SetNumSets(5);

	m_graph.SetAutoInc(1);
	m_graph.SetColorData(15); 	// White
	m_graph.SetColorData(12);	// Light Red
	m_graph.SetColorData(10);	// Green
	m_graph.SetColorData(9);	// Light Blue
	m_graph.SetColorData(11);	// Light Cyan

	m_graph.SetAutoInc(1);
	m_graph.SetLegendText("Population");
	m_graph.SetLegendText("Deaths");
	m_graph.SetLegendText("Births");
	m_graph.SetLegendText("Departed");
	m_graph.SetLegendText("Arrived");

	if (m_pEnv->m_stats.m_peakPopulation > 50)
		m_graph.SetYAxisMax((float) (m_pEnv->m_stats.m_peakPopulation));
	else
		m_graph.SetYAxisMax((float) 50);

	int i;
	POSITION pos;

	m_graph.SetAutoInc(1);

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData((float)list.GetNext(pos).m_population);
		i++;
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData((float)list.GetNext(pos).m_deaths);
		i++;
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData((float)list.GetNext(pos).m_births);
		i++;
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData((float)list.GetNext(pos).m_departures);
		i++;
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData((float)list.GetNext(pos).m_arrivals);
		i++;
	}

	m_graph.SetDrawMode(3);
}

void CEnvStatsWnd::OnUpdatePopulation(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_choice == ID_POPULATION);
	pCmdUI->Enable(TRUE);
	m_graph.SetDataReset(9);
}


void CEnvStatsWnd::OnEvolution() 
{
	m_choice = ID_EVOLUTION;
	SetTitle(true);

	m_breedView.ShowWindow(SW_HIDE);
	m_graph.ShowWindow(SW_SHOWNORMAL);

	SetEvolution();
}

void CEnvStatsWnd::SetEvolution(bool bDraw)
{
	CEnvStatsList& list = m_pEnv->m_statsList;

 	m_graph.SetDataReset(9);
	m_graph.SetGraphType(6);
	m_graph.SetGraphStyle(4);
	m_graph.SetBackground(0);
	m_graph.SetPatternedLines(1);
	m_graph.SetPatternData(6);
	m_graph.SetRandomData(0);

	// Only ticks on both axis
	m_graph.SetTicks(1);
	m_graph.SetTickEvery(20);

	m_graph.SetNumPoints(CEnvStats::SAMPLES);
	m_graph.SetNumSets(5);

	m_graph.SetAutoInc(1);
	m_graph.SetColorData(15); 	// White
	m_graph.SetColorData(12);	// Light Red
	m_graph.SetColorData(10);	// Green
	m_graph.SetColorData(9);	// Light Blue
	m_graph.SetColorData(11);	// Light Cyan

	m_graph.SetAutoInc(1);
	

	
	m_graph.SetYAxisMax((float)50);
	
	int i;
	POSITION pos;

	m_graph.SetAutoInc(1);

	long nLastDiversity;
	float fltLastMeanCActive, fltLastStdCActive, fltLastNewActive, fltLastStdNActive;
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	pos = list.GetHeadPosition();
	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		m_graph.SetGraphData((float)stats.m_nDiversity);
		i++;
		nLastDiversity = stats.m_nDiversity;
	}

	
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);
	pos = list.GetHeadPosition();
		
	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		
		CEnvStats stats = list.GetNext(pos);
		if (stats.m_dblMeanCumActivity < 0 )
			m_graph.SetGraphData(0.0);	
		else
			m_graph.SetGraphData((float)log(stats.m_dblMeanCumActivity));
		i++;
		fltLastMeanCActive = stats.m_dblMeanCumActivity;
	}


	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);
	
	pos = list.GetHeadPosition();
	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		if (stats.m_dblStdActivity < 0 )
			m_graph.SetGraphData(0.0);	
		else
			m_graph.SetGraphData((float)log(stats.m_dblStdActivity));
		
		i++;
		fltLastStdCActive = stats.m_dblStdActivity;
	}

	
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	pos = list.GetHeadPosition();
	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		if (stats.m_dblMeanNewActivity < 0 )
			m_graph.SetGraphData(0.0);	
		else
			m_graph.SetGraphData((float)stats.m_dblMeanNewActivity/10);

		
		i++;
		fltLastNewActive = stats.m_dblMeanNewActivity;
	}

	
	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		if (stats.m_dblStdNewActivity < 0 )
			m_graph.SetGraphData(0.0);	
		else
			m_graph.SetGraphData((float)stats.m_dblStdNewActivity/3);
		
		i++;
		fltLastStdNActive = stats.m_dblStdNewActivity;
	}

	//Set Legend
	CString strLegend;
	strLegend.Format(_T("Diversity   %ld"), nLastDiversity);
	m_graph.SetLegendText(strLegend);
	strLegend.Format(_T("Log Mean Cum Activity   [%f]"), fltLastMeanCActive);
	m_graph.SetLegendText(strLegend);
	strLegend.Format(_T("Log Std Cum. Activity   [%f]"), fltLastStdCActive);
	m_graph.SetLegendText(strLegend);
	strLegend.Format(_T("Mean New Activity/10  %f"), fltLastNewActive);
	m_graph.SetLegendText(strLegend);
	strLegend.Format(_T("Std. New Activity/3  %f"), fltLastStdNActive);
	m_graph.SetLegendText(strLegend);

	m_graph.SetDrawMode(3);
}

void CEnvStatsWnd::OnUpdateEvolution(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_choice == ID_EVOLUTION);
	pCmdUI->Enable(TRUE);
	m_graph.SetDataReset(9);
}


void CEnvStatsWnd::OnCPUTime() 
{
	m_choice = ID_CPUTIME;
	SetTitle(true);

	m_breedView.ShowWindow(SW_HIDE);
	m_graph.ShowWindow(SW_SHOWNORMAL);

	SetCPUTime();
}

void CEnvStatsWnd::SetCPUTime(bool bDraw)
{
	CEnvStatsList& list = m_pEnv->m_statsList;

 	m_graph.SetDataReset(9);
	m_graph.SetGraphType(6);
	m_graph.SetGraphStyle(4);
	m_graph.SetBackground(0);
	m_graph.SetPatternedLines(1);
	m_graph.SetPatternData(6);
	m_graph.SetRandomData(0);

	// Only ticks on both axis
	m_graph.SetTicks(1);
	m_graph.SetTickEvery(20);

	m_graph.SetNumPoints(CEnvStats::SAMPLES);
	m_graph.SetNumSets(1);

	m_graph.SetAutoInc(1);
	m_graph.SetColorData(15); 	// White
	//m_graph.SetColorData(12);	// Light Red
	//m_graph.SetColorData(10);	// Green
	//m_graph.SetColorData(9);	// Light Blue
	//m_graph.SetColorData(11);	// Light Cyan

	m_graph.SetAutoInc(1);
	//m_graph.SetLegendText("Diversity");	
	//m_graph.SetLegendText("Mean Cum Activity/20");
	//m_graph.SetLegendText("Std Cum. Activity");
	//m_graph.SetLegendText("Mean New Activity");
	//m_graph.SetLegendText("Std. Mean New Activity");

	
	m_graph.SetYAxisMax((float)10);
	
	int i;
	POSITION pos;

	m_graph.SetAutoInc(1);

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		CEnvStats stats = list.GetNext(pos);
		if (stats.m_dblSecsPerBiot < 0)
			m_graph.SetGraphData((float)0.0);
		else
			m_graph.SetGraphData((float)stats.m_dblSecsPerBiot * 1000);
		//m_graph.SetGraphData((float)stats.m_deaths
		i++;
	}

	
	/*for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	
	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData(list.GetNext(pos).m_dblMeanCumActivity/20);
		i++;
	}


	pos = list.GetHeadPosition();
	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData((float)list.GetNext(pos).m_dblStdActivity);
		i++;
	}

	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData((float)list.GetNext(pos).m_dblMeanNewActivity);
		i++;
	}

	
	pos = list.GetHeadPosition();
	for (i = 0; i < CEnvStats::SAMPLES - list.GetCount(); i++)
		m_graph.SetGraphData((float)0);

	while (pos != NULL && i < CEnvStats::SAMPLES)
	{
		m_graph.SetGraphData((float)list.GetNext(pos).m_dblStdNewActivity);
		i++;
	}
	*/
	m_graph.SetDrawMode(3);
}

void CEnvStatsWnd::OnUpdateCPUTime(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_choice == ID_CPUTIME);
	pCmdUI->Enable(TRUE);
	m_graph.SetDataReset(9);
}

void CEnvStatsWnd::OnEnergySpread() 
{
	m_choice = ID_ENERGY_SPREAD;
	SetTitle(true);

	m_breedView.ShowWindow(SW_HIDE);
	m_graph.ShowWindow(SW_SHOWNORMAL);

	SetEnergySpread();
}

void CEnvStatsWnd::OnUpdateEnergySpread(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_choice == ID_ENERGY_SPREAD);
	pCmdUI->Enable(TRUE);
}

void CEnvStatsWnd::OnStatsLog() 
{
	m_bLogging = !m_bLogging;
}

void CEnvStatsWnd::OnUpdateStatsLog(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bLogging);
	if (m_bLogging)
		pCmdUI->SetText (_T("Turn Logging Off"));
	else
		pCmdUI->SetText (_T("Turn Logging On"));
	
	}

void CEnvStatsWnd::OnSpecies() 
{
	m_choice = ID_SPECIES;
	SetTitle(true);

	m_breedView.ShowWindow(SW_HIDE);
	m_graph.ShowWindow(SW_SHOWNORMAL);

	SetSpecies();
}

void CEnvStatsWnd::SetSpecies(bool bDraw)
{
	CBiotList& list = m_pEnv->m_biotList;

 	m_graph.SetDataReset(9);
	m_graph.SetGraphType(4);
	m_graph.SetGraphStyle(2);
	m_graph.SetBackground(0);
	m_graph.SetPatternedLines(1);
	m_graph.SetPatternData(0);
	m_graph.SetRandomData(0);

	// Only ticks on both axis
	m_graph.SetTicks(3);
//	m_graph.SetTickEvery(20);

	m_graph.SetNumPoints((short) MAX_SPECIES + 1);
	m_graph.SetNumSets(1);

	m_graph.SetAutoInc(1);
	m_graph.SetColorData(3);	// Cyan

	if (list.GetSize() == 0)
	{
		m_graph.SetDrawMode(3);
		return;
	}

	long nPopulation = list.GetSize ();
	long m_Diversity[MAX_SPECIES + 1];
	memset(m_Diversity, 0, sizeof(m_Diversity));
	//memset(arCumulativeActivity, 0, sizeof(m_arCumulativeActivity));
	//vector < CString > m_lstSpecies;
	for (int j = 0; j < nPopulation; j++)
		{
		Biot* pBiot = list[j];
		m_Diversity[pBiot->trait.GetSpecies()]++;
		}

	m_graph.SetAutoInc(1);

	for (int i = 0; i < MAX_SPECIES + 1; i++)
		m_graph.SetGraphData((float) m_Diversity[i]);

	m_graph.SetAutoInc(1);
	for (i = 0; i < MAX_SPECIES + 1; i++)
		{
		if (m_Diversity[i] > 0)
			m_graph.SetLabelText((LPCTSTR)CBreedView::strNames[i]);
		}

	m_graph.SetDrawMode(3);
	}

void CEnvStatsWnd::OnUpdateSpecies(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_choice == ID_SPECIES);
	pCmdUI->Enable(TRUE);
}

