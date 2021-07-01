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

#include "Etools.h"

class Biot;

////////////////////////////////////////////////////////////////
// Fifo
//
//
class Fifo
{
  public:
      enum {
        QUEUE_SIZE = 5
      };

      Fifo();
      ~Fifo();

//      int Count();

 //     int FreeCount() 
 //     {
 //       return QUEUE_SIZE - Count();
 //     }

      bool   Put(void* pClass);
      void* Get();
//	  void* Peek();
		void Empty();

                 
  private:
      int    m_read;
      int    m_write; 
      int    m_size;
      void* m_pVoid[QUEUE_SIZE];
};


//////////////////////////////////////////////////////////////////////
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
};

//////////////////////////////////////////////////////////////////////
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

    virtual void AdjustBiot(Biot& biot) = 0;
    virtual void RejectBiot(Biot& biot) = 0;
    virtual int  SideSize() = 0;

    protected:
      int m_sideId;
      int m_lines;
      CLine m_line[4];
      BRect* m_pEnv;
      Fifo m_inComing;
      Fifo m_outGoing;
      bool m_isConnected;
      class SideListener *m_listener;
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
