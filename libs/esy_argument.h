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

#ifndef INTERFACE_ARGUMENT_H
#define INTERFACE_ARGUMENT_H

#include <string>
#include <vector>

using namespace std;

/** @defgroup argumentinterface Argument Interface. */

/**
* @ingroup argumentinterface
* @brief Enumerate item in argument list. Convert from name to enuerate value directly.
*
* Holds parameters and values of one enumerate. Return the value of enumerate if 
* set the string value or digital value.
*/
class EsyArgumentEnum
{
private:
	vector< string > m_enum_str;  /**< list of name of enumerate */
	vector< long > m_enum_value;  /**< list of value of enumerate */
	long m_value;  /**< current value of enumerate */

public:
	/*!
	* \brief Constructs an empty item.
	* \param def   default value.
	*/
	EsyArgumentEnum( long def ) :
		m_enum_str(), m_enum_value(), m_value( def )
	{}

	/*!
	* \brief Add options in enumerate list.
	* \param value  value of this opion.
	* \param str    name of this option.
	*/
	void addOption(long value, const string & str)
	{
		m_enum_str.push_back(str);
		m_enum_value.push_back(value);
	}

	string optionList()
	{
		string str;
		for (size_t i = 0; i < m_enum_str.size(); i++)
		{
			str = str + m_enum_str[i] + ", ";
		}
		return str;
	}

	/** @name Functions to overload operators. */
	///@{
	/*!
	* \brief Access current value #m_value.
	* \return current value #m_value.
	*/
	operator long() const { return m_value; }
	/*!
	* \brief Access current value #m_value and convert it to string.
	* \return enumerate name.
	*/
	operator string();
	/*!
	* \brief Convert enumerate name #str and set current value #m_value.
	* \param str  enumerate name.
	*/
	void operator =(const string & str);
	/*!
	* \brief Set current value #m_value.
	* \param value  enumerate value.
	*/
	void operator =(long value) { m_value = value; }
    ///@}
};

/**
 * @ingroup argumentinterface
 * @brief Item in argument list.
 *
 * Holds parameters and value of one argument. Each item contains following
 * information:
 * - Option string, usually start with '-', e.g. "-name';
 * - Variable type of argument;
 * - String of help information;
 * - Pointer to the argument variable, no specified type.
 *
 * \sa ArgumentList
 */
class EsyArgumentItem
{
public:
	/*!
	 * \brief Variable type of argument.
	 */
	enum ArgumentType
	{
		STRING_TYPE,         /**< string */
		LONG_TYPE,           /**< long, integer variable */
		DOUBLE_TYPE,         /**< double, float variable */
		BOOL_TYPE,           /**< bool */
		STRING_VECTOR_TYPE,  /**< vector< string > */
		LONG_VECTOR_TYPE,    /**< vector< long > */
		DOUBLE_VECTOR_TYPE,  /**< vector< double > */
		NEW_FILE_TYPE,       /**< string, file name of a new file */
		OPEN_FILE_TYPE,      /**< string, file name of a exist file */
		NEW_FILE_IF_TYPE,    /**< string, file name of a new file using
			data file interface */
		OPEN_FILE_IF_TYPE,   /**< string, file name of a exist file using
			data file interface */
		ENUM_TYPE            /**< string, name of enumerate and convert
			name to value */
	};

protected:
	string m_opt;  /**< \brief Option string.

		Access functions: #opt() */
	ArgumentType m_type;  /**< \brief Variable type.

		Access functions: #type() */
	string m_help;  /**< \brief Help string.

		Access functions: #help() */
	void * mp_var;  /**< \brief Pointer to the variable.

		Access functions: #var() */
	string m_depend_exp;  /**< \brief Regular express to describe depended 
		argument.

		Access functions: #dependExp() */
	long m_tab;  /**< \brief Value of tab level.

		Access functions: #tab() */

