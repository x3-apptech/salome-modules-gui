// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxBackgroundTool.h
// Author:    Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef QTXBACKGROUNDTOOL_H
#define QTXBACKGROUNDTOOL_H

#include "Qtx.h"
#include "QtxDialog.h"

#include <QWidget>
#include <QMap>

class QCheckBox;
class QComboBox;
class QStackedWidget;
class QLineEdit;
class QPushButton;
class QtxColorButton;

class QTX_EXPORT QtxBackgroundTool : public QWidget
{
  enum { Color, Gradient };
  enum { TypeRole = Qt::UserRole, IdRole };

  Q_OBJECT

public:
  QtxBackgroundTool( QWidget* = 0 );
  QtxBackgroundTool( Qt::Orientation, QWidget* = 0 );
  virtual ~QtxBackgroundTool();

  Qtx::BackgroundData data() const;
  void                setData( const Qtx::BackgroundData& );

  void                gradients( QStringList&, QIntList& ) const;
  void                setGradients( const QStringList&, const QIntList& = QIntList() );

  bool                isModeAllowed( Qtx::BackgroundMode ) const;
  void                setModeAllowed( Qtx::BackgroundMode, bool = true );

  bool                isTextureModeAllowed( Qtx::TextureMode ) const;
  void                setTextureModeAllowed( Qtx::TextureMode, bool = true );

  bool                isTextureAllowed() const;
  void                setTextureAllowed( bool = true );

  QString             imageFormats() const;
  void                setImageFormats( const QString& );

  Qt::Orientation     orientation() const;
  void                setOrientation( Qt::Orientation );

private:
  void                init( Qt::Orientation );
  void                internalUpdate();

private slots:
  void                updateState();
  void                browse();
  
private:
  QComboBox*          myModeCombo;
  QStackedWidget*     myCContainer;
  QWidget*            myTContainer;
  QCheckBox*          myTextureCheck;
  QLineEdit*          myFileName;
  QPushButton*        myBrowseBtn;
  QComboBox*          myTextureMode;
  QtxColorButton*     myFirstColor;
  QtxColorButton*     mySecondColor;
  QStringList         myGradients;
  QIntList            myGradientsIds;
  bool                myTextureAllowed;
  QMap<int, bool>     myTypesAllowed;
  QMap<int, bool>     myTextureTypesAllowed;
  QString             myImageFormats;
  int                 myLastGradient;
};

class QTX_EXPORT QtxBackgroundDialog : public QtxDialog
{
  Q_OBJECT

public:
  QtxBackgroundDialog( QWidget* = 0 );
  QtxBackgroundDialog( const Qtx::BackgroundData&, QWidget* = 0 );
  virtual ~QtxBackgroundDialog();

  void                       setData( const Qtx::BackgroundData& );
  Qtx::BackgroundData        data() const;

  static Qtx::BackgroundData getBackground( const Qtx::BackgroundData& = Qtx::BackgroundData(),
					    QWidget* = 0,
					    bool = true, bool = true, bool = true, bool = true,
					    const QStringList& = QStringList(),
					    const QIntList& = QIntList(),
					    const QString& = QString() );
  static Qtx::BackgroundData getBackground( const Qtx::BackgroundData&,
					    QWidget*,
					    const QIntList&,
					    bool = true, bool = true, bool = true, bool = true,
					    const QStringList& = QStringList(),
					    const QIntList& = QIntList(),
					    const QString& = QString() );


  void                       setGradients( const QStringList&, const QIntList& = QIntList() );
  void                       setModeAllowed( Qtx::BackgroundMode, bool = true );
  void                       setTextureAllowed( bool = true );
  void                       setTextureModeAllowed( Qtx::TextureMode, bool = true );
  void                       setImageFormats( const QString& );

private:
  void                       init();

private:
  QtxBackgroundTool*         myTool;
};

#endif // QTXBACKGROUNDTOOL_H
