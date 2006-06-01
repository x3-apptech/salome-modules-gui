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

#include "QtxParser.h"
#include "QtxOperations.h"

/*!
  Constructor
*/
QtxParser::QtxParser( QtxOperations* operations, const QString& expr )
: myOperations( operations )
{
    if( myOperations )
    {
        setLastError( OK );
        setExpr( expr );
    }
    else
        setLastError( OperationsNull );
}

/*!
  Destructor
*/
QtxParser::~QtxParser()
{
}

/*!
  Search elements of list as substrings starting on 'offset'
  \returns the least position of substrings inside string
  \param list - list of substrings
  \param str - string where search
  \param offset - starting index for search
  \param matchLen - the length of appropriate substring
  \param listind - list index of appropriate substring
*/
int QtxParser::search( const QStringList& list, const QString& str, int offset,
                       int& matchLen, int& listind )
{
    QStringList::const_iterator anIt = list.begin(), aLast = list.end();
    int min = -1;
    for( int ind = 0; anIt!=aLast; anIt++, ind++ )
    {
      //const char* where = str.latin1(), *what = (*anIt).latin1();
        int pos = str.find( *anIt, offset );
        if( pos>=0 && ( min < 0 || min > pos ||
                        ( min==pos && matchLen< (int)(*anIt).length() ) ) )
        {
            min = pos;
            listind = ind;
            matchLen = (*anIt).length();
        }
    }
    if( min<0 )
        matchLen = 0;
    return min;
}

/*!
  \return substring
  \param str - string
  \param pos - start position of substring
  \param len - length of substring
*/
QString QtxParser::note( const QString& str, int pos, int len )
{
    return str.mid( pos, len ).stripWhiteSpace();
}

