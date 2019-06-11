/*
 Copyright @ UESTC

 ESY-series Many-core System-on-chip Simulation Environment is free software:
 you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3
 of the License, or (at your option) any later version.

 ESY-series Many-core System-on-chip Simulation Environment is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with
 this program. If not, see http://www.gnu.org/licenses/.
 */

#include "esy_conv.h"
#include "esy_argument.h"
#include "tinyxml.h"
#include <fstream>
#include <sstream>
#include <iostream>

#define ARGUMENT_DECLARATION_VERSION  "1.0"
#define ARGUMENT_DECLARATION_ENCODING  "UTF-8"

#define ARGUMENT_ROOT  "Argumentconfiguration"
#define ARGUMENT_ROOT_NAME  "name"
#define ARGUMENT_ROOT_VERSION  "version"
#define ARGUMENT_ROOT_EXEC  "exec"
#define ARGUMENT_ROOT_COMPILE  "compile"

#define ARGUMENT_ARGU  "Argu"

#define ARGUMENT_ARGU_ARGUMENT  "argument"
#define ARGUMENT_ARGU_LOOP  "loop"
#define ARGUMENT_ARGU_VALUE  "value"
#define ARGUMENT_ARGU_NOTE  "note"
#define ARGUMENT_ARGU_TYPE  "type"
#define ARGUMENT_ARGU_DEPARG  "darg"
#define ARGUMENT_ARGU_DEPVAL  "dval"
#define ARGUMENT_ARGU_EXTENSION "extension"

#define LINKCFG_EXTENSION "lkcfg"

EsyArgumentEnum::operator string()
{
	for (size_t i = 0; i < m_enum_value.size(); i++)
	{
		if (m_enum_value[i] == m_value)
		{
			return m_enum_str[i];
		}
	}
	return "";
}

void EsyArgumentEnum::operator =(const string & str)
{
	for ( size_t i = 0; i < m_enum_str.size(); i ++ )
	{
		if ( m_enum_str[ i ] == str )
		{
			m_value = m_enum_value[ i ];
			return;
		}
	}
}

string EsyArgumentItem::value2Str(string head, string dec, string end)
{
	string str;
	switch( m_type )
	{
	case NEW_FILE_TYPE:
	case OPEN_FILE_TYPE:
	case NEW_FILE_IF_TYPE:
	case OPEN_FILE_IF_TYPE:
	case STRING_TYPE:
		str = value2StrString();
		break;
	case LONG_TYPE:
		str = value2StrLong();
		break;
	case DOUBLE_TYPE:
		str = value2StrDouble();
		break;
	case BOOL_TYPE:
		str = value2StrBool();
		break;
	case STRING_VECTOR_TYPE:
		str = value2StrStringVector( head, dec, end );
		break;
	case LONG_VECTOR_TYPE:
		str = value2StrLongVector( head, dec, end );
		break;
	case DOUBLE_VECTOR_TYPE	:
		str = value2StrDoubleVector( head, dec, end );
		break;
	case ENUM_TYPE:
		str = value2StrEnum();
		break;
	default:
		str = "Unknown support type";
		break;
	}
	return str;
}

#define GETVALUESINGLE(ArguT, Name) \
string EsyArgumentItem::value2Str##Name() \
{\
	ostringstream os; \
	if ( mp_var ) \
	{ \
		os << *( ( ArguT *) mp_var); \
	} \
	return os.str(); \
}
GETVALUESINGLE(string, String)
GETVALUESINGLE(long, Long)
GETVALUESINGLE(double, Double)
#undef GETVALUESINGLE

string EsyArgumentItem::value2StrBool()
{
	if ( mp_var )
	{
		if ( *( (bool *)mp_var ) == true )
		{
			return "1";
		}
		else
		{
			return "0";
		}
	}
	return string();
}

string EsyArgumentItem::value2StrEnum()
{
	if (mp_var)
	{
		return (string)(*((EsyArgumentEnum*)mp_var));
	}
	else
	{
		return string();
	}
}

