#include "DriftEngine.h"
#include <cctype>
#include <cmath>
#include <sstream>

namespace drift {

// （你的实现原样保留，下方即你发来的完整版）
static std::string joinStrings(const std::vector<std::string>& v) {
    std::ostringstream oss;
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) oss << ' ';
        oss << v[i];
    }
    return oss.str();
}

bool DriftEngine::isOp(char c) {
    return c=='+'||c=='-'||c=='*'||c=='/'||c=='^'||c=='%';
}

int DriftEngine::precedence(char c, bool unary, bool& rightAssoc) {
    rightAssoc = false;
    if (unary && c=='-') { rightAssoc = true; return 4; }
    switch (c) {
        case '^': rightAssoc = true; return 3;
        case '*': case '/': case '%': return 2;
        case '+': case '-':          return 1;
        default: return -1;
    }
}

std::string DriftEngine::tokToString(const Tok& t) {
    switch (t.type) {
        case Tok::Number: {
            std::ostringstream oss; oss << t.value; return oss.str();
        }
        case Tok::Op: {
            if (t.unary && t.op=='-') return "u-";
            return std::string(1, t.op);
        }
        case Tok::LParen: return "(";
        case Tok::RParen: return ")";
    }
    return "?";
}

void DriftEngine::snapShunting(const std::vector<Tok>& out, const std::vector<Tok>& ops, const std::string& note) {
    Snapshot s;
    for (auto& t : out) s.outputQueue.push_back(tokToString(t));
    for (auto& t : ops) s.opStack.push_back(tokToString(t));
    s.note = note;
    steps_.push_back(std::move(s));
}

void DriftEngine::snapEval(const std::vector<double>& st, const std::string& note) {
    Snapshot s;
    for (double v : st) s.evalStack.push_back(std::to_string(v));
    s.note = note;
    steps_.push_back(std::move(s));
}

void DriftEngine::tokenize(const std::string& s, std::vector<Tok>& out) {
    out.clear();
    bool prevWasValue = false;
    for (size_t i=0; i<s.size(); ) {
        if (std::isspace((unsigned char)s[i])) { ++i; continue; }

        if (std::isdigit((unsigned char)s[i]) || s[i]=='.') {
            size_t j=i;
            bool dot=false;
            while (j<s.size() && (std::isdigit((unsigned char)s[j]) || s[j]=='.')) {
                if (s[j]=='.') {
                    if (dot) break;
                    dot=true;
                }
                ++j;
            }
            double v = std::stod(s.substr(i, j-i));
            out.push_back(Tok{Tok::Number, v});
            prevWasValue = true;
            i = j;
            continue;
        }

        if (s[i]=='(') {
            out.push_back(Tok{Tok::LParen});
            prevWasValue = false;
            ++i; continue;
        }
        if (s[i]==')') {
            out.push_back(Tok{Tok::RParen});
            prevWasValue = true;
            ++i; continue;
        }

        if (isOp(s[i])) {
            char op = s[i];
            bool unary = (!prevWasValue && op=='-');
            bool ra=false;
            int prec = precedence(op, unary, ra);
            if (prec<0) throw std::runtime_error("Unknown operator");
            Tok t; t.type=Tok::Op; t.op=op; t.prec=prec; t.rightAssoc=ra; t.unary=unary;
            out.push_back(t);
            prevWasValue = false;
            ++i; continue;
        }

        throw std::runtime_error(std::string("Illegal character: ")+s[i]);
    }
}

