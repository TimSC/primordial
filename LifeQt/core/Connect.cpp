#include <iostream>
#include "Etools.h"
#include "Connect.h"
#include "Environ.h"
#include "Biots.h"

const int SIDE_BUFFER_SOFT_MAX=5;
const int SIDE_BUFFER_HARD_MAX=50;

// //////////////////////////////////////////////////////////////////

SideListener::SideListener()
{

}

SideListener::~SideListener()
{

}

void SideListener::BiotLeavingSide(int side, Biot *pBiot)
{

}

void SideListener::ReadyToReceive(int sideId, bool ready)
{

}

// //////////////////////////////////////////////////////////////////
// Side
//
//
//
//
Side::Side()
{
    m_sideId = -1;
	m_lines = 0;
	m_pEnv = NULL;
    m_isConnected = false;
    m_listener = nullptr;
    m_readyToReceive = true;
    m_remoteReady = true;
}

void Side::Clear(BRect* pEnvRect)
{
	// Initially we have not connected, thus set up without connection
	Set(0,0,0,0);

	m_pEnv  = pEnvRect;
	m_lines = 1;
    m_inComing.clear();
}

bool Side::Export(Biot* pBiot)
{
    if(!m_remoteReady) return false;
    if(m_listener)
    {
        m_listener->BiotLeavingSide(m_sideId, pBiot);
        return true;
    }
    return false;
}

Biot*  Side::Import()
{
    if(m_inComing.isEmpty())
        return nullptr;
    Biot* out = m_inComing.dequeue();

    if(m_inComing.size() < SIDE_BUFFER_SOFT_MAX and !m_readyToReceive)
    {
        m_readyToReceive = true;
        if(m_listener)
            m_listener->ReadyToReceive(m_sideId, true);
    }

    return out;
}

void Side::ReceiveBiotFromNetwork(Biot *pBiot)
{
    if(m_inComing.size() >= SIDE_BUFFER_HARD_MAX)
    {
        std::cout << "Error buffer overflow receiving biot" << std::endl;
        assert(!m_readyToReceive);
        return;
    }

    m_inComing.enqueue(pBiot);

    if(m_inComing.size() > SIDE_BUFFER_SOFT_MAX and m_readyToReceive)
    {
        m_readyToReceive = false;
        if(m_listener)
            m_listener->ReadyToReceive(m_sideId, false);
    }
}

bool Side::IsConnected()
{
    return m_isConnected;
}

void Side::SetConnected(bool conn)
{
    m_isConnected = conn;
}

void Side::SetListener(class SideListener *listenerIn)
{
    m_listener = listenerIn;
}

void Side::SetRemoteReady(bool isReady)
{
    m_remoteReady = isReady;
}

// ************************************

RightSide::RightSide() : Side()
{
    m_sideId = 1;
}

int RightSide::SideSize()
{
	return m_bottom - m_top;
}

void RightSide::AdjustBiot(Biot& biot)
{
    biot.MoveBiot(m_left - biot.m_left - 1, m_top);
    if(biot.posAndSpeed.getDeltaX()>-1.0)
        biot.posAndSpeed.setDeltaX(-1.0);
}

void RightSide::RejectBiot(Biot& biot)
{
    biot.MoveBiot(m_left - biot.m_left - 1, m_top);
}

// ************************************

LeftSide::LeftSide() : Side()
{
    m_sideId = 0;
}

int LeftSide::SideSize()
{
  return m_bottom - m_top;
}

void LeftSide::AdjustBiot(Biot& biot)
{
    biot.MoveBiot(m_right - biot.m_right  + 1, m_top);
    if(biot.posAndSpeed.getDeltaX()<1.0)
        biot.posAndSpeed.setDeltaX(1.0);
}

void LeftSide::RejectBiot(Biot& biot)
{
  biot.MoveBiot(m_right - biot.m_right  + 1, 0);
}

TopSide::TopSide() : Side()
{
    m_sideId = 2;
}

int TopSide::SideSize()
{
  return m_right - m_left;
}

void TopSide::AdjustBiot(Biot& biot)
{
    biot.MoveBiot(m_left, m_bottom - biot.m_bottom + 1);
    if(biot.posAndSpeed.getDeltaY()<1.0)
        biot.posAndSpeed.setDeltaY(1.0);
}

void TopSide::RejectBiot(Biot& biot)
{
  biot.MoveBiot(0, m_bottom - biot.m_bottom + 1);
}


////////////////////////////////////////////////////////////////////
// BottomSide
//
//
//
//

