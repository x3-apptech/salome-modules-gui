// File:      QtxListAction.hxx
// Author:    Sergey TELKOV (Based on code by Eugene AKSENOV)

#ifndef QTXLISTACTION_H
#define QTXLISTACTION_H

#include "QtxAction.h"

#include <qmap.h>
#include <qframe.h>
#include <qstringlist.h>

class QLabel;
class QListBox;
class QPopupMenu;
class QToolButton;
class QToolTipGroup;

class QtxListFrame;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxListAction : public QtxAction
{
    Q_OBJECT

    Q_PROPERTY( QStringList names READ names WRITE addNames )

    class ToolButton;

public:
    enum { Item, SubMenu } PopupMode;

public:
    QtxListAction( QObject* = 0, const char* = 0, bool = false );
    QtxListAction( const QString&, const QIconSet&, const QString&, int, QObject*, const char* = 0, bool = false );
    QtxListAction( const QString&, const QString&, int, QObject*, const char* = 0, bool = false );
    virtual ~QtxListAction();

public:
    virtual bool            addTo( QWidget* );
    virtual bool            addTo( QWidget*, const int );

    virtual bool            removeFrom( QWidget* );
    virtual bool            eventFilter( QObject*, QEvent* );

    int                     popupMode() const;
    void                    setPopupMode( const int );

    QStringList             names() const;
    void                    addNames( const QStringList&, bool = true );
    void                    setComment( const QString&, const QString& = QString::null );

    void                    setMaxLines( int );
    void                    setMaxLineChars( int );

signals:
    void                    activated( int );

public slots:
    virtual void            setEnabled( bool );

private slots:
    void                    onHided();
    void                    onSingle();
    void                    onExpand( bool );
    void                    onMultiple( int );
    void                    onActivated( int );
    void                    onDestroyed( QObject* );

protected:
    virtual void            addedTo( QWidget*, QWidget* );

private:
    void                    initialize();
    QWidget*                widget( QWidget* ) const;
    QPopupMenu*             listPopup( QWidget* ) const;
    QToolButton*            mainButton( QWidget* ) const;
    QToolButton*            dropButton( QWidget* ) const;

    void                    controlDeleted( QWidget* );

private:
    typedef struct { int id; QPopupMenu* popup; } Popups;
    typedef struct { QToolButton* main; QToolButton* drop; } Buttons;
    typedef QMap<QWidget*, Popups>  PopupsMap;
    typedef QMap<QWidget*, Buttons> ButtonsMap;
    
private:
    int                     myMode;
    QtxListFrame*           myFrame;
    bool                    myRaise;
    PopupsMap               myPopups;
    ButtonsMap              myButtons;
    QToolTipGroup*          myTipGroup;

    friend class ToolButton;
    friend class QtxListFrame;
};

/*************************************
**  Class: QtxListFrame
**  Level: Internal
*************************************/

class QtxListFrame : public QFrame
{
    Q_OBJECT

    class ScrollEvent;

public:
    QtxListFrame( QtxListAction*, QWidget* parent, WFlags f = 0 );
    virtual ~QtxListFrame();

    void                    clear();
    const QStringList       names() const;
    void                    addNames( const QStringList& );
    
    void                    setSingleComment( const QString& );
    void                    setMultipleComment( const QString& );
    
    int                     selected() const;
    void                    setSelected( const int );
    
    void                    setMaxLines( int );
    void                    setMaxLineChars( int );
    
    virtual bool            event( QEvent* );
    virtual bool            eventFilter( QObject*, QEvent* );

    void                    setOwner( QWidget* );

    void                    updateComment();
    
signals:
    void                    hided();
    void                    selected( int );
    
public slots:
    virtual void            show();
    virtual void            hide();
    
private slots:
    void                    reject();
    void                    accept();
    
    void                    onScroll( int, int );
    
private:
    void                    setNames( const QStringList& );
    bool                    handleKeyEvent( QObject*, QKeyEvent* );
    bool                    handleMouseEvent( QObject*, QMouseEvent* );

    friend class QtxListAction;
    
private:
    QListBox*               myList;
    QStringList             myNames;
    QWidget*                myOwner;
    QtxListAction*          myAction;
    QLabel*                 myComment;
    
    QString                 mySingleComment;
    QString                 myMultipleComment;
    
    int                     myMaxLines;
    int                     myMaxLineChars;
    
    int                     myScrollVal;
    bool                    myScrollBlock;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
