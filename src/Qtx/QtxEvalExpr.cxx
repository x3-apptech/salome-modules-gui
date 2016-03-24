// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      QtxEvalExpr.cxx
// Author:    Alexander SOLOVYOV, Sergey TELKOV
//
#include "QtxEvalExpr.h"

#include <QStack>

#include <math.h>

/*!
  \class QtxEvalExpr
  \brief String expression evaluator.
*/

/*!
  \brief Constructor.

  The evaluator is initalized by standard operations. Use another constructor with parameter
  \a stdSets = \c false to avoid initialization of evaluator with standard operations.

  \param expr expression to be evaluated
*/
QtxEvalExpr::QtxEvalExpr( const QString& expr )
{
  intialize( true, expr );
}

/*!
  \brief Constructor.
  \param stdSets if \c true, the evaluator is initalized by standard operations
  \param expr expression to be evaluated
*/
QtxEvalExpr::QtxEvalExpr( const bool stdSets, const QString& expr )
{
  intialize( stdSets, expr );
}

/*!
  \brief Destructor.
*/
QtxEvalExpr::~QtxEvalExpr()
{
  delete myParser;
}

/*!
  \brief Initialize the evaluator.
  \param stdSets if \c true, the evaluator is initalized by standard operations
  \param expr expression to be evaluated
*/
void QtxEvalExpr::intialize( const bool stdSets, const QString& expr )
{
  myParser = new QtxEvalParser();
  if ( stdSets )
  {
    myParser->setAutoDeleteOperationSets( true );
    myParser->insertOperationSet( new QtxEvalSetLogic() );
    myParser->insertOperationSet( new QtxEvalSetArithmetic() );
    myParser->insertOperationSet( new QtxEvalSetString() );
    myParser->insertOperationSet( new QtxEvalSetMath() );
    myParser->insertOperationSet( new QtxEvalSetSets() );
    myParser->insertOperationSet( new QtxEvalSetConst() );
  }
  setExpression( expr );
}

/*!
  \brief Evaluate the expression.
  \param expr expression to be evaluated
  \return result of the evaluation
*/
QVariant QtxEvalExpr::calculate( const QString& expr )
{
  if ( !expr.isEmpty() )
    setExpression( expr );
  return myParser->calculate();
}

/*!
  \brief Get the expression.
  \return expression string
*/
QString QtxEvalExpr::expression() const
{
  return myExpr;
}

/*!
  \brief Set the expression.
  \param expr expression string
*/
void QtxEvalExpr::setExpression( const QString& expr )
{
  if ( expr == expression() )
    return;

  myExpr = expr;
  myParser->setExpression( myExpr );
}

/*!
  \brief Get the code of latest parsing error.
  \return the code of the last error
*/
QtxEvalExpr::Error QtxEvalExpr::error() const
{
  return myParser->error();
}

/*!
  \brief Get the expression parser.
  \return expression parser
*/
QtxEvalParser* QtxEvalExpr::parser() const
{
  return myParser;
}

/*!
  \brief Get current set of operations.
  \return operations set
  \sa insertOperationSet(), removeOperationSet(), operationSet()
*/
QList<QtxEvalSet*> QtxEvalExpr::operationSets() const
{
  return myParser->operationSets();
}

/*!
  \brief Install the operation.
  \param operation to be added
  \param idx index in the list at which the operation shoud be inserted
  \sa operationSets(), removeOperationSet(), operationSet()
*/
void QtxEvalExpr::insertOperationSet( QtxEvalSet* set, const int idx )
{
  myParser->insertOperationSet( set, idx );
}

/*!
  \brief Remove the operation.
  \param operation to be removed
  \sa operationSets(), insertOperationSet(), operationSet()
*/
void QtxEvalExpr::removeOperationSet( QtxEvalSet* set )
{
  myParser->removeOperationSet( set );
}

/*!
  \brief Get the operation by name.
  \param name operation name
  \return operation of 0 if not found
  \sa operationSets(), insertOperationSet(), removeOperationSet()
*/
QtxEvalSet* QtxEvalExpr::operationSet( const QString& name ) const
{
  return myParser->operationSet( name );
}

/*!
  \brief Get the 'auto-delete operations' flag value.
  \return \c true if all operations shoud be automatically deleted when the evaluator is destroyed
  \sa setAutoDeleteOperationSets()
*/
bool QtxEvalExpr::autoDeleteOperationSets() const
{
  return myParser->autoDeleteOperationSets();
}

/*!
  \brief Set the 'auto-delete operations' flag value.
  \param on if \c true, all operations shoud be automatically deleted when the evaluator is destroyed
  \sa autoDeleteOperationSets()
*/
void QtxEvalExpr::setAutoDeleteOperationSets( const bool on )
{
  myParser->setAutoDeleteOperationSets( on );
}

/*!
  \class QtxEvalParser
  \brief Expression parser.

  This class provides the functionality to calculate value of the expression using defined set of operations.
  Standard operations (arithmetics, logic, strings, etc) are implemented in the corresponding successors of the
  QtxEvalSet class: QtxEvalSetArithmetic, QtxEvalSetLogic, QtxEvalSetMath, QtxEvalSetString, ...

  The parser allows using parameters with help of methods has(), set(), remove(), value(). It uses
  postfix representation of expressions and uses class QtxEvalSet in order to make certain operation.

  Every instance of parser contains only one postfix - thus, if the expression is changed, the postfix
  must be rebuilt. In order to increase performance of frequent calculations for many of expressions it is 
  recommended to use different instances of the parser for each expression.
*/

/*!
  \brief Constructor.
*/
QtxEvalParser::QtxEvalParser()
: myAutoDel( false )
{
  setError( QtxEvalExpr::OK );
}

/*!
  \brief Destructor.
*/
QtxEvalParser::~QtxEvalParser()
{
  if ( autoDeleteOperationSets() )
    qDeleteAll( mySets );
}

/*!
  \brief Get current operations set.
  \return current operations set
*/
QList<QtxEvalSet*> QtxEvalParser::operationSets() const
{
  return mySets;
}

/*!
  \brief Get the operations set by \a name.
  \param name the name of the operation set
  \return operation set or 0 if not found
*/
QtxEvalSet* QtxEvalParser::operationSet( const QString& name ) const
{
  QtxEvalSet* set = 0;
  for ( SetList::const_iterator it = mySets.begin(); it != mySets.end() && !set; ++it )
  {
    if ( (*it)->name() == name )
      set = *it;
  }
  return set;
}

/*!
  \brief Install the operations set.
  \param operations set to be added
  \param idx index in the list at which the operations set shoud be inserted
  \sa operationSets(), removeOperationSet(), operationSet()
*/
void QtxEvalParser::insertOperationSet( QtxEvalSet* set, const int idx )
{
  if ( mySets.contains( set ) )
    return;

  int index = idx < 0 ? mySets.count() : idx;
  index = qMin( index, mySets.count() );
  mySets.insert( index, set );
}

/*!
  \brief Remove the operations set.
  \param operations set to be removed
  \sa operationSets(), insertOperationSet(), operationSet()
*/
void QtxEvalParser::removeOperationSet( QtxEvalSet* set )
{
  mySets.removeAll( set );
}

/*!
  \brief Get the 'auto-delete operations' flag value.
  \return \c true if all operations shoud be automatically deleted when the parser is destroyed
  \sa setAutoDeleteOperationSets()
*/
bool QtxEvalParser::autoDeleteOperationSets() const
{
  return myAutoDel;
}

/*!
  \brief Set the 'auto-delete operations' flag value.
  \param on if \c true, all operations shoud be automatically deleted when the parser is destroyed
  \sa autoDeleteOperationSets()
*/
void QtxEvalParser::setAutoDeleteOperationSets( const bool on )
{
  myAutoDel = on;
}

