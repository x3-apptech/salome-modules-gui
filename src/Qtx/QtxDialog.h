// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

// File:      QtxDialog.h
// Author:    Sergey TELKOV
//
#ifndef QTXDIALOG_H
#define QTXDIALOG_H

#include "Qtx.h"

#include <QDialog>
#include <QMap>

class QFrame;
class QLabel;
class QAbstractButton;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxDialog : public QDialog
{
  Q_OBJECT

  class Area;
  class Border;

public:
  //! Buttons alignment type
  typedef enum { Position,          //!< buttons are placed according their position
                 Expand,            //!< buttons occupy all available space
                 Uniform            //!< buttons are uniformly placed in the area
  } PlacePolicy;
  //! Buttons area
  typedef enum { TopArea,           //!< horizontal area at the top side of the dialog box
                 BottomArea,        //!< horizontal area at the bottom side of the dialog box
                 LeftArea,          //!< vertical area at the left side of the dialog box
                 RightArea          //!< vertical area at the right side of the dialog box
  } ButtonArea;
  //! Button position
  typedef enum { Left,              //!< set button left-most
                 Right,             //!< set button right-most
                 Center,            //!< place button in the center
                 Top = Left,        //!< set button top-most
                 Bottom = Right     //!< set button bottom-most
  } ButtonPosition;
  //! Button ID flags
  typedef enum { None      = 0x00000000,                      //!< no button used
                 OK        = 0x00000001,                      //!< OK button
                 Apply     = 0x00000002,                      //!< Apply button
                 Cancel    = 0x00000004,                      //!< Cancel button
                 Yes       = 0x00000008,                      //!< Yes button
                 No        = 0x00000010,                      //!< No button
                 Close     = 0x00000020,                      //!< Close button
                 Help      = 0x00000040,                      //!< Help button
                 OKCancel  = OK  | Cancel,                    //!< OK & Cancel button
                 YesNo     = Yes | No,                        //!< Yes & No buttons
                 Standard  = OK  | Cancel | Help,             //!< OK, Cancel & Help buttons
                 All       = Standard | YesNo | Apply | Close //!< all buttons
  } ButtonFlags;
  //! Dialog box flags
  typedef enum { Accept    = 0x000001, //!< allow dialog box accepting control
                 Reject    = 0x000002, //!< allow dialog box rejecting control
                 AlignOnce = 0x000004, //!< align dialog box only when it is first time shown
                 SetFocus  = 0x000008  //!< allow to set focus on dialog box when it is shown (user can use setFocusProxy() and specify own initial focus widget)
  } DialogFlags;
  
public:
  QtxDialog( QWidget* = 0, bool = false, bool = false, const int = Standard, Qt::WindowFlags = 0 );
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
  
  QIntList         userButtonIds() const;
  QAbstractButton* userButton( const int ) const;
  
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
  virtual void     setVisible( bool );

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
  typedef QMap<int, QAbstractButton*> ButtonMap;   //!< button map

protected:
  QFrame*          mainFrame() const;
  
  virtual bool     acceptData() const;
  virtual bool     rejectData() const;
  
  virtual QAbstractButton* createButton( QWidget* );
  
  QAbstractButton* button( const int ) const;
  ButtonMap        buttons( const int = All ) const;
  int              buttonId( const QAbstractButton* ) const;
  int              buttonPosition( QAbstractButton* ) const;
  
  virtual void     showEvent( QShowEvent* );
  virtual void     hideEvent( QHideEvent* );
  virtual void     closeEvent( QCloseEvent* );
  virtual void     childEvent( QChildEvent* );
  virtual void     keyPressEvent( QKeyEvent* );
  
private:
  void             adjustButtons();
  void             emitSignal();
  
private:
  typedef QMap<int, Area*> AreaMap;        //!< button area map
  typedef QMap<int, int>   PositionMap;    //!< button position map
  
  friend class Area;

private:
  AreaMap          myArea;                 //!< buttons areas map
  ButtonMap        myButton;               //!< buttons map
  PositionMap      myPosition;             //!< buttons position map
  
  bool             myInited;               //!< dialog's "initialized" flag
  const QObject*   mySender;               //!< signal sender
  uint             myAlignment;            //!< dialog box alignment type
  QFrame*          myMainFrame;            //!< main frame
  int              myButtonFlags;          //!< button flags
  int              myDialogFlags;          //!< dialog flags
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
