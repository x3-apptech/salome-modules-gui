// File:      QtxDockWindow.h
// Author:    Sergey TELKOV

#include "Qtx.h"

#include <qdockwindow.h>

class QTX_EXPORT QtxDockWindow : public QDockWindow
{
  Q_OBJECT

  class Watcher;

public:
  QtxDockWindow( Place = InDock, QWidget* = 0, const char* = 0, WFlags = 0 );
  QtxDockWindow( const bool, QWidget*, const char* = 0, WFlags = 0 );
  QtxDockWindow( QWidget*, const char* = 0, WFlags = 0 );
  virtual ~QtxDockWindow();

  virtual void  setWidget( QWidget* );

  bool          isStretchable() const;
  virtual void  setStretchable( const bool );

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

  QMainWindow*  mainWindow() const;

public slots:
  virtual void  show();
  virtual void  hide();

private:
  Watcher*      myWatcher;
  bool          myStretch;
};
