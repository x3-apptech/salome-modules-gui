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

//  SALOME TOOLSGUI : implementation of desktop "Tools" optioins
//  File   : ToolsGUI_CatalogGeneratorDlg.h
//  Author : Nicolas REJNERI
//
#ifndef TOOLSGUI_CATALOGGENERATORDLG_H
#define TOOLSGUI_CATALOGGENERATORDLG_H

#include "ToolsGUI.h"

#include <QDialog>

class QLineEdit;
class QPushButton;

class TOOLSGUI_EXPORT ToolsGUI_CatalogGeneratorDlg : public QDialog
{ 
    Q_OBJECT

public:
    ToolsGUI_CatalogGeneratorDlg( QWidget* parent = 0 );
    ~ToolsGUI_CatalogGeneratorDlg();

    QString getIdlPath();
    QString getIdlFile();
    QString getXmlFile();
    QString getPngFile();
    QString getAuthor();
    QString getVersion();
    QString getCompName();
    QString getCompUserName();
    QString getCompType();
    QString getCompMultiStd();

protected slots:
    void    onBrowseBtnClicked();
    void    updateButtonState();
    void    onApply();

private:
    QLineEdit*     myIdlEdit;
    QLineEdit*     myXmlEdit;
    QLineEdit*     myPngEdit;
    QLineEdit*     myVersionEdit;
    QLineEdit*     myAuthorEdit;
    QLineEdit*     myCompName;
    QLineEdit*     myCompUserName;
    QLineEdit*     myCompType;
    QLineEdit*     myCompMultiStd;
    QPushButton*   myBrowseIdlBtn;
    QPushButton*   myBrowseXmlBtn;
    QPushButton*   myBrowsePngBtn;

    QPushButton*   myApplyBtn;
    QPushButton*   myCloseBtn;
};

#endif // TOOLSGUI_CATALOGGENERATORDLG_H