/*!
  \brief Search elements of the list as substrings starting from \a offset.
  \param list list of substrings
  \param str string in which the searching is performed
  \param offset starting index for search
  \param matchLen returning matching length of any substring
  \param listind returning index of the found substring in the \a list
  \return position of first found substring inside the \a str or -1 if no matches is found
*/
int QtxEvalParser::search( const QStringList& list, const QString& str,
                           int offset, int& matchLen, int& listind )
{
  int min = -1;
  QStringList::const_iterator anIt = list.begin(), aLast = list.end();
  for ( int ind = 0; anIt != aLast; anIt++, ind++ )
  {
    int pos = str.indexOf( *anIt, offset );
    if ( pos >= 0 && ( min < 0 || min > pos ||
                       ( min == pos && matchLen < (int)(*anIt).length() ) ) )
    {
      min = pos;
      listind = ind;
      matchLen = (*anIt).length();
    }
  }
  if ( min < 0 )
    matchLen = 0;
  return min;
}

/*!
  \brief Get the substring field from the string \a str.
  \param str source string
  \param pos start position of the substring
  \param len length of the substring
  \return substring (leading and trailing spaces are truncated)
*/
QString QtxEvalParser::note( const QString& str, int pos, int len )
{
    return str.mid( pos, len ).trimmed();
}

/*!
  \brief Prepare to the parsing.

  Performs the first step of the parsing:
  - find tokens
  - determine tokens types
  - create unsorted pseudo-postfix (with brackets)

  \param expr string expression
  \param post postfix to be created
  \return \c true on success and \c false if error is found
*/
bool QtxEvalParser::prepare( const QString& expr, Postfix& post )
{
  int pos = 0;
  int len = expr.length();
  QStack<int> aBracketStack;
  QStringList anOpers, anOpenBr, aCloseBr;
  if ( !checkOperations() )
    return false;

  bracketsList( anOpenBr, true );
  bracketsList( aCloseBr, false );
  operationList( anOpers );

  while ( pos < len && error() == QtxEvalExpr::OK )
  {
    PostfixItem item;
    while ( pos < len && expr[pos].isSpace() )
      pos++;
    if ( pos >= len )
      break;

    int mBrLen = 0, mLen = 0, br_ind = -1, op_ind = -1;
    int oPos = search( anOpenBr, expr, pos, mBrLen, br_ind );
    int cPos = oPos == pos ? -1 : search( aCloseBr, expr, pos, mBrLen, br_ind );
    int opPos = search( anOpers, expr, pos, mLen, op_ind );

    if ( expr[pos] == QChar( '\'' ) )
    {
      int vpos = pos + 1;
      while ( vpos < (int)expr.length() && expr[vpos] != QChar( '\'' ) )
        vpos++;

      mLen = vpos - pos + 1;

      int res = createValue( note( expr, pos, mLen ), item.myValue );
      item.myType = res ? Value : Param;
      post.append( item );
      pos = vpos + 1;
      continue;
    }

    if ( oPos == pos )
    {
      aBracketStack.push( br_ind );
      item.myValue = note( expr, pos, mBrLen );
      item.myType = Open;
      post.append( item );
    }
    else if ( cPos == pos )
    {
      if ( aBracketStack.count() == 0 )
      {
        setError( QtxEvalExpr::ExcessClose );
        break;
      }
      if ( br_ind != aBracketStack.top() )
      {
        setError( QtxEvalExpr::BracketsNotMatch );
        break;
      }
      else
      {
        aBracketStack.pop();
        item.myValue = note( expr, pos, mBrLen );
        item.myType = Close;
        post.append( item );
      }
    }
    else
      mBrLen = 0;

    if ( opPos == pos )
    {
      mBrLen = 0;
      item.myValue = note( expr, pos, mLen );
      item.myType = Binary;

      if ( oPos == pos )
        post.insert( post.count() - 1, item );
      else
        post.append( item );
    }
    else
    {
      mLen = 0;
      if ( oPos != pos && cPos != pos )
      {
              int i; 
        for ( i = pos + 1; i < (int)expr.length(); i++ )
        {
          if ( expr[i].isSpace() )
            break;
        }

        int vpos = i;
        if ( oPos >= 0 && oPos < vpos )
          vpos = oPos;
        if ( cPos >= 0 && cPos < vpos )
          vpos = cPos;
        if ( opPos >= 0 && opPos < vpos )
          vpos = opPos;

        while( vpos < (int)expr.length() && 
               ( expr[vpos].isLetter() || expr[vpos].isDigit() || expr[vpos]=='_' ) )
          vpos++;

        mLen = vpos - pos;
        bool res = createValue( note( expr, pos, mLen ), item.myValue );
        item.myType = res ? Value : Param;
        post.append( item );
      }
    }

    pos += mBrLen + mLen;
  }

  //Bracket checking
  int brValue = 0;
  for ( Postfix::iterator anIt = post.begin(); anIt != post.end(); ++anIt )
  {
    if ( (*anIt).myType == Open )
      brValue++;
    else if ( (*anIt).myType == Close )
    {
      if ( brValue > 0 )
        brValue--;
      else
      {
        setError( QtxEvalExpr::ExcessClose );
        break;
      }
    }
  }

  if ( brValue > 0 )
    setError( QtxEvalExpr::CloseExpected );

  return error() == QtxEvalExpr::OK;
}

/*!
  \brief Analyze the operations used.
  
  Second step of the parsing: analyze the types of the operations used in the expression.

  \param post unsorted postfix
  \return \c true on success and \c false if error is found
*/
bool QtxEvalParser::setOperationTypes( Postfix& post )
{
  if ( !checkOperations() )
    return false;

  QStringList anOpen, aClose;
  bracketsList( anOpen, true );
  bracketsList( aClose, false );

  Postfix::iterator aPrev, aNext;
  for ( Postfix::iterator anIt = post.begin(); anIt != post.end(); ++anIt )
  {
    aPrev = anIt;
    aPrev--;
    aNext = anIt;
    aNext++;
    if ( (*anIt).myType != Binary )
      continue;

    if ( ( anIt == post.begin() || (*aPrev).myType == Open ||
           (*aPrev).myType == Pre || (*aPrev).myType == Binary ) &&  aNext != post.end() &&
         ( (*aNext).myType == Value || (*aNext).myType == Param ||
           (*aNext).myType == Open  || (*aNext).myType == Binary ) )
      (*anIt).myType = Pre;
    else if ( anIt != post.begin() && ( (*aPrev).myType == Close || (*aPrev).myType == Param ||
                                        (*aPrev).myType == Value || (*aPrev).myType == Pre ||
                                        (*aPrev).myType == Post || (*aPrev).myType == Binary ) &&
              ( aNext == post.end() || (*aNext).myType == Close ) )
      (*anIt).myType = Post;

    if ( anOpen.contains( (*anIt).myValue.toString() ) )
      (*anIt).myType = Pre;
    else if ( aClose.contains( (*anIt).myValue.toString() ) )
      (*anIt).myType = Post;
  }

  return error() == QtxEvalExpr::OK;
}

/*!
  \brief Get the number of the globar brackets pairs.

  For example, the expression '((2+3))' has 2 global brackets pairs.

  \param post postfix to be checked
  \param f starting position for the search
  \param l last position for the search
  \return number of brackets pairs
*/
int QtxEvalParser::globalBrackets( const QtxEvalParser::Postfix& post, int f, int l )
{
  int i;
  int start_br = 0;
  int fin_br = 0;
  int br = 0;
  int br_num = 0;
  int min_br_num = (l-f+1)*5;
    
  for( i=f; i<=l; i++ )
    if( post[ i ].myType==QtxEvalParser::Open )
      start_br++;
    else
      break;
  for( i=l; i>=f; i-- )
    if( post[ i ].myType==QtxEvalParser::Close )
      fin_br++;
    else
      break;

  br = start_br<fin_br ? start_br : fin_br;
  for( i=f+br; i<=l-br; i++ )
  {
    if( post[i].myType==QtxEvalParser::Open )
      br_num++;
    else if( post[i].myType==QtxEvalParser::Close )
      br_num--;
    if( br_num<min_br_num )
      min_br_num = br_num;
  }
  
  return br+min_br_num;
}