	string m_extension; /**< \brief Extension name of file interface.
	
		Access functions: #extension() */

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item.
	 */
	EsyArgumentItem() :
		m_opt( "" ), m_type( STRING_TYPE ), m_help( "" ), mp_var( 0 ),
		m_depend_exp( "" ), m_tab( 0 )
	{}
	/*!
	 * \brief Constructs an item with specified value for fields.
	 * \param opt  option string.
	 * \param type variable type.
	 * \param help help string.
	 * \param var  pointer to variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	EsyArgumentItem(
		const string & opt, ArgumentType type, const string & help, void * var,
		const string & darg = "", long dval = 0, const string & extension = ""
	) :
		m_opt( opt ), m_type( type ), m_help( help ), mp_var( var ),
		m_depend_exp( darg ), m_tab( dval ), m_extension( extension )
	{}
	///@}

	/** @name Functions to access variables. */
	///@{
	/*!
	 * \brief Access option string #m_opt.
	 * \return option string #m_opt.
	 */
	const string & opt() const { return m_opt; }
	/*!
	 * \brief Access variable type #m_type.
	 * \return variable type #m_type.
	 */
	ArgumentType type() const { return m_type; }
	/*!
	 * \brief Access help string #m_help.
	 * \return help string #m_help.
	 */
	const string & help() const { return m_help; }
	/*!
	 * \brief Access pointer to the variable #mp_var.
	 * \return pointer to the variable #mp_var .
	 */
	void * var() const { return mp_var; }
	/*!
	 * \brief Access to the dependency expersion #m_depend_exp.
	 * \return dependency expersion #m_depend_exp.
	 */
	const string & dependExp() const { return m_depend_exp; }
	/*!
	 * \brief Access to the tab level #m_tab.
	 * \return tab level #m_tab.
	 */
	long tab() const { return m_tab; }
	/*!
	 * \brief Access to the extension name #m_extension.
	 * \return extension name #m_extension .
	 */
	const string & extension() const { return m_extension; }
	///@}

	/*!
	 * \brief Converts the value of argument to string.
	 *
	 * If the argument is not vecter type (STRING_TYPE, LONG_TYPE, DOUBLE_TYPE,
	 * BOOL_TYPE, NEW_FILE_TYPE, OPEN_FILE_TYPE, NEW_FILE_IF_TYPE,
	 * OPEN_FILE_IF_TYPE), the string is the value. For BOOL_TYPE, the string
	 * is TRUE or FALSE.
	 * Otherwise, if the argument is vector type (STRING_VECTOR_TYPE,
	 * LONG_VECTOR_TYPE, DOUBLE_VECTOR_TYPE), the string starts with the head
	 * and ends with end. In the string, values are separated by dec.
	 *
	 * \param head start of the string for vecter type.
	 * \param dec  separator of the values for vector type.
	 * \param end  end of the string for vector type.
	 * \return string of value.
	 *
	 * \sa value2StrBool(), value2StrDouble(), value2StrDoubleVector(),
	 * value2StrLong(), value2StrLongVector(), value2StrString(),
	 * value2StrStringVector()
	 */
	string value2Str(string head, string dec, string end);

protected:
	/** @name Function to covert value to string */
	///@{
	/*!
	 * \brief Coverts value to string for string type argument.
	 * \return string of value.
	 *
	 * \sa value2Str()
	 */
	string value2StrString();
	/*!
	 * \brief Coverts value to string for long type argument.
	 * \return string of value.
	 *
	 * \sa value2Str()
	 */
	string value2StrLong();
	/*!
	 * \brief Coverts value to string for double type argument.
	 * \return string of value.
	 *
	 * \sa value2Str()
	 */
	string value2StrDouble();
	/*!
	 * \brief Coverts value to string for bool type argument.
	 * \return string of value, TRUE or FALSE.
	 *
	 * \sa value2Str()
	 */
	string value2StrBool();
	/*!
	* \brief Coverts value to string for enumerate type argument.
	* \return name of enumerate
	*
	* \sa value2Str()
	*/
	string value2StrEnum();
	/*!
	 * \brief Coverts value to string for string vector type argument.
	 * \param head start of the string.
	 * \param dec  separator of the values.
	 * \param end  end of the string.
	 * \return string of value.
	 *
	 * \sa value2Str()
	 */
	string value2StrStringVector(string head, string dec, string end);
	/*!
	 * \brief Coverts value to string for long vector type argument.
	 * \param head start of the string.
	 * \param dec  separator of the values.
	 * \param end  end of the string.
	 * \return string of value.
	 *
	 * \sa value2Str()
	 */
	string value2StrLongVector(string head, string dec, string end);
	/*!
	 * \brief Coverts value to string for double vector type argument.
	 * \param head start of the string.
	 * \param dec  separator of the values.
	 * \param end  end of the string.
	 * \return string of value.
	 *
	 * \sa value2Str()
	 */
	string value2StrDoubleVector(string head, string dec, string end);
	};