void DriftEngine::infixToPostfix() {
    postfix_.clear();
    std::vector<Tok> opStack;
    snapShunting(postfix_, opStack, "Start shunting-yard");

    for (size_t i=0;i<infix_.size();++i) {
        const Tok& t = infix_[i];
        if (t.type==Tok::Number) {
            postfix_.push_back(t);
            snapShunting(postfix_, opStack, "Push number");
        } else if (t.type==Tok::Op) {
            while (!opStack.empty()) {
                const Tok& top = opStack.back();
                if (top.type!=Tok::Op) break;
                bool cond = (!t.rightAssoc && top.prec>=t.prec) || (t.rightAssoc && top.prec>t.prec);
                if (cond) {
                    postfix_.push_back(top);
                    opStack.pop_back();
                    snapShunting(postfix_, opStack, "Pop op to output");
                } else break;
            }
            opStack.push_back(t);
            snapShunting(postfix_, opStack, "Push op");
        } else if (t.type==Tok::LParen) {
            opStack.push_back(t);
            snapShunting(postfix_, opStack, "Push '('");
        } else if (t.type==Tok::RParen) {
            bool matched=false;
            while (!opStack.empty()) {
                Tok top = opStack.back(); opStack.pop_back();
                if (top.type==Tok::LParen) { matched=true; break; }
                postfix_.push_back(top);
                snapShunting(postfix_, opStack, "Pop until '('");
            }
            if (!matched) throw std::runtime_error("Mismatched parentheses");
            snapShunting(postfix_, opStack, "Consume ')'");
        }
    }
    while (!opStack.empty()) {
        Tok top = opStack.back(); opStack.pop_back();
        if (top.type==Tok::LParen || top.type==Tok::RParen)
            throw std::runtime_error("Mismatched parentheses");
        postfix_.push_back(top);
        snapShunting(postfix_, opStack, "Drain op stack");
    }
    snapShunting(postfix_, {}, "Shunting complete");
}

double DriftEngine::evalPostfix() {
    std::vector<double> st;
    auto apply = [&](char op, bool unary){
        if (unary) {
            if (st.empty()) throw std::runtime_error("Malformed expression (unary)");
            double a = st.back(); st.pop_back();
            st.push_back(-a);
            snapEval(st, "Apply unary -");
            return;
        }
        if (st.size()<2) throw std::runtime_error("Malformed expression");
        double b = st.back(); st.pop_back();
        double a = st.back(); st.pop_back();
        double r=0.0;
        switch (op) {
            case '+': r=a+b; break;
            case '-': r=a-b; break;
            case '*': r=a*b; break;
            case '/':
                if (std::fabs(b)<1e-15) throw std::runtime_error("Division by zero");
                r=a/b; break;
            case '%':
                if (std::fabs(b)<1e-15) throw std::runtime_error("Modulo by zero");
                r=std::fmod(a,b); break;
            case '^': r=std::pow(a,b); break;
            default: throw std::runtime_error("Unknown operator");
        }
        st.push_back(r);
        snapEval(st, std::string("Apply '")+op+"'");
    };

    for (auto& t : postfix_) {
        if (t.type==Tok::Number) {
            st.push_back(t.value);
            snapEval(st, "Push number");
        } else if (t.type==Tok::Op) {
            apply(t.op, t.unary);
        } else {
            throw std::runtime_error("Unexpected token in postfix");
        }
    }
    if (st.size()!=1) throw std::runtime_error("Malformed expression (result stack)");
    snapEval(st, "Evaluation complete");
    return st.back();
}

bool DriftEngine::setExpression(const std::string& expr) {
    clear();
    expr_ = expr;
    tokenize(expr_, infix_);
    infixToPostfix();
    return true;
}

double DriftEngine::evaluate(const std::string& expr) {
    setExpression(expr);
    return evalPostfix();
}

std::string DriftEngine::toPostfixString() const {
    if (postfix_.empty()) return {};
    std::ostringstream oss;
    for (size_t i=0;i<postfix_.size();++i) {
        if (i) oss << ' ';
        oss << tokToString(postfix_[i]);
    }
    return oss.str();
}

double DriftEngine::evaluateCached() {
    if (postfix_.empty()) {
        if (infix_.empty()) throw std::runtime_error("No expression cached");
    }
    return evalPostfix();
}

void DriftEngine::clear() {
    expr_.clear();
    infix_.clear();
    postfix_.clear();
    steps_.clear();
    runtimeOps_.clear();
    runtimeVals_.clear();
    lastWasValue_ = false;
}

