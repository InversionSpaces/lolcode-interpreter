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

template<typename T>
const T& get(const Rep<T>& rep) {
    return *rep;
}

class ExprBase;
using ExprRep = Rep<ExprBase>;

class StmtBase;
using StmtRep = Rep<StmtBase>;

class Assign;
using AssignRep = Rep<Assign>;

class Block;
using BlockRep = Rep<Block>;

class Constant;
using ConstantRep = Rep<Constant>;

union untyped_t {};
using Value = std::variant<std::string, int, float, bool, untyped_t>;
static const untyped_t untyped {};

struct value_str {
    std::string operator()(const Value& val) const {
        if (std::holds_alternative<std::string>(val))
            return std::string("string: ") + std::get<std::string>(val);

        else if (std::holds_alternative<int>(val))
            return std::string("int: ") + std::to_string(std::get<int>(val));
        
        else if (std::holds_alternative<float>(val))
            return std::string("float: ") + std::to_string(std::get<float>(val));

        else if (std::holds_alternative<bool>(val))
            return std::string("bool: ") + (std::get<bool>(val) ? "true" : "false");
        
        else return "untyped";
    }
};

class Visitor;

class ASTNode {
public:
    virtual void accept(Visitor& visitor) const = 0;
};

class ExprBase : public ASTNode {
public:
    virtual void accept(Visitor& visitor) const = 0;
};

class StmtBase : public ASTNode {
public:
    virtual void accept(Visitor& visitor) const = 0;
};

class Visitor {
public:
    // should just call accept
    virtual void visit(const ASTNode&) = 0;

    virtual void visit(const Assign&) = 0;
    virtual void visit(const Block&) = 0;
    virtual void visit(const Constant&) = 0;
};

class Assign : public StmtBase {
    std::string _var;
    ExprRep _expr;
public:
    Assign(const std::string& var, const ExprRep& expr) :
        _var(var), _expr(expr) {}

    const std::string& var() const {
        return _var;
    }

    const ExprRep& expr() const {
        return _expr;
    }

    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

class Block : public StmtBase {
    std::list<StmtRep> _stmts;
    
    Block(std::list<StmtRep> stmts) :
        _stmts(stmts) {}
public:
    Block() {}

    Block(const StmtRep& stmt, const BlockRep& other) :
        _stmts(other->_stmts) {
        _stmts.push_front(stmt);
    }

    const std::list<StmtRep>& stmts() const {
        return _stmts;
    }

    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

class Constant : public ExprBase {
    Value _val;
public:
    Constant(const Value& val) :
        _val(val) {}

    const Value& val() const {
        return _val;
    }
    
    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

class AST {
    std::string _ver;
    BlockRep _code;
public:
    AST() {}

    AST(const std::string& ver, const BlockRep& code) :
        _ver(ver), _code(code) {}
    
    const std::string& ver() const {
        return _ver;
    }

    const BlockRep& code() const {
        return _code;
    }
};

class PrintVisitor : public Visitor {
public:
    void traverse(const AST& ast) {
        std::cout   << "<AST ver="
                    << ast.ver()
                    << ">\n";

        visit(get(ast.code()));

        std::cout << "</AST>\n";
    }

    virtual void visit(const ASTNode& node) final {
        node.accept(*this);
    }

    virtual void visit(const Block& block) {
        std::cout << "<block>\n";

        for (const StmtRep& stmt: block.stmts())
            visit(get(stmt));

        std::cout << "</block>\n";
    }

    virtual void visit(const Assign& assign) {
        std::cout   << "<assign var="
                    << assign.var()
                    << ">\n";

        visit(get(assign.expr()));

        std::cout << "</assign>\n";
    }

    virtual void visit(const Constant& cnst) {
        std::cout   << "<constant>\n"
                    << value_str {} (cnst.val()) << "\n"
                    << "</constant>\n";
    }
};
} // namespace ast