/**
 * @ingroup argumentinterface
 * @brief Interface to the arguments from command line.
 *
 * This interface parses the arguments from command line and holds the values of
 * arguments for further use in program.
 *
 * To use this interface, users should create an entity at first. Then, register
 * arguments and variables by insert functions. After that, call #analyse() to
 * parse the arguments. If the parsec succeed, the values of argments are
 * already stored in variables.
 *
 * An empty interface already contains several arguments:
 * <table>
 * <tr>
 *   <td> -help </td>
 *   <td> print help information. </td>
 * </tr>
 * <tr>
 *   <td> -xml </td>
 *   <td> print argument XML file. </td>
 * </tr>
 * <tr>
 *   <td> -xmlexec </td>
 *   <td> command to execute. </td>
 * </tr>
 * <tr>
 *   <td> -xmlcomp </td>
 *   <td> command to compile. </td>
 * </tr>
 * <tr>
 *   <td> -xmlname </td>
 *   <td> name of this tool. </td>
 * </tr>
 * </table>
 * "-xml" can print the argument XML file used to link this tool to VisualNoC.
 * The command looks like
 * \code{.sh}
 * -xml <xml file path> -xmlexec <command to execute> -xmlcomp < command to compile > -xmlname <name of this tool>
 * \endcode
 *
 * Arguments can be printed to console, command line and xml file as well.
 *
 * \sa ArgumentItem
 */
class EsyArgumentList : public vector< EsyArgumentItem >
{
public:
	/*! \brief The result code of parse function. */
	enum AnalyseResultCode
	{
		RESULT_HELP,  /**< print help, triggered by "-help" */
		RESULT_XML,   /**< print argument xml file, triggered by "-xml" */
		RESULT_ERROR, /**< errors occured during parse */
		RESULT_OK     /**< parse functions correct and arguments have valid
			value */
	};

protected:
	string m_head;    /**< \brief Head string of the argument list.

		Includes a brief introduction of program usually.

		Access functions: #setHead()  */
	string m_version; /**< \brief Version code of the program.

		Access functions: #setVersion()  */

	string m_exec;  /**< \brief Execution command.

		Variable registers to argument -xmlexec.
		\sa #insertDefaultArgument() */
	string m_compile; /**< \brief Compile command.

		Variable registers to argument -xmlcomp.
		\sa #insertDefaultArgument() */
	string m_name;  /**< \brief Program name

		Variable registers to argument -xmlname.
		\sa #insertDefaultArgument() */
	string m_xml_str; /**< \brief Path of xml argument file.

		Variable registers to argument -xml.
		\sa #insertDefaultArgument() */
	bool m_help; /**< \brief Whether option "-help" is used.

		Variable registers to argument -help.
		\sa #insertDefaultArgument() */

public:
	/** @name Constructor */
	///@{
	/*!
	 * \brief Constructs an empty item.
	 */
	EsyArgumentList();
	/*!
	 * \brief Constructs an item with specified value for head and version.
	 *
	 * Register default arguments in the list.
	 *
	 * \param head    head of argument list
	 * \param version version of program
	 *
	 * \sa insertDefaultArgument()
	 */
	EsyArgumentList(const string & head, const string & version);
	///@}

	/** @name Functions to access variables. */
	///@{
	/*!
	 * \brief Sets head string of argument list #m_head.
	 * \param head head string of argument list.
	 */
	void setHead(const string & head) { m_head = head; }
	/*!
	 * \brief Sets version of program #m_version.
	 * \param version version of program.
	 */
	void setVersion(const string & version) { m_version = version; }
	///@}

