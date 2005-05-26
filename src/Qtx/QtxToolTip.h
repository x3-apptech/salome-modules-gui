// File:      QtxToolTip.h
// Author:    Sergey TELKOV

#ifndef QTXTOOLTIP_H
#define QTXTOOLTIP_H

#include "Qtx.h"

#include <qlabel.h>

class QTimer;

class QTX_EXPORT QtxToolTip : public QLabel
{
	Q_OBJECT

public:
	QtxToolTip( QWidget* = 0 );
	virtual ~QtxToolTip();

	void          hideTip();
  
	virtual void  showTip( const QPoint& aPos,
                         const QString& text, const QRect& aWidgetRegion );
	virtual void  showTip( const QRect& aRegion,
                         const QString& text, const QRect& aWidgetRegion );

	virtual bool  eventFilter( QObject* o, QEvent* e );

  void          setWakeUpDelayTime( int );
  void          setShowDelayTime( int );

  int           wakeUpDelayTime() const { return myWakeUpDelayTime; }
  int           showDelayTime() const { return myShowDelayTime; }

signals:
  void          maybeTip( QPoint, QString&, QFont&, QRect&, QRect& );

protected slots:
	void          onSleepTimeOut();
	void          onWakeUpTimeOut();

protected:
	virtual void  maybeTip( const QPoint& );
	virtual void  mousePressEvent( QMouseEvent* );
	virtual void  mouseDoubleClickEvent( QMouseEvent* );

  QTimer*       sleepTimer() const;
  QTimer*       wakeUpTimer() const;

private:
	QTimer*       myWakeUpTimer;
	QTimer*       mySleepTimer;
	QRect         myWidgetRegion;

  int           myShowDelayTime;
  int           myWakeUpDelayTime;
};

#endif