/*!
  \brief Sort the operations in the postfix.

  Third step of parsing: sort the postfix operations in order to convert it to real postfix.

  \param post source postfix
  \param res returning resulting postfix
  \param anOpen list of open brackets
  \param aClose list of close brackets
  \param f start index of postfix for sorting
  \param l last index of postfix for sorting
  \return \c true on success and \c false if error is found
*/
bool QtxEvalParser::sort( const Postfix& post, Postfix& res, const QStringList& anOpen,
                          const QStringList& aClose, int f, int l )
{
  if ( l < f )
    return true;

  if ( f < 0 )
    f = 0;

  if ( l < 0 )
    l = post.count() - 1;

  int br = globalBrackets( post, f, l );
  f += br;
  l -= br;

  if ( f == l && f >= 0 )
    res.append( post[f] );

  if ( l <= f )
    return true;

  if ( !checkOperations() )
    return false;

  int min = -1;
  QIntList argmin;
  QList<PostfixItemType> min_types;

  for ( int i = 0, j = f; j <= l; i++, j++ )
  {
    const PostfixItem& item = post[j];
    PostfixItemType tt = item.myType;
    if ( tt == Binary || tt == Pre || tt == Post )
    {
      int cur_pr = priority( item.myValue.toString(), tt == Binary );
      if ( cur_pr > 0 )
      {
        if ( min < 0 || min >= cur_pr )
        {
          if ( min == cur_pr )
          {
            argmin.append( f + i );
            min_types.append( tt );
          }
          else
          {
            min = cur_pr;
            argmin.clear();
            argmin.append( f + i );
            min_types.clear(); min_types.append( tt );
          }
        }
      }
      else
      {
        setError( QtxEvalExpr::InvalidOperation );
        break;
      }
    }
    else if ( tt == Open )
    {
      QString opBr = item.myValue.toString();
      int ind = anOpen.indexOf( opBr ), brValue = 0;
      while ( j <= l )
      {
        const PostfixItem& anItem = post[j];
        if ( anItem.myType == Open )
          brValue++;

        if ( anItem.myType == Close )
        {
          brValue--;
          QString clBr = anItem.myValue.toString();
          if ( aClose.indexOf( clBr ) == ind && brValue == 0 )
            break;
        }
        i++; j++;
      }

      if ( brValue > 0 )
      {
        setError( QtxEvalExpr::CloseExpected );
          break;
      }
    }
  }

  if ( error() == QtxEvalExpr::OK )
  {
    if ( min >= 0 )
    {
      Postfix one;
      QList<Postfix> parts;
      QIntList::const_iterator anIt = argmin.begin(), aLast = argmin.end();
      bool ok = sort( post, one, anOpen, aClose, f, *anIt - 1 );
      parts.append( one );
      one.clear();
      for ( ; anIt != aLast && ok; anIt++ )
      {
        QIntList::const_iterator aNext = anIt; aNext++;
        ok = sort( post, one, anOpen, aClose, *anIt + 1, aNext == aLast ? l : *aNext - 1 );
        parts.append( one );
        one.clear();
      }

      if ( !ok )
        return false;

      QStack<PostfixItem> aStack;
      QList<Postfix>::const_iterator aPIt = parts.begin();
      QList<PostfixItemType>::const_iterator aTIt = min_types.begin();
      res += (*aPIt);
      aPIt++;
      anIt = argmin.begin();
      for ( ; anIt != aLast; anIt++, aPIt++, aTIt++ )
      {
        if ( *aTIt == Pre )
        {
          if ( anOpen.contains( post[*anIt].myValue.toString() ) == 0 )
          {
            res += (*aPIt);
            aStack.push( post[ *anIt ] );
          }
          else
          {
            res.append( post[*anIt] );
            res += *aPIt;
          }
        }
        else
        {
          res += *aPIt;
          while ( !aStack.isEmpty() )
          {
            res.append( aStack.top() );
            aStack.pop();
          }
          res.append( post[*anIt] );
        }
      }
      while ( !aStack.isEmpty() )
      {
        res.append( aStack.top() );
        aStack.pop();
      }
    }
    else
    { //there are no operations
      for ( int k = f; k <= l; k++ )
      {
        if ( post.at( k ).myType==Value || post.at( k ).myType == Param )
          res.append( post.at( k ) );
      }
    }
  }

  return error() == QtxEvalExpr::OK;
}

/*!
  \brief Parse the expression and build the posfix.

  If by parsing error is found, the function returns \c false. In this case the code of the error
  can be retrieved with error() method.

  \param expr string expression
  \return \c true on success and \c false if error is found
*/
bool QtxEvalParser::parse( const QString& expr )
{
  myPostfix.clear();

  if ( !checkOperations() )
    return false;

  Postfix p;
  QStringList opens, closes;

  setError( QtxEvalExpr::OK );
  bracketsList( opens, true );
  bracketsList( closes, false );

  return prepare( expr, p ) && setOperationTypes( p ) && sort( p, myPostfix, opens, closes );
}

/*!
  \brief Calculate the operation.

  The result of the operation is returned in the parameter \a v1.

  \param op operation name
  \param v1 first argument (not valid for unary prefix operations)
  \param v2 second argument (not valid for unary postfix operations)
  \return \c true on success and \c false if error is found
*/
bool QtxEvalParser::calculate( const QString& op, QVariant& v1, QVariant& v2 )
{
  QtxEvalExpr::Error err = isValid( op, v1.type(), v2.type() );
  if ( err == QtxEvalExpr::OK )
    setError( calculation( op, v1, v2 ) );
  else
    setError( err );

  return error() == QtxEvalExpr::OK;
}

/*!
  \brief Calculate the expression without postfix rebuilding.
  \return QVariant as result (it is invalid if there were errors during calculation)
*/
QVariant QtxEvalParser::calculate()
{
  if ( !checkOperations() )
    return QVariant();

  setError( QtxEvalExpr::OK );

  QStringList anOpen, aClose;
  bracketsList( anOpen, true );
  bracketsList( aClose, false );

  QStack<QVariant> aStack;
  Postfix::iterator anIt = myPostfix.begin(), aLast = myPostfix.end();
  for ( ; anIt != aLast && error() == QtxEvalExpr::OK; anIt++ )
  {
    QString nn = (*anIt).myValue.toString();
    if ( (*anIt).myType == Param )
    {
      if ( hasParameter( nn ) )
      {
        QVariant& v = myParams[nn];
        if ( v.isValid() )
          aStack.push( v );
        else
          setError( QtxEvalExpr::InvalidToken );
      }
      else
        setError( QtxEvalExpr::InvalidToken );
    }
    else if ( (*anIt).myType == Value )
      aStack.push( (*anIt).myValue );
    else if ( (*anIt).myType == Pre || (*anIt).myType == Post )
    {
      if ( anOpen.contains( nn ) )
      {
        QVariant inv;
        if ( calculate( nn, inv, inv ) )
          aStack.push( QVariant() );
      }
      else if ( aClose.contains( nn ) )
      {
        QList<QVariant> set;
        while ( true )
        {
          if ( aStack.isEmpty() )
          {
            setError( QtxEvalExpr::StackUnderflow );
            break;
          }
          if ( aStack.top().isValid() )
          {
            set.append( aStack.top() );
            aStack.pop();
          }
          else
          {
            aStack.pop();
            break;
          }
        }

        QVariant qSet = set, inv;
        if ( calculate( nn, qSet, inv ) )
          aStack.push( set );
      }
      else if ( aStack.count() >= 1 )
      {
        QVariant inv;
        QVariant* v1 = &aStack.top(), *v2 = &inv; //"post-" case
        if ( (*anIt).myType == Pre )
        {
          v2 = &aStack.top();
          v1 = &inv;
        }
        calculate( nn, *v1, *v2 );
      }
      else
        setError( QtxEvalExpr::StackUnderflow );
    }
    else if ( (*anIt).myType == Binary )
    {
      if ( aStack.count() >= 2 )
      {
        QVariant v2 = aStack.top();
        aStack.pop();
        calculate( nn, aStack.top(), v2 );
      }
      else
        setError( QtxEvalExpr::StackUnderflow );
    }
  }

  QVariant res;
  if ( error() == QtxEvalExpr::OK )
  {
    int count = aStack.count();
    if ( count == 0 )
      setError( QtxEvalExpr::StackUnderflow );
    else if( count == 1 )
      res = aStack.top();
    else
      setError( QtxEvalExpr::ExcessData );
  }
  return res;
}

