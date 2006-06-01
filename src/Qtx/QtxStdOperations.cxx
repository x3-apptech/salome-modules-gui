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
#include "QtxStdOperations.h"

#include <math.h>
#include <stdlib.h>

/*!
   Default constructor
*/
QtxStdOperations::QtxStdOperations()
{
}

/*!
   Destructor
*/
QtxStdOperations::~QtxStdOperations()
{
}

/*!
   Fills list of brackets treated as open (when 'open' is 'true') or close ('open' is 'false')
*/
void QtxStdOperations::bracketsList( QStringList& list, bool open ) const
{
    if( open )
        list.append( "(" );
    else
        list.append( ")" );
}

/*!
   Fills list with operation names by copying of internal list of operations
*/
void QtxStdOperations::opersList( QStringList& list ) const
{
  list += myOpers;
}

/*!
   Add operation names from list to internal list of operations
*/
void QtxStdOperations::addOperations( const QStringList& list )
{
    QStringList::const_iterator anIt = list.begin(),
                                aLast = list.end();
    for( ; anIt!=aLast; anIt++ )
        if( myOpers.contains( *anIt )==0 )
            myOpers.append( *anIt );
}

/*!
   Append operation names from 'list' to internal list of operations
*/
void QtxStdOperations::addTypes( const ListOfTypes& list )
{
    ListOfTypes::const_iterator anIt = list.begin(),
                                aLast = list.end();
    for( ; anIt!=aLast; anIt++ )
        if( myTypes.contains( *anIt )==0 )
            myTypes.append( *anIt );
}

/*!
   \return whether values with passed types are valid for arguments of operation
   \param op - name of operation
   \param t1 - type of first argument
   \param t2 - type of second argument
*/
QtxParser::Error QtxStdOperations::isValid( const QString& op,
                                            const QVariant::Type t1,
                                            const QVariant::Type t2 ) const
{
    if( ( t1==QVariant::Invalid || myTypes.contains( t1 ) ) &&
        ( t2==QVariant::Invalid || myTypes.contains( t2 ) ) &&
        ( t1!=QVariant::Invalid || t2!=QVariant::Invalid ) )
        if( prior( op, t1!=QVariant::Invalid && t2!=QVariant::Invalid ) > 0 )
            return QtxParser::OK;
        else
            return QtxParser::InvalidOperation;
    else
        return QtxParser::OperandsNotMatch;
}








/*!
   Default constructor
*/
QtxArithmetics::QtxArithmetics()
: QtxStdOperations()
{
    QStringList aList;
    aList.append( "+" );
    aList.append( "-" );
    aList.append( "*" );
    aList.append( "/" );
    aList.append( "=" );
    aList.append( "<" );
    aList.append( ">" );
    aList.append( "<=" );
    aList.append( ">=" );
    aList.append( "<>" );
    aList.append( "!=" ); // same as "<>" - for C++ addicts
    addOperations( aList );

    ListOfTypes aTypes;
    aTypes.append( QVariant::Int );
    aTypes.append( QVariant::UInt );
    aTypes.append( QVariant::Double );
    addTypes( aTypes );
}

/*!
   Destructor
*/
QtxArithmetics::~QtxArithmetics()
{
}

/*!
    Creates numbers by it's string representation [redefined virtual]
*/
bool QtxArithmetics::createValue( const QString& str, QtxValue& v ) const
{
    bool ok = false;
    v = str.toInt( &ok );

    if( !ok )
    {
        v = str.toDouble( &ok );
        if( !ok )
            ok = QtxStdOperations::createValue( str, v );
    }
    return ok;
}

/*!
    \return priority of arithmetic operation 'op'.
    \param isBin indicate whether the operation is binary
*/
int QtxArithmetics::prior( const QString& op, bool isBin ) const
{
    if( isBin )
        if( op=="<" || op==">" || op=="=" || 
            op=="<=" || op==">=" || op=="<>" || op=="!=" )
            return 1;
        else if( op=="+" || op=="-" )
            return 2;
        else if( op=="*" || op=="/" )
            return 3;
        else
            return 0;
    else if( op=="+" || op=="-" )
        return 5;
    else
        return 0;
}

void set( QVariant& v1, bool v2 )
{
    v1 = QVariant( v2, 0 );
}