#define GETVALUEVECTOR(ArguT, Name) \
string EsyArgumentItem::value2Str##Name##Vector( \
	string head, string dec, string end ) \
{ \
	ostringstream os; \
	if ( mp_var ) \
	{ \
		vector< ArguT >* tp; \
		tp = ( vector< ArguT > * )mp_var; \
		os << head; \
		for ( size_t i = 0; i < tp->size(); i ++ ) \
		{ \
			if ( i == 0 ) \
			{ \
				os << tp->at( i ); \
			} \
			else \
			{ \
				os << dec << tp->at( i ); \
			} \
		} \
		os << end ; \
	} \
	return os.str(); \
}
GETVALUEVECTOR(string, String)
GETVALUEVECTOR(long, Long)
GETVALUEVECTOR(double, Double)
#undef GETVALUEVECTOR

EsyArgumentList::EsyArgumentList() :
	vector<EsyArgumentItem>(), m_head( "" ), m_version( "1.0" ), m_help( false )
{
	insertDefaultArgument();
}

EsyArgumentList::EsyArgumentList(const string & head, const string & version) :
	vector<EsyArgumentItem>(),
	m_head( head ), m_version( version ), m_help( false )
{
	insertDefaultArgument();
}

void EsyArgumentList::insertDefaultArgument()
{
	insertBool( "-help", "Print help information", &m_help );
	insertString( "-xml",
		"Print argument XML file, format: -xml <xml file path> -xmlexec <command for executing> -xmlcomp < command for compiling > -xmlname <name of this tool>",
		&m_xml_str );
	insertString( "-xmlexec", "The command for executing", &m_exec);
	insertString( "-xmlcomp", "The command for compiling", &m_compile);
	insertString( "-xmlname", "The name of this tool", &m_name);
}

void EsyArgumentList::insertVariable(const string & opt, 
	EsyArgumentItem::ArgumentType type, const string & help, void * var, 
	const string & darg, bool dval, const string & extension)
{
	push_back(EsyArgumentItem(opt, type, help, var, darg, dval, extension));
}

#define INSERTVARIABLE(Name, ArguT, PointerT) \
void EsyArgumentList::insert##Name(const string & opt, const string & help, \
	PointerT * var, const string & darg, bool dval) \
{ \
	insertVariable(opt, EsyArgumentItem::ArguT, help, (void *)var, darg, dval); \
}
INSERTVARIABLE(Long, LONG_TYPE, long)
INSERTVARIABLE(Double, DOUBLE_TYPE, double)
INSERTVARIABLE(Bool, BOOL_TYPE, bool)
INSERTVARIABLE(String, STRING_TYPE, string)
INSERTVARIABLE(StringVector, STRING_VECTOR_TYPE, vector<string>)
INSERTVARIABLE(LongVector, LONG_VECTOR_TYPE, vector<long>)
INSERTVARIABLE(DoubleVector, DOUBLE_VECTOR_TYPE, vector<double>)
INSERTVARIABLE(Enum, ENUM_TYPE, EsyArgumentEnum)
#undef INSERTVARIABLE

#define INSERTVARIABLE(Name, ArguT, PointerT) \
void EsyArgumentList::insert##Name(const string & opt, const string & help, \
	PointerT * var, const string & darg, bool dval, const string & extension) \
{ \
	insertVariable(opt, EsyArgumentItem::ArguT, help, (void *)var, \
		darg, dval, extension); \
}
INSERTVARIABLE(NewFile, NEW_FILE_TYPE, string)
INSERTVARIABLE(OpenFile, OPEN_FILE_TYPE, string)
INSERTVARIABLE(NewFileIF, NEW_FILE_IF_TYPE, string)
INSERTVARIABLE(OpenFileIF, OPEN_FILE_IF_TYPE, string)
#undef INSERTVARIABLE

int EsyArgumentList::findArgu(const string & opt)
{
	for ( size_t i = 0; i < size(); i ++ )
	{
		if ( at( i ).opt() == opt )
		{
			return (int)i;
		}
	}
	return -1;
}

