#ifndef ZPARSER_SCOPE_H
#define ZPARSER_SCOPE_H

#include <assert.h>
#include "DataStructs.h"

class Scope
{
public:
	struct Variable
	{
		Variable(ZVarType const* type, string const& name, int id) : type(type), name(name), id(id) {}
		ZVarType const* type;
		string name;
		int id;
	};

	struct Function
	{
		Function(ZVarType const* returnType, string const& name, vector<ZVarType const*> paramTypes, int id)
				: returnType(returnType), name(name), paramTypes(paramTypes), id(id) {}
		ZVarType const* returnType;
		string name;
		vector<ZVarType const*> paramTypes;
		int id;
	};

	static Scope* makeGlobalScope(SymbolTable& table);

	Scope(SymbolTable& table);

	// SymbolTable
	SymbolTable const& getTable() const {return table;}
	SymbolTable& getTable() {return table;}

	// Others
	virtual bool isGlobal() const {return false;}

	////////////////
	// Virtual Methods

	// Inheritance
	virtual Scope* getParent() const = 0;
	virtual Scope* makeChild() = 0;
	virtual Scope* makeChild(string const& name) = 0;
	virtual Scope* getLocalChild(string const& name) const = 0;
	virtual vector<Scope*> getAnonymousChildren() const = 0;
	// Types
	virtual int getLocalTypeId(string const& name) const = 0;
	virtual int addType(string const& name, ZVarTypeId typeId, AST* node) = 0;
	// Classes
	virtual int getLocalClassId(string const& name) const = 0;
	virtual int addClass(string const& name, AST* node) = 0;
	// Variables
	virtual Variable* getLocalVariable(string const& name) const = 0;
	virtual Variable* addVariable(ZVarType const& type, string const& name, AST* node = NULL) = 0;
	// Properties
	virtual Function* getLocalGetter(string const& name) const = 0;
	virtual Function* getLocalSetter(string const& name) const = 0;
	virtual Function* addGetter(ZVarType const* returnType, string const& name,
								vector<ZVarType const*> const& paramTypes, AST* node = NULL) = 0;
	virtual Function* addSetter(ZVarType const* returnType, string const& name,
								vector<ZVarType const*> const& paramTypes, AST* node = NULL) = 0;
	// Functions
	virtual void getLocalFunctions(string const& name, vector<Function*>& out) const = 0;
	virtual Function* getLocalFunction(FunctionSignature const& signature) const = 0;
	virtual Function* addFunction(
			ZVarType const* returnType, string const& name,
			vector<ZVarType const*> const& paramTypes, AST* node = NULL) = 0;

	////////////////
	// Convenience Methods

	// Inheritance
	Scope& getOrMakeLocalChild(string const& name);
	Scope* getNamespace(string const& name) const;
	Scope* getLocalChild(vector<string> const& names) const;
	Scope* getChild(vector<string>const& names)const;
	// Types
	int getTypeId(string const& name) const;
	ZVarType* getLocalType(string const& name) const;
	ZVarType* getType(string const& name) const;
	int addType(string const& name, ZVarType const& type, AST* node);
	int addType(string const& name, ZVarTypeId typeId);
	int addType(string const& name, ZVarType const& type);
	// Classes
	int getClassId(string const& name) const;
	ZClass* getLocalClass(string const& name) const;
	ZClass* getClass(string const& name) const;
	int addClass(string const& name);
	// Variables
	Variable* getVariable(string const& name) const;
	Variable* getVariable(vector<string> const& name) const;
	// Properties
	Function* getGetter(string const& name) const;
	Function* getSetter(string const& name) const;
	// Functions
	vector<Function*> getLocalFunctions(string const& name) const;
	void getLocalFunctionIds(string const& name, vector<int>& out) const;
	vector<int> getLocalFunctionIds(string const& name) const;
	void getFunctions(string const& name, vector<Function*>& out) const;
	void getFunctionIds(string const& name, vector<int>& out) const;
	vector<Function*> getFunctions(string const& name) const;
	vector<int> getFunctionIds(string const& name) const;
	vector<Function*> getFunctions(vector<string> const& names)const;
	vector<int> getFunctionIds(vector<string> const& names)const;
	Function* getFunction(FunctionSignature const& signature) const;
	int getFunctionId(FunctionSignature const& signature) const;
	Function* addFunction(ZVarTypeId returnTypeId, string const& name, vector<ZVarTypeId> const& paramTypeIds, AST* node = NULL);

	bool varDeclsDeprecated;
protected:
	SymbolTable& table;
};

class BasicScope : public Scope
{
public:
	BasicScope(SymbolTable& table);
	BasicScope(Scope* parent);
	~BasicScope();
	// Children.
	Scope* getParent() const;
	Scope* makeChild();
	Scope* makeChild(string const& name);
	Scope* getLocalChild(string const& name) const;
	vector<Scope*> getAnonymousChildren() const;
	// Types
	int getLocalTypeId(string const& name) const;
	using Scope::addType;
	int addType(string const& name, ZVarTypeId typeId, AST* node);
	// Classes
	int getLocalClassId(string const& name) const;
	int addClass(string const& name, AST* node);
	// Variables
	Variable* getLocalVariable(string const& name) const;
	using Scope::addVariable;
	Variable* addVariable(ZVarType const& type, string const& name, AST* node = NULL);
	// Properties
	Function* getLocalGetter(string const& name) const;
	Function* getLocalSetter(string const& name) const;
	Function* addGetter(ZVarType const* returnType, string const& name, vector<ZVarType const*> const& paramTypes, AST* node = NULL);
	Function* addSetter(ZVarType const* returnType, string const& name, vector<ZVarType const*> const& paramTypes, AST* node = NULL);
	// Functions
	using Scope::getLocalFunctions;
	void getLocalFunctions(string const& name, vector<Function*>& out) const;
	Function* getLocalFunction(FunctionSignature const& signature) const;
	using Scope::addFunction;
	Function* addFunction(ZVarType const* returnType, string const& name,
						  vector<ZVarType const*> const& paramTypes, AST* node = NULL);
private:
    Scope* parent;
    map<string, Scope*> children;
	vector<Scope*> anonymousChildren;
	map<string, int> types;
	map<string, int> classes;
	map<string, Variable*> variables;
	map<string, Function*> getters;
	map<string, Function*> setters;
	map<string, vector<Function*> > functionsByName;
	map<FunctionSignature, Function*> functionsBySignature;

	// Disabled since it's easy to call by accident instead of the Scope*
	// constructor.
	BasicScope(BasicScope const& base) : Scope(base.table) {assert(false);}
};

class GlobalScope : public BasicScope
{
public:
	GlobalScope(SymbolTable& table);
	bool isGlobal() const {return true;}
};

enum ZClassIdBuiltin
{
	ZCLASSID_START = 0,
    ZCLASSID_GAME = 0, ZCLASSID_LINK, ZCLASSID_SCREEN,
    ZCLASSID_FFC, ZCLASSID_ITEM, ZCLASSID_ITEMCLASS, ZCLASSID_NPC, ZCLASSID_LWPN, ZCLASSID_EWPN,
	ZCLASSID_END
};

class ZClass : public BasicScope
{
public:
	ZClass(SymbolTable& table, string const& name, int id);
	string const name;
	int const id;
};

#endif