/*!
  \brief Change the expression, rebuild the postfix and calculate it.
  \param expr new expression
  \return QVariant as result (it is invalid if there were errors during calculation)
*/
QVariant QtxEvalParser::calculate( const QString& expr )
{
  setExpression( expr );
  return calculate();
}

/*!
  \brief Change the expression and rebuild the postfix.
  \param expr new expression
  \return \c true on success and \c false if error is found
*/
bool QtxEvalParser::setExpression( const QString& expr )
{
  return parse( expr );
}

/*!
  \brief Check if the parser contains specified parameter.
  \param name parameter name
  \return \c true, if the parser contains parameter
*/
bool QtxEvalParser::hasParameter( const QString& name ) const
{
  return myParams.contains( name.trimmed() );
}

/*!
  \brief Set parameters value.
  \param name parameter name
  \param value parameter value
*/
void QtxEvalParser::setParameter( const QString& name, const QVariant& value )
{
  myParams.insert( name.trimmed(), value );
}

/*!
  \brief Remove parameter.
  \param name parameter name
  \return \c true on success
*/
bool QtxEvalParser::removeParameter( const QString& name )
{
  return myParams.remove( name.trimmed() );
}

/*!
  \brief Get the parameter value.
  \param name parameter name
  \return parameter value or invalud QVariant if there is no such parameter
*/
QVariant QtxEvalParser::parameter( const QString& name ) const
{
  QVariant res;
  if ( myParams.contains( name.trimmed() ) )
    res = myParams[name.trimmed()].toString();
  return res;
}

/*!
  \brief Search first parameter with assigned invalid value.
  \param name used to retrieve the name of the parameter if it is found
  \return \c true if parameter is found
*/
bool QtxEvalParser::firstInvalid( QString& name ) const
{
  for ( ParamMap::const_iterator anIt = myParams.begin(); anIt != myParams.end(); anIt++ )
  {
    if ( !anIt.value().isValid() )
    {
      name = anIt.key();
      return true;
    }
  }
  return false;
}

/*!
  \brief Remove all parameters with assigned invalid values.
*/
void QtxEvalParser::removeInvalids()
{
  QStringList toDelete;
  for ( ParamMap::const_iterator anIt = myParams.begin(); anIt != myParams.end(); anIt++ )
  {
    if ( !anIt.value().isValid() )
      toDelete.append( anIt.key() );
  }

  for ( QStringList::const_iterator aLIt = toDelete.begin(); aLIt != toDelete.end(); aLIt++ )
    myParams.remove( *aLIt );
}

/*!
  \brief Get the code of the latest parsing error.
  \return last error code
*/
QtxEvalExpr::Error QtxEvalParser::error() const
{
  return myError;
}

/*!
  \brief Set the error vode.
  \internal
  \param err error code
*/
void QtxEvalParser::setError( QtxEvalExpr::Error err )
{
  myError = err;
}

/*!
  \brief Dump the current postfix contents to the string.
  \return string representation of the internal parser postfix
*/
QString QtxEvalParser::dump() const
{
  return dump( myPostfix );
}

/*!
  \brief Dump the postfix contents to the string.
  \param post postfix to be dumped
  \return string representation of the postfix
*/
QString QtxEvalParser::dump( const Postfix& post ) const
{
  QString res;

  if ( !checkOperations() )
    return res;

  for ( Postfix::const_iterator anIt = post.begin(); anIt != post.end(); anIt++ )
  {
    if ( (*anIt).myType == Value && (*anIt).myValue.type() == QVariant::String )
      res += "'" + (*anIt).myValue.toString() + "'";
    else
      res += (*anIt).myValue.toString();
    
    if ( (*anIt).myType == Pre )
      res += "(pre)";
    else if ( (*anIt).myType == Post )
      res += "(post)";
    else if ( (*anIt).myType == Binary )
      res += "(bin)";

    res += "_";
  }

  return res;
}

/*!
  \brief Get the list of the parameters names.
  \return parameters names
*/
QStringList QtxEvalParser::parameters() const
{
  QStringList lst;
  for ( Postfix::const_iterator anIt = myPostfix.begin(); anIt != myPostfix.end(); anIt++ )
  {
    if ( (*anIt).myType == Param )
    {
      QString name = (*anIt).myValue.toString();
      if ( !lst.contains( name ) )
        lst.append( name );
    }
  }
  return lst;
}

/*!
  \brief Remove all parameters.
*/
void QtxEvalParser::clearParameters()
{
  myParams.clear();
}

/*!
  \brief Get the string representation for the list of QVariant values.
  \param list list to be converted
  \return string representation for the list
*/
QString QtxEvalParser::toString( const QList<QVariant>& list )
{
  QString res = "set : [ ";
  for ( QList<QVariant>::const_iterator anIt = list.begin(); anIt != list.end(); anIt++ )
    res += (*anIt).toString() + " ";
  res += "]";
  return res;
}

/*!
  \brief Get names of all operations used in the expression.
  \param list returning list of the operations names
*/
void QtxEvalParser::operationList( QStringList& list ) const
{
  for ( SetList::const_iterator it = mySets.begin(); it != mySets.end(); ++it )
  {
    QStringList custom;
    QtxEvalSet* set = *it;
    set->operationList( custom );
    for ( QStringList::const_iterator sIt = custom.begin(); sIt != custom.end(); ++sIt )
    {
      if ( !list.contains( *sIt ) )
        list.append( *sIt );
    }
  }
}

/*!
  \brief Get list of brackets.
  \param list returning list of brackets
  \param open if \c true, collect opening brackets, or closing brackets otherwise
*/
void QtxEvalParser::bracketsList( QStringList& list, bool open ) const
{
  for ( SetList::const_iterator it = mySets.begin(); it != mySets.end(); ++it )
  {
    QStringList custom;
    QtxEvalSet* set = *it;
    set->bracketsList( custom, open );
    for ( QStringList::const_iterator sIt = custom.begin(); sIt != custom.end(); ++sIt )
    {
      if ( !list.contains( *sIt ) )
        list.append( *sIt );
    }
  }
}

/*!
  \brief Create value.
  \param str parsed string
  \param val returning value
  \return \c true on success
*/
bool QtxEvalParser::createValue( const QString& str, QVariant& val ) const
{
  bool ok = false;
  for ( SetList::const_iterator it = mySets.begin(); it != mySets.end() && !ok; ++it )
    ok = (*it)->createValue( str, val );
  return ok;
}

/*!
  \brief Get the operation priority level.
  \param op operation
  \param isBin \c true if the operation is binary and \c false if it is unary
  \return operation priority
*/
int QtxEvalParser::priority( const QString& op, bool isBin ) const
{
  int i = 0;
  int priority = 0;
  for ( SetList::const_iterator it = mySets.begin(); it != mySets.end() && priority <= 0; ++it, i++ )
    priority = (*it)->priority( op, isBin );

  return priority > 0 ? priority + i * 50 : 0;
}