EsyArgumentList::AnalyseResultCode EsyArgumentList::analyse(char * str)
{
	char t_str[4096];
#ifdef _WIN32
	strcpy_s(t_str, str);
#else
	strcpy(t_str, str);
#endif
	char * p, *pnext;
	int argc = 0;
#ifdef _WIN32
	p = strtok_s(t_str, " ", &pnext);
#else
						
	p = strtok_r(t_str, " ", &pnext);
#endif
	while ( p )
	{
		argc ++;
#ifdef _WIN32
		p = strtok_s(NULL, " ", &pnext);
#else
		p = strtok_r(NULL, " ", &pnext);
#endif
	}

	if ( argc == 0 )
	{
						
						
		return RESULT_OK;
	}

	char ** argv = (char **) calloc( argc, sizeof( char * ) );
	if ( argv == 0 )
	{
		cerr << "Can not calloc free space" << endl;
		return RESULT_ERROR;
	} /* if ( argv == 0 ) */

	int t = 0;
#ifdef _WIN32
	strcpy_s(t_str, str);
	argv[ t ] = strtok_s(t_str, " ", &pnext);
#else
	strcpy(t_str, str);
	argv[ t ] = strtok_r(t_str, " ", &pnext);
#endif
	for( t = 1; t < argc ; t ++ )
	{
#ifdef _WIN32
		argv[t] = strtok_s(NULL, " ", &pnext);
#else
		argv[t] = strtok_r(NULL, " ", &pnext);
#endif
	}

	EsyArgumentList::AnalyseResultCode r =
						 analyse(argc, argv);

	free (argv);

	return r;
}

EsyArgumentList::AnalyseResultCode EsyArgumentList::analyse(
		int argc, char * const argv [] )
{
	bool m_xml = false;
	for ( int seq = 1; seq < argc; seq ++ 
						)
	{
		string t_opt = argv[ seq ];

		if ( t_opt == "-xml" )
		{
			m_xml = true;
		}
		long t_opt_seq = findArgu( t_opt );

		if ( t_opt_seq == -1 )
		{
			cerr << "Wrong argument " << t_opt << endl;
			printHelp2Console( cout );
			return RESULT_ERROR;
		}

		int next_seq = seq;
		int length = 0;
		switch ( at( t_opt_seq ).type() )
		{
		case EsyArgumentItem::NEW_FILE_TYPE:
		case EsyArgumentItem::OPEN_FILE_TYPE:
		case EsyArgumentItem::NEW_FILE_IF_TYPE:
		case EsyArgumentItem::OPEN_FILE_IF_TYPE:
		case EsyArgumentItem::STRING_TYPE:
		case EsyArgumentItem::LONG_TYPE:
		case EsyArgumentItem::DOUBLE_TYPE:
		case EsyArgumentItem::ENUM_TYPE:
			if ( next_seq == argc - 1 )
			{
				cerr << "wrong architecture of argument " << t_opt << endl;
				printHelp2Console( cout );
				return RESULT_ERROR;
			}
			next_seq ++;
			length ++;
			break;
		case EsyArgumentItem::BOOL_TYPE:
			break;
		case EsyArgumentItem::STRING_VECTOR_TYPE:
		case EsyArgumentItem::LONG_VECTOR_TYPE:
		case EsyArgumentItem::DOUBLE_VECTOR_TYPE:
			for ( next_seq = next_seq + 1; next_seq < argc;
				  next_seq ++, length ++ )
			{
				long t_next_opt_seq = findArgu( argv[ next_seq ] );
				if ( t_next_opt_seq >= 0 )
				{
					next_seq --;
					break;
				}
			}
			break;
		default:
			cerr << "Unknown or unsupported type " << t_opt << endl;
			printHelp2Console( cout );
			return RESULT_ERROR;
			break;
		}

		switch( at( t_opt_seq ).type() )
		{
		case EsyArgumentItem::NEW_FILE_TYPE:
		case EsyArgumentItem::OPEN_FILE_TYPE:
		case EsyArgumentItem::NEW_FILE_IF_TYPE:
		case EsyArgumentItem::OPEN_FILE_IF_TYPE:
		case EsyArgumentItem::STRING_TYPE:
			analyseString( t_opt_seq, argv[ seq + 1 ] );
			break;
		case EsyArgumentItem::ENUM_TYPE:
			analyseEnum(t_opt_seq, argv[seq + 1]);
			break;
		case EsyArgumentItem::LONG_TYPE:
			analyseLong( t_opt_seq, argv[ seq + 1 ] );
			break;
		case EsyArgumentItem::DOUBLE_TYPE:
			analyseDouble( t_opt_seq, argv[ seq + 1 ] );
			break;
		case EsyArgumentItem::BOOL_TYPE:
			analyseBool( t_opt_seq );
			break;
		case EsyArgumentItem::STRING_VECTOR_TYPE:
			analyseStringVector( t_opt_seq, &(argv[ seq + 1]), length );
			break;
		case EsyArgumentItem::LONG_VECTOR_TYPE:
			analyseLongVector( t_opt_seq, &(argv[ seq + 1]), length );
			break;
		case EsyArgumentItem::DOUBLE_VECTOR_TYPE:
			analyseDoubleVector( t_opt_seq, &(argv[ seq + 1]), length );
			break;
		default:
			cerr << "Unknown or unsupported type " << t_opt << endl;
			printHelp2Console( cout );
			return RESULT_ERROR;
			break;
		}

		seq = next_seq;
	}

	if ( m_help )
	{
		printHelp2Console( cout );
		return RESULT_HELP;
	}

	if ( m_xml )
	{
		print2Xml();
		return RESULT_XML;
	}

	return RESULT_OK;
}

