// Copyright (C) 2015 OPEN CASCADE
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

class PyEditor_Editor;
class QAbstractButton;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QFontComboBox;
class QLabel;
class QSpinBox;

class PyEditor_SettingsDlg : public QDialog
{
  Q_OBJECT

public:
  explicit PyEditor_SettingsDlg( PyEditor_Editor*, QWidget* = 0 );

  void    setCurrentFont( const QFont& );
  QFont   currentFont() const;

  void    setFontSize( const int );
  int     fontSize() const;

  void    setFontFamily( const QString& );
  QString fontFamily() const;

  bool    isSetAsDefault();

public Q_SLOTS:
  void onClick( QAbstractButton* );

private Q_SLOTS:
  void onVerticalEdgeChecked( bool );
  void onFontChanged( const QFont& );

private:
  void settingsToGui();
  void settingsFromGui();
  void setSettings();
  
  QCheckBox*        w_HighlightCurrentLine;
  QCheckBox*        w_TextWrapping;
  QCheckBox*        w_CenterCursorOnScroll;
  QCheckBox*        w_LineNumberArea;

  QCheckBox*        w_TabSpaceVisible;
  QSpinBox*         w_TabSize;

  QCheckBox*        w_VerticalEdge;
  QSpinBox*         w_NumberColumns;
  QLabel*           lbl_NumColumns;

  QFontComboBox*    w_FontFamily;
  QComboBox*        w_FontSize;

  QCheckBox*        w_DefaultCheck;

  QDialogButtonBox* w_ButtonBox;

  PyEditor_Editor*  my_Editor;
};

#endif // PYEDITOR_SETTINGSDLG_H
