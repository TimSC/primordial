 ///////////////////////////////////////////////////////////////////////////////
//
// Connect
//
// This class is responsible for establishing and controlling connections to
// other worlds.
//
//

#ifndef connect_h
#define connect_h

#include <QQueue>
#include "Etools.h"

class Biot;

// ////////////////////////////////////////////////////////////////////
//
// SideListener
//
//
class SideListener
{
public:
    SideListener();
    virtual ~SideListener();

    virtual void BiotLeavingSide(int side, Biot *pBiot);
    virtual void ReadyToReceive(int sideId, bool ready);
};

// ////////////////////////////////////////////////////////////////////
//
// Side
//
//
class Side : public BRect
{
  public:
    Side();
    
    // We want this width larger than the largest biot width
    enum {
      RECT_WIDTH = 200
    };

    enum {
      LEFT   = 0,
      RIGHT  = 1,
      TOP    = 2,
      BOTTOM = 3
    };

    void Clear(BRect* pEnvRect);
    virtual void SetSize(bool connected) = 0;

    int IsIntersect(CLine& bLine, int& x, int& y)
    {
      for (int i = 0; i < m_lines; i++)
        if (m_line[i].Intersect(bLine, x, y))
          return true;
    
      return false;
    }

    bool Export(Biot* pBiot);
    virtual Biot* Import();

    void ReceiveBiotFromNetwork(Biot *pBiot);
    Biot* GetBiot();

    bool IsConnected();
    void SetConnected(bool conn);
    void SetListener(class SideListener *listenerIn);
    void SetRemoteReady(bool isReady);

    virtual void AdjustBiot(Biot& biot) = 0;
    virtual void RejectBiot(Biot& biot) = 0;
    virtual int  SideSize() = 0;

    protected:
      int m_sideId;
      int m_lines;
      CLine m_line[4];
      BRect* m_pEnv;
      QQueue<Biot *> m_inComing;
      bool m_isConnected;
      class SideListener *m_listener;
      bool m_readyToReceive;
      bool m_remoteReady;
};


class RightSide : public Side
{
  public:
    RightSide();
    void SetSize(bool connected);
    void AdjustBiot(Biot& biot);
    void RejectBiot(Biot& biot);
    int  SideSize();
};


class LeftSide : public Side
{
  public:
    LeftSide();
    void SetSize(bool connected);
    void AdjustBiot(Biot& biot);
    void RejectBiot(Biot& biot);
    int  SideSize();
};


class TopSide : public Side
{
  public:
    TopSide();
    void SetSize(bool connected);
    void AdjustBiot(Biot& biot);
    void RejectBiot(Biot& biot);
    int  SideSize();
};


class BottomSide : public Side
{
  public:
    BottomSide();
    void SetSize(bool connected);
    void AdjustBiot(Biot& biot);
    void RejectBiot(Biot& biot);
    int  SideSize();
}; 




#endif
