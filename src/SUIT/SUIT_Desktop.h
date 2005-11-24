#ifndef SUIT_DESKTOP_H
#define SUIT_DESKTOP_H

#include "SUIT.h"

#include <qmap.h>
#include <qpixmap.h>
#include <QtxMainWindow.h>

class QPopupMenu;
class QtxLogoMgr;
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

  void                     addLogo( const QString&, const QPixmap& ); // Not should be used. Will be removed.
  void                     removeLogo( const QString& );              // Not should be used. Will be removed.

  int                      logoCount() const;

  void                     logoClear();
  void                     logoRemove( const QString& );
  void                     logoInsert( const QString&, const QPixmap&, const int = -1 );

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
  QtxLogoMgr*              myLogoMgr;
};

#endif