/*!
  \brief Check operation validity.

  If the operation is valid, QtxEvalExpr::OK is returned.

  \param op operation
  \param t1 first operand type
  \param t2 second operand type
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalParser::isValid( const QString& op,
                                           const QVariant::Type t1, const QVariant::Type t2 ) const
{
  QtxEvalExpr::Error err = QtxEvalExpr::OK;
  for ( SetList::const_iterator it = mySets.begin(); it != mySets.end(); ++it )
  {
    err = (*it)->isValid( op, t1, t2 );
    if ( err == QtxEvalExpr::OK )
      break;
  }
  return err;
}

/*!
  \brief Perform calculation
  
  The result of the operation is returned in the parameter \a v1.
  If the operation is calculated correctly, the function returns QtxEvalExpr::OK.

  \param op operation name
  \param v1 first argument (not valid for unary prefix operations)
  \param v2 second argument (not valid for unary postfix operations)
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalParser::calculation( const QString& op, QVariant& v1, QVariant& v2 ) const
{
  QVariant nv1, nv2;
  for ( SetList::const_iterator it = mySets.begin(); it != mySets.end(); ++it )
  {
    nv1 = v1;
    nv2 = v2;
    if ( (*it)->isValid( op, v1.type(), v2.type() ) == QtxEvalExpr::OK )
    {
      QtxEvalExpr::Error err = (*it)->calculate( op, nv1, nv2 );
      if ( err == QtxEvalExpr::OK || err == QtxEvalExpr::InvalidResult )
      {
        v1 = nv1;
        v2 = nv2;
        return err;
      }
    }
  }
  return QtxEvalExpr::InvalidOperation;
}

/*!
  \brief Check current operations set.
  \return \c false if current set of operations is empty
*/
bool QtxEvalParser::checkOperations() const
{
  if ( !mySets.isEmpty() )
    return true;

  QtxEvalParser* that = (QtxEvalParser*)this;
  that->setError( QtxEvalExpr::OperationsNull );
  return false;
}

/*!
  \class QtxEvalSet
  \brief Generic class for all the operations sets used in expressions.
*/

/*!
  \brief Constructor.
*/
QtxEvalSet::QtxEvalSet()
{
}

/*!
  \brief Destructor.
*/
QtxEvalSet::~QtxEvalSet()
{
}

/*!
  \fn void QtxEvalSet::operationList( QStringList& list ) const;
  \brief Get the list of possible operations.
  \param list returning list of operations supported by the class
*/

/*!
  \fn void QtxEvalSet::bracketsList( QStringList& list, bool open ) const;
  \brief Get list of brackets.
  \param list returning list of brackets
  \param open if \c true, collect opening brackets, or closing brackets otherwise
*/

/*!
  \brief Create value from its string representation.

  By default, the string value is set, that corresponds to the parameter.
  Base implementation always returns \c false (it means that string 
  is evaluated to the parameter).
  Successor class can re-implement this method to return \c true 
  if the argument being parsed can be evaluated as custom value.

  \param str string representration of the value
  \param val returning value
  \return \c true if \a str can be evaluated as custom value and \c false
          otherwise (parameter)
*/
bool QtxEvalSet::createValue( const QString& str, QVariant& val ) const
{
  val = str;
  return false;
}

/*!
  \fn int QtxEvalSet::priority( const QString& op, bool isBin ) const;
  \brief Get the operation priority.

  Operation priority counts from 1.
  If the operation is impossible, this function should return value <= 0.
  
  \param op operation
  \param isBin \c true if the operation is binary and \c false if it is unary
  \return operation priority
*/

/*!
  \fn QtxEvalExpr::Error QtxEvalSet::isValid( const QString& op, const QVariant::Type t1, 
                                              const QVariant::Type t2 ) const;
  \brief Check operation validity.

  If the operation is valid, QtxEvalExpr::OK is returned.
  If types of operands are invalid, the function returns QtxEvalExpr::OperandsNotMatch
  or QtxEvalExpr::InvalidOperation.

  \param op operation
  \param t1 first operand type
  \param t2 second operand type
  \return error code (QtxEvalExpr::Error)
*/

/*!
  \fn QtxEvalExpr::Error QtxEvalSet::calculate( const QString& op, QVariant& v1,
                                                QVariant& v2 ) const;
  \brief Calculate the operation.

  Process binary operation with values \a v1 and \a v2.
  For unary operation the \v2 is invalid.
  The result of the operation is returned in the parameter \a v1.

  \param op operation name
  \param v1 first argument (not valid for unary prefix operations)
  \param v2 second argument (not valid for unary postfix operations)
  \return error code (QtxEvalExpr::Error)
*/

/*!
  \fn QString QtxEvalSet::name() const;
  \brief Get unique operations set name.

  Should be redefined in the successor classes.

  \return operations set name
*/

/*!
  \class QtxEvalSetBase
  \brief Generic class. Provides functionality for standard operations sets.
*/

/*!
  \brief Constructor.
*/
QtxEvalSetBase::QtxEvalSetBase()
{
}

/*!
  \brief Destructor.
*/
QtxEvalSetBase::~QtxEvalSetBase()
{
}

/*!
  \brief Get list of brackets.
  \param list returning list of brackets
  \param open if \c true, collect opening brackets, or closing brackets otherwise
*/
void QtxEvalSetBase::bracketsList( QStringList& list, bool open ) const
{
  list.append( open ? "(" : ")" );
}

/*!
  \brief Get the list of possible operations.
  \param list returning list of operations supported by the class
*/
void QtxEvalSetBase::operationList( QStringList& list ) const
{
  list += myOpers;
}

/*!
  \brief Add operation names to the internal list of operations.
  \param list operations to be added
*/
void QtxEvalSetBase::addOperations( const QStringList& list )
{
  for ( QStringList::const_iterator anIt = list.begin(); anIt != list.end(); ++anIt )
  {
    if ( !myOpers.contains( *anIt ) )
      myOpers.append( *anIt );
  }
}

/*!
  \brief Add operand types.
  \param list operand types to be added
*/
void QtxEvalSetBase::addTypes( const ListOfTypes& list )
{
  for ( ListOfTypes::const_iterator anIt = list.begin(); anIt != list.end(); ++anIt )
  {
    if ( !myTypes.contains( *anIt ) )
      myTypes.append( *anIt );
  }
}

/*!
  \brief Check operation validity.

  If the operation is valid, QtxEvalExpr::OK is returned.
  If types of operands are invalid, the function returns QtxEvalExpr::OperandsNotMatch
  or QtxEvalExpr::InvalidOperation.

  \param op operation
  \param t1 first operand type
  \param t2 second operand type
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetBase::isValid( const QString& op,
                                            const QVariant::Type t1, const QVariant::Type t2 ) const
{
  if ( ( t1 == QVariant::Invalid || myTypes.contains( t1 ) ) &&
       ( t2 == QVariant::Invalid || myTypes.contains( t2 ) ) &&
       ( t1 != QVariant::Invalid || t2 != QVariant::Invalid ) )
  {
    if ( priority( op, t1 != QVariant::Invalid && t2 != QVariant::Invalid ) > 0 )
      return QtxEvalExpr::OK;
    else
      return QtxEvalExpr::InvalidOperation;
  }
  else
    return QtxEvalExpr::OperandsNotMatch;
}

/*!
  \class QtxEvalSetArithmetic
  \brief Provides set of arithmetical operations for the parser.
*/

/*!
  \brief Constructor.
*/
QtxEvalSetArithmetic::QtxEvalSetArithmetic()
: QtxEvalSetBase()
{
  addOperations( QString( "+;-;*;/;=;<;>;<=;>=;<>;!=" ).split( ";" ) );

  ListOfTypes aTypes;
  aTypes.append( QVariant::Int );
  aTypes.append( QVariant::UInt );
  aTypes.append( QVariant::Double );
  addTypes( aTypes );
}