BottomSide::BottomSide() : Side()
{
    m_sideId = 3;
}

void BottomSide::AdjustBiot(Biot& biot)
{
    biot.MoveBiot(m_left, m_top - biot.m_top - 1);
    if(biot.posAndSpeed.getDeltaY()>-1.0)
        biot.posAndSpeed.setDeltaY(-1.0);
}

void BottomSide::RejectBiot(Biot& biot)
{
  biot.MoveBiot(0, m_top - biot.m_top - 1);
}

int BottomSide::SideSize()
{
  return m_right - m_left;
}

////////////////////////////////////////////////////////////////////
// RightSide
//
//
//
//
void RightSide::SetSize(bool connected)
{
    if(connected)
    {
        int width  = m_pEnv->Height();
        int right  = m_pEnv->Width() + RECT_WIDTH;
        int left   = m_pEnv->Width();
        int top    = (m_pEnv->Height() - width) / 2;
        int bottom = m_pEnv->Height() - top;

        Set(left, top, right, bottom);

        m_line[0].Set(left, bottom, right, bottom);
        m_line[1].Set(left, top, right, top);
        m_line[2].Set(left, bottom, left, m_pEnv->Width() + RECT_WIDTH);
        m_line[3].Set(left, top, left, -RECT_WIDTH);
        m_lines = 4;
    }
    else
    {
        //Clear(pEnvRect);
        m_line[0].Set(m_pEnv->m_right , m_pEnv->m_top, m_pEnv->m_right, m_pEnv->m_bottom);
        m_lines = 1;
    }
}


////////////////////////////////////////////////////////////////////
// LeftSide
//
//
//
//
void LeftSide::SetSize(bool connected)
{
    if(connected)
    {
        int width  = m_pEnv->Height();
        int right  = 0;
        int left   = -RECT_WIDTH;
        int top    = (m_pEnv->Height() - width) / 2;
        int bottom = m_pEnv->Height() - top;

        Set(left, top, right, bottom);

        m_line[0].Set(right, bottom, left, bottom);
        m_line[1].Set(right, top, left, top);
        m_line[2].Set(right, bottom, right, m_pEnv->Width() + RECT_WIDTH);
        m_line[3].Set(right, top, right, -RECT_WIDTH);
        m_lines = 4;
    }
    else
    {
        //Clear(pEnvRect);
        m_line[0].Set(m_pEnv->m_left, m_pEnv->m_top, m_pEnv->m_left, m_pEnv->m_bottom);
        m_lines = 1;
    }
}


////////////////////////////////////////////////////////////////////
// TopSide
//
//
//
//
void TopSide::SetSize(bool connected)
{
    if(connected)
    {
        int width  = m_pEnv->Width();
        m_left   = (m_pEnv->Width() - width) / 2;
        m_right  = m_pEnv->Width() - m_left;
        m_top    = -RECT_WIDTH;
        m_bottom = 0;

        Set(m_left, m_top, m_right, m_bottom);
        m_line[0].Set(m_left, m_bottom, m_left, m_top);
        m_line[1].Set(m_right, m_bottom, m_right, m_top);
        m_line[2].Set(m_left, m_bottom, -RECT_WIDTH, m_bottom);
        m_line[3].Set(m_right, m_bottom, m_pEnv->Width() + RECT_WIDTH, m_bottom);
        m_lines = 4;
    }
    else
    {
        //Clear(pEnvRect);
        m_line[0].Set(m_pEnv->m_left, m_pEnv->m_top, m_pEnv->m_right, m_pEnv->m_top);
        m_lines = 1;
    }
}


void BottomSide::SetSize(bool connected)
{
    if(connected)
    {
        int width  = m_pEnv->Width();
        m_left   = (m_pEnv->Width() - width) / 2;
        m_right  = m_pEnv->Width() - m_left;
        m_top    = m_pEnv->Height();
        m_bottom = m_pEnv->Height() + RECT_WIDTH;

        Set(m_left, m_top, m_right, m_bottom);
        m_line[0].Set(m_left, m_top, m_left, m_bottom);
        m_line[1].Set(m_right, m_top, m_right, m_bottom);
        m_line[2].Set(m_left, m_top, -RECT_WIDTH, m_top);
        m_line[3].Set(m_right, m_top, m_pEnv->Width() + RECT_WIDTH, m_top);
        m_lines = 4;
    }
    else
    {
        //Clear(pEnvRect);
        m_line[0].Set(m_pEnv->m_left, m_pEnv->m_bottom, m_pEnv->m_right, m_pEnv->m_bottom);
        m_lines = 1;
    }
}



