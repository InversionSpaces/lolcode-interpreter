#pragma once

#include <memory>
#include <string>
#include <list>
#include <variant>

#include <iostream>

namespace ast {

union untyped_t {};
using Value = std::variant<std::string, int, float, bool, untyped_t>;
static const untyped_t untyped {};

template<typename T>
using Rep = std::shared_ptr<T>;

struct util {
    template<typename T, typename... Args>
    static Rep<T> create(Args&&... args) {
        return std::make_shared<T, Args...>(std::forward<Args>(args)...);
    }

    template<typename T>
    static const T& get(const Rep<T>& rep) {
        return *rep;
    }

    static std::string to_string(const Value& val) {
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

class ExprBase;
using ExprRep = Rep<ExprBase>;

class StmtBase;
using StmtRep = Rep<StmtBase>;

class Assign;
using AssignRep = Rep<Assign>;

class Block;
using BlockRep = Rep<Block>;

class IfStmt;
using IfStmtRep = Rep<IfStmt>;

class Loop;
using LoopRep = Rep<Loop>;

template<int>
class BinOp;

enum BinOpID {
    SUM,
    DIFF,
    PRODUCT,
    SLASH
};

using Sum = BinOp<SUM>;
using SumRep = Rep<Sum>;

using Diff = BinOp<DIFF>;
using DiffRep = Rep<Diff>;

using Product = BinOp<PRODUCT>;
using ProductRep = Rep<Product>;

using Slash = BinOp<SLASH>;
using SlashRep = Rep<Slash>;

class Constant;
using ConstantRep = Rep<Constant>;

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
    virtual void visit(const IfStmt&) = 0;
    virtual void visit(const Loop&) = 0;
    virtual void visit(const Block&) = 0;
    virtual void visit(const Sum&) = 0;
    virtual void visit(const Diff&) = 0;
    virtual void visit(const Product&) = 0;
    virtual void visit(const Slash&) = 0;
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

class IfStmt : public StmtBase {
    ExprRep     _condition;
    BlockRep    _then;
    BlockRep    _otherwise;
public:
    IfStmt( const ExprRep& condition, 
            const BlockRep& then,
            const BlockRep& otherwise=util::create<Block>()) :
        _condition(condition), _then(then), _otherwise(otherwise) {}

    const ExprRep& condition() const {
        return _condition;
    }

    const BlockRep& then() const {
        return _then;
    }

    const BlockRep& otherwise() const {
        return _otherwise;
    }

    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

class Loop : public StmtBase {
    StmtRep     _init;
    StmtRep     _step;
    ExprRep     _condition;
    BlockRep    _code;
public:
    Loop(   const StmtRep& init,
            const StmtRep& step, 
            const ExprRep& condition,
            const BlockRep& code) :
        _init(init), _step(step), _condition(condition), _code(code) {}

    const StmtRep& init() const {
        return _init;
    }

    const StmtRep& step() const {
        return _step;
    }

    const ExprRep& condition() const {
        return _condition;
    }

    const BlockRep& code() const {
        return _code;
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

template<int id>
class BinOp : public ExprBase {
    ExprRep _lhs;
    ExprRep _rhs;
public:
    BinOp(const ExprRep& lhs, const ExprRep& rhs) :
        _lhs(lhs), _rhs(rhs) {}

    const ExprRep& lhs() const {
        return _lhs;
    }

    const ExprRep& rhs() const {
        return _rhs;
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
    int level;
public:
    PrintVisitor() : level(0) {}

    void traverse(const AST& ast) {
        std::cout   << "<AST ver="
                    << ast.ver()
                    << ">\n";

        visit(util::get(ast.code()));

        std::cout << "</AST>\n";
    }

    virtual void visit(const ASTNode& node) override {
        node.accept(*this);
    }

    virtual void visit(const Block& block) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<block>\n";

        ++level;
        for (const StmtRep& stmt: block.stmts())
            visit(util::get(stmt));
        --level;

        std::cout   << prefix
                    << "</block>\n";
    }

    virtual void visit(const IfStmt& ifstmt) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<if>\n";
        
        level++;
        visit(util::get(ifstmt.condition()));
        visit(util::get(ifstmt.then()));
        visit(util::get(ifstmt.otherwise()));
        level--;

        std::cout   << prefix
                    << "</if>\n";
    }

    virtual void visit(const Loop& loop) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<loop>\n";
        
        level++;
        visit(util::get(loop.init()));
        visit(util::get(loop.step()));
        visit(util::get(loop.condition()));
        visit(util::get(loop.code()));
        level--;

        std::cout   << prefix
                    << "</loop>\n";
    }

    virtual void visit(const Assign& assign) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<assign var="
                    << assign.var()
                    << ">\n";

        ++level;
        visit(util::get(assign.expr()));
        --level;

        std::cout   << prefix
                    << "</assign>\n";
    }

    virtual void visit(const Sum& op) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<sum>\n";
        
        level++;
        visit(util::get(op.lhs()));
        visit(util::get(op.rhs()));
        level--;
        
        std::cout   << prefix
                    << "</sum>\n";
    }

    virtual void visit(const Diff& op) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<diff>\n";
        
        level++;
        visit(util::get(op.lhs()));
        visit(util::get(op.rhs()));
        level--;
        
        std::cout   << prefix
                    << "</diff>\n";
    }

    virtual void visit(const Product& op) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<product>\n";
        
        level++;
        visit(util::get(op.lhs()));
        visit(util::get(op.rhs()));
        level--;
        
        std::cout   << prefix
                    << "</product>\n";
    }

    virtual void visit(const Slash& op) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<slash>\n";
        
        level++;
        visit(util::get(op.lhs()));
        visit(util::get(op.rhs()));
        level--;
        
        std::cout   << prefix
                    << "</slash>\n";
    }

    virtual void visit(const Constant& cnst) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<constant> "
                    << util::to_string(cnst.val())
                    << " </constant>\n";
    }
};
} // namespace ast