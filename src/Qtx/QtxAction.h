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
// File:      QtxAction.hxx
// Author:    Sergey TELKOV

#ifndef QTXACTION_H
#define QTXACTION_H

#include "Qtx.h"

#include <qaction.h>
#include <qmap.h>

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class QTX_EXPORT QtxAction : public QAction
{
    Q_OBJECT

public:
    QtxAction( QObject* = 0, const char* = 0, bool = false );
    QtxAction( const QString&, const QString&, int, QObject*, const char* = 0, bool = false );
    QtxAction( const QString&, const QIconSet&, const QString&, int, QObject*, const char* = 0, bool = false );
    virtual ~QtxAction();

    virtual bool addTo( QWidget* );
    virtual bool addTo( QWidget*, const int );
    virtual bool removeFrom( QWidget* );

protected:
    void         setPopup( QWidget*, const int, QPopupMenu* ) const;

private:
    QMap<QWidget*,int> myMenuIds;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
