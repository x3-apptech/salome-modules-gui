#include "QtxStdOperations.h"

#include <math.h>
#include <stdlib.h>

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxStdOperations::QtxStdOperations()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxStdOperations::~QtxStdOperations()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxStdOperations::bracketsList( QStringList& list, bool open ) const
{
    if( open )
        list.append( "(" );
    else
        list.append( ")" );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxStdOperations::opersList( QStringList& list ) const
{
    list += myOpers;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxStdOperations::addOperations( const QStringList& list )
{
    QStringList::const_iterator anIt = list.begin(),
                                aLast = list.end();
    for( ; anIt!=aLast; anIt++ )
        if( myOpers.contains( *anIt )==0 )
            myOpers.append( *anIt );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxStdOperations::addTypes( const ListOfTypes& list )
{
    ListOfTypes::const_iterator anIt = list.begin(),
                                aLast = list.end();
    for( ; anIt!=aLast; anIt++ )
        if( myTypes.contains( *anIt )==0 )
            myTypes.append( *anIt );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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








//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxArithmetics::~QtxArithmetics()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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











//================================================================
// Function : 
// Purpose  : 
//================================================================
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
    addTypes( aTypes );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxLogic::~QtxLogic()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
bool QtxLogic::createValue( const QString& str, QtxValue& v ) const
{
    bool ok = true;
    if( str.lower()=="true" )
        v = true;
    else if( str.lower()=="false" )
        v = false;
    else
        ok = QtxStdOperations::createValue( str, v );

    return ok;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxParser::Error QtxLogic::calculate( const QString& op,
                                          QtxValue& v1, QtxValue& v2 ) const
{
    QtxParser::Error err = QtxParser::OK;
    if( v1.isValid() && v2.isValid() )
    {
        if( op=="and" || op=="&&" )
            set( v1, v1.toBool() && v2.toBool() );
        else if( op=="or" || op=="||" )
            set( v1, v1.toBool() || v2.toBool() );
        else if( op=="xor" )
            set( v1, ( !v1.toBool() && v2.toBool() ) || ( v1.toBool() && !v2.toBool() ) );
        else if( op=="imp" )
            set( v1, !v1.toBool() || v2.toBool() );
        else if( op=="=" )
            set( v1, v1.toBool()==v2.toBool() );
    }
    else
        if( op=="not" || op=="!" )
            set( v2, !v2.toBool() );

    return err;
}






//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxFunctions::~QtxFunctions()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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








//================================================================
// Function : 
// Purpose  : 
//================================================================
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


//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxStrings::~QtxStrings()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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






//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxSets::~QtxSets()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxSets::bracketsList( QStringList& list, bool open ) const
{
    if( open )
        list.append( "{" );
    else 
        list.append( "}" );
    QtxStdOperations::bracketsList( list, open );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
bool QtxSets::createValue( const QString& str, QtxValue& val ) const
{
    return QtxStdOperations::createValue( str, val );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
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

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxSets::add( ValueSet& set, const QtxValue& v )
{
    if( v.isValid() && set.contains( v )==0 )
        set.append( v );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxSets::add( ValueSet& s1, const ValueSet& s2 )
{
    ValueSet::const_iterator anIt = s2.begin(),
                             aLast = s2.end();
    for( ; anIt!=aLast; anIt++ )
        add( s1, *anIt );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxSets::remove( ValueSet& set, const QtxValue& v )
{
    set.remove( v );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxSets::remove( ValueSet& s1, const ValueSet& s2 )
{
    ValueSet::const_iterator anIt = s2.begin(),
                             aLast = s2.end();
    for( ; anIt!=aLast; anIt++ )
        s1.remove( *anIt );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
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
                    set( v1, aNewList.isEmpty() );
                else if( op=="<>" || op=="!=" )
                    set( v1, !aNewList.isEmpty() );
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
