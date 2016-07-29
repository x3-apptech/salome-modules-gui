// Copyright (C) 2015-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef OCCVIEWER_ENVTEXTUREDLG_H
#define OCCVIEWER_ENVTEXTUREDLG_H

#include "OCCViewer.h"
#include <QDialog>
#include <V3d_View.hxx>
#include <Basics_OCCTVersion.hxx>

class OCCViewer_ViewWindow;
class OCCViewer_ViewFrame;
class QGroupBox;
class QComboBox;
class QLineEdit;

class OCCVIEWER_EXPORT OCCViewer_EnvTextureDlg : public QDialog
{
  Q_OBJECT

  public:
  OCCViewer_EnvTextureDlg( OCCViewer_ViewWindow* );
  ~OCCViewer_EnvTextureDlg();

  static QString getName();

private slots:

  void onEnvTexture( bool );
  void onTextureChanged();
  void onFileSelectionClicked();

  void ClickOnHelp();

private:
  void initParam();
#if OCC_VERSION_LARGE > 0x07000000
  void setEnvTexture( Handle(Graphic3d_TextureEnv) );
#else
  void setEnvTexture( Handle(Graphic3d_TextureEnv), V3d_TypeOfSurfaceDetail );
#endif

  OCCViewer_ViewFrame* myViewFrame;
  Handle(V3d_View) myView3d;

  QGroupBox*   myEnvTextureGroup;
  QComboBox*   myEnvTextureId;
  QLineEdit*   myEnvTextureName;
  QPushButton* myFileButton;
};

#endif // OCCVIEWER_ENVTEXTUREDLG_H