/*!
    Calculates result of operation
    \return one of error states
    \param op - name of operation
    \param v1 - first operation argument (must be used also to store result)
    \param v2 - second operation argument
*/
QtxParser::Error QtxArithmetics::calculate( const QString& op, 
                                                QtxValue& v1, QtxValue& v2 ) const
{
    QtxParser::Error err = QtxParser::OK;

    if( v1.isValid() && v2.isValid() )
        // binary operations
        if( ( v1.type()==QVariant::Int || v1.type()==QVariant::UInt ) &&
            ( v2.type()==QVariant::Int || v2.type()==QVariant::UInt ) )
        {
            int _v1 = v1.toInt(),
                _v2 = v2.toInt();

            if( op=="+" )
                v1 = _v1 + _v2;
            else if( op=="-" )
                v1 = _v1 - _v2;
            else if( op=="*" )
                v1 = _v1 * _v2;
            else if( op=="/" )
                if( _v2!=0 )
                    if( _v1%_v2==0 )
                        v1 = _v1 / _v2;
                    else
                        v1 = double( _v1 ) / double( _v2 );
                else
                    err = QtxParser::InvalidResult;
            else if( op=="<" )
                set( v1, _v1<_v2 );
            else if( op==">" )
                set( v1, _v1>_v2 );
            else if( op=="=" )
                set( v1, _v1==_v2 );
            else if( op=="<=" )
                set( v1, _v1<=_v2 );
            else if( op==">=" )
                set( v1, _v1>=_v2 );
            else if( op=="<>" || op=="!=" )
                set( v1, _v1!=_v2 );
        }
        else if( ( v1.type()==QVariant::Int || v1.type()==QVariant::Double ) &&
                 ( v2.type()==QVariant::Int || v2.type()==QVariant::Double ) )
        {
            double _v1 = v1.toDouble(),
                   _v2 = v2.toDouble();

            if( op=="+" )
                v1 = _v1 + _v2;
            else if( op=="-" )
                v1 = _v1 - _v2;
            else if( op=="*" )
                v1 = _v1 * _v2;
            else if( op=="/" )
                if( _v2!=0 )
                    v1 = _v1 / _v2;
                else
                    err = QtxParser::InvalidResult;
            else if( op=="<" )
                set( v1, _v1<_v2 );
            else if( op==">" )
                set( v1, _v1>_v2 );
            else if( op=="=" )
                set( v1, _v1==_v2 );
            else if( op=="<=" )
                set( v1, _v1<=_v2 );
            else if( op==">=" )
                set( v1, _v1>=_v2 );
            else if( op=="<>" || op=="!=" )
                set( v1, _v1!=_v2 );
        }
    else
        // prefix operations
        if( op=="-" )
            if( v2.type()==QVariant::Int )
                v2 = -v2.toInt();
            else if( v2.type()==QVariant::Double )
                v2 = -v2.toDouble();

    return err;
}











/*!
   Default constructor
*/
QtxLogic::QtxLogic()
: QtxStdOperations()
{
    QStringList aList;
    aList.append( "and" );
    aList.append( "&&" );
    aList.append( "or" );
    aList.append( "||" );
    aList.append( "xor" );
    aList.append( "not" );
    aList.append( "!" );
    aList.append( "imp" );
    aList.append( "=" );
    addOperations( aList );

    ListOfTypes aTypes;
    aTypes.append( QVariant::Bool );
    aTypes.append( QVariant::Int );
    aTypes.append( QVariant::UInt );
    addTypes( aTypes );
}

/*!
   Destructor
*/
QtxLogic::~QtxLogic()
{
}

/*!
    Creates value 'true' or 'false' it's string representation [redefined virtual]
*/
bool QtxLogic::createValue( const QString& str, QtxValue& v ) const
{
    bool ok = true;
    if( str.lower()=="true" )
        v = QtxValue( true, 0 );
    else if( str.lower()=="false" )
        v = QtxValue( false, 0 );
    else
        ok = QtxStdOperations::createValue( str, v );

    return ok;
}