#define ANALYSESINGLE(ArguT, Name) \
void EsyArgumentList::analyse##Name( long seq, char * argv ) \
{ \
	ArguT * tp_var = (ArguT *)(at( seq ).var()); \
	*tp_var = EsyConvert(string(argv)); \
}
ANALYSESINGLE(long, Long)
ANALYSESINGLE(double, Double)
#undef ANALYSESINGLE

void EsyArgumentList::analyseString(long seq, char * argv)
{
	string * tp_var = (string *)(at( seq ).var() );
	*tp_var = string( argv );
}

void EsyArgumentList::analyseEnum(long seq, char * argv)
{
	EsyArgumentEnum * tp_var = (EsyArgumentEnum *)(at(seq).var());
	*tp_var = string( argv );
}

void EsyArgumentList::analyseBool(long seq)
{
	bool * tp_var = (bool *)(at( seq ).var() );
	*tp_var = true;
}

#define ANALYSEVECTOR(ArguT, Name) \
void EsyArgumentList::analyse##Name##Vector( long seq, char * const argv[], long length ) \
{ \
	vector< ArguT > * tp_var = (vector< ArguT > *)(at( seq ).var() ); \
	tp_var->clear(); \
	for ( int i = 0; i < length; i ++ ) \
	{ \
		tp_var->push_back( EsyConvert( string( argv[ i ] ) ) );  \
	} \
}
ANALYSEVECTOR(string, String)
ANALYSEVECTOR(long, Long)
ANALYSEVECTOR(double, Double)
#undef ANALYSEVECTOR

void EsyArgumentList::printHelp2Console(ostream& os)
{
	os << "--------" << endl;
	os << m_head << endl;
	os << "--------" << endl;

	for ( size_t i = 0; i < size(); i ++ )
	{
		if ( at(i).opt() == "-xmlexec" || at(i).opt() == "-xmlcomp" ||
			 at(i).opt() == "-xmlname" )
		{
			continue;
		}
		os << at( i ).opt() << "\t" << at( i ).help() << endl;
		if (at(i).type() == EsyArgumentItem::ENUM_TYPE)
		{
			EsyArgumentEnum * enum_t = (EsyArgumentEnum *)(at(i).var());
			os << "\t\tOptions: " << enum_t->optionList() << endl;
		}
	}
	os << "--------" << endl;
}

