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

#ifndef OCCVIEWER_CREATERESTOREVIEWDLG_H
#define OCCVIEWER_CREATERESTOREVIEWDLG_H

#include "OCCViewer.h"

#include "OCCViewer_ViewWindow.h"

#include <QtxDialog.h>
#include <SUIT_Application.h>

#include <QDialog>
#include <QListWidget>

class OCCViewer_ViewPort3d;

class QListWidgetItem;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_CreateRestoreViewDlg : public QDialog
{
        Q_OBJECT

public:
        OCCViewer_CreateRestoreViewDlg( QWidget*, OCCViewer_ViewWindow* );
        virtual ~OCCViewer_CreateRestoreViewDlg();

        const viewAspectList&         parameters() const;
        viewAspect                    currentItem() const;

public slots:
        void                          OKpressed();
        void                          clearList();
        void                          editItemText( QListWidgetItem* );
        void                          changeImage( QListWidgetItem* );
        void                          deleteSelectedItems();

signals:
        void                          dlgOk();

private:
        QListWidget*                  myListBox;
        OCCViewer_ViewPort3d*         myCurViewPort;
        viewAspect                    myCurrentItem;
        viewAspectList                myParametersMap;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
