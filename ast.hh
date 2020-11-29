#pragma once

#include <memory>
#include <string>
#include <list>
#include <variant>

#include <iostream>

namespace ast {

template<typename T>
using Rep = std::shared_ptr<T>;

template<typename T>
struct creator;

template<typename T>
struct creator<Rep<T>> {
    template<typename... Args>
    static Rep<T> create(Args&&... args) {
        return std::make_shared<T, Args...>(std::forward<Args>(args)...);
    }
};

using Value = std::variant<std::string, int, float>;

class ExprBase {
public:
    virtual ~ExprBase() {}
};

using Expr = Rep<ExprBase>;

class StmtBase {
public:
    virtual ~StmtBase() {}
};

using Stmt = Rep<StmtBase>;

class AssignImpl : public StmtBase {
    std::string var;
    Expr expr;
public:
    AssignImpl(const std::string& var, const Expr& expr) :
        var(var), expr(expr) {}

    friend class PrintVisitor;
};

using Assign = Rep<AssignImpl>;

class BlockImpl;
using Block = Rep<BlockImpl>;

class BlockImpl : public StmtBase {
    std::list<Stmt> stmts;

    BlockImpl(std::list<Stmt> stmts) :
        stmts(stmts) {}
public:
    BlockImpl() {}

    BlockImpl(const Stmt& stmt, const Block& other) :
        stmts(other->stmts) {
        stmts.push_front(stmt);
    }

    friend class PrintVisitor;
};

class ConstantImpl : public ExprBase {
    Value val;
public:
    ConstantImpl(const Value& val) :
        val(val) {}
    
    friend class PrintVisitor;
};

using Constant = Rep<ConstantImpl>;

class AST {
    std::string version;
    Block code;
public:
    AST() {}

    AST(const std::string& version, const Block& code) :
        version(version), code(code) {}
    
    friend class PrintVisitor;
};

class PrintVisitor {
public:
    void visit(const AST* ast) {
        std::cout
            << "<AST ver="
            << ast->version
            << ">\n";

        visit(ast->code.get());

        std::cout << "</AST>\n";
    }

    void visit(const BlockImpl* block) {
        std::cout << "<block>\n";

        //for (auto stmt: block->stmts)
        //    visit(stmt.get());

        std::cout << "</block>\n";
    }

    void visit(const AssignImpl* assign) {
        std::cout
            << "<assign var="
            << assign->var
            << ">\n";

        //visit(assign->expr.get());

        std::cout << "</assign>\n";
    }

    void visit(const ConstantImpl* constant) {
        std::cout << "<constant>\n";
        //std::cout << constant->val.get<decltype(constant->val)>();
        std::cout << "</constant>\n";
    }
};
} // namespace ast