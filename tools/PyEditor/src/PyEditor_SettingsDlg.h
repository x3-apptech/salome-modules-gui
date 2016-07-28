// Copyright (C) 2015-2016  OPEN CASCADE
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
// File   : PyEditor_SettingsDlg.h
// Author : Maxim GLIBIN, Open CASCADE S.A.S. (maxim.glibin@opencascade.com)
//

#ifndef PYEDITOR_SETTINGSDLG_H
#define PYEDITOR_SETTINGSDLG_H

#include <QDialog>
#include "PyEditor.h"

class PyEditor_Editor;
class QCheckBox;
class QComboBox;
class QFontComboBox;
class QLabel;
class QSpinBox;

class PYEDITOR_EXPORT PyEditor_SettingsDlg : public QDialog
{
  Q_OBJECT

public:
  PyEditor_SettingsDlg( PyEditor_Editor*, bool = false, QWidget* = 0 );
  ~PyEditor_SettingsDlg();

private Q_SLOTS:
  void onVerticalEdgeChecked();
  void onFontChanged();
  void onOk();
  void onDefault();
  void onHelp();

Q_SIGNALS:
  void help();

private:
  void settingsToGui();
  void settingsFromGui();
  void setFontSize( const QString& );
  
  QCheckBox*        myHighlightCurrentLine;
  QCheckBox*        myTextWrapping;
  QCheckBox*        myCenterCursorOnScroll;
  QCheckBox*        myLineNumberArea;

  QCheckBox*        myTabSpaceVisible;
  QSpinBox*         myTabSize;

  QCheckBox*        myVerticalEdge;
  QSpinBox*         myNumberColumns;
  QLabel*           myNumberColumnsLbl;

  QFontComboBox*    myFontFamily;
  QComboBox*        myFontSize;

  QCheckBox*        myDefaultCheck;

  PyEditor_Editor*  myEditor;
};

#endif // PYEDITOR_SETTINGSDLG_H
