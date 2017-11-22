/*  TestSubstitutionField.cpp
 *
 *  Copyright (C) 2017  Jim Evins <evins@snaught.com>
 *
 *  This file is part of gLabels-qt.
 *
 *  gLabels-qt is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gLabels-qt is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with gLabels-qt.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "TestSubstitutionField.h"

#include "SubstitutionField.h"


QTEST_MAIN(TestSubstitutionField)


void TestSubstitutionField::parseValid()
{
	using namespace glabels;

	//
	// Valid substitution fields (concatenated in single input string)
	//
	QString input = "${1234}${abc:=ABC}${x:%08.2f}${y:%08.2f:=12.34}${ADDR2:n}";
	QStringRef s = &input;
	
	SubstitutionField f1;
	QCOMPARE( SubstitutionField::parse( s, f1 ), true );
	QCOMPARE( f1.fieldName(),    QString( "1234" ) );
	QCOMPARE( f1.newLine(),      false );

	SubstitutionField f2;
	QCOMPARE( SubstitutionField::parse( s, f2 ), true );
	QCOMPARE( f2.fieldName(),    QString( "abc" ) );
	QCOMPARE( f2.defaultValue(), QString( "ABC" ) );
	QCOMPARE( f2.newLine(),      false );

	SubstitutionField f3;
	QCOMPARE( SubstitutionField::parse( s, f3 ), true );
	QCOMPARE( f3.fieldName(),    QString( "x" ) );
	QCOMPARE( f3.format(),       QString( "%08.2f" ) );
	QCOMPARE( f3.formatType(),   QChar('f') );
	QCOMPARE( f3.newLine(),      false );

	SubstitutionField f4;
	QCOMPARE( SubstitutionField::parse( s, f4 ), true );
	QCOMPARE( f4.fieldName(),    QString( "y" ) );
	QCOMPARE( f4.defaultValue(), QString( "12.34" ) );
	QCOMPARE( f4.format(),       QString( "%08.2f" ) );
	QCOMPARE( f4.formatType(),   QChar('f') );
	QCOMPARE( f4.newLine(),      false );

	SubstitutionField f5;
	QCOMPARE( SubstitutionField::parse( s, f5 ), true );
	QCOMPARE( f5.fieldName(),    QString( "ADDR2" ) );
	QCOMPARE( f5.newLine(),      true );
}


void TestSubstitutionField::parseInvalid()
{
	using namespace glabels;

	//
	// Ordinary text
	//
	QString input5 = "Abcdefg";
	QStringRef s5 = &input5;
	SubstitutionField f5;
	QCOMPARE( SubstitutionField::parse( s5, f5 ), false );
	QCOMPARE( s5, QStringRef( &input5 ) ); // Should not advance string reference

	//
	// Invalid substitution fields (which are treated as ordinary text)
	//
	QString input6 = "$abc";
	QStringRef s6 = &input6;
	SubstitutionField f6;
	QCOMPARE( SubstitutionField::parse( s6, f6 ), false );
	QCOMPARE( s6, QStringRef( &input6 ) ); // Should not advance string reference

	QString input7 = "${abc";
	QStringRef s7 = &input7;
	SubstitutionField f7;
	QCOMPARE( SubstitutionField::parse( s7, f7 ), false );
	QCOMPARE( s7, QStringRef( &input7 ) ); // Should not advance string reference

	QString input8 = "${abc:}";
	QStringRef s8 = &input8;
	SubstitutionField f8;
	QCOMPARE( SubstitutionField::parse( s8, f8 ), false );
	QCOMPARE( s8, QStringRef( &input8 ) ); // Should not advance string reference

	// Even though format is invalid, let it slide.  Overall structure still good.  Format will be ignored.
	QString input9 = "${abc:%3.2}";
	QStringRef s9 = &input9;
	SubstitutionField f9;
	QCOMPARE( SubstitutionField::parse( s9, f9 ), true );
}


void TestSubstitutionField::construction()
{
	using namespace glabels;

	SubstitutionField f1( "${1234}" );
	QCOMPARE( f1.fieldName(),    QString( "1234" ) );

	SubstitutionField f2( "${abc:=ABC}" );
	QCOMPARE( f2.fieldName(),    QString( "abc" ) );
	QCOMPARE( f2.defaultValue(), QString( "ABC" ) );

	SubstitutionField f3( "${x:%08.2f}" );
	QCOMPARE( f3.fieldName(),    QString( "x" ) );
	QCOMPARE( f3.format(),       QString( "%08.2f" ) );
	QCOMPARE( f3.formatType(),   QChar('f') );

	SubstitutionField f4( "${y:%08.2f:=12.34}" );
	QCOMPARE( f4.fieldName(),    QString( "y" ) );
	QCOMPARE( f4.defaultValue(), QString( "12.34" ) );
	QCOMPARE( f4.format(),       QString( "%08.2f" ) );
	QCOMPARE( f4.formatType(),   QChar('f') );
}


void TestSubstitutionField::simpleEvaluation()
{
	using namespace glabels;

	SubstitutionField f1( "${1}" );
	SubstitutionField f2( "${2}" );
	SubstitutionField f3( "${3}" );
	SubstitutionField f4( "${4}" );

	merge::Record record1;
	record1[ "1" ] = "Abcdefg";
	record1[ "2" ] = "Hijklmn";
	record1[ "3" ] = "Opqrstu";
	record1[ "4" ] = "Vwxyz!@";

	QCOMPARE( f1.evaluate( &record1 ), QString( "Abcdefg" ) );
	QCOMPARE( f2.evaluate( &record1 ), QString( "Hijklmn" ) );
	QCOMPARE( f3.evaluate( &record1 ), QString( "Opqrstu" ) );
	QCOMPARE( f4.evaluate( &record1 ), QString( "Vwxyz!@" ) );

	merge::Record record2;
	record2[ "1" ] = "1234567";
	record2[ "2" ] = "FooBar";
	record2[ "3" ] = "8901234";
	record2[ "4" ] = "#$%^&*";

	QCOMPARE( f1.evaluate( &record2 ), QString( "1234567" ) );
	QCOMPARE( f2.evaluate( &record2 ), QString( "FooBar" ) );
	QCOMPARE( f3.evaluate( &record2 ), QString( "8901234" ) );
	QCOMPARE( f4.evaluate( &record2 ), QString( "#$%^&*" ) );
}


void TestSubstitutionField::defaultValueEvaluation()
{
	using namespace glabels;

	SubstitutionField f1( "${1:=foo1}" );
	SubstitutionField f2( "${2:=foo2}" );
	SubstitutionField f3( "${3:=foo3}" );
	SubstitutionField f4( "${4:=foo4}" );

	merge::Record record1;
	record1[ "1" ] = "Abcdefg";
	record1[ "2" ] = "Hijklmn";
	record1[ "3" ] = "Opqrstu";
	record1[ "4" ] = "Vwxyz!@";

	QCOMPARE( f1.evaluate( &record1 ), QString( "Abcdefg" ) );
	QCOMPARE( f2.evaluate( &record1 ), QString( "Hijklmn" ) );
	QCOMPARE( f3.evaluate( &record1 ), QString( "Opqrstu" ) );
	QCOMPARE( f4.evaluate( &record1 ), QString( "Vwxyz!@" ) );

	merge::Record record2; // All fields empty

	QCOMPARE( f1.evaluate( &record2 ), QString( "foo1" ) );
	QCOMPARE( f2.evaluate( &record2 ), QString( "foo2" ) );
	QCOMPARE( f3.evaluate( &record2 ), QString( "foo3" ) );
	QCOMPARE( f4.evaluate( &record2 ), QString( "foo4" ) );

	merge::Record record3;
	record3[ "1" ] = "xyzzy";
	// Field "2" empty
	// Field "3" empty
	record3[ "4" ] = "plugh";
	
	QCOMPARE( f1.evaluate( &record3 ), QString( "xyzzy" ) );
	QCOMPARE( f2.evaluate( &record3 ), QString( "foo2" ) );
	QCOMPARE( f3.evaluate( &record3 ), QString( "foo3" ) );
	QCOMPARE( f4.evaluate( &record3 ), QString( "plugh" ) );
}


void TestSubstitutionField::formattedStringEvaluation()
{
	using namespace glabels;

	SubstitutionField f1( "${1:%10s}" );
	SubstitutionField f2( "${2:%10s}" );
	SubstitutionField f3( "${3:%10s}" );
	SubstitutionField f4( "${4:%10s}" );

	SubstitutionField f5( "${5:%-10s}" );
	SubstitutionField f6( "${6:%-10s}" );
	SubstitutionField f7( "${7:%-10s}" );
	SubstitutionField f8( "${8:%-10s}" );

	merge::Record record1;
	record1[ "1" ] = "0";
	record1[ "2" ] = "1";
	record1[ "3" ] = "-1";
	record1[ "4" ] = "3.14";

	record1[ "5" ] = "0";
	record1[ "6" ] = "100";
	record1[ "7" ] = "-100";
	record1[ "8" ] = "3.14";
	
	QCOMPARE( f1.evaluate( &record1 ), QString( "         0" ) );
	QCOMPARE( f2.evaluate( &record1 ), QString( "         1" ) );
	QCOMPARE( f3.evaluate( &record1 ), QString( "        -1" ) );
	QCOMPARE( f4.evaluate( &record1 ), QString( "      3.14" ) );

	QCOMPARE( f5.evaluate( &record1 ), QString( "0         " ) );
	QCOMPARE( f6.evaluate( &record1 ), QString( "100       " ) );
	QCOMPARE( f7.evaluate( &record1 ), QString( "-100      " ) );
	QCOMPARE( f8.evaluate( &record1 ), QString( "3.14      " ) );
}


void TestSubstitutionField::formattedFloatEvaluation()
{
	using namespace glabels;

	SubstitutionField f1( "${1:%+5.2f}" );
	SubstitutionField f2( "${2:%+5.2f}" );
	SubstitutionField f3( "${3:%+5.2f}" );
	SubstitutionField f4( "${4:%+5.2f}" );

	SubstitutionField f5( "${5:%+5.2e}" );
	SubstitutionField f6( "${6:%+5.2e}" );
	SubstitutionField f7( "${7:%+5.2e}" );
	SubstitutionField f8( "${8:%+5.2e}" );

	merge::Record record1;
	record1[ "1" ] = "0";
	record1[ "2" ] = "1";
	record1[ "3" ] = "-1";
	record1[ "4" ] = "3.14";

	record1[ "5" ] = "0";
	record1[ "6" ] = "100";
	record1[ "7" ] = "-100";
	record1[ "8" ] = "3.14";
	
	QCOMPARE( f1.evaluate( &record1 ), QString( "+0.00" ) );
	QCOMPARE( f2.evaluate( &record1 ), QString( "+1.00" ) );
	QCOMPARE( f3.evaluate( &record1 ), QString( "-1.00" ) );
	QCOMPARE( f4.evaluate( &record1 ), QString( "+3.14" ) );

	QCOMPARE( f5.evaluate( &record1 ), QString( "+0.00e+00" ) );
	QCOMPARE( f6.evaluate( &record1 ), QString( "+1.00e+02" ) );
	QCOMPARE( f7.evaluate( &record1 ), QString( "-1.00e+02" ) );
	QCOMPARE( f8.evaluate( &record1 ), QString( "+3.14e+00" ) );
}


void TestSubstitutionField::formattedIntEvaluation()
{
	using namespace glabels;

	SubstitutionField f1( "${1:%08d}" );
	SubstitutionField f2( "${2:%08d}" );
	SubstitutionField f3( "${3:%08d}" );
	SubstitutionField f4( "${4:%08d}" );

	SubstitutionField f5( "${5:%08x}" );
	SubstitutionField f6( "${6:%08x}" );
	SubstitutionField f7( "${7:%08x}" );
	SubstitutionField f8( "${8:%08x}" );

	merge::Record record1;
	record1[ "1" ] = "0";
	record1[ "2" ] = "1";
	record1[ "3" ] = "-1";
	record1[ "4" ] = "3.14";

	record1[ "5" ] = "100";
	record1[ "6" ] = "0x100";
	record1[ "7" ] = "-1";
	record1[ "8" ] = "314";
	
	QCOMPARE( f1.evaluate( &record1 ), QString( "00000000" ) );
	QCOMPARE( f2.evaluate( &record1 ), QString( "00000001" ) );
	QCOMPARE( f3.evaluate( &record1 ), QString( "-0000001" ) );
	QCOMPARE( f4.evaluate( &record1 ), QString( "00000000" ) ); // Invalid integer value

	QCOMPARE( f5.evaluate( &record1 ), QString( "00000064" ) ); // 100(decimal) == 64(hex)
	QCOMPARE( f6.evaluate( &record1 ), QString( "00000100" ) );
	QCOMPARE( f7.evaluate( &record1 ), QString( "00000000" ) ); // Invalid unsigned integer
	QCOMPARE( f8.evaluate( &record1 ), QString( "0000013a" ) ); // 314(decimal) == 13a(hex)
}


void TestSubstitutionField::newLineEvaluation()
{
	using namespace glabels;

	SubstitutionField addr2( "${ADDR2:n}" );
	QCOMPARE( addr2.fieldName(),    QString( "ADDR2" ) );
	QCOMPARE( addr2.newLine(),      true );

	merge::Record record1;
	record1[ "ADDR2" ] = "Apt. 5B";

	merge::Record record2;
	record2[ "ADDR2" ] = ""; // ADDR2 Empty
	
	merge::Record record3;
	// ADDR2 not defined

	QCOMPARE( addr2.evaluate( &record1 ), QString( "\nApt. 5B" ) ); // Prepends a newline
	QCOMPARE( addr2.evaluate( &record2 ), QString( "" ) ); // Evaluates empty
	QCOMPARE( addr2.evaluate( &record3 ), QString( "" ) ); // Evaluates empty
}