// ===== 事件式 API（给赛车/DSL 用） =====

void DriftEngine::pushOperand(double v) {
    runtimeVals_.push_back(v);
    lastWasValue_ = true;
    snapEval(runtimeVals_, "push operand");
}

void DriftEngine::pushOperator(char op) {
    if (op=='(') {
        runtimeOps_.push_back(Tok{Tok::LParen});
        lastWasValue_ = false;
        snapShunting({}, runtimeOps_, "push '(' (runtime)");
        return;
    }
    if (op==')') {
        bool matched=false;
        while (!runtimeOps_.empty()) {
            Tok top = runtimeOps_.back(); runtimeOps_.pop_back();
            if (top.type==Tok::LParen) { matched=true; break; }
            runtimeOps_.push_back(top);
            reduceOnce();
        }
        if (!matched) throw std::runtime_error("Mismatched parentheses (runtime)");
        lastWasValue_ = true;
        snapShunting({}, runtimeOps_, "consume ')'(runtime)");
        return;
    }

    bool unary = (!lastWasValue_ && op=='-');
    bool ra=false; int prec = precedence(op, unary, ra);
    if (prec<0) throw std::runtime_error("Unknown operator (runtime)");

    Tok cur; cur.type=Tok::Op; cur.op=op; cur.prec=prec; cur.rightAssoc=ra; cur.unary=unary;

    while (!runtimeOps_.empty()) {
        Tok& top = runtimeOps_.back();
        if (top.type!=Tok::Op) break;
        bool cond = (!cur.rightAssoc && top.prec>=cur.prec) || (cur.rightAssoc && top.prec>cur.prec);
        if (!cond) break;
        reduceOnce();
    }
    runtimeOps_.push_back(cur);
    lastWasValue_ = false;
    snapShunting({}, runtimeOps_, std::string("push op '") + (unary? "u-":std::string(1, op)) + "' (runtime)");
}

bool DriftEngine::reduce() {
    if (runtimeOps_.empty()) return false;
    reduceOnce();
    return true;
}

void DriftEngine::reduceOnce() {
    if (runtimeOps_.empty()) return;
    Tok top = runtimeOps_.back(); runtimeOps_.pop_back();

    if (top.type!=Tok::Op) return;

    if (top.unary) {
        if (runtimeVals_.empty()) throw std::runtime_error("Malformed (runtime unary)");
        double a = runtimeVals_.back(); runtimeVals_.pop_back();
        runtimeVals_.push_back(-a);
        snapEval(runtimeVals_, "apply unary - (runtime)");
    } else {
        if (runtimeVals_.size()<2) throw std::runtime_error("Malformed (runtime binary)");
        double b = runtimeVals_.back(); runtimeVals_.pop_back();
        double a = runtimeVals_.back(); runtimeVals_.pop_back();
        double r=0;
        switch (top.op) {
            case '+': r=a+b; break;
            case '-': r=a-b; break;
            case '*': r=a*b; break;
            case '/':
                if (std::fabs(b)<1e-15) throw std::runtime_error("Division by zero (runtime)");
                r=a/b; break;
            case '%':
                if (std::fabs(b)<1e-15) throw std::runtime_error("Modulo by zero (runtime)");
                r=std::fmod(a,b); break;
            case '^': r=std::pow(a,b); break;
            default: throw std::runtime_error("Unknown op (runtime)");
        }
        runtimeVals_.push_back(r);
        snapEval(runtimeVals_, std::string("apply '") + top.op + "' (runtime)");
    }
    lastWasValue_ = true;
}

double DriftEngine::finalize() {
    while (!runtimeOps_.empty()) {
        if (runtimeOps_.back().type==Tok::LParen)
            throw std::runtime_error("Mismatched '(' (runtime)");
        reduceOnce();
    }
    if (runtimeVals_.size()!=1) throw std::runtime_error("Malformed result (runtime)");
    return runtimeVals_.back();
}

} // namespace drift