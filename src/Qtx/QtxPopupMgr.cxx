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

// File:      QtxPopupMgr.cxx
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#include "QtxPopupMgr.h"
#include "QtxAction.h"
#include "QtxEvalExpr.h"
#include <QList>
#include <QMenu>
#include <QVariant>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
 
/*!
  \brief Used for comparing of two QVariant values.
  \param v1 first argument for comparison
  \param v2 second argument for comparison
  \return \c true if \a v1 less than \a v2
*/
bool operator<( const QVariant& v1, const QVariant& v2 )
{
  QVariant::Type t1 = v1.type(), t2 = v2.type();
  if ( t1 == t2 )
  {
    switch( t1 )
    {
    case QVariant::Int:
      return v1.toInt() < v2.toInt();
    case QVariant::Double:
      return v1.toDouble() < v2.toDouble();
    case QVariant::String:
      return v1.toString() < v2.toString();
    case QVariant::StringList:
    case QVariant::List:
    {
      const QList<QVariant>& aList1 = v1.toList(), aList2 = v2.toList();
      QList<QVariant>::const_iterator anIt1 = aList1.begin(), aLast1 = aList1.end(),
        anIt2 = aList2.begin(), aLast2 = aList2.end();
      for ( ; anIt1 != aLast1 && anIt2 != aLast2;  anIt1++, anIt2++ )
      {
        if ( (*anIt1) != (*anIt2) )
          return (*anIt1) < (*anIt2);
      }
      return anIt1 == aLast1 && anIt2 != aLast2;
    }
    default:
      return v1.toString() < v2.toString();
    }
  }
  return t1 < t2;
}

#else

bool operator<( const QList<QVariant>& v1, const QList<QVariant>& v2 )
{
  QList<QVariant>::const_iterator anIt1 = v1.begin(), aLast1 = v1.end(),
    anIt2 = v2.begin(), aLast2 = v2.end();
  for ( ; anIt1 != aLast1 && anIt2 != aLast2;  anIt1++, anIt2++ )
  {
    if ( (*anIt1) != (*anIt2) )
      return (*anIt1) < (*anIt2);
  }
  return anIt1 == aLast1 && anIt2 != aLast2;
}

#endif // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)

/*!
  \class QtxPopupMgr::PopupCreator
  \internal
  \brief Popup menu actions creator.

  Used by Reader to create actions by reading descriptions from the file
  and fill in the action manager with the actions.
*/

class QtxPopupMgr::PopupCreator : public QtxActionMgr::Creator
{
public:
  PopupCreator( QtxActionMgr::Reader*, QtxPopupMgr* );
  virtual ~PopupCreator();

  virtual int     append( const QString&, const bool,
                          const ItemAttributes&, const int );

  virtual QString rule( const ItemAttributes&, 
                        const QtxPopupMgr::RuleType = VisibleRule ) const;

private:
  QtxPopupMgr*    myMgr;
};

/*!
  \brief Constructor.
  \param r menu action reader
  \param mgr popup menu manager
*/
QtxPopupMgr::PopupCreator::PopupCreator( QtxActionMgr::Reader* r,
                                         QtxPopupMgr* mgr )
: QtxActionMgr::Creator( r ),
  myMgr( mgr )
{
}

/*!
  \brief Destructor.
*/
QtxPopupMgr::PopupCreator::~PopupCreator()
{
}

