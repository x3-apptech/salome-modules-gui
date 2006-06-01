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

#include "QtxPopupMgr.h"
#include "QtxListOfOperations.h"
#include "QtxStdOperations.h"
#include "QtxAction.h"

#include <qpopupmenu.h>
#include <qdatetime.h>


/*!
  \return value of global parameter (depending on whole selection, but not dependending on one object of selection)
  \param str - name of parameter

  By default, it returns count of selected objects ("selcount") and list of parameters ("$<name>")
*/
QtxValue QtxPopupMgr::Selection::globalParam( const QString& str ) const
{
  if( str==selCountParam() )
    return count();

  else if( str[0]==equality() )
  {
    QtxSets::ValueSet set;
    QString par = str.mid( 1 );

    for( int i=0, n=count(); i<n; i++ )
    {
      QtxValue v = param( i, par );
      if( v.isValid() )
	QtxSets::add( set, v );
      else
	return QtxValue();	
    }
    return set;
  }

  else
    return QtxValue();
}

/*!
  \return symbole to detect name of parameter list
*/
QChar QtxPopupMgr::Selection::equality() const
{
  return defEquality();
}

/*!
  \return name of parameter for count of selected objects
*/
QString QtxPopupMgr::Selection::selCountParam() const
{
  return defSelCountParam();
}

/*!
  \return default symbole to detect name of parameter list
*/
QChar QtxPopupMgr::Selection::defEquality()
{
    return '$';
}

/*!
  \return default name of parameter for count of selected objects
*/
QString QtxPopupMgr::Selection::defSelCountParam()
{
    return "selcount";
}



/*!
  \class QtxCacheSelection

  Special selection class, that caches parameter values.
  Every parameter during popup building is calculated only one time,
  although it may be included to many rules. After calculation
  it is stored in internal map
*/

class QtxCacheSelection : public QtxPopupMgr::Selection
{
public:
  QtxCacheSelection( QtxPopupMgr::Selection* );
  virtual ~QtxCacheSelection();

  virtual int      count() const;
  virtual QtxValue param( const int, const QString& ) const;
  virtual QtxValue globalParam( const QString& ) const;

private:
  typedef QMap< QString, QtxValue >  CacheMap;

  QtxPopupMgr::Selection*    mySel;
  CacheMap                   myParamCache;
};

/*!
  Constructor
  \param sel - base selection used for parameter calculation
*/
QtxCacheSelection::QtxCacheSelection( QtxPopupMgr::Selection* sel )
: mySel( sel )
{
}

/*!
  Destructor
*/
QtxCacheSelection::~QtxCacheSelection()
{
}

/*!
  \return count of selected objects
*/
int QtxCacheSelection::count() const
{
  return mySel ? mySel->count() : 0;
}

/*!
  Calculates and caches parameters.
  Already calculated parameters are returned without calculation
  \return parameter value
  \param i - index of selected object
  \param name - name of parameter
*/
QtxValue QtxCacheSelection::param( const int i, const QString& name ) const
{
  QString param_name = name + "#####" + QString::number( i );
  if( myParamCache.contains( param_name ) )
    return myParamCache[ param_name ];
  else
  {
    QtxValue v;
    if( mySel )
      v = mySel->param( i, name );
    if( v.isValid() )
      ( ( CacheMap& )myParamCache ).insert( param_name, v );
    return v;
  }
}

/*!
  Calculates and caches global parameters.
  Already calculated parameters are returned without calculation
  \return parameter value
  \param name - name of parameter
*/
QtxValue QtxCacheSelection::globalParam( const QString& name ) const
{
  if( myParamCache.contains( name ) )
    return myParamCache[ name ];
  else
  {
    QtxValue v;
    if( mySel )
      v = mySel->globalParam( name );
    if( v.isValid() )
      ( ( CacheMap& )myParamCache ).insert( name, v );
    return v;
  }
}




