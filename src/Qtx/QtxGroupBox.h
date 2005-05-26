// File:      QtxGroupBox.h
// Author:    Sergey TELKOV

#ifndef QTXGROUPBOX_H
#define QTXGROUPBOX_H

#include "Qtx.h"

#include <qgroupbox.h>
#include <qwidgetlist.h>

class QTX_EXPORT QtxGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  QtxGroupBox( QWidget* = 0, const char* = 0 );
  QtxGroupBox( const QString&, QWidget* = 0, const char* = 0 );
  QtxGroupBox( int, Orientation, QWidget* = 0, const char* = 0 );
  QtxGroupBox( int, Orientation, const QString&, QWidget* = 0, const char* = 0 );
  virtual ~QtxGroupBox();

#if QT_VER < 3
  int           insideMargin() const;
  int           insideSpacing() const;
  void          setInsideMargin( int );
  void          setInsideSpacing( int );
#endif

  virtual void  setAlignment( int );
  virtual void  setTitle( const QString& );
  virtual void  setColumnLayout( int, Orientation );

  virtual void  insertTitleWidget( QWidget* );
  virtual void  removeTitleWidget( QWidget* );

  virtual void  show();
  virtual void  update();

  void          adjustInsideMargin();

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

  virtual bool  eventFilter( QObject*, QEvent* );

protected:
  virtual void  frameChanged();
  virtual void  childEvent( QChildEvent* );
  virtual void  resizeEvent( QResizeEvent* );
  virtual void  customEvent( QCustomEvent* );

private:
  void          initialize();
  void          updateTitle();
  QSize         titleSize() const;

private:
  QWidget*      myContainer;
};

#endif