/*!
    \return priority of arithmetic operation 'op'.
    \param isBin indicate whether the operation is binary
*/
int QtxLogic::prior( const QString& op, bool isBin ) const
{
    if( isBin )
        if( op=="and" || op=="or" || op=="xor" ||
            op=="&&"  || op=="||" ||
            op=="imp" )
            return 1;
        else if( op=="=" )
            return 2;
        else 
            return 0;
    else if( op=="not" || op=="!" )
            return 5;
         else
            return 0;
}

bool boolean_value( const QtxValue& v )
{
  if( v.type()==QVariant::Bool )
    return v.toBool();
  else if( v.type()==QVariant::Int )
    return v.toInt()!=0;
  else if( v.type()==QVariant::UInt )
    return v.toUInt()!=0;
  else
    return false;
}

/*!
    Calculates result of operation
    \return one of error states
    \param op - name of operation
    \param v1 - first operation argument (must be used also to store result)
    \param v2 - second operation argument
*/
QtxParser::Error QtxLogic::calculate( const QString& op,
                                          QtxValue& v1, QtxValue& v2 ) const
{
    QtxParser::Error err = QtxParser::OK;
    bool val1 = boolean_value( v1 ),
         val2 = boolean_value( v2 );
    if( v1.isValid() && v2.isValid() )
    {
        if( op=="and" || op=="&&" )
            set( v1, val1 && val2 );
        else if( op=="or" || op=="||" )
            set( v1, val1 || val2 );
        else if( op=="xor" )
            set( v1, ( !val1 && val2 ) || ( val1 && !val2 ) );
        else if( op=="imp" )
            set( v1, !val1 || val2 );
        else if( op=="=" )
            set( v1, val1==val2 );
    }
    else
        if( op=="not" || op=="!" )
            set( v2, !val2 );

    return err;
}






/*!
   Default constructor
*/
QtxFunctions::QtxFunctions()
: QtxStdOperations()
{
    QStringList aList;
    aList.append( "sqrt" );
    aList.append( "abs" );
    aList.append( "sin" );
    aList.append( "cos" );
    aList.append( "rad2grad" );
    aList.append( "grad2rad" );
    addOperations( aList );

    ListOfTypes aTypes;
    aTypes.append( QVariant::Int );
    aTypes.append( QVariant::Double );
    addTypes( aTypes );
}

/*!
   Destructor
*/
QtxFunctions::~QtxFunctions()
{
}

/*!
    Creates numbers by it's string representation [redefined virtual]
*/
bool QtxFunctions::createValue( const QString& str, QtxValue& v ) const
{
    bool ok = false;
    v = str.toInt( &ok );

    if( !ok )
    {
        v = str.toDouble( &ok );
        if( !ok )
            ok = QtxStdOperations::createValue( str, v );
    }
    return ok;
}

/*!
    \return priority of arithmetic operation 'op'.
    \param isBin indicate whether the operation is binary
*/
int QtxFunctions::prior( const QString& op, bool isBin ) const
{
    if( isBin )
        return 0;
    else if( op=="sqrt" || op=="abs" || op=="sin" || op=="cos" ||
             op=="rad2grad" || op=="grad2rad" )
        return 1;
    else
        return 0;
}

/*!
    Calculates result of operation
    \return one of error states
    \param op - name of operation
    \param v1 - first operation argument (must be used also to store result)
    \param v2 - second operation argument
*/
QtxParser::Error QtxFunctions::calculate( const QString& op,
                                          QtxValue& v1, QtxValue& v2 ) const
{
    QtxParser::Error err = QtxParser::OK;
    double val = v2.toDouble();
    if( op=="sqrt" )
        if( val>=0 )
            v2 = sqrt( val );
        else
            err = QtxParser::InvalidResult;
    else if( op=="abs" )
        if( v2.type()==QVariant::Int )
            v2 = abs( v2.toInt() );
        else 
            v2 = fabs( v2.toDouble() );
    else if( op=="sin" )
        v2 = sin( val );
    else if( op=="cos" )
        v2 = cos( val );
    else if( op=="grad2rad" )
        v2 = val * 3.14159256 / 180.0;
    else if( op=="rad2grad" )
        v2 = val * 180.0 / 3.14159256;

    return err;
}