/*!
  Constructor
  \param mgr - popup manager
*/
QtxPopupMgr::Operations::Operations( QtxPopupMgr* mgr )
: QtxStrings(),
  myPopupMgr( mgr )
{
    QStringList aList;
    aList.append( "every" );
    aList.append( "any" );
    aList.append( "onlyone" );
    addOperations( aList );

    myParser = new QtxParser( mgr->myOperations );
}

/*!
  Destructor
  Deletes internal parser
*/
QtxPopupMgr::Operations::~Operations()
{
    delete myParser;
}

/*!
    \return priority of popup operation 'op'.
    \param isBin indicate whether the operation is binary
*/
int QtxPopupMgr::Operations::prior( const QString& op, bool isBin ) const
{
    if( !isBin && ( op=="every" || op=="any" || op=="onlyone" ) )
        return 1;
    else
        return QtxStrings::prior( op, isBin );

}

/*!
    Calculates result of operation
    \return one of error states
    \param op - name of operation
    \param v1 - first operation argument (must be used also to store result)
    \param v2 - second operation argument
*/
QtxParser::Error QtxPopupMgr::Operations::calculate
    ( const QString& op, QtxValue& v1, QtxValue& v2 ) const
{
    int ind = -1;
    if( op=="every" )
        ind = 0;
    else if( op=="any" )
        ind = 1;
    else if( op=="onlyone" )
        ind = 2;

    if( ind>=0 && ind<=2 )
    {
        QString val_name = op + "(" + v2.toString() + ")";
        QtxParser::Error err = QtxParser::OK;

        if( !myValues.contains( val_name ) )
        {
            myParser->setExpr( v2.toString() );
            QStringList params, specific;
            myParser->paramsList( params );

            myParser->clear();
            myPopupMgr->setParams( myParser, specific );

            QtxPopupMgr::Selection* sel = myPopupMgr->myCurrentSelection;

            int global_result = 0;
            if( sel )
                for( int i=0, n=sel->count(); i<n; i++ )
                {
                    QStringList::const_iterator anIt = specific.begin(),
                                                aLast = specific.end();
                    for( ; anIt!=aLast; anIt++ )
                    {
                        QtxValue v = sel->param( i, *anIt );
                        if( v.isValid() )
                            myParser->set( *anIt, v );
                        else
                            return QtxParser::InvalidToken;
                    }

                    QtxValue res = myParser->calculate();
                    err = myParser->lastError();
                    if( err==QtxParser::OK )
                        if( res.type()==QVariant::Bool )
                        {
                            if( res.toBool() )
                                global_result++;
                            if( ind==2 && global_result>1 )
                                break;
                        }
                        else
                            return QtxParser::InvalidResult;
                    else
                        return err;
                }

            QtxValue& vv = ( QtxValue&  )myValues[ val_name ];
            vv = ( ind==0 && global_result==sel->count() ) ||
                 ( ind==1 ) ||
                 ( ind==2 && global_result==1 );
        }

        v2 = myValues[ val_name ];

        return err;
    }
    else
        return QtxStrings::calculate( op, v1, v2 );
}

/*!
  Clears internal map of values
*/
void QtxPopupMgr::Operations::clear()
{
    myValues.clear();
}








/*!
  Constructor
*/
QtxPopupMgr::QtxPopupMgr( QPopupMenu* popup, QObject* parent )
: QtxActionMenuMgr( popup, parent ),
  myCurrentSelection( 0 )
{
    createOperations();
}

/*!
  Destructor
*/
QtxPopupMgr::~QtxPopupMgr()
{
}

/*!
  Creates popup operations instance
*/
void QtxPopupMgr::createOperations()
{
    myOperations = new QtxListOfOperations;
    myOperations->prepend( "logic",   new QtxLogic(),           0 );
    myOperations->prepend( "arithm",  new QtxArithmetics(),    50 );
    myOperations->append( "strings", new QtxStrings(),       100 );
    myOperations->append( "sets",    new QtxSets(),          150 );
    myOperations->append( "custom",  new Operations( this ), 200 );
}