	/*!
	 * \brief register default arguments to argument list.
	 *
	 * Default arguments includes: -help, -xml, -xmlexec, -xmlcomp, -xmlname.
	 *
	 * \sa #m_help #m_xml_str, #m_exec, #m_compile, #m_name
	 */
	void insertDefaultArgument();

	/** @name Functions to register arguments and variables. */
	///@{
	/*!
	 * \brief Registers a string type argument.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	void insertString(const string & opt, const string & help, string * var,
		const string & darg = "", bool dval = false );
	/*!
	 * \brief Registers a long type argument.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	void insertLong(const string & opt, const string & help, long * var,
		const string & darg = "", bool dval = false );
	/*!
	 * \brief Registers a double type argument.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	void insertDouble(const string & opt, const string & help, double * var,
		const string & darg = "", bool dval = false );
	/*!
	 * \brief Registers a bool type argument.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	void insertBool(const string & opt, const string & help, bool * var,
		const string & darg = "", bool dval = false );
	/*!
	 * \brief Registers an argument for new file.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 * \param extension extension name.
	 */
	void insertNewFile( const string & opt, const string & help, string * var,
		const string & darg = "", bool dval = false, 
		const string & extension = "" );
	/*!
	 * \brief Registers an argument for exist file.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 * \param extension extension name.
	 */
	void insertOpenFile(const string & opt, const string & help, string * var,
		const string & darg = "", bool dval = false,
		const string & extension = "" );
	/*!
	 * \brief Registers an argument for new file by data file interface.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 * \param extension extension name.
	 */
	void insertNewFileIF(const string & opt, const string & help, string * var,
		const string & darg = "", bool dval = false,
		const string & extension = "" );
	/*!
	 * \brief Registers an argument for exist file by data file interface.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 * \param extension extension name.
	 */
	void insertOpenFileIF(const string & opt, const string & help, string * var,
		const string & darg = "", bool dval = false,
		const string & extension = "" );
	/*!
	 * \brief Registers a string vector type argument.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	void insertStringVector(const string & opt, const string & help, 
		vector<string> * var, const string & darg = "", bool dval = false );
	/*!
	 * \brief Registers a long vector type argument.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	void insertLongVector(const string & opt, const string & help, 
		vector<long> * var, const string & darg = "", bool dval = false );
	/*!
	 * \brief Registers a double vector type argument.
	 * \param opt  option string.
	 * \param help help string.
	 * \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	void insertDoubleVector(const string & opt, const string & help, 
		vector<double> * var, const string & darg = "", bool dval = false );
	/*!
	* \brief Registers an enumerate type argument.
	* \param opt  option string.
	* \param help help string.
	* \param var  pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	*/
	void insertEnum(const string & opt, const string & help, 
		EsyArgumentEnum * var, const string & darg = "", bool dval = false );
	///@}

	/** @name Functions to parse the arguments from commands. */
	///@{
	/*!
	 * \brief Parses the arguments in commands line.
	 *
	 * Command line has been divided into several segments without space
	 * already. Parser goes through all strings, locates the option and values
	 * of each arguments and call functions to parse the value according to the
	 * type of arguments.
	 *
	 * If the command line contains "-help", return RESULT_HELP; If the command
	 * line contains "-xml", return RESULT_XML; If there is any error during
	 * parse, including unknown type, wrong number of values, return
	 * RESULT_ERROR. Otherwise, return RESULT_OK.
	 *
	 * \param argc  count of strings in commands
	 * \param argv  pointer of strings in commands
	 * \return  result code of parse.
	 *
	 * \sa #analyse(char* argv), #analyseString(), #analyseLong(),
	 * #analyseDouble(), #analyseBool(), #analyseStringVector(),
	 * #analyseLongVector(), #analyseDoubleVector()
	 */
	AnalyseResultCode analyse(int argc, char * const argv []);
	/*!
	 * \brief Parses the arguments in commands line.
	 *
	 * Command line is in one line. This function divided the command line into
	 * several segments before call #analyse(int argc, char* const argv[]) to
	 * parse arguments.
	 *
	 * \param argv  pointer of command line
	 * \return  result code of parse.
	 *
	 * \sa #analyse(int argc, char* const argv[])
	 */
	AnalyseResultCode analyse(char * argv);
	///@}