/*!
   Default constructor
*/
QtxStrings::QtxStrings()
: QtxStdOperations()
{
    QStringList aList;
    aList.append( "+" );
    aList.append( "=" );
    aList.append( "<" );
    aList.append( ">" );
    aList.append( "<=" );
    aList.append( ">=" );
    aList.append( "<>" );
    aList.append( "!=" ); // same as "<>" - for C++ addicts
    aList.append( "length" );
    aList.append( "lower" );
    aList.append( "upper" );
    addOperations( aList );

    ListOfTypes aTypes;
    aTypes.append( QVariant::Int );
    aTypes.append( QVariant::Double );
    aTypes.append( QVariant::String );
    aTypes.append( QVariant::CString );
    addTypes( aTypes );
}


/*!
   Destructor
*/
QtxStrings::~QtxStrings()
{
}

/*!
    Creates string QtxValue by it's Qt string representation [redefined virtual]
*/
bool QtxStrings::createValue( const QString& str, QtxValue& v ) const
{
    QChar st = str[0],
          fin = str[ ( int )( str.length()-1 ) ];
    if( st=="'" && fin=="'" )
    {
        v = str.mid( 1, str.length()-2 );
        return true;
    }
    else
        return QtxStdOperations::createValue( str, v );
}

/*!
    \return priority of arithmetic operation 'op'.
    \param isBin indicate whether the operation is binary
*/
int QtxStrings::prior( const QString& op, bool isBin ) const
{
    if( isBin )
        if( op=="+" ) 
            return 2;
        else if( op=="="  || op=="<"  || op==">"  ||
                 op=="<=" || op==">=" || op=="<>" || op=="!=" )
            return 1;
        else
            return 0;
    else
        if( op=="length" || op=="lower" || op=="upper" )
            return 5;
        else
            return 0;
}

/*!
    Calculates result of operation
    \return one of error states
    \param op - name of operation
    \param v1 - first operation argument (must be used also to store result)
    \param v2 - second operation argument
*/
QtxParser::Error QtxStrings::calculate( const QString& op,
                                            QtxValue& v1, QtxValue& v2 ) const
{
    QtxParser::Error err = QtxParser::OK;
    if( v1.isValid() && v2.isValid() )
    {
        QString _v1 = v1.toString(),
                _v2 = v2.toString();
        if( op=="+" )
            v1 = _v1 + _v2;
        else if( op=="=" )
            set( v1, _v1==_v2 );
        else if( op=="<" )
            set( v1, _v1<_v2 );
        else if( op==">" )
            set( v1, _v1>_v2 );
        else if( op=="<>" || op=="!=" )
            set( v1, _v1!=_v2 );
        else if( op=="<=" )
            set( v1, _v1<_v2 || _v1==_v2 );
        else if( op==">=" )
            set( v1, _v1>_v2 || _v1==_v2 );
    }
    else if( !v1.isValid() && v2.isValid() )
    {
        QString val = v2.toString();
        if( op=="length" )
            v2 = ( int )val.length();
        else if( op=="lower" )
            v2 = val.lower();
        else if( op=="upper" )
            v2 = val.upper();
    }

    return err;
}






/*!
   Default constructor
*/
QtxSets::QtxSets()
{
    QStringList aList;
    aList.append( "{" );
    aList.append( "}" );
    aList.append( "=" );
    aList.append( "<>" );
    aList.append( "!=" ); // same as "<>" - for C++ addicts
    aList.append( "+" );
    aList.append( "-" );
    aList.append( "*" );
    aList.append( "in" );
    aList.append( "count" );
    addOperations( aList );

    ListOfTypes aTypes;
    aTypes.append( QVariant::List );
    addTypes( aTypes );
}

/*!
   Destructor
*/
QtxSets::~QtxSets()
{
}

/*!
   Fills list of brackets treated as open (when 'open' is 'true') or close ('open' is 'false')
*/
void QtxSets::bracketsList( QStringList& list, bool open ) const
{
    if( open )
        list.append( "{" );
    else 
        list.append( "}" );
    QtxStdOperations::bracketsList( list, open );
}

/*!
    Creates set of QtxValues by their string representation [redefined virtual]
*/
bool QtxSets::createValue( const QString& str, QtxValue& val ) const
{
    return QtxStdOperations::createValue( str, val );
}

