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
using Rep = std::shared_ptr<const T>;

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

struct ASTNode {
    virtual void accept(Visitor& visitor) const = 0;
};

struct ExprBase : public ASTNode {
    virtual void accept(Visitor& visitor) const = 0;
};

struct StmtBase : public ASTNode {
    virtual void accept(Visitor& visitor) const = 0;
};

class Visitor {
public:
    // should just call accept
    virtual void visit(const ASTNode&) = 0;

    template<typename T>
    void rvisit(const Rep<T>& rep) {
        visit(util::get(rep));
    }

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

struct Assign : public StmtBase {
    std::string var;
    ExprRep expr;

    Assign(const std::string& var_, const ExprRep& expr_) :
        var(var_), expr(expr_) {}

    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

struct IfStmt : public StmtBase {
    ExprRep     condition;
    BlockRep    then;
    BlockRep    otherwise;

    IfStmt( const ExprRep& condition_,
            const BlockRep& then_,
            const BlockRep& otherwise_=util::create<Block>()) :
        condition(condition_), then(then_), otherwise(otherwise_) {}

    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

struct Loop : public StmtBase {
    StmtRep     init;
    StmtRep     step;
    ExprRep     condition;
    BlockRep    code;

    Loop(   const StmtRep& init_,
            const StmtRep& step_,
            const ExprRep& condition_,
            const BlockRep& code_) :
        init(init_), step(step_), condition(condition_), code(code_) {}

    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

struct Block : public StmtBase {
    std::list<StmtRep> stmts;
    
    Block(std::list<StmtRep> stmts_) :
        stmts(stmts_) {}

    Block() {}

    Block(const StmtRep& stmt_, const BlockRep& other_) :
        stmts(other_->stmts) {
        stmts.push_front(stmt_);
    }

    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

struct Constant : public ExprBase {
    Value val;

    Constant(const Value& val_) :
        val(val_) {}
    
    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

template<int id>
struct BinOp : public ExprBase {
    ExprRep lhs;
    ExprRep rhs;

    BinOp(const ExprRep& lhs_, const ExprRep& rhs_) :
        lhs(lhs_), rhs(rhs_) {}
    
    virtual void accept(Visitor& visitor) const {
        visitor.visit(*this);
    }
};

struct AST {
    std::string ver;
    BlockRep code;

    AST() {}

    AST(const std::string& ver_, const BlockRep& code_) :
        ver(ver_), code(code_) {}
};

class PrintVisitor : public Visitor {
    int level;
public:
    PrintVisitor() : level(0) {}

    virtual void visit(const ASTNode& node) override {
        node.accept(*this);
    }

    void traverse(const AST& ast) {
        std::cout   << "<AST ver="
                    << ast.ver
                    << ">\n";

        rvisit(ast.code);

        std::cout << "</AST>\n";
    }

    virtual void visit(const Block& block) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<block>\n";

        ++level;
        for (const StmtRep& stmt: block.stmts)
            rvisit(stmt);
        --level;

        std::cout   << prefix
                    << "</block>\n";
    }

    virtual void visit(const IfStmt& ifstmt) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<if>\n";
        
        level++;
        rvisit(ifstmt.condition);
        rvisit(ifstmt.then);
        rvisit(ifstmt.otherwise);
        level--;

        std::cout   << prefix
                    << "</if>\n";
    }

    virtual void visit(const Loop& loop) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<loop>\n";
        
        level++;
        rvisit(loop.init);
        rvisit(loop.step);
        rvisit(loop.condition);
        rvisit(loop.code);
        level--;

        std::cout   << prefix
                    << "</loop>\n";
    }

    virtual void visit(const Assign& assign) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<assign var="
                    << assign.var
                    << ">\n";

        ++level;
        rvisit(assign.expr);
        --level;

        std::cout   << prefix
                    << "</assign>\n";
    }

    virtual void visit(const Sum& op) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<sum>\n";
        
        level++;
        rvisit(op.lhs);
        rvisit(op.rhs);
        level--;
        
        std::cout   << prefix
                    << "</sum>\n";
    }

    virtual void visit(const Diff& op) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<diff>\n";
        
        level++;
        rvisit(op.lhs);
        rvisit(op.rhs);
        level--;
        
        std::cout   << prefix
                    << "</diff>\n";
    }

    virtual void visit(const Product& op) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<product>\n";
        
        level++;
        rvisit(op.lhs);
        rvisit(op.rhs);
        level--;
        
        std::cout   << prefix
                    << "</product>\n";
    }

    virtual void visit(const Slash& op) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<slash>\n";
        
        level++;
        rvisit(op.lhs);
        rvisit(op.rhs);
        level--;
        
        std::cout   << prefix
                    << "</slash>\n";
    }

    virtual void visit(const Constant& cnst) override {
        std::string prefix(level, '\t');

        std::cout   << prefix
                    << "<constant> "
                    << util::to_string(cnst.val)
                    << " </constant>\n";
    }
};
} // namespace ast