/*!
  Additional version of registerAction
  \param act - action to be registered
  \param visible - rule for visibility state
  \param toggle - rule for toggle on state
  \param id - proposed id (if it is less than 0, then id will be generated automatically)
*/
int QtxPopupMgr::registerAction( QAction* act,
                                 const QString& visible,
                                 const QString& toggle,
                                 const int id )
{
    int _id = QtxActionMenuMgr::registerAction( act, id );
    setRule( _id, visible, true );
    setRule( _id, toggle, false );
    return _id;
}

/*!
  Removes action from internal map
  \param id - action id
*/
void QtxPopupMgr::unRegisterAction( const int id )
{
    QAction* act = action( id );

    myVisibility.remove( act );
    myToggle.remove( act );

    remove( id );
    //QtxActionMenuMgr::unRegisterAction( id );
}

/*!
  \return true if manager has rule for action
  \param act - action
  \param visibility - if it is true, then rule for "visibility" is checked, otherwise - for "toggle"
*/
bool QtxPopupMgr::hasRule( QAction* act, bool visibility ) const
{
    return map( visibility ).contains( act );
}

/*!
  \return true if manager has rule for action
  \param id - action id
  \param visibility - if it is true, then rule for "visibility" is checked, otherwise - for "toggle"
*/
bool QtxPopupMgr::hasRule( const int id, bool visibility ) const
{
    return hasRule( action( id ), visibility );
}

/*!
  Sets new rule for action
  \param act - action
  \param rule - string expression of rule
  \param visibility - if it is true, then rule for "visibility" will be set, otherwise - for "toggle"
*/
void QtxPopupMgr::setRule( QAction* act, const QString& rule, bool visibility )
{
    if( !act || rule.isEmpty() )
        return;

    if( !hasRule( act, visibility ) )
    {
        QtxParser* p = new QtxParser( myOperations, rule );
        if( p->lastError()==QtxParser::OK )
            map( visibility ).insert( act, p );
        else
            delete p;
    }
    else
    {
        QtxParser* p = map( visibility )[ act ];
        p->setExpr( rule );
        if( p->lastError()!=QtxParser::OK )
            p->setExpr( QString() );
    }
}

/*!
  Sets new rule for action
  \param id - action id
  \param rule - string expression of rule
  \param visibility - if it is true, then rule for "visibility" will be set, otherwise - for "toggle"
*/
void QtxPopupMgr::setRule( const int id, const QString& rule, bool visibility )
{
    setRule( action( id ), rule, visibility );
}

/*!
  \return true if parser has finished work without errors
  \param p - parser
*/
bool result( QtxParser* p )
{
    bool res = false;
    if( p )
    {
        QtxValue vv = p->calculate();
        res = p->lastError()==QtxParser::OK &&
            ( ( vv.type()==QVariant::Int && vv.toInt()!=0 ) ||
              ( vv.type()==QVariant::Bool && vv.toBool() ) );
    }
    return res;
}

/*!
  Fills parser parameters with help of Selection::globalParam() method
  \param p - parser
  \param specific - list will be filled with names of parameters depending on selection objects (not global)
*/
void QtxPopupMgr::setParams( QtxParser* p, QStringList& specific ) const
{
    if( !p || !myCurrentSelection )
        return;

    QStringList params;

    p->paramsList( params );
    QStringList::const_iterator anIt = params.begin(),
                                aLast = params.end();
    for( ; anIt!=aLast; anIt++ )
    {
      QtxValue v = myCurrentSelection->globalParam( *anIt );
      if( v.isValid() )
	p->set( *anIt, v );
      else
        specific.append( *anIt );
    }
}