/*!
    \return priority of arithmetic operation 'op'.
    \param isBin indicate whether the operation is binary
*/
int QtxSets::prior( const QString& op, bool isBin ) const
{
    if( isBin )
        if( op=="=" || op=="<>" || op=="!=" )
            return 1;
        else if( op=="+" || op=="-" || op=="*" )
            return 2;
        else if( op=="in" )
            return 3;
        else
            return 0;

    else 
        if( op=="{" || op=="}" )
            return 5;
        else if( op=="count" )
            return 4;
        else
            return 0;
}

/*!
   \return whether values with passed types are valid for arguments of operation
   \param op - name of operation
   \param t1 - type of first argument
   \param t2 - type of second argument
*/
QtxParser::Error QtxSets::isValid( const QString& op,
                                   const QVariant::Type t1,
                                   const QVariant::Type t2 ) const
{
    if( op=="{" )
        return QtxParser::OK;
    else if( op=="in" )
        if( t1!=QVariant::Invalid && t2==QVariant::List )
            return QtxParser::OK;
        else
            return QtxParser::OperandsNotMatch;
    else
        return QtxStdOperations::isValid( op, t1, t2 );
}

/*!
   Adds new value 'v' to set 'set' [static]
*/
void QtxSets::add( ValueSet& set, const QtxValue& v )
{
    if( v.isValid() && set.contains( v )==0 )
        set.append( v );
}

/*!
   Adds values from set 's2' to set 's1'
*/
void QtxSets::add( ValueSet& s1, const ValueSet& s2 )
{
    ValueSet::const_iterator anIt = s2.begin(),
                             aLast = s2.end();
    for( ; anIt!=aLast; anIt++ )
        add( s1, *anIt );
}

/*!
   Removes value 'v' from set 'set'
*/
void QtxSets::remove( ValueSet& set, const QtxValue& v )
{
    set.remove( v );
}

/*!
   Removes values of set 's2' from set 's1'
*/
void QtxSets::remove( ValueSet& s1, const ValueSet& s2 )
{
    ValueSet::const_iterator anIt = s2.begin(),
                             aLast = s2.end();
    for( ; anIt!=aLast; anIt++ )
        s1.remove( *anIt );
}

/*!
    Calculates result of operation
    \return one of error states
    \param op - name of operation
    \param v1 - first operation argument (must be used also to store result)
    \param v2 - second operation argument
*/
QtxParser::Error QtxSets::calculate( const QString& op, QtxValue& v1, QtxValue& v2 ) const
{
    QtxParser::Error err = QtxParser::OK;

    if( op!="{" )
        if( op=="}" )
        {
            ValueSet aNewList;
            add( aNewList, v1.toList() );
            v1 = aNewList;
        }

        else if( op=="=" || op=="<>" || op=="!=" || op=="+" || op=="-" || op=="*" )
        {
            ValueSet aNewList;
            add( aNewList, v1.toList() );

            if( op=="=" || op=="<>" || op=="!=" || op=="-" )
            {
                remove( aNewList, v2.toList() );

                if( op=="=" )
                    set( v1, aNewList.isEmpty() && v1.toList().count()==v2.toList().count() );
                else if( op=="<>" || op=="!=" )
                    set( v1, !aNewList.isEmpty() || v1.toList().count()!=v2.toList().count() );
                else
                    v1 = aNewList;
            }
            else if( op=="+" )
            {
                add( aNewList, v2.toList() );
                v1 = aNewList;
            }
            else if( op=="*" )
            {
                ValueSet toDelete;
                add( toDelete, aNewList );
                remove( toDelete, v2.toList() );
                remove( aNewList, toDelete );
                v1 = aNewList;
            }
        }
        else if( op=="count" )
            v2 = ( int )v2.toList().count();
        else if( op=="in" )
        {
            if( v1.type()==QVariant::List )
            {
                bool res = true;
                ValueSet::const_iterator anIt = v1.toList().begin(),
                                         aLast = v1.toList().end();
                for( ; anIt!=aLast && res; anIt++ )
                    res = v2.toList().contains( *anIt )>0;

                set( v1, res );
            }
            else
	    {
	      const QValueList< QVariant >& list = v2.toList();
	      v1 = ( bool )( list.find( v1 )!=list.end() );
	    }
        }

    return err;
}
