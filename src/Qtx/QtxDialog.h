// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      QtxDialog.hxx
// Author:    Sergey TELKOV

#ifndef QTXDIALOG_H
#define QTXDIALOG_H

#include "Qtx.h"

#include <qmap.h>
#include <qdialog.h>
#include <qvaluelist.h>

class QFrame;
class QLabel;
class QButton;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxDialog : public QDialog
{
	  Q_OBJECT

	  class Area;
	  class Border;

public:
    typedef enum { Position, Expand, Uniform } PlacePolicy;
    typedef enum { TopArea, BottomArea, LeftArea, RightArea } ButtonArea;
    typedef enum { Left, Right, Center, Top = Left, Bottom = Right } ButtonPosition;

    typedef enum { None      = 0x00000000,
                   OK        = 0x00000001,
                   Apply     = 0x00000002,
                   Cancel    = 0x00000004,
                   Yes       = 0x00000008,
                   No        = 0x00000010,
                   Close     = 0x00000020,
                   Help      = 0x00000040,
                   OKCancel  = OK  | Cancel,
                   YesNo     = Yes | No,
                   Standard  = OK  | Cancel | Help,
                   All       = Standard | YesNo | Apply | Close } ButtonFlags;

    typedef enum { Accept    = 0x000001,
				           Reject    = 0x000002,
				           AlignOnce = 0x000004,
                   SetFocus  = 0x000008 } DialogFlags;

public:
    QtxDialog( QWidget* = 0, const char* = 0, bool = false,
               bool = false, const int = Standard, WFlags = 0 );
    virtual ~QtxDialog();

    void             setDialogFlags( const int );
    void             clearDialogFlags( const int );
    bool             testDialogFlags( const int ) const;

    void             setButtonFlags( const int );
    void             clearButtonFlags( const int );
    bool             testButtonFlags( const int ) const;

    int              buttonPosition( const int ) const;
    void             setButtonPosition( const int, const int = -1 );
    void             setPlacePosition( const int, const int );

    int              placePolicy( const int ) const;
    void             setPlacePolicy( const int, const int );
    void             setButtonPlace( const int, const int );

    QString          buttonText( const int );
    void             setButtonText( const int, const QString& text );

    void             setButtonFocus( const int );
    bool             hasButtonFocus( const int ) const;

    bool             isButtonEnabled( const int ) const;
    void             setButtonEnabled( const bool, const int );

    bool             isBorderEnabled( const int ) const;
    void             setBorderEnabled( const bool, const int );

    void             removeButton( const int );
    int              insertButton( const QString&, const int = BottomArea );

    QValueList<int>  userButtonIds() const;
    QButton*         userButton( const int ) const;

    virtual void     show();
    virtual void     hide();

    uint             setAlignment( uint align );
    static  void     setUnits( QLabel*, const QString& );

signals:
    void             dlgButton( int );
    void             dlgParamChanged();

    void             dlgHelp();
    void             dlgApply();

    void             dlgOk();
    void             dlgNo();
    void             dlgYes();
    void             dlgClose();
    void             dlgCancel();

public slots:
    void             update();

protected slots:
    virtual void     accept();
    virtual void     reject();

private slots:
    void             onAccept();
    void             onReject();
    void             onButton();
    void             onSizeGripDestroyed();
    void             onDestroyed( QObject* );

protected:
    typedef QMap<int, QButton*> ButtonMap;

protected:
    QFrame*          mainFrame() const;

    virtual bool     acceptData() const;
    virtual bool     rejectData() const;

    virtual QButton* createButton( QWidget* );

    QButton*         button( const int ) const;
    ButtonMap        buttons( const int = All ) const;
    int              buttonId( const QButton* ) const;
    int              buttonPosition( QButton* ) const;

    virtual void     showEvent( QShowEvent* );
    virtual void     hideEvent( QHideEvent* );
    virtual void     closeEvent( QCloseEvent* );
    virtual void     childEvent( QChildEvent* );
    virtual void     keyPressEvent( QKeyEvent* );

private:
	  void             adjustButtons();
    void             emitSignal();

private:
    typedef QMap<int, Area*> AreaMap;
    typedef QMap<int, int>   PositionMap;

    friend class Area;

private:
    AreaMap          myArea;
    ButtonMap        myButton;
    PositionMap      myPosition;

    bool             myInited;
    const QObject*   mySender;
    uint             myAlignment;
    QFrame*          myMainFrame;
    int              myButtonFlags;
    int              myDialogFlags;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
