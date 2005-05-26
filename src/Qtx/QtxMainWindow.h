// File:      QtxMainWindow.h
// Author:    Sergey TELKOV

#ifndef QTXMAINWINDOW_H
#define QTXMAINWINDOW_H

#include "Qtx.h"

#include <qmainwindow.h>

class QDockWindow;
class QtxResourceMgr;

class QTX_EXPORT QtxMainWindow : public QMainWindow
{
  Q_OBJECT

  class Filter;

  enum { WS_Normal, WS_Minimized, WS_Maximized, WS_Hidden };
  enum { WP_Absolute, WP_Center, WP_Left, WP_Right, WP_Top = WP_Left, WP_Bottom = WP_Right };

public:
  QtxMainWindow( QWidget* = 0, const char* = 0, WFlags = WType_TopLevel );
  virtual ~QtxMainWindow();

  bool              isDockableMenuBar() const;
  void              setDockableMenuBar( const bool );

  bool              isDockableStatusBar() const;
  void              setDockableStatusBar( const bool );

  void              loadGeometry( QtxResourceMgr*, const QString& );
  void              saveGeometry( QtxResourceMgr*, const QString& ) const;

  virtual bool      eventFilter( QObject*, QEvent* );

public slots:
  virtual void      setAppropriate( QDockWindow*, bool );

protected:
  virtual void      setUpLayout();

private slots:
  void              onDestroyed( QObject* );

private:
  int               windowState( const QString& ) const;
  int               windowPosition( const QString& ) const;
  int               relativeCoordinate( const int, const int, const int ) const;

private:
  QDockWindow*      myMenuBar;
  QDockWindow*      myStatusBar;
};

#endif