/*!
  \return true if 'v1'<'v2'
  This function can work with many types of values
*/
bool operator<( const QtxValue& v1, const QtxValue& v2 )
{
  QVariant::Type t1 = v1.type(), t2 = v2.type();
  if( t1==t2 )
  {
    switch( t1 )
    {
    case QVariant::Int:
      return v1.toInt() < v2.toInt();
      
    case QVariant::Double:
      return v1.toDouble() < v2.toDouble();

    case QVariant::CString:
    case QVariant::String:
      return v1.toString() < v2.toString();

    case QVariant::StringList:
    case QVariant::List:
    {
      const QValueList<QtxValue>& aList1 = v1.toList(), aList2 = v2.toList();
      QValueList<QtxValue>::const_iterator
	anIt1 = aList1.begin(), aLast1 = aList1.end(),
        anIt2 = aList2.begin(), aLast2 = aList2.end();
      for( ; anIt1!=aLast1 && anIt2!=aLast2; anIt1++, anIt2++ )
	if( (*anIt1)!=(*anIt2) )
	  return (*anIt1)<(*anIt2);

      return anIt1==aLast1 && anIt2!=aLast2;
    }

    default:
      return v1.toString()<v2.toString();
    }
  }
  else
    return t1<t2;
}

/*!
  \return true if rule of action is satisfied on current selection
  \param act - action
  \param visibility - what rule is checked: for visibility(true) or for toggle(false)
*/
bool QtxPopupMgr::isSatisfied( QAction* act, bool visibility ) const
{
  QString menu = act->menuText();

  bool res = false;
  if( !act )
    return res;

  if( hasRule( act, visibility ) )
  {
    QtxParser* p = map( visibility )[ act ];
    QStringList specific;
    p->clear();
    ( ( Operations* )myOperations->operations( "custom" ) )->clear();

    setParams( p, specific );

    QMap<QValueList<QtxValue>,int> aCorteges;
    QValueList<QtxValue> c;

    if( specific.count()>0 )
      if( myCurrentSelection )
      {
	res = false;

	for( int i=0, n=myCurrentSelection->count(); i<n && !res; i++ )
	{
	  QStringList::const_iterator anIt1 = specific.begin(), aLast1 = specific.end();
	  c.clear();
	  for( ; anIt1!=aLast1; anIt1++ )
	    c.append( myCurrentSelection->param( i, *anIt1 ) );
	  aCorteges.insert( c, 0 );
	}
	
	//qDebug( QString( "%1 corteges" ).arg( aCorteges.count() ) );
	QMap<QValueList<QtxValue>,int>::const_iterator anIt = aCorteges.begin(), aLast = aCorteges.end();
	for( ; anIt!=aLast; anIt++ )
	{
	  QStringList::const_iterator anIt1 = specific.begin(), aLast1 = specific.end();
	  const QValueList<QtxValue>& aCortege = anIt.key();
	  QValueList<QtxValue>::const_iterator anIt2 = aCortege.begin();
	  for( ; anIt1!=aLast1; anIt1++, anIt2++ )
	    p->set( *anIt1, *anIt2 );
	  res = res || result( p );
	}

	/*
	for( int i=0, n=myCurrentSelection->count(); i<n && !res; i++ )
	{
	  QStringList::const_iterator anIt1 = specific.begin(), aLast1 = specific.end();
	  for( ; anIt1!=aLast1; anIt1++ )
	    p->set( *anIt1, myCurrentSelection->param( i, *anIt1 ) );
	  res = res || result( p );
	}*/
      }
      else
	res = false;
    else
      res = result( p );
  }

  return res;
}

/*!
  \return true if item corresponding to action is visible
  \param actId - action id
  \param place - index of place
*/
bool QtxPopupMgr::isVisible( const int actId, const int place ) const
{
    bool res = QtxActionMenuMgr::isVisible( actId, place );
    QAction* act = action( actId );
    if( hasRule( act, true ) )
        res = res && isSatisfied( act, true );
    return res;
}

/*!
  Updates popup according to selection
  \param p - popup menu
  \param sel - selection
*/
void QtxPopupMgr::updatePopup( QPopupMenu* p, Selection* sel )
{
  QTime t1 = QTime::currentTime();

  if( !p || !sel )
    return;

  myCurrentSelection = new QtxCacheSelection( sel );
  RulesMap::iterator anIt = myToggle.begin(),
                            aLast = myToggle.end();
  for( ; anIt!=aLast; anIt++ )
    if( anIt.key()->isToggleAction() && hasRule( anIt.key(), false ) )
      anIt.key()->setOn( isSatisfied( anIt.key(), false ) );

  setWidget( ( QWidget* )p );
  updateMenu();
  QTime t2 = QTime::currentTime();
  qDebug( QString( "update popup time = %1 msecs" ).arg( t1.msecsTo( t2 ) ) );
  qDebug( QString( "number of objects = %1" ).arg( myCurrentSelection->count() ) );

  delete myCurrentSelection;
}

