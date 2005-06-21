// File:      QtxComboBox.h
// Author:    Sergey TELKOV

#ifndef QTXCOMBOBOX_H
#define QTXCOMBOBOX_H

#include "Qtx.h"

#include <qmap.h>
#include <qcombobox.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxComboBox : public QComboBox
{
    Q_OBJECT

    typedef QMap<int, int> IndexIdMap;

public:
    QtxComboBox( QWidget* = 0, const char* = 0 );
    QtxComboBox( bool, QWidget* = 0, const char* = 0 );
    virtual ~QtxComboBox();

    bool         isCleared() const;
    void         setCleared( const bool );

    virtual void setCurrentItem( int );
    virtual void setCurrentText( const QString& );

    int          currentId() const;
    void         setCurrentId( int );

signals:
    void         activatedId( int );
    void         highlightedId( int );

private slots:
    void         onActivated( int );
    void         onActivated( const QString& );

protected:
    virtual void paintEvent( QPaintEvent* );

private:
    int          id( const int ) const;
    int          index( const int ) const;

    void         resetClear();
    void         paintClear( QPaintEvent* );

private:
    bool         myCleared;
    IndexIdMap   myIndexId;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
