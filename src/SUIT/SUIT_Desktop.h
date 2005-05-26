#ifndef SUIT_DESKTOP_H
#define SUIT_DESKTOP_H

#include "SUIT.h"

#include <QtxMainWindow.h>

class QPopupMenu;
class SUIT_ViewWindow;
class QtxActionMenuMgr;
class QtxActionToolMgr;

class SUIT_EXPORT SUIT_Desktop : public QtxMainWindow
{
  Q_OBJECT

public:
  SUIT_Desktop();
  virtual ~SUIT_Desktop();

  QtxActionMenuMgr*        menuMgr() const;
  QtxActionToolMgr*        toolMgr() const;

  virtual SUIT_ViewWindow* activeWindow() const = 0;
  virtual QPtrList<SUIT_ViewWindow> windows() const = 0;

signals:
  void                     activated();
  void                     deactivated();
  void                     windowActivated( SUIT_ViewWindow* );
  void                     closing( SUIT_Desktop*, QCloseEvent* );

protected:
  virtual bool             event( QEvent* );
  virtual void             closeEvent( QCloseEvent* );
  virtual void             childEvent( QChildEvent* );

  virtual QWidget*         parentArea() const = 0;

private:
  QtxActionMenuMgr*        myMenuMgr;
  QtxActionToolMgr*        myToolMgr;
};

#endif
