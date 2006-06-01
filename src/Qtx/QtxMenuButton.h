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
// File:      QtxMenuButton.h
// Author:    Sergey TELKOV

#ifndef QTXMENUBUTTON_H
#define QTXMENUBUTTON_H

#include "Qtx.h"

#include <qpushbutton.h>

class QIconSet;
class QPopupMenu;

class QTX_EXPORT QtxMenuButton : public QPushButton
{
    Q_OBJECT

    class PopupMenu;

public:
    enum { Left, Right, Top, Bottom };

public:
    QtxMenuButton( int, QWidget* = 0, const char* = 0 );
	QtxMenuButton( const QString&, QWidget* = 0, const char* = 0 );
	QtxMenuButton( int, const QString&, QWidget* = 0, const char* = 0 );
	QtxMenuButton( QWidget* = 0, const char* = 0 );
	virtual ~QtxMenuButton();

	int           position() const;
	void          setPosition( const int );

    bool          isAlignWidth() const;
    void          setAlignWidth( const bool );

	bool          isArrowEnabled() const;
	void          setArrowEnabled( const bool );

	void          clear();
	void          removeItem( int );
	int           insertSeparator( int = -1 );
	int           insertItem( const QString&, int = -1, int = -1 );
	int           insertItem( const QIconSet&, const QString&, int = -1, int = -1 );

	virtual QSize sizeHint() const;
	virtual QSize minimumSizeHint() const;

signals:
	void          activated( int );

private slots:
	void          onShowPopup();

protected:
    QPopupMenu*   popup() const;
	virtual bool  event( QEvent* );
	virtual void  resizeEvent( QResizeEvent* );
	virtual void  drawButtonLabel( QPainter* );

private:
    void          initialize();

private:
	int           myPos;
	bool          myArrow;
    bool          myAlign;
	QPopupMenu*   myPopup;
};

#endif