/*!
  \return reference to map of rules
  \param visibility - type of map: visibility of toggle
*/
QtxPopupMgr::RulesMap& QtxPopupMgr::map( bool visibility ) const
{
    return ( RulesMap& )( visibility ? myVisibility : myToggle );
}

/*!
  Loads actions description from file
  \param fname - name of file
  \param r - reader of file
  \return true on success
*/
bool QtxPopupMgr::load( const QString& fname, QtxActionMgr::Reader& r )
{
  PopupCreator cr( &r, this );
  return r.read( fname, cr );
}




/*!
  Constructor
  \param r - menu reader
  \param mgr - menu manager
*/
QtxPopupMgr::PopupCreator::PopupCreator( QtxActionMgr::Reader* r,
                                         QtxPopupMgr* mgr )
: QtxActionMgr::Creator( r ),
  myMgr( mgr )
{
}

/*!
  Destructor
*/QtxPopupMgr::PopupCreator::~PopupCreator()
{
}


/*!
  Appends new menu items
  \param tag - tag of item
  \param subMenu - it has submenu
  \param attr - list of attributes
  \param pId - id of action corresponding to parent item
*/
int QtxPopupMgr::PopupCreator::append( const QString& tag, const bool subMenu,
                                       const ItemAttributes& attr, const int pId )
{
  if( !myMgr || !reader() )
    return -1;

  QString label   = reader()->option( "label",     "label"     ),
          id      = reader()->option( "id",        "id"        ),
          pos     = reader()->option( "pos",       "pos"       ),
          group   = reader()->option( "group",     "group"     ),
          tooltip = reader()->option( "tooltip",   "tooltip"   ),
          sep     = reader()->option( "separator", "separator" ),
          accel   = reader()->option( "accel",     "accel"     ),
          icon    = reader()->option( "icon",      "icon"      ),
          toggle  = reader()->option( "toggle",    "toggle"    );

  int res = -1, actId = intValue( attr, id, -1 );;
  if( subMenu )
    res = myMgr->insert( strValue( attr, label ), pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  else if( tag==sep )
    res = myMgr->insert( separator(), pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  else //if( !myMgr->contains( actId ) )
  {
    QPixmap pix; QIconSet set;
    QString name = strValue( attr, icon );
    if( !name.isEmpty() )
    {
      if( loadPixmap( name, pix ) )
        set = QIconSet( pix );
    }

    QString actLabel = strValue( attr, label );
    QtxAction* newAct = new QtxAction( strValue( attr, tooltip ), set, actLabel,
                                       QKeySequence( strValue( attr, accel ) ),
                                       myMgr );
    newAct->setToolTip( strValue( attr, tooltip ) );
    QString toggleact = strValue( attr, toggle );
    bool isToggle = !toggleact.isEmpty();
    newAct->setToggleAction( isToggle );
    newAct->setOn( toggleact.lower()=="true" );
        
    connect( newAct );
    int aid = myMgr->registerAction( newAct, visibleRule( attr ), 
                                     isToggle ? toggleRule( attr ) : QString::null,
                                     actId );
    res = myMgr->insert( aid, pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  }

  return res;
}

/*!
  \return visibility rule by attributes
  Default implementation is empty
*/
QString QtxPopupMgr::PopupCreator::visibleRule( const ItemAttributes& ) const
{
  return QString::null;
}

/*!
  \return toggle rule by attributes
  Default implementation is empty
*/
QString QtxPopupMgr::PopupCreator::toggleRule( const ItemAttributes& ) const
{
  return QString::null;
}
