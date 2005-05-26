// File:      QtxListBox.h
// Author:    Sergey TELKOV

#ifndef QTXLISTBOX_H
#define QTXLISTBOX_H

#include "Qtx.h"

#include <qlistbox.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QLineEdit;
class QValidator;

class QTX_EXPORT QtxListBox : public QListBox
{
    Q_OBJECT

public:
    QtxListBox( QWidget* = 0, const char* = 0, WFlags = 0 );
    virtual ~QtxListBox();

    bool              isEditEnabled() const;
    bool              defaultEditAction() const;
    bool              isModificationEnabled() const;

    QListBoxItem*     editedItem() const;
    int               editedIndex() const;

    void              startEdition( const int );
    void              startEdition( const QListBoxItem* );
    void              endEdition( const bool );

    void              ensureItemVisible( const int );
    void              ensureItemVisible( const QListBoxItem* );

    virtual bool      eventFilter( QObject*, QEvent* );

    const QValidator* validator() const;

    void              moveItemToTop( const int );
    void              moveItemToBottom( const int );
    void              moveItem( const int, const int );

    void              createItem( const int = -1 );
    void              deleteItem( const int = -1 );

signals:
    void              itemEdited( int );
    void              itemEdited( QListBoxItem* );
    void              itemMoved( int, int );

public slots:
    virtual void      setEditEnabled( bool );
    virtual void      setDefaultEditAction( bool );
    virtual void      setModificationEnabled( bool );

    virtual void      clearValidator();
    virtual void      setValidator( const QValidator* );
    virtual void      setContentsPos( int, int );

private slots:
    void              onContentsMoving( int, int );

protected:
    virtual void      keyPressEvent( QKeyEvent* );
    virtual void      viewportResizeEvent( QResizeEvent* );
    virtual void      mouseDoubleClickEvent( QMouseEvent* );

private:
    QLineEdit*        editor() const;
    void              createEditor();
    void              updateEditor();

private:
    QLineEdit*        myEditor;
    int               myEditIndex;
    bool              myEditState;
    bool              myEditDefault;
    bool              myModifEnabled;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
