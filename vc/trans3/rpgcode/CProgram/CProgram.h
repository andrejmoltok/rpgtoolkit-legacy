/*
 * All contents copyright 2005, Colin James Fitzpatrick.
 * All rights reserved. You may not remove this notice.
 * Read license.txt for licensing details.
 */

/*
 * Protect the header.
 */
#ifndef _CPROGRAM_H_
#define _CPROGRAM_H_

/*
 * Plugin data types.
 */
#define PLUG_DT_VOID -1		// Void data element
#define PLUG_DT_NUM	0		// Numerical data element
#define PLUG_DT_LIT	1		// Literal data element

/*
 * Inclusions.
 */
#include <vector>
#include <stack>
#include <map>
#include <string>
#include <sstream>
#include "../CVariant/CVariant.h"
#include "../../input/input.h"
#include "../../plugins/CPlugin.h"

/*
 * A loaded program.
 */
class CProgram
{

/*
 * Public visibility.
 */
public:

	/*
	 * A vector of lines.
	 */
	typedef std::vector<std::string> VECTOR_STR;

	/*
	 * An internal function.
	 */
	typedef const std::vector<CVariant> &PARAMETERS;
	typedef CVariant(*INTERNAL_FUNCTION)(PARAMETERS, CProgram *const);

	/*
	 * A method.
	 */
	typedef struct tagMethod
	{
		tagMethod(void):
			params(),
			lines(),
			name(""),
			func(NULL) { }
		VECTOR_STR params;		// Parameters the method takes.
		VECTOR_STR lines;		// The program.
		INTERNAL_FUNCTION func;	// Function to call.
		std::string name;		// Name of function.
	} METHOD;

	/*
	 * Default constructor.
	 */
	CProgram(void)
	{
		/*
		 * Does nothing yet.
		 */
	}

	/*
	 * Construct from a program file.
	 *
	 * str (in) - file to construct from
	 */
	CProgram(const std::string str)
	{
		open(str);
	}

	/*
	 * Open a program.
	 *
	 * file (in) - file to open
	 */
	void open(const std::string file);

	/*
	 * Run the program.
	 *
	 * return (out) - program exit return
	 */
	CVariant run(void);

	/*
	 * Report an error.
	 *
	 * str (in) - string to show
	 */
	static void debugger(const std::string str)
	{
		CProgram *prg = getCurrentProgram();
		std::stringstream ss;
		ss	<< "Line "
			<< prg->m_currentLine + 1
			<< "; "
			<< (*prg->m_process)[prg->m_currentLine]
			<< std::endl
			<< str
			<< std::endl;
		MessageBox(NULL, ss.str().c_str(), "RPGCode Error", 0);
	}

	/*
	 * Independently run a line.
	 *
	 * str (in) - line to run
	 */
	void CProgram::runLine(const std::string str);

	/*
	 * Call a function.
	 *
	 * funcName (in) - function to call
	 * params (in) - vector of parameters to pass
	 * return (out) - return value
	 */
	CVariant callFunction(const std::string funcName, PARAMETERS params);

	/*
	 * Call a function from a METHOD object.
	 *
	 * method (in) - method to call
	 * params (in) - parameters to pass
	 * return (out) - return value
	 */
	CVariant callFunction(const METHOD &method, PARAMETERS params);

	/*
	 * Construct a variant of the correct type from a string.
	 *
	 * str (in) - string to construct from
	 * bFromVar (out) - constructed from a variable?
	 * return (out) - the variant
	 */
	CVariant constructVariant(const std::string str, bool *bFromVar = NULL);

	/*
	 * Add an internal function.
	 *
	 * str (in) - name of function
	 * func (in) - address of function
	 */
	static void addFunction(const std::string str, const INTERNAL_FUNCTION func);

	/*
	 * Get the current program.
	 */
	static CProgram *const getCurrentProgram(void) { return m_currentProgram; }

	/*
	 * Set a variable.
	 */
	void setVariable(const std::string name, const CVariant value);
	static void setGlobal(const std::string name, const CVariant value);

	/*
	 * Get a global.
	 */
	static CVariant getGlobal(const std::string name);

	/*
	 * End the program.
	 */
	void end(void)
	{
		m_process = &m_lines;
		m_currentLine = m_process->size() + 1;
	}

	/*
	 * Plugins.
	 */
	static CPlugin *addPlugin(const std::string file);
	static void freePlugins(void);

/*
 * Private visibility.
 */
private:

	/*
	 * Parse an array.
	 */
	std::string parseArray(const std::string str);

	/*
	 * Evaluate a string.
	 *
	 * str (in) - string to evaluate
	 * return (out) - result of evaluation
	 */
	CVariant evaluate(const std::string str);

	/*
	 * Run a block of lines.
	 *
	 * lines (in) - block to run
	 */
	void run(const VECTOR_STR &lines)
	{
		const VECTOR_STR *const oldProcess = m_process;
		m_process = &lines;
		const int len = lines.size();
		const int i = m_currentLine;
		for (m_currentLine = 0; m_currentLine < len; m_currentLine++)
		{
			/*
			 * Evaluate this line.
			 */
			processEvent();
			evaluate(lines[m_currentLine]);
		}
		m_currentLine = i;
		m_process = oldProcess;
	}

	/*
	 * Break a string into multiple lines.
	 *
	 * if (x) { func(x); foo(x); }
	 *
	 * ...becomes:
	 *
	 * if (x)
	 * {
	 * func(x)
	 * foo(x)
	 * }
	 *
	 * str (in) - string to break
	 * bComment (in + out) - whether the line is within a comment
	 * lines (out) - resulting lines
	 */
	void CProgram::breakString(const std::string str, bool &bComment, VECTOR_STR &lines);

	/*
	 * Run a block of code.
	 *
	 * bRun (in) - run?
	 */
	void runBlock(const bool bRun);

	/*
	 * A class.
	 */
	typedef struct tagClass: CVariant::CObject
	{
		typedef struct tagScope
		{
			std::map<std::string, CVariant> m_members;	// Members.
			std::map<std::string, METHOD> m_methods;	// Methods.
		} SCOPE;
		static const int PUBLIC, PRIVATE;
		SCOPE scopes[2];
		double getNum(void) { return 0.0; }
		std::string getLit(void) { return ""; }
		CVariant::DATA_TYPE getType(void) { return CVariant::DT_NUM; }
	} CLASS;

	/*
	 * The actual contents of the program.
	 */
	VECTOR_STR m_lines;
	int m_currentLine;

	/*
	 * Methods in the program.
	 */
	std::map<std::string, METHOD> m_methods;
	std::map<std::string, CLASS> m_classes;

	/*
	 * The stack.
	 */
	typedef std::map<std::string, CVariant> STACK_FRAME;
	std::vector<STACK_FRAME> m_stack;
	static STACK_FRAME m_global;
	const VECTOR_STR *m_process;
	std::vector<CLASS *> m_objects;
	std::stack<CLASS *> m_this;

	/*
	 * Internal functions.
	 */
	static std::map<std::string, INTERNAL_FUNCTION> m_functions;

	/*
	 * The current program.
	 */
	static CProgram *m_currentProgram;

	/*
	 * Plugins.
	 */
	static std::vector<CPlugin *> m_plugins;

};

#endif
