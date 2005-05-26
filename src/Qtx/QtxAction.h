// File:      QtxAction.hxx
// Author:    Sergey TELKOV

#ifndef QTXACTION_H
#define QTXACTION_H

#include "Qtx.h"

#include <qaction.h>

class QTX_EXPORT QtxAction : public QAction
{
    Q_OBJECT

public:
    QtxAction( QObject* = 0, const char* = 0, bool = false );
    QtxAction( const QString&, const QString&, int, QObject*, const char* = 0, bool = false );
    QtxAction( const QString&, const QIconSet&, const QString&, int, QObject*, const char* = 0, bool = false );
    virtual ~QtxAction();

    virtual bool addTo( QWidget* );
    virtual bool addTo( QWidget*, int );

protected:
    void         setPopup( QPopupMenu*, const int, QPopupMenu* ) const;
};

#endif