void EsyArgumentList::printValue2Console(ostream& os)
{
	os << "---- ----" << endl;
	for ( size_t i = 0; i < size(); i ++ )
	{
		if ( at(i).opt() == "-xmlexec" || at(i).opt() == "-xmlcomp" ||
			 at(i).opt() == "-xmlname" )
		{
			continue;
		}

		os << at( i ).opt() << "\t" <<
			  at( i ).value2Str( "{", ",", "}" ) << endl;
	}
	os << "---- ----" << endl;
}

string EsyArgumentList::print2Command(const string & proName)
{
	ostringstream out;
	out << proName << " ";
	for ( size_t i = 0; i < size(); i++ )
	{
		if ( at(i).opt() == "-xml" || at(i).opt() == "-help" ||
			 at(i).opt() == "-xmlexec" || at(i).opt() == "-xmlcomp" ||
			 at(i).opt() == "-xmlname" )
		{
			continue;
		}

		if ( at( i ).type() != EsyArgumentItem::BOOL_TYPE )
		{
			out << at( i ).opt() << " " << at( i ).value2Str( "", " ", "" ) << " ";
		}
		else
		{
			if ( *( (bool *)(at( i ).var()) ) )
			{
				out << at( i ).opt() << " ";
			}
		}
	}

	return out.str();
}

void EsyArgumentList::print2Xml()
{
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration(
		ARGUMENT_DECLARATION_VERSION, ARGUMENT_DECLARATION_ENCODING, "" );
	doc.LinkEndChild( decl );

	TiXmlElement * root = new TiXmlElement( ARGUMENT_ROOT );
	root->SetAttribute( ARGUMENT_ROOT_NAME, m_name.c_str() );
	root->SetAttribute( ARGUMENT_ROOT_VERSION, m_version.c_str() );
	root->SetAttribute( ARGUMENT_ROOT_EXEC, m_exec.c_str() );
	root->SetAttribute( ARGUMENT_ROOT_COMPILE, m_compile.c_str() );
	doc.LinkEndChild( root );

	for ( size_t i = 0; i < size(); i++ )
	{
		if ( at(i).opt() == "-xml" || at(i).opt() == "-help" ||
			 at(i).opt() == "-xmlexec" || at(i).opt() == "-xmlcomp" ||
			 at(i).opt() == "-xmlname" )
		{
			continue;
		}

		TiXmlElement * argu_item = new TiXmlElement( ARGUMENT_ARGU );
		argu_item->SetAttribute( ARGUMENT_ARGU_ARGUMENT, at( i ).opt().c_str() );
		argu_item->SetAttribute( ARGUMENT_ARGU_LOOP, "" );
		argu_item->SetAttribute( ARGUMENT_ARGU_VALUE,
			at( i ).value2Str("", " ", "").c_str() );

		ostringstream help_str;
		help_str << at( i ).help();
		if (at(i).type() == EsyArgumentItem::ENUM_TYPE)
		{
			EsyArgumentEnum * enum_t = (EsyArgumentEnum *)(at(i).var());
			help_str << endl << "Options: " << enum_t->optionList();
		}

		argu_item->SetAttribute( ARGUMENT_ARGU_NOTE, help_str.str().c_str() );
		argu_item->SetAttribute( ARGUMENT_ARGU_TYPE, (int)(at(i).type()) );
		argu_item->SetAttribute( ARGUMENT_ARGU_DEPARG, 
			at(i).dependExp().c_str() );
		argu_item->SetAttribute( ARGUMENT_ARGU_DEPVAL, (int)(at(i).tab()) );
		argu_item->SetAttribute( ARGUMENT_ARGU_EXTENSION, 
			at(i).extension().c_str());
		root->LinkEndChild( argu_item );
	}

	string t_name = m_xml_str + "." + LINKCFG_EXTENSION;
	doc.SaveFile( t_name.c_str() );
    
    doc.Clear();
}
