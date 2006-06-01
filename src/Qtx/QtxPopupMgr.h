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

#ifndef __QTX_POPUP_MGR_HEADER__
#define __QTX_POPUP_MGR_HEADER__

#include "Qtx.h"
#include "QtxActionMenuMgr.h"
#include "QtxParser.h"
#include "QtxStdOperations.h"

#include <qmap.h>

class QtxListOfOperations;

class QTX_EXPORT QtxPopupMgr : public QtxActionMenuMgr
{
    Q_OBJECT

public:
    class QTX_EXPORT Selection
    {
    public:
        virtual int      count() const = 0;
        virtual QtxValue param( const int, const QString& ) const = 0;
	virtual QtxValue globalParam( const QString& ) const;

	virtual QChar    equality() const;
	virtual QString  selCountParam() const;

	static QChar    defEquality();
	static QString  defSelCountParam();
    };

protected:
    class Operations : public QtxStrings
    {
    public:
        Operations( QtxPopupMgr* );
        virtual ~Operations();

        virtual int   prior( const QString&, bool isBin ) const;
        virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;

        void clear();

    private:
        QtxPopupMgr*               myPopupMgr;
        QtxParser*                 myParser;
        QMap< QString, QtxValue >  myValues;
    };

    friend class Operations;

protected:
  class PopupCreator;

public:
    QtxPopupMgr( QPopupMenu*, QObject* = 0 );
    virtual ~QtxPopupMgr();

    virtual int  registerAction( QAction*,
                                 const QString& visible,
                                 const QString& toggle = QString::null,
                                 const int = -1 );
    virtual void unRegisterAction( const int );

    virtual bool isVisible( const int actId, const int place ) const;

    bool    hasRule( QAction*, bool visibility ) const;
    bool    hasRule( const int, bool visibility ) const;
    void    setRule( QAction*, const QString&, bool visibility );
    void    setRule( const int, const QString&, bool visibility );
    void    updatePopup( QPopupMenu*, Selection* );

    //return name of parameter corresponding to selected objects count
    //it will be set automatically

    virtual bool load( const QString&, QtxActionMgr::Reader& );

protected:
    typedef QMap< QAction*, QtxParser* > RulesMap;

protected:
    virtual bool      isSatisfied( QAction*, bool visibility ) const;
            void      setParams( QtxParser*, QStringList& ) const;
            RulesMap& map( bool visibility ) const;

    void createOperations();

private:
    RulesMap                 myVisibility, myToggle;
    Selection*               myCurrentSelection;
    QtxListOfOperations*     myOperations;
};



class QtxPopupMgr::PopupCreator : public QtxActionMgr::Creator
{
public:
  PopupCreator( QtxActionMgr::Reader*, QtxPopupMgr* );
  virtual ~PopupCreator();

  virtual int append( const QString&, const bool,
                      const ItemAttributes&, const int );

  virtual QString visibleRule( const ItemAttributes& ) const;
  virtual QString toggleRule( const ItemAttributes& ) const;

private:
  QtxPopupMgr* myMgr;
};


#endif
