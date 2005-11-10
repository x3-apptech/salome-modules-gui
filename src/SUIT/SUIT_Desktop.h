#ifndef SUIT_DESKTOP_H
#define SUIT_DESKTOP_H

#include "SUIT.h"

#include <qmap.h>
#include <qpixmap.h>
#include <QtxMainWindow.h>

class QPopupMenu;
class SUIT_ViewWindow;
class QtxActionMenuMgr;
class QtxActionToolMgr;

class SUIT_EXPORT SUIT_Desktop : public QtxMainWindow
{
  Q_OBJECT

  class LogoManager
  {
  public:
    LogoManager( SUIT_Desktop* );
    void                   addLogo( const QString&, const QPixmap& );
    void                   removeLogo( const QString& );
    void                   clearLogo();

  private:
    void                   generateLogo();
    
  private:
    SUIT_Desktop*          myDesktop;
    QMap<QString, QPixmap> myLogoMap;
    int                    myId;
  };

public:
  SUIT_Desktop();
  virtual ~SUIT_Desktop();

  QtxActionMenuMgr*        menuMgr() const;
  QtxActionToolMgr*        toolMgr() const;

  virtual SUIT_ViewWindow* activeWindow() const = 0;
  virtual QPtrList<SUIT_ViewWindow> windows() const = 0;

  void                     addLogo( const QString&, const QPixmap& );
  void                     removeLogo( const QString& );
  void                     clearLogo();

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
  LogoManager              myLogoMan;
};

#endif
