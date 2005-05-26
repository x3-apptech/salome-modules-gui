// File:      QtxIntSpinBox.h
// Author:    Sergey TELKOV

#ifndef QTXINTSPINBOX_H
#define QTXINTSPINBOX_H

#include "Qtx.h"

#include <qspinbox.h>

class QTX_EXPORT QtxIntSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    QtxIntSpinBox( QWidget* = 0, const char* = 0 );
    QtxIntSpinBox( int, int, int = 1, QWidget* = 0, const char* = 0 );
    virtual ~QtxIntSpinBox();
    
    bool         isCleared() const;
    void         setCleared( const bool );
    
    virtual bool eventFilter( QObject*, QEvent* );
    
public slots:
    virtual void setValue( int );
    
protected:
    virtual void interpretText();
    virtual void updateDisplay();
    virtual void leaveEvent( QEvent* );
    virtual void wheelEvent( QWheelEvent* );
    
private:
    bool         myCleared;
};

#endif