	/*!
	 * \brief Prints help information to console.
	 *
	 * Option -xmlexec, -xmlcomp, -xmlname are not printed. The format is below:
	 * \code{.sh}
	 * --------
	 * <head string>
	 * --------
	 * <option string>  <help string>
	 * --------
	 * \endcode
	 * \param os output stream to console.
	 */
	void printHelp2Console(ostream& os);
	/*!
	 * \brief Prints the value of arguments to console.
	 *
	 * Option -xmlexec, -xmlcomp, -xmlname are not printed. The format is below:
	 * \code{.sh}
	 * --------
	 * <head string>
	 * --------
	 * <option string>  <value string>
	 * --------
	 * \endcode
	 * Value string starts with '{', ends with '}' and sperates by ','
	 *
	 * \param os output stream to console.
	 */
	void printValue2Console(ostream& os);
	/*!
	 * \brief Prints the arguments to xml argument file.
	 *
	 * Using TinyXML to generate the XML file.
	 */
	void print2Xml();
	/*!
	 * \brief Print the value of arguments to a command line format.
	 *
	 * Option -xmlexec, -xmlcomp, -xmlname are not printed. The format is below:
	 * \code{.sh}
	 * <proName> <option string> <value string> ...
	 * \endcode
	 * \param proName  command name at start.
	 * \return string of command line.
	 */
	string print2Command(const string & proName);

protected:
	/*!
	 * \brief Registers an argument of any type.
	 * \param opt   option string.
	 * \param type  variable type.
	 * \param help  help string.
	 * \param var   pointer to the variable.
	 * \param darg dependent argument.
	 * \param dval value of dependent argument.
	 */
	void insertVariable(const string & opt, EsyArgumentItem::ArgumentType type,
		const string & help, void * var, const string & darg = "", 
		bool dval = false, const string & extension = "");
	/*!
	 * \brief Finds the index of the argument with option string opt.
	 * \param opt  option string to find.
	 * \return Returns the index of the match argument; If not find, return -1.
	 */
	int findArgu(const string & opt);

	/** @name Functions to convert strings to value of variable. */
	///@{
	/*!
	 * \brief Parses the value of string type argument.
	 * \param seq  index of the argument in list.
	 * \param argv string of this argument.
	 */
	void analyseString(long seq, char * argv);
	/*!
	 * \brief Parses the value of long type argument.
	 * \param seq  index of the argument in list.
	 * \param argv string of this argument.
	 */
	void analyseLong(long seq, char * argv);
	/*!
	 * \brief Parses the value of double type argument.
	 * \param seq  index of the argument in list.
	 * \param argv string of this argument.
	 */
	void analyseDouble(long seq, char * argv);
	/*!
	 * \brief Parses the value of bool type argument.
	 * \param seq  index of the argument in list.
	 */
	void analyseBool(long seq);
	/*!
	 * \brief Parses the value of string vector type argument.
	 * \param seq    index of the argument in list.
	 * \param argv   strings of this argument.
	 * \param length the number of strings of this argument.
	 */
	void analyseStringVector(long seq,  char * const argv[], long length);
	/*!
	 * \brief Parses the value of long vector type argument.
	 * \param seq    index of the argument in list.
	 * \param argv   strings of this argument.
	 * \param length the number of strings of this argument.
	 */
	void analyseLongVector(long seq,  char * const argv[], long length);
	/*!
	 * \brief Parses the value of double vector type argument.
	 * \param seq    index of the argument in list.
	 * \param argv   strings of this argument.
	 * \param length the number of strings of this argument.
	 */
	void analyseDoubleVector(long seq,  char * const argv[], long length);
	/*!
	 * \brief Parses the value of enum type argument.
	 * \param seq  index of the argument in list.
	 * \param argv string of this argument.
	 */
	void analyseEnum(long seq, char * argv);
	///@}
};

/** @} */

#endif
