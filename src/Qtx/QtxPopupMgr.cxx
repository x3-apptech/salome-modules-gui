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
// See http://www.salome-platform.org/
//

#include "QtxPopupMgr.h"
#include "QtxListOfOperations.h"
#include "QtxStdOperations.h"
#include "QtxAction.h"

#include <qpopupmenu.h>
#include <qdatetime.h>

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QChar QtxPopupMgr::Selection::equality() const
{
  return defEquality();
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QString QtxPopupMgr::Selection::selCountParam() const
{
  return defSelCountParam();
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QChar QtxPopupMgr::Selection::defEquality()
{
    return '$';
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QString QtxPopupMgr::Selection::defSelCountParam()
{
    return "selcount";
}





//================================================================
// Class : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxCacheSelection::QtxCacheSelection( QtxPopupMgr::Selection* sel )
: mySel( sel )
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxCacheSelection::~QtxCacheSelection()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
int QtxCacheSelection::count() const
{
  return mySel ? mySel->count() : 0;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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





//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxPopupMgr::Operations::~Operations()
{
    delete myParser;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
int QtxPopupMgr::Operations::prior( const QString& op, bool isBin ) const
{
    if( !isBin && ( op=="every" || op=="any" || op=="onlyone" ) )
        return 1;
    else
        return QtxStrings::prior( op, isBin );

}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxPopupMgr::Operations::clear()
{
    myValues.clear();
}









//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxPopupMgr::QtxPopupMgr( QPopupMenu* popup, QObject* parent )
: QtxActionMenuMgr( popup, parent ),
  myCurrentSelection( 0 )
{
    createOperations();
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxPopupMgr::~QtxPopupMgr()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxPopupMgr::createOperations()
{
    myOperations = new QtxListOfOperations;
    myOperations->prepend( "logic",   new QtxLogic(),           0 );
    myOperations->prepend( "arithm",  new QtxArithmetics(),    50 );
    myOperations->append( "strings", new QtxStrings(),       100 );
    myOperations->append( "sets",    new QtxSets(),          150 );
    myOperations->append( "custom",  new Operations( this ), 200 );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxPopupMgr::unRegisterAction( const int id )
{
    QAction* act = action( id );

    myVisibility.remove( act );
    myToggle.remove( act );

    remove( id );
    //QtxActionMenuMgr::unRegisterAction( id );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
bool QtxPopupMgr::hasRule( QAction* act, bool visibility ) const
{
    return map( visibility ).contains( act );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
bool QtxPopupMgr::hasRule( const int id, bool visibility ) const
{
    return hasRule( action( id ), visibility );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxPopupMgr::setRule( const int id, const QString& rule, bool visibility )
{
    setRule( action( id ), rule, visibility );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
bool QtxPopupMgr::isVisible( const int actId, const int place ) const
{
    bool res = QtxActionMenuMgr::isVisible( actId, place );
    QAction* act = action( actId );
    if( hasRule( act, true ) )
        res = res && isSatisfied( act, true );
    return res;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxPopupMgr::RulesMap& QtxPopupMgr::map( bool visibility ) const
{
    return ( RulesMap& )( visibility ? myVisibility : myToggle );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
bool QtxPopupMgr::load( const QString& fname, QtxActionMgr::Reader& r )
{
  PopupCreator cr( &r, this );
  return r.read( fname, cr );
}




//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxPopupMgr::PopupCreator::PopupCreator( QtxActionMgr::Reader* r,
                                         QtxPopupMgr* mgr )
: QtxActionMgr::Creator( r ),
  myMgr( mgr )
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxPopupMgr::PopupCreator::~PopupCreator()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QString QtxPopupMgr::PopupCreator::visibleRule( const ItemAttributes& ) const
{
  return QString::null;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QString QtxPopupMgr::PopupCreator::toggleRule( const ItemAttributes& ) const
{
  return QString::null;
}