/*!
  First step of parsing: finding tokens, determining its types and creating of unsorted pseudo-postfix (with brackets)
  \param expr - string expression
  \param post - postfix to be created
*/
bool QtxParser::prepare( const QString& expr, Postfix& post )
{
    int pos = 0, len = expr.length();
    QValueStack< int > aBracketStack;
    QStringList anOpers, anOpenBr, aCloseBr;
    if( myOperations )
    {
        myOperations->bracketsList( anOpenBr, true );
        myOperations->bracketsList( aCloseBr, false );
        myOperations->opersList( anOpers );
    }
    else
    {
        setLastError( OperationsNull );
        return false;
    }

    while( pos < len && lastError()==OK )
    {
        PostfixItem item;
        while( expr[ pos ].isSpace() && pos<len ) pos++;
        if( pos>=len )
            break;

        int mBrLen = 0, mLen = 0, br_ind = -1, op_ind = -1;
        int oPos = search( anOpenBr, expr, pos, mBrLen, br_ind ),
            cPos = oPos==pos ? -1 : search( aCloseBr, expr, pos, mBrLen, br_ind ),
            opPos = search( anOpers, expr, pos, mLen, op_ind );

        if( expr[ pos ]=="'" )
        {
            int vpos = pos+1;
            while ( vpos< (int)expr.length() && expr[ vpos ]!="'" )
                vpos++;

            mLen = vpos-pos+1;

            int res = myOperations->createValue( note( expr, pos, mLen ), item.myValue );
            item.myType = res ? Value : Param;
            post.append( item );
            pos = vpos+1;
            continue;
        }

        if( oPos==pos )
        {
            aBracketStack.push( br_ind );
            item.myValue = note( expr, pos, mBrLen );
            item.myType = Open;
            post.append( item );
        }

        else if( cPos==pos )
        {
            if( aBracketStack.count()==0 )
            {
                setLastError( ExcessClose );
                break;
            }
            if( br_ind!=aBracketStack.top() )
            {
                setLastError( BracketsNotMatch );
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

        if( opPos==pos )
        {
            mBrLen = 0;
            item.myValue = note( expr, pos, mLen );
            item.myType = Binary;
            //the type is set by default;
            //the method setOperationTypes will set correct types

            if( oPos==pos )
                post.insert( post.at( post.count()-1 ), item );
            else
                post.append( item );
        }
        else
        {
            mLen = 0;
            if( oPos!=pos && cPos!=pos )
            {
	        int i; 
                for( i=pos+1; i<(int)expr.length(); i++ )
                    if( expr[ i ].isSpace() )
                        break;

                int vpos = i;
                if( oPos>=0 && oPos<vpos )
                    vpos = oPos;
                if( cPos>=0 && cPos<vpos )
                    vpos = cPos;
                if( opPos>=0 && opPos<vpos )
                    vpos = opPos;

                while( vpos<(int)expr.length() && 
                       ( expr[vpos].isLetter() || expr[vpos].isDigit() || expr[vpos]=='_' ) )
                    vpos++;

                mLen = vpos-pos;
                bool res = myOperations->createValue( note( expr, pos, mLen ), item.myValue );
                item.myType = res ? Value : Param;
                post.append( item );
            }
        }

        pos+=mBrLen+mLen;
    }

    //Bracket checking
    PostfixIterator anIt = post.begin(),
                    aLast = post.end();
    int brValue = 0;
    for( ; anIt!=aLast; anIt++ )
        if( (*anIt).myType==Open )
            brValue++;
        else if( (*anIt).myType==Close )
            if( brValue>0 )
                brValue--;
            else
            {
                setLastError( ExcessClose );
                break;
            }
    if( brValue>0 )
        setLastError( CloseExpected );

    return lastError()==OK;
}

/*!
  Second step of parsing: determining types of operations
  \param post - unsorted postfix
*/
bool QtxParser::setOperationTypes( Postfix& post )
{
    Postfix::iterator aStart = post.begin(),
                      aLast = post.end(),
                      anIt = aStart, aPrev, aNext;
    QStringList anOpen, aClose;
    if( myOperations )
    {
        myOperations->bracketsList( anOpen, true );
        myOperations->bracketsList( aClose, false );
    }
    else
        return false;

    for( ; anIt!=aLast; anIt++ )
    {
        aPrev = anIt; aPrev--;
        aNext = anIt; aNext++;
        if( (*anIt).myType != Binary )
            continue;

        if( ( anIt==aStart || (*aPrev).myType == Open ||
                              (*aPrev).myType == Pre ||
                              (*aPrev).myType == Binary )
            && 
            aNext!=aLast && ( (*aNext).myType == Value ||
                              (*aNext).myType == Param ||
                              (*aNext).myType == Open  ||
                              (*aNext).myType == Binary ) )
            (*anIt).myType = Pre;

        else if( anIt!=aStart && ( (*aPrev).myType == Close ||
                                   (*aPrev).myType == Param ||
                                   (*aPrev).myType == Value ||
                                   (*aPrev).myType == Pre ||
                                   (*aPrev).myType == Post ||
                                   (*aPrev).myType == Binary )
                 &&
                 ( aNext==aLast || (*aNext).myType == Close ) )
            (*anIt).myType = Post;

        if( anOpen.contains( ( *anIt ).myValue.toString() )>0 )
            (*anIt).myType = Pre;
        else if( aClose.contains( ( *anIt ).myValue.toString() )>0 )
            (*anIt).myType = Post;
    }

    return lastError()==OK;
}

/*!
  \return how many global brackets there is (for example '((2+3))' has 2 global brackets)
  \param post - postfix to be checked
  \param f - start index to search
  \param l - last index to search
*/
int QtxParser::globalBrackets( const QtxParser::Postfix& post, int f, int l )
{
    int i,
        start_br = 0,
        fin_br = 0,
        br = 0,
        br_num = 0,
        min_br_num = (l-f+1)*5;
    
    for( i=f; i<=l; i++ )
        if( post[ i ].myType==QtxParser::Open )
            start_br++;
        else
            break;
    for( i=l; i>=f; i-- )
        if( post[ i ].myType==QtxParser::Close )
            fin_br++;
        else
            break;

    br = start_br<fin_br ? start_br : fin_br;
    for( i=f+br; i<=l-br; i++ )
    {
        if( post[i].myType==QtxParser::Open )
            br_num++;
        else if( post[i].myType==QtxParser::Close )
            br_num--;
        if( br_num<min_br_num )
            min_br_num = br_num;
    }

    return br+min_br_num;
}

/*!
  Third step of parsing: sorting of postfix in order to convert it to real postfix
  \param post - source postfix
  \param res - destination postfix
  \param anOpen - list of open brackets
  \param aClose - list of close brackets
  \param f - start index of postfix to sorting
  \param l - last index of postfix to sorting
*/
bool QtxParser::sort( const Postfix& post, Postfix& res,
                      const QStringList& anOpen,
                      const QStringList& aClose,
                      int f, int l )
{
    if( l<f )
        return true;

    if( f<0 )
        f = 0;
    if( l<0 )
        l = post.count()-1;

    int br = globalBrackets( post, f, l );
    f+=br; l-=br;

    if( f==l && f>=0 )
        res.append( post[ f ] );
    if( l<=f )
        return true;

    if( myOperations )
    {
        int min = -1;
        QIntList argmin;
        QValueList< PostfixItemType > min_types;

        //Find operation with minimal priority
        //PostfixIterator anIt = post.at( f ),
        //                aLast = post.at( l+1 );
        for( int i=0, j=f; j<=l; i++, j++ )
        {
            const PostfixItem& item = post[ j ];
            PostfixItemType tt = item.myType;
            if( tt==Binary || tt==Pre || tt==Post )
            {
                int cur_pr = myOperations->prior( item.myValue.toString(), tt==Binary );
                if( cur_pr>0 )
                {
                    if( min<0 || min>=cur_pr )
                    {
                        if( min==cur_pr )
                        {
                            argmin.append( f+i );
                            min_types.append( tt );
                        }
                        else
                        {
                            min = cur_pr;
                            argmin.clear(); argmin.append( f+i );
                            min_types.clear(); min_types.append( tt );
                        }
                    }
                }
                else
                {
                    setLastError( InvalidOperation );
                    break;
                }
            }
            else if( tt==Open )
            {
                QString opBr = item.myValue.toString();
                int ind = anOpen.findIndex( opBr ), brValue = 0;
                while( j<=l )
                {
                    const PostfixItem& anItem = post[ j ];
                    if( anItem.myType==Open )
                        brValue++;

                    if( anItem.myType==Close )
                    {
                        brValue--;
                        QString clBr = anItem.myValue.toString();
                        if( aClose.findIndex( clBr )==ind && brValue==0 )
                            break;
                    }
                    i++; j++;
                }
                if( brValue>0 )
                {
                    setLastError( CloseExpected );
                    break;
                }
            }
        }

        if( lastError()==OK )
            if( min>=0 )
            {
                QValueList< Postfix > parts;
                QIntList::const_iterator anIt = argmin.begin(),
                                         aLast = argmin.end();
                Postfix one;
                bool ok = sort( post, one, anOpen, aClose, f, *anIt - 1 );
                parts.append( one );
                one.clear();
                for( ; anIt!=aLast && ok; anIt++ )
                {
                    QIntList::const_iterator aNext = anIt; aNext++;
                    ok = sort( post, one, anOpen, aClose, *anIt + 1, aNext==aLast ? l : *aNext - 1 );
                    parts.append( one );
                    one.clear();
                }
                if( !ok )
                    return false;

                QValueList< Postfix >::const_iterator aPIt = parts.begin();
                QValueList< PostfixItemType >::const_iterator aTIt = min_types.begin();
                QValueStack< PostfixItem > aStack;
                res += (*aPIt); aPIt++;
                anIt = argmin.begin();
                for( ; anIt!=aLast; anIt++, aPIt++, aTIt++ )
                {
                    if( *aTIt==Pre )
                        if( anOpen.contains( post[ *anIt ].myValue.toString() )==0 )
                        {
                            res+=(*aPIt);
                            aStack.push( post[ *anIt ] );
                        }
                        else
                        {
                            res.append( post[ *anIt ] );
                            res+=(*aPIt);
                        }
                    else
                    {
                        res+=(*aPIt);
                        while( !aStack.isEmpty() )
                        {
                            res.append( aStack.top() );
                            aStack.pop();
                        }
                        res.append( post[ *anIt ] );
                    }
                }
                while( !aStack.isEmpty() )
                {
                    res.append( aStack.top() );
                    aStack.pop();
                }
            }
            else
            { //there are no operations
                PostfixIterator anIt = post.at( f ),
                                aLast = post.at( l+1 );
                for( ; anIt!=aLast; anIt++ )
                    if( (*anIt).myType==Value || (*anIt).myType==Param )
                        res.append( *anIt );
            }
    }
    else 
        setLastError( OperationsNull );

    return lastError()==OK;
}

/*!
  Build posfix by expression
  \param expr - string expression
*/
bool QtxParser::parse( const QString& expr )
{
    myPost.clear();

    Postfix p;
    QStringList opens, closes;

    if( myOperations )
    {
        setLastError( OK );
        myOperations->bracketsList( opens, true );
        myOperations->bracketsList( closes, false );
    }
    else
    {
        setLastError( OperationsNull );
        return false;
    }

    //return prepare( expr, myPost ) && setOperationTypes( myPost );
    return prepare( expr, p ) && setOperationTypes( p ) && 
           sort( p, myPost, opens, closes );
}

/*!
  Calculate operation
  \param op - operation name
  \param v1 - first argument (it is not valid for unary prefix operations and it is used to store result)
  \param v2 - second argument (it is not valid for unary postfix operations)
*/
bool QtxParser::calculate( const QString& op, QtxValue& v1, QtxValue& v2 )
{
    Error err = myOperations->isValid( op, v1.type(), v2.type() );
    if( err==OK )
        setLastError( myOperations->calculate( op, v1, v2 ) );
    else
        setLastError( err );

    return lastError()==OK;
}

/*!
  Calculates expression without postfix rebuilding
  \return QtxValue as result (it is invalid if there were errors during calculation)
*/
QtxValue QtxParser::calculate()
{
    setLastError( OK );

    QStringList anOpen, aClose;
    if( myOperations )
    {
        myOperations->bracketsList( anOpen, true );
        myOperations->bracketsList( aClose, false );
    }
    else
    {
        setLastError( OperationsNull );
        return QtxValue();
    }

    QtxValueStack aStack;
    PostfixIterator anIt = myPost.begin(),
                    aLast = myPost.end();
    for( ; anIt!=aLast && lastError()==OK; anIt++ )
    {
        QString nn = (*anIt).myValue.toString();
        if( (*anIt).myType==Param )
        {
            if( has( nn ) )
            {
                QVariant& v = myParameters[ nn ];
                if( v.isValid() )
                    aStack.push( v );
                else
                    setLastError( InvalidToken );
            }
            else
                setLastError( InvalidToken );
        }

        else if( (*anIt).myType==Value )
            aStack.push( (*anIt).myValue );

        else if( (*anIt).myType==Pre || (*anIt).myType==Post )
        {
            if( anOpen.contains( nn )>0 )
            {
                QtxValue inv;
                if( calculate( nn, inv, inv ) )
                    aStack.push( QtxValue() );
            }
            else if( aClose.contains( nn )>0 )
            {
                QValueList< QtxValue > set;
                while( true )
                {
                    if( aStack.count()==0 )
                    {
                        setLastError( StackUnderflow );
                        break;
                    }
                    if( aStack.top().isValid() )
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

                QtxValue qSet = set, inv;
                if( calculate( nn, qSet, inv ) )
                    aStack.push( set );
            }
            else if( aStack.count()>=1 )
            {
                QtxValue inv;
                QtxValue* v1 = &aStack.top(), *v2 = &inv; //"post-" case
                if( (*anIt).myType==Pre )
                {
                    v2 = &aStack.top(); v1 = &inv;
                }

                calculate( nn, *v1, *v2 );
            }
            else
                setLastError( StackUnderflow );
        }

        else if( (*anIt).myType==Binary )
        {
            if( aStack.count()>=2 )
            {
                QVariant v2 = aStack.top(); aStack.pop();
                calculate( nn, aStack.top(), v2 );
            }
            else
                setLastError( StackUnderflow );
        }
    }

    QtxValue res;
    if( lastError()==OK )
    {
        int count = aStack.count();
        if( count==0 )
            setLastError( StackUnderflow );
        else if( count==1 )
            res = aStack.top();
        else
            setLastError( ExcessData );
    }
    return res;
}

/*!
  Change expression, rebuild postfix and calculate it
  \return QtxValue as result (it is invalid if there were errors during calculation)
*/
QtxValue QtxParser::calculate( const QString& expr )
{
    setExpr( expr );
    return calculate();
}

/*!
  Change expression and rebuild postfix
*/
bool QtxParser::setExpr( const QString& expr )
{
    return parse( expr );
}

/*!
  \return true, if parser contain parameter
  \param name - name of parameter
*/
bool QtxParser::has( const QString& name ) const
{
    return myParameters.contains( name.stripWhiteSpace() );
}

/*!
  Sets parameter value
  \param name - name of parameter
  \param value - value of parameter
*/
void QtxParser::set( const QString& name, const QtxValue& value )
{
    myParameters[ name.stripWhiteSpace() ] = value;
}

/*!
  Removes parameter
  \param name - name of parameter
*/
bool QtxParser::remove( const QString& name )
{
    QString sname = name.stripWhiteSpace();
    bool res = has( sname );
    if( res )
        myParameters.remove( sname );
    return res;
}

/*!
  \return value of parameter (result is invalid if there is no such parameter)
  \param name - name of parameter
*/
QtxValue QtxParser::value( const QString& name ) const
{
    QString sname = name.stripWhiteSpace();
    if( has( sname ) )
        return myParameters[ sname ].toString();
    else
        return QtxValue();
}

/*!
  Searches first parameter with assigned invalid QtxValue
  \return true if it is found
  \param name - variable to return name of parameter
*/
bool QtxParser::firstInvalid( QString& name ) const
{
    QMap< QString, QtxValue >::const_iterator anIt = myParameters.begin(),
                                              aLast = myParameters.end();
    for( ; anIt!=aLast; anIt++ )
        if( !anIt.data().isValid() )
        {
            name = anIt.key();
            return true;
        }
    return false;
}

/*!
  Removes all parameters with assigned invalid QtxValues
*/
void QtxParser::removeInvalids()
{
    QStringList toDelete;
    QMap< QString, QtxValue >::const_iterator anIt = myParameters.begin(),
                                              aLast = myParameters.end();
    for( ; anIt!=aLast; anIt++ )
        if( !anIt.data().isValid() )
            toDelete.append( anIt.key() );

    QStringList::const_iterator aLIt = toDelete.begin(),
                                aLLast = toDelete.end();
    for( ; aLIt!=aLLast; aLIt++ )
        myParameters.remove( *aLIt );
}

/*!
  \return last error occured during parsing
*/
QtxParser::Error QtxParser::lastError() const
{
    return myLastError;
}

/*!
  Sets last error occured during parsing (for internal using only)
*/
void QtxParser::setLastError( QtxParser::Error err )
{
    myLastError = err;
}

/*!
  \return string dump of internal parser postfix
*/
QString QtxParser::dump() const
{
    return dump( myPost );
}

/*!
  \return string dump of postfix
  \param post - postfix to be dumped
*/
QString QtxParser::dump( const Postfix& post ) const
{
    QString res;

    if( myOperations )
    {
        PostfixIterator anIt = post.begin(),
                        aLast = post.end();
        for( ; anIt!=aLast; anIt++ )
        {
            if( (*anIt).myType == Value && 
                    ( ( *anIt ).myValue.type()==QVariant::String ||
                    ( *anIt ).myValue.type()==QVariant::CString ) )
                res += "'" + ( *anIt ).myValue.toString() + "'";
            else
                res += ( *anIt ).myValue.toString();
            if( (*anIt).myType == Pre )
                res += "(pre)";
            else if( (*anIt).myType == Post )
                res += "(post)";
            else if( (*anIt).myType == Binary )
                res += "(bin)";

            res += "_";
        }
    }
    return res;
}

/*!
  Fills list with names of parameters
  \param list - list to be filled
*/
void QtxParser::paramsList( QStringList& list )
{
    PostfixIterator anIt = myPost.begin(),
                    aLast = myPost.end();
    for( ; anIt!=aLast; anIt++ )
        if( (*anIt).myType==Param )
        {
            QString name = (*anIt).myValue.toString();
            if( list.contains( name )==0 )
                list.append( name );
        }
}

/*!
  Removes all parameters
*/
void QtxParser::clear()
{
    myParameters.clear();
}

/*!
  \return string representation for list of QtxValues
  \param list - list to be converted
*/
QString QtxParser::toString( const QValueList< QtxValue >& list )
{
    QValueList< QtxValue >::const_iterator anIt = list.begin(),
                                           aLast = list.end();
    QString res = "set : [ ";
    for( ; anIt!=aLast; anIt++ )
        res+=(*anIt).toString()+" ";
    res+="]";
    return res;
}