/*!
  \brief Destructor.
*/
QtxEvalSetArithmetic::~QtxEvalSetArithmetic()
{
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetArithmetic::Name()
{
  return "Arithmetic";
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetArithmetic::name() const
{
  return Name();
}

/*!
  \brief Create value from its string representation.

  Creates numbers from string representation.

  \param str string representration of the value
  \param val returning value
  \return \c true if \a str can be evaluated as custom value and \c false
          otherwise (parameter)
*/
bool QtxEvalSetArithmetic::createValue( const QString& str, QVariant& val ) const
{
  bool ok = false;
  val = str.toInt( &ok );

  if ( !ok )
  {
    val = str.toDouble( &ok );
    if ( !ok )
      ok = QtxEvalSetBase::createValue( str, val );
  }
  return ok;
}

/*!
  \brief Get the operation priority.

  Operation priority counts from 1.
  If the operation is impossible, this function returns value <= 0.
  
  \param op operation
  \param isBin \c true if the operation is binary and \c false if it is unary
  \return operation priority
*/
int QtxEvalSetArithmetic::priority( const QString& op, bool isBin ) const
{
  if ( isBin )
  {
    if ( op == "<" || op == ">" || op == "=" ||
         op == "<=" || op == ">=" || op == "<>" || op == "!=" )
      return 1;
    else if ( op == "+" || op == "-" )
      return 2;
    else if( op == "*" || op == "/" )
      return 3;
    else
      return 0;
  }
  else if ( op == "+" || op == "-" )
    return 5;
  else
    return 0;
}

/*!
  \brief Calculate the operation.

  Process binary operation with values \a v1 and \a v2.
  For unary operation the \v2 is invalid.
  The result of the operation is returned in the parameter \a v1.

  \param op operation name
  \param v1 first argument (not valid for unary prefix operations)
  \param v2 second argument (not valid for unary postfix operations)
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetArithmetic::calculate( const QString& op, QVariant& v1, QVariant& v2 ) const
{
  QtxEvalExpr::Error err = QtxEvalExpr::OK;

  if ( v1.isValid() && v2.isValid() )
  {
    // binary operations
    if ( ( v1.type() == QVariant::Int || v1.type() == QVariant::UInt ) &&
         ( v2.type() == QVariant::Int || v2.type() == QVariant::UInt ) )
    {
      int _v1 = v1.toInt();
      int _v2 = v2.toInt();

      if ( op == "+" )
        v1 = _v1 + _v2;
      else if ( op == "-" )
        v1 = _v1 - _v2;
      else if ( op == "*" )
        v1 = _v1 * _v2;
      else if ( op == "/" )
      {
        if ( _v2 != 0 )
        {
          if ( _v1 % _v2 == 0 )
            v1 = _v1 / _v2;
          else
            v1 = double( _v1 ) / double( _v2 );
        }
        else
          err = QtxEvalExpr::InvalidResult;
      }
      else if ( op == "<" )
        v1 = _v1 < _v2;
      else if ( op == ">" )
        v1 = _v1 > _v2;
      else if ( op == "=" )
        v1 = _v1 == _v2;
      else if ( op == "<=" )
        v1 = _v1 <= _v2;
      else if ( op == ">=" )
        v1 = _v1 >= _v2;
      else if ( op == "<>" || op == "!=" )
        v1 = _v1 != _v2;
    }
    else if ( ( v1.type() == QVariant::Int || v1.type() == QVariant::Double ) &&
              ( v2.type() == QVariant::Int || v2.type() == QVariant::Double ) )
    {
      double _v1 = v1.toDouble();
      double _v2 = v2.toDouble();

      if ( op == "+" )
        v1 = _v1 + _v2;
      else if ( op == "-" )
        v1 = _v1 - _v2;
      else if ( op == "*" )
          v1 = _v1 * _v2;
      else if ( op == "/" )
      {
        if ( _v2 != 0 )
          v1 = _v1 / _v2;
        else
          err = QtxEvalExpr::InvalidResult;
      }
      else if ( op == "<" )
        v1 = _v1 < _v2;
      else if ( op == ">" )
        v1 = _v1 > _v2;
      else if ( op == "=" )
        v1 = _v1 == _v2;
      else if ( op == "<=" )
        v1 = _v1 <= _v2;
      else if ( op == ">=" )
        v1 = _v1 >= _v2;
      else if ( op == "<>" || op == "!=" )
        v1 = _v1 != _v2;
    }
    else // prefix operations
    {
      if ( op == "-" )
      {
        if ( v2.type() == QVariant::Int )
          v2 = -v2.toInt();
        else if ( v2.type() == QVariant::Double )
          v2 = -v2.toDouble();
      }
    }
  }

  return err;
}

/*!
  \class QtxEvalSetLogic
  \brief Provides set of logical operations for the parser.
*/

/*!
  \brief Constructor.
*/
QtxEvalSetLogic::QtxEvalSetLogic()
: QtxEvalSetBase()
{
  addOperations( QString( "and;&&;or;||;xor;not;!;imp;=" ).split( ";" ) );

  ListOfTypes aTypes;
  aTypes.append( QVariant::Bool );
  aTypes.append( QVariant::Int );
  aTypes.append( QVariant::UInt );
  addTypes( aTypes );
}

/*!
  \brief Destructor.
*/
QtxEvalSetLogic::~QtxEvalSetLogic()
{
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetLogic::Name()
{
  return "Logic";
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetLogic::name() const
{
  return Name();
}

/*!
  \brief Create value from its string representation.

  Create \c true or \c false value from string representation.

  \param str string representration of the value
  \param val returning value
  \return \c true if \a str can be evaluated as custom value and \c false
          otherwise (parameter)
*/
bool QtxEvalSetLogic::createValue( const QString& str, QVariant& val ) const
{
  bool ok = true;
  QString valStr = str.toLower();
  if ( valStr == "true" || valStr == "yes" )
    val = QVariant( true );
  else if ( valStr == "false" || valStr == "no" )
    val = QVariant( false );
  else
    ok = QtxEvalSetBase::createValue( str, val );

  return ok;
}

/*!
  \brief Get the operation priority.

  Operation priority counts from 1.
  If the operation is impossible, this function returns value <= 0.
  
  \param op operation
  \param isBin \c true if the operation is binary and \c false if it is unary
  \return operation priority
*/
int QtxEvalSetLogic::priority( const QString& op, bool isBin ) const
{
  if ( isBin )
  {
    if ( op == "and" || op == "or" || op == "xor" || op == "&&" || op == "||" || op == "imp" )
      return 1;
    else if ( op == "=" )
      return 2;
    else 
      return 0;
  }
  else if ( op == "not" || op == "!" )
    return 5;
  else
    return 0;
}

/*!
  \brief Calculate the operation.

  Process binary operation with values \a v1 and \a v2.
  For unary operation the \v2 is invalid.
  The result of the operation is returned in the parameter \a v1.

  \param op operation name
  \param v1 first argument (not valid for unary prefix operations)
  \param v2 second argument (not valid for unary postfix operations)
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetLogic::calculate( const QString& op, QVariant& v1, QVariant& v2 ) const
{
  QtxEvalExpr::Error err = QtxEvalExpr::OK;
  int val1 = intValue( v1 );
  int val2 = intValue( v2 );
  if ( v1.isValid() && v2.isValid() )
  {
    if ( op == "and" || op == "&&" )
      v1 = val1 && val2;
    else if ( op == "or" || op == "||" )
      v1 = val1 || val2;
    else if ( op == "xor" )
      v1 = ( !val1 && val2 ) || ( val1 && !val2 );
    else if ( op == "imp" )
      v1 = !val1 || val2;
    else if ( op == "=" )
      v1 = val1 == val2;
  }
  else if ( op == "not" || op == "!" )
    v2 = !val2;

  return err;
}

/*!
  \brief Convert value to the integer.

  Note: the value is converted to the integer (not boolean) in order
  to compare integer numbers correctly.

  \param v value being converted
  \return converted value
*/
int QtxEvalSetLogic::intValue( const QVariant& v ) const
{
  int res = 0;
  switch ( v.type() )
  {
  case QVariant::Bool:
    res = v.toBool() ? 1 : 0;
    break;
  case QVariant::Int:
  case QVariant::UInt:
    res = v.toInt();
    break;
  default:
    break;
  }
  return res;
}

/*!
  \class QtxEvalSetMath
  \brief Provides a set of more complex operations (mathematical functions)
         for the parser (sqrt, sin, cos, etc).
*/

/*!
  \brief Constructor.
*/
QtxEvalSetMath::QtxEvalSetMath()
: QtxEvalSetBase()
{
  addOperations( QString( "sqrt;abs;sin;cos;rad2grad;grad2rad" ).split( ";" ) );

  ListOfTypes aTypes;
  aTypes.append( QVariant::Int );
  aTypes.append( QVariant::Double );
  addTypes( aTypes );
}

/*!
  \brief Destructor.
*/
QtxEvalSetMath::~QtxEvalSetMath()
{
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetMath::Name()
{
  return "Math";
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetMath::name() const
{
  return Name();
}

/*!
  \brief Create value from its string representation.

  Creates numbers from string representation.

  \param str string representration of the value
  \param val returning value
  \return \c true if \a str can be evaluated as custom value and \c false
          otherwise (parameter)
*/
bool QtxEvalSetMath::createValue( const QString& str, QVariant& val ) const
{
  bool ok = false;
  val = str.toInt( &ok );

  if ( !ok )
  {
    val = str.toDouble( &ok );
    if ( !ok )
      ok = QtxEvalSetBase::createValue( str, val );
  }
  return ok;
}

/*!
  \brief Get the operation priority.

  Operation priority counts from 1.
  If the operation is impossible, this function returns value <= 0.
  
  \param op operation
  \param isBin \c true if the operation is binary and \c false if it is unary
  \return operation priority
*/
int QtxEvalSetMath::priority( const QString& op, bool isBin ) const
{
  if ( isBin )
    return 0;
  else if ( op == "sqrt" || op == "abs" || op == "sin" ||
            op == "cos" || op == "rad2grad" || op == "grad2rad" )
    return 1;
  else
    return 0;
}

/*!
  \brief Calculate the operation.

  Process binary operation with values \a v1 and \a v2.
  For unary operation the \v2 is invalid.
  The result of the operation is returned in the parameter \a v1.

  \param op operation name
  \param v1 first argument (not valid for unary prefix operations)
  \param v2 second argument (not valid for unary postfix operations)
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetMath::calculate( const QString& op, QVariant& v1, QVariant& v2 ) const
{
  QtxEvalExpr::Error err = QtxEvalExpr::OK;
  double val = v2.toDouble();
  if ( op == "sqrt" )
  {
    if ( val >= 0 )
      v2 = sqrt( val );
    else
      err = QtxEvalExpr::InvalidResult;
  }
  else if ( op == "abs" )
  {
    if ( v2.type() == QVariant::Int )
      v2 = abs( v2.toInt() );
    else 
      v2 = fabs( v2.toDouble() );
  }
  else if ( op == "sin" )
    v2 = sin( val );
  else if ( op == "cos" )
    v2 = cos( val );
  else if ( op == "grad2rad" )
    v2 = val * 3.14159256 / 180.0;
  else if ( op == "rad2grad" )
    v2 = val * 180.0 / 3.14159256;

  return err;
}

/*!
  \class QtxEvalSetString
  \brief Provides set of string operations for the parser.
*/

/*!
  \brief Constructor.
*/
QtxEvalSetString::QtxEvalSetString()
: QtxEvalSetBase()
{
  addOperations( QString( "+;=;<;>;<=;>=;<>;!=;length;lower;upper" ).split( ";" ) );

  ListOfTypes aTypes;
  aTypes.append( QVariant::Int );
  aTypes.append( QVariant::Double );
  aTypes.append( QVariant::String );
  addTypes( aTypes );
}

/*!
  \brief Destructor.
*/
QtxEvalSetString::~QtxEvalSetString()
{
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetString::Name()
{
  return "String";
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetString::name() const
{
  return Name();
}

/*!
  \brief Create value from its string representation.

  Creates string value from Qt string representation.

  \param str string representration of the value
  \param val returning value
  \return \c true if \a str can be evaluated as custom value and \c false
          otherwise (parameter)
*/
bool QtxEvalSetString::createValue( const QString& str, QVariant& val ) const
{
  bool ok = false;
  if ( str.length() > 1 && str[0] == '\'' && str[str.length() - 1] == '\'' )
  {
    val = str.mid( 1, str.length() - 2 );
    ok = true;
  }
  else
    ok = QtxEvalSetBase::createValue( str, val );
  return ok;
}

/*!
  \brief Get the operation priority.

  Operation priority counts from 1.
  If the operation is impossible, this function returns value <= 0.
  
  \param op operation
  \param isBin \c true if the operation is binary and \c false if it is unary
  \return operation priority
*/
int QtxEvalSetString::priority( const QString& op, bool isBin ) const
{
  if ( isBin )
  {
    if ( op == "+" ) 
      return 2;
    else if ( op == "=" || op == "<" || op == ">" ||
              op == "<=" || op == ">=" || op == "<>" || op == "!=" )
      return 1;
    else
      return 0;
  }
  else if ( op == "length" || op == "lower" || op=="upper" )
    return 5;
  else
    return 0;
}

/*!
  \brief Calculate the operation.

  Process binary operation with values \a v1 and \a v2.
  For unary operation the \v2 is invalid.
  The result of the operation is returned in the parameter \a v1.

  \param op operation name
  \param v1 first argument (not valid for unary prefix operations)
  \param v2 second argument (not valid for unary postfix operations)
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetString::calculate( const QString& op, QVariant& v1, QVariant& v2 ) const
{
  QtxEvalExpr::Error err = QtxEvalExpr::OK;
  if ( v1.isValid() && v2.isValid() )
  {
    QString _v1 = v1.toString();
    QString _v2 = v2.toString();
    if ( op == "+" )
      v1 = _v1 + _v2;
    else if ( op == "=" )
      v1 = _v1 ==_v2;
    else if ( op == "<" )
      v1 = _v1 < _v2;
    else if ( op == ">" )
      v1 = _v1 > _v2;
    else if ( op == "<>" || op == "!=" )
      v1 = _v1 != _v2;
    else if ( op == "<=" )
      v1 = _v1 < _v2 || _v1 == _v2;
    else if ( op == ">=" )
      v1 = _v1 > _v2 || _v1 == _v2;
  }
  else if ( !v1.isValid() && v2.isValid() )
  {
    QString val = v2.toString();
    if ( op == "length" )
      v2 = (int)val.length();
    else if ( op == "lower" )
      v2 = val.toLower();
    else if ( op == "upper" )
      v2 = val.toUpper();
  }
  return err;
}

/*!
  \class QtxEvalSetSets
  \brief Provides set of operations with sequences for the parser.
*/

/*!
  \brief Constructor.
*/
QtxEvalSetSets::QtxEvalSetSets()
: QtxEvalSetBase()
{
  addOperations( QString( "{;};=;<>;!=;+;-;*;in;count" ).split( ";" ) );

  ListOfTypes aTypes;
  aTypes.append( QVariant::List );
  addTypes( aTypes );
}

/*!
  \brief Destructor.
*/
QtxEvalSetSets::~QtxEvalSetSets()
{
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetSets::Name()
{
  return "Sets";
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetSets::name() const
{
  return Name();
}

/*!
  \brief Get list of brackets.
  \param list returning list of brackets
  \param open if \c true, collect opening brackets, or closing brackets otherwise
*/
void QtxEvalSetSets::bracketsList( QStringList& list, bool open ) const
{
  list.append( open ? "{" : "}" );
  QtxEvalSetBase::bracketsList( list, open );
}

/*!
  \brief Get the operation priority.

  Operation priority counts from 1.
  If the operation is impossible, this function returns value <= 0.
  
  \param op operation
  \param isBin \c true if the operation is binary and \c false if it is unary
  \return operation priority
*/
int QtxEvalSetSets::priority( const QString& op, bool isBin ) const
{
  if ( isBin )
  {
    if ( op == "=" || op == "<>" || op == "!=" )
      return 1;
    else if ( op == "+" || op == "-" || op == "*" )
      return 2;
    else if ( op == "in" )
      return 3;
    else
      return 0;
  }
  else if ( op == "{" || op == "}" )
    return 5;
  else if ( op == "count" )
    return 4;
  else
    return 0;
}

/*!
  \brief Check operation validity.

  If the operation is valid, QtxEvalExpr::OK is returned.
  If types of operands are invalid, the function returns QtxEvalExpr::OperandsNotMatch
  or QtxEvalExpr::InvalidOperation.

  \param op operation
  \param t1 first operand type
  \param t2 second operand type
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetSets::isValid( const QString& op,
                                            const QVariant::Type t1, const QVariant::Type t2 ) const
{
  if ( op == "{" )
    return QtxEvalExpr::OK;

  if ( op != "in" )
    return QtxEvalSetBase::isValid( op, t1, t2 );

  if ( t1 != QVariant::Invalid && t2 == QVariant::List )
    return QtxEvalExpr::OK;
  else
    return QtxEvalExpr::OperandsNotMatch;
}

/*!
  \brief Add new value \a v to the sequence \a set.
  \param set sequence
  \param v value to be added
*/
void QtxEvalSetSets::add( ValueSet& set, const QVariant& v )
{
  if ( v.isValid() && !set.contains( v ) )
    set.append( v );
}

/*!
  \brief Add all values from sequence \a s2 to the sequence \a s1.
  \param s1 destination sequence
  \param s2 source sequence
*/
void QtxEvalSetSets::add( ValueSet& s1, const ValueSet& s2 )
{
  for ( ValueSet::const_iterator anIt = s2.begin(); anIt != s2.end(); ++anIt )
    add( s1, *anIt );
}

/*!
  \brief Remove value \a v from sequence \a set.
  \param set sequence
  \param v value to be removed
*/
void QtxEvalSetSets::remove( ValueSet& set, const QVariant& v )
{
  set.removeAll( v );
}

/*!
  \brief Remove all values listed in the sequence \a s2 from the sequence \a s1.
  \param s1 sequence from which items are removed
  \param s2 sequence which items are removed
*/
void QtxEvalSetSets::remove( ValueSet& s1, const ValueSet& s2 )
{
  for ( ValueSet::const_iterator anIt = s2.begin(); anIt != s2.end(); ++anIt )
    s1.removeAll( *anIt );
}

/*!
  \brief Calculate the operation.

  Process binary operation with values \a v1 and \a v2.
  For unary operation the \v2 is invalid.
  The result of the operation is returned in the parameter \a v1.

  \param op operation name
  \param v1 first argument (not valid for unary prefix operations)
  \param v2 second argument (not valid for unary postfix operations)
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetSets::calculate( const QString& op, QVariant& v1, QVariant& v2 ) const
{
  QtxEvalExpr::Error err = QtxEvalExpr::OK;

  if ( op != "{" )
  {
    if ( op == "}" )
    {
      ValueSet aNewList;
      add( aNewList, v1.toList() );
      v1 = aNewList;
    }
    else if ( op == "=" || op == "<>" || op == "!=" || op == "+" || op == "-" || op == "*" )
    {
      ValueSet aNewList;
      add( aNewList, v1.toList() );
      if ( op == "=" || op == "<>" || op == "!=" || op == "-" )
      {
        remove( aNewList, v2.toList() );
        if ( op == "=" )
          v1 = aNewList.isEmpty() && v1.toList().count() == v2.toList().count();
        else if ( op == "<>" || op == "!=" )
          v1 = !aNewList.isEmpty() || v1.toList().count() != v2.toList().count();
        else
          v1 = aNewList;
      }
      else if ( op == "+" )
      {
        add( aNewList, v2.toList() );
        v1 = aNewList;
      }
      else if ( op == "*" )
      {
        ValueSet toDelete;
        add( toDelete, aNewList );
        remove( toDelete, v2.toList() );
        remove( aNewList, toDelete );
        v1 = aNewList;
      }
    }
    else if ( op== "count" )
      v2 = (int)v2.toList().count();
    else if ( op == "in" )
    {
      if ( v1.type() == QVariant::List )
      {
        bool res = true;
        ValueSet lst1 = v1.toList();
        ValueSet lst2 = v2.toList();
        for ( ValueSet::const_iterator anIt = lst1.begin(); anIt != lst1.end() && res; ++anIt )
          res = lst2.contains( *anIt );

        v1 = res;
      }
      else
              v1 = QVariant( v2.toList().contains( v1 ) );
    }
  }
  return err;
}

/*!
  \class QtxEvalSetConst
  \brief Provides different standard constants.
*/
QtxEvalSetConst::QtxEvalSetConst()
: QtxEvalSet()
{
}

/*!
  \brief Destructor.
*/
QtxEvalSetConst::~QtxEvalSetConst()
{
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetConst::Name()
{
  return "Const";
}

/*!
  \brief Get operations set name.
  \return operations set name
*/
QString QtxEvalSetConst::name() const
{
  return Name();
}

/*!
  \brief Create value from its string representation.

  Convert constant name to its value.

  \param str string representration of the constant
  \param val returning value
  \return \c true if \a str can be evaluated as custom value and \c false
          otherwise (parameter)
*/
bool QtxEvalSetConst::createValue( const QString& str, QVariant& val ) const
{
  bool ok = true;
  if ( str == "pi" )            // PI number
    val = QVariant( 3.141593 );
  else if ( str == "exp" )      // Exponent value (e)
    val = QVariant( 2.718282 );
  else if ( str == "g" )        // Free fall acceleration (g)
    val = QVariant( 9.80665 );
  else
    ok = false;

  return ok;
}

/*!
  \brief Get the list of possible operations.
  \param list returning list of operations supported by the class (not used)
*/
void QtxEvalSetConst::operationList( QStringList& /*list*/ ) const
{
}

/*!
  \brief Get list of brackets.
  \param list returning list of brackets (not used)
  \param open if \c true, collect opening brackets, or closing brackets otherwise (not used)
*/
void QtxEvalSetConst::bracketsList( QStringList& /*list*/, bool /*open*/ ) const
{
}

/*!
  \brief Get the operation priority.

  Operation priority counts from 1.
  If the operation is impossible, this function returns value <= 0.
  
  \param op operation (not used)
  \param isBin \c true if the operation is binary and \c false if it is unary (not used)
  \return operation priority
*/
int QtxEvalSetConst::priority( const QString& /*op*/, bool /*isBin*/ ) const
{
  return 0;
}

/*!
  \brief Check operation validity.

  Always returns QtxEvalExpr::InvalidOperation.

  \param op operation (not used)
  \param t1 first operand type (not used)
  \param t2 second operand type (not used)
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetConst::isValid( const QString&       /*op*/, 
                                             const QVariant::Type /*t1*/,
                                             const QVariant::Type /*t2*/ ) const
{
  return QtxEvalExpr::InvalidOperation;
}

/*!
  \brief Calculate the operation.

  Always returns QtxEvalExpr::InvalidOperation.

  \param op operation name (not used)
  \param v1 first argument (not valid for unary prefix operations) (not used)
  \param v2 second argument (not valid for unary postfix operations) (not used)
  \return error code (QtxEvalExpr::Error)
*/
QtxEvalExpr::Error QtxEvalSetConst::calculate( const QString&, QVariant&, QVariant& ) const
{
  return QtxEvalExpr::InvalidOperation;
}