/*!
  \brief Create and append new action to the action manager.
  \param tag item tag name
  \param subMenu \c true if this item is submenu
  \param attr attributes map
  \param pId parent action ID
  \return menu action ID
*/
int QtxPopupMgr::PopupCreator::append( const QString& tag, const bool subMenu,
                                       const ItemAttributes& attr, const int pId )
{
  if ( !myMgr || !reader() )
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

  QtxActionMenuMgr* mgr = myMgr;

  int res = -1, actId = intValue( attr, id, -1 );;
  if ( subMenu )
    res = mgr->insert( strValue( attr, label ), pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  else if ( tag == sep )
    res = mgr->insert( separator(), pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  else
  {
    QIcon set;
    QPixmap pix;
    QString name = strValue( attr, icon );
    if( !name.isEmpty() )
    {
      if ( loadPixmap( name, pix ) )
        set = QIcon( pix );
    }

    QString actLabel = strValue( attr, label );
    QtxAction* newAct = new QtxAction( strValue( attr, tooltip ), set, actLabel,
                                       QKeySequence( strValue( attr, accel ) ),
                                       myMgr );
    newAct->setToolTip( strValue( attr, tooltip ) );
    QString toggleact = strValue( attr, toggle );
    bool isToggle = !toggleact.isEmpty();
    newAct->setCheckable( isToggle );
    newAct->setChecked( toggleact.toLower() == "true" );
        
    connect( newAct );
    int aid = mgr->registerAction( newAct, actId ); 
    QString arule = rule( attr, QtxPopupMgr::VisibleRule );
    if ( !arule.isEmpty() )
      myMgr->setRule( newAct, arule, QtxPopupMgr::VisibleRule );
    arule = rule( attr, QtxPopupMgr::EnableRule );
    if ( !arule.isEmpty() )
      myMgr->setRule( newAct, arule, QtxPopupMgr::EnableRule );
    arule = rule( attr, QtxPopupMgr::ToggleRule );
    if ( isToggle && !arule.isEmpty() )
      myMgr->setRule( newAct, arule, QtxPopupMgr::ToggleRule );
    res = mgr->insert( aid, pId, intValue( attr, group, 0 ), intValue( attr, pos, -1 ) );
  }

  return res;
}

/*!
  \brief Get the rule for the menu item.

  Default implementation returns empty rule.

  \param attr attributes map
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return rule for the menu item corresponding to the rule type
*/
QString QtxPopupMgr::PopupCreator::rule( const ItemAttributes& /*attr*/, 
                                         const QtxPopupMgr::RuleType /*ruleType*/ ) const
{
  return QString();
}

/*!
  \class QtxPopupMgr
  \brief Popup menu manager.

  Menu manager allows using of set of action for automatic generating of
  application context popup menu by reuquest and dynamic update of its
  contents.

  Use insert() methods to add menu items to the popup menu.

  The visibility, enable and toggle state of the menu item is controlled
  by the syntaxic rules, which can be set with setRule() methods.
  The rules are parsed automatically with help of QtxEvalParser class.

  QtxPopupSelection class is used as back-end for getting value of each
  parameter found in the rule by the expression parser.
  Use setSelection() and selection() to set/get the selection instance
  for the popup menu manager.
  
  Popup menu manager automatically optimizes the menu by removing 
  extra separators, hiding empty popup submenus etc.
*/

/*!
  \brief Constructor.
  \param object parent object
*/
QtxPopupMgr::QtxPopupMgr( QObject* parent )
: QtxActionMenuMgr( 0, parent ),
  mySelection( 0 )
{
}

/*!
  \brief Constructor.
  \param popup popup menu
  \param object parent object
*/
QtxPopupMgr::QtxPopupMgr( QMenu* popup, QObject* parent )
: QtxActionMenuMgr( popup, parent ),
  mySelection( 0 )
{
}

/*!
  \brief Destructor.
*/
QtxPopupMgr::~QtxPopupMgr()
{
}

/*!
  \brief Get popup menu.
  \return popup menu
*/
QMenu* QtxPopupMgr::menu() const
{
  return ::qobject_cast<QMenu*>( menuWidget() );
}

/*!
  \brief Get popup menu.
  \param menu popup menu
*/
void QtxPopupMgr::setMenu( QMenu* menu )
{
  setMenuWidget( menu );
}

/*!
  \brief Get selection.
  \return current selection object
*/
QtxPopupSelection* QtxPopupMgr::selection() const
{
  return mySelection;
}

/*!
  \brief Set selection.
  \param sel new selection object
*/
void QtxPopupMgr::setSelection( QtxPopupSelection* sel )
{
  if ( mySelection == sel )
    return;

  delete mySelection;

  mySelection = sel;

  if ( mySelection ) {
    mySelection->setParent( this );
    mySelection->setPopupMgr( this );
  }

  connect( mySelection, SIGNAL( destroyed( QObject* ) ), 
           this,        SLOT( onSelectionDestroyed( QObject* ) ) );

  QtxActionMgr::triggerUpdate();
}

/*!
  \brief Register an action and return its identifier.

  If \a id is less than 0, the identifier for the action is generated automatically.
  If action with given \a id is already registered, it will be re-registered.
  If required \a id is already in use, new identifier is generatied; in this case
  returning value will different from required one.

  \param act action to be registered
  \param id action ID
  \param rule syntax rule
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return action ID (the same as \a id or generated one)
*/
int QtxPopupMgr::registerAction( QAction* act, const int id, const QString& rule, const QtxPopupMgr::RuleType ruleType )
{
  int _id = QtxActionMenuMgr::registerAction( act, id );
  setRule( act, rule, ruleType );
  return _id;
}

/*!
  \brief Unregister action from internal map.
  \param id action ID
*/
void QtxPopupMgr::unRegisterAction( const int id )
{
  QAction* a = action( id );
  if ( myRules.contains( a ) )
  {
    for ( ExprMap::iterator it = myRules[a].begin(); it != myRules[a].end(); ++it )
      delete it.value();
  }
  myRules.remove( a );

  remove( id );

  QtxActionMenuMgr::unRegisterAction( id );
}

/*!
  \brief Insert action to the popup menu manager.
  \param id action ID
  \param pId parent menu action ID
  \param rule syntax rule
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return action ID
*/
int QtxPopupMgr::insertAction( const int id, const int pId, const QString& rule, const RuleType ruleType )
{
  int res = QtxActionMenuMgr::insert( id, pId, -1 );
  setRule( action( id ), rule, ruleType );
  return res;
}

/*!
  \brief Insert action to the popup menu manager.
  \param a action
  \param pId parent menu action ID
  \param rule syntax rule
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return action ID
*/
int QtxPopupMgr::insertAction( QAction* a, const int pId, const QString& rule, const RuleType ruleType )
{
  int res = QtxActionMenuMgr::insert( a, pId, -1 );
  setRule( a, rule, ruleType );
  return res;
}

/*!
  \return true if action has rule of given type
  \param a - action
  \param t - rule type
*/
bool QtxPopupMgr::hasRule( QAction* a, const RuleType t ) const
{
  return a ? expression( a, t, false )!=0 : false;
}

/*!
  \return true if action with given id has rule of given type
  \param id - action id
  \param t - rule type
*/
bool QtxPopupMgr::hasRule( const int id, const RuleType t ) const
{
  return hasRule( action( id ), t );
}

/*!
  \brief Get rule of type \a type for the action \a a.
  \param a action
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return rule of required type
*/
QString QtxPopupMgr::rule( QAction* a, const RuleType ruleType ) const
{
  QString rule;
  QtxEvalExpr* expr = expression( a, ruleType );
  if ( expr )
    rule = expr->expression();
  return rule;
}

/*!
  \brief Get rule of type \a type for the action \a id.
  \param id action ID
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return rule of required type
*/
QString QtxPopupMgr::rule( const int id, const RuleType ruleType ) const
{
  return rule( action( id ), ruleType );
}

/*!
  \brief Set rule of type \a type for the action \a a.
  \param a action
  \param rule rule
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return rule of required type
*/
void QtxPopupMgr::setRule( QAction* a, const QString& rule, const RuleType ruleType )
{
  if ( !a )
    return;

  QtxEvalExpr* expr = expression( a, ruleType, true );

  expr->setExpression( rule );
}

/*!
  \brief Set rule of type \a type for the action \a id.
  \param id action ID
  \param rule rule
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return rule of required type
*/
void QtxPopupMgr::setRule( const int id, const QString& rule, const RuleType ruleType )
{
  setRule( action( id ), rule, ruleType );
}

/*!
  \brief Calculate an expression.
  \param p expression parser
  \return \c true if parser has finished work without errors
*/
bool QtxPopupMgr::result( QtxEvalParser* p ) const
{
  bool res = false;
  if ( p )
  {
    QVariant vv = p->calculate();
    res = p->error() == QtxEvalExpr::OK &&
          ( ( vv.type() == QVariant::Int && vv.toInt() != 0 ) ||
            ( vv.type() == QVariant::Bool && vv.toBool() ) );
  }
  return res;
}

/*!
  \brief Fill the parser with parameters of the expression.

  The values of the parameters are given from the selection object
  (QtxPopupSelection).
  
  \param p expression parser
  \param returning list of parameters names which are not retrieved from the selection
  \sa selection()
*/
void QtxPopupMgr::setParameters( QtxEvalParser* p, QStringList& specific ) const
{
  if ( !p || !mySelection )
    return;

  QStringList params = p->parameters();
  for ( QStringList::const_iterator it = params.begin(); it != params.end(); ++it )
  {
    QVariant v = parameter( *it );
    if ( v.isValid() )
      p->setParameter( *it, v );
    else
      specific.append( *it );
  }
}

/*!
  \brief Check the rule for the action.
  \param act action
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \return \c true if current selection satisfies the action rule
*/
bool QtxPopupMgr::isSatisfied( QAction* act, const RuleType ruleType ) const
{
  if ( !act )
    return false;

  QtxEvalExpr* exp = expression( act, ruleType );
  if ( !exp )
    return true;

  bool res = false;

  QtxEvalParser* p = exp->parser();

  QStringList specific;
  p->clearParameters();
  setParameters( p, specific );

  QMap<QList<QVariant>, int> aCorteges;
  if ( !specific.isEmpty() )
  {
    if ( mySelection )
    {
      res = false;
      for ( int i = 0; i < mySelection->count() && !res; i++ )
      {
        QList<QVariant> c;
        for ( QStringList::const_iterator anIt1 = specific.begin(); anIt1 != specific.end(); ++anIt1 )
          c.append( parameter( *anIt1, i ) );
        aCorteges.insert( c, 0 );
      }
      for ( QMap<QList<QVariant>, int>::const_iterator anIt = aCorteges.begin(); anIt  != aCorteges.end(); ++anIt )
      {
        const QList<QVariant>& aCortege = anIt.key();
        QStringList::const_iterator anIt1 = specific.begin(), aLast1 = specific.end();
        QList<QVariant>::const_iterator anIt2 = aCortege.begin();
        for ( ; anIt1 != aLast1; anIt1++, anIt2++ )
          p->setParameter( *anIt1, *anIt2 );
        res = res || result( p );
      }
    }
    else
      res = false;
  }
  else
    res = result( p );

  return res;
}

/*!
  \brief Check if the menu item is visible.
  \param id action ID
  \param place some parent action ID
  \return \c true if the action is visible
*/
bool QtxPopupMgr::isVisible( const int id, const int place ) const
{
  return QtxActionMenuMgr::isVisible( id, place ) && ( !hasRule( id ) || isSatisfied( action( id ) ) );
}

/*!
  \brief Perform internal update of the popup menu according 
  to the current selection.
*/
void QtxPopupMgr::internalUpdate()
{
  myCache.clear();

  for ( RuleMap::iterator it = myRules.begin(); it != myRules.end(); ++it )
  {
    ExprMap& map = it.value();
    if ( it.key()->isCheckable() && map.contains( ToggleRule ) &&
         !map[ToggleRule]->expression().isEmpty() )
      it.key()->setChecked( isSatisfied( it.key(), ToggleRule ) );
  }

  QtxActionMenuMgr::internalUpdate();

  myCache.clear();
}

/*!
  \brief Update popup according to the current selection.
*/
void QtxPopupMgr::updateMenu()
{
  internalUpdate();
}

/*!
  \brief Get an syntax expression for the action according to the specified rule type.
  \param a action
  \param ruleType rule type (QtxPopupMgr::RuleType)
  \param create if \c true an expression does not exist, create it
  \return syntax expression
*/
QtxEvalExpr* QtxPopupMgr::expression( QAction* a, const RuleType ruleType, const bool create ) const
{
  QtxEvalExpr* res = 0;

  QtxPopupMgr* that = (QtxPopupMgr*)this;
  RuleMap& ruleMap = that->myRules;
  if ( !ruleMap.contains( a ) && create )
    ruleMap.insert( a, ExprMap() );

  if ( ruleMap.contains( a ) )
  {
    ExprMap& exprMap = ruleMap[a];
    if ( exprMap.contains( ruleType ) )
      res = exprMap[ruleType];
    else if ( create )
      exprMap.insert( ruleType, res = new QtxEvalExpr() );
  }

  return res;
}

/*!
  \brief Load actions description from the file.
  \param fname file name
  \param r action reader
  \return \c true on success and \c false on error
*/
bool QtxPopupMgr::load( const QString& fname, QtxActionMgr::Reader& r )
{
  PopupCreator cr( &r, this );
  return r.read( fname, cr );
}

/*
  \brief Get the specified parameter value.
  \param name parameter name
  \param idx additional index used when used parameters with same names 
  \return parameter value
  \sa selection()
*/
QVariant QtxPopupMgr::parameter( const QString& name, const int idx ) const
{
  QVariant val;
  QString cacheName = name + ( idx >= 0 ? QString( "_%1" ).arg( idx ) : QString() );
  if ( myCache.contains( cacheName ) )
    val = myCache[cacheName];
  else
  {
    if ( selection() )
      val = idx < 0 ? selection()->parameter( name ) : 
                      selection()->parameter( idx, name );
    if ( val.isValid() )
    {
      QtxPopupMgr* that = (QtxPopupMgr*)this;
      that->myCache.insert( cacheName, val );
    }
  }
  return val;
}

/*!
  \brief Called when selection is destroyed.
  
  Prevents crashes when the selection object is destroyed outside the
  popup manager.

  \param o selection object being destroyed
*/
void QtxPopupMgr::onSelectionDestroyed( QObject* o )
{
  if ( o == mySelection )
    mySelection = 0;
}

/*!
  \class QtxPopupSelection
  \brief This class is a part of the popup menu management system. 

  The QtxPopupSelection class is used as back-end for getting value
  of each parameter found in the rule by the expression parser.
  
  For example, it can be used for the analyzing of the currently 
  selected objects and defining the values of the parameters used
  in the rules syntax expression. Rules, in their turn, define
  each action state - visibility, enabled and toggled state.
*/

/*!
  \brief Constructor.
*/
QtxPopupSelection::QtxPopupSelection()
  : QObject( 0 ),
    myPopupMgr( 0 )
{
}

/*!
  \brief Destructor.
*/
QtxPopupSelection::~QtxPopupSelection()
{
}

/*!
  \brief Get an option value.
  \param optName option name
  \return option value or empty string if option is not found
*/
QString QtxPopupSelection::option( const QString& optName ) const
{
  QString opt;
  if ( myOptions.contains( optName ) )
    opt = myOptions[optName];
  return opt;
}

/*!
  \brief Set an option value.
  \param optName option name
  \param opt option value
*/
void QtxPopupSelection::setOption( const QString& optName, const QString& opt )
{
  myOptions.insert( optName, opt );
}

QtxPopupMgr* QtxPopupSelection::popupMgr() const
{
  return myPopupMgr;
}

void QtxPopupSelection::setPopupMgr( QtxPopupMgr* pm )
{
  myPopupMgr = pm;
}

/*!
  \brief Get the parameter value.
  \param str parameter name
  \return parameter value
*/
QVariant QtxPopupSelection::parameter( const QString& str ) const
{
  if ( str == selCountParam() )
    return count();
  else if ( str.startsWith( equalityParam() ) )
  {
    QtxEvalSetSets::ValueSet set;
    QString par = str.mid( equalityParam().length() );

    QtxPopupMgr* pMgr = popupMgr();
    for ( int i = 0; i < (int)count(); i++ )
    {
      QVariant v = pMgr ? pMgr->parameter( par, i ) : parameter( i, par );
      if ( v.isValid() )
        QtxEvalSetSets::add( set, v );
      else
        return QVariant();
    }
    return set;
  }
  else
    return QVariant();
}

/*!
  \brief Get symbol which detects the name of the parameter list.
  \return equality symbol (by default, "$")
*/
QString QtxPopupSelection::equalityParam() const
{
  QString str = option( "equality" );
  if ( str.isEmpty() )
    str = "$";
  return str;
}

/*!
  \brief Get name of the parameter, specifing number of selected objects
  \return parameter name (by default, "selcount")
*/
QString QtxPopupSelection::selCountParam() const
{
  QString str = option( "selcount" );
  if ( str.isEmpty() )
    str = "selcount";
  return str;
}

/*!
  \fn int QtxPopupSelection::count() const;
  \brief Get number of the selected objects.
  \return nb of selected objects
*/

/*!
  \fn QVariant QtxPopupSelection::parameter( const int idx, const QString& name ) const;
  \brief Get value of the parameter which is of list type
  \param idx parameter index
  \param name parameter name
  \return parameter value
*/
