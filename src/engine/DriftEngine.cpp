#include "DriftEngine.h"
#include <sstream>

namespace drift {

bool DriftEngine::isOp(char c) {
    return c=='+'||c=='-'||c=='*'||c=='/'||c=='^'||c=='%';
}

int DriftEngine::precedence(char c, bool unary, bool& rightAssoc) {
    rightAssoc = false;
    if (unary && c=='-') { rightAssoc = true; return 4; } // 一元负号最高
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
    bool prevWasValue = false; // 判断一元负号
    for (size_t i=0; i<s.size();) {
        if (std::isspace((unsigned char)s[i])) { ++i; continue; }

        if (std::isdigit((unsigned char)s[i]) || s[i]=='.') {
            size_t j=i; bool dot=false;
            while (j<s.size() && (std::isdigit((unsigned char)s[j]) || s[j]=='.')) {
                if (s[j]=='.') { if (dot) break; dot=true; }
                ++j;
            }
            double v = std::stod(s.substr(i, j-i));
            out.push_back(Tok{Tok::Number, v});
            prevWasValue = true; i=j; continue;
        }

        if (s[i]=='(') { out.push_back(Tok{Tok::LParen}); prevWasValue=false; ++i; continue; }
        if (s[i]==')') { out.push_back(Tok{Tok::RParen}); prevWasValue=true;  ++i; continue; }

        if (isOp(s[i])) {
            char op = s[i];
            bool unary = (!prevWasValue && op=='-');
            bool ra=false; int prec=precedence(op, unary, ra);
            if (prec<0) throw std::runtime_error("Unknown operator");
            Tok t; t.type=Tok::Op; t.op=op; t.prec=prec; t.rightAssoc=ra; t.unary=unary;
            out.push_back(t);
            prevWasValue=false; ++i; continue;
        }

        throw std::runtime_error(std::string("Illegal char: ")+s[i]);
    }
}

void DriftEngine::infixToPostfix() {
    postfix_.clear();
    std::vector<Tok> ops;
    snapShunting(postfix_, ops, "start");

    for (auto& t: infix_) {
        if (t.type==Tok::Number) {
            postfix_.push_back(t);
            snapShunting(postfix_, ops, "number");
        } else if (t.type==Tok::Op) {
            while (!ops.empty()) {
                Tok top=ops.back();
                if (top.type!=Tok::Op) break;
                bool cond = (!t.rightAssoc && top.prec>=t.prec) ||
                            ( t.rightAssoc && top.prec> t.prec);
                if (cond) {
                    postfix_.push_back(top);
                    ops.pop_back();
                    snapShunting(postfix_, ops, "pop op");
                } else break;
            }
            ops.push_back(t);
            snapShunting(postfix_, ops, "push op");
        } else if (t.type==Tok::LParen) {
            ops.push_back(t);
            snapShunting(postfix_, ops, "push '('");
        } else if (t.type==Tok::RParen) {
            bool matched=false;
            while (!ops.empty()) {
                Tok top=ops.back(); ops.pop_back();
                if (top.type==Tok::LParen) { matched=true; break; }
                postfix_.push_back(top);
                snapShunting(postfix_, ops, "pop till '('");
            }
            if (!matched) throw std::runtime_error("Mismatched parentheses");
            snapShunting(postfix_, ops, "consume ')'");
        }
    }
    while (!ops.empty()) {
        Tok top=ops.back(); ops.pop_back();
        if (top.type==Tok::LParen) throw std::runtime_error("Mismatched '('");
        postfix_.push_back(top);
        snapShunting(postfix_, ops, "drain");
    }
}

double DriftEngine::evalPostfix() {
    std::vector<double> st;
    auto apply = [&](char op, bool unary){
        if (unary) {
            if (st.empty()) throw std::runtime_error("Malformed (unary)");
            double a=st.back(); st.pop_back();
            st.push_back(-a);
            snapEval(st, "u-");
            return;
        }
        if (st.size()<2) throw std::runtime_error("Malformed");
        double b=st.back(); st.pop_back();
        double a=st.back(); st.pop_back();
        double r=0.0;
        switch (op) {
            case '+': r=a+b; break;
            case '-': r=a-b; break;
            case '*': r=a*b; break;
            case '/': if (std::fabs(b)<1e-15) throw std::runtime_error("Div0"); r=a/b; break;
            case '%': if (std::fabs(b)<1e-15) throw std::runtime_error("Mod0"); r=std::fmod(a,b); break;
            case '^': r=std::pow(a,b); break;
            default: throw std::runtime_error("Unknown op");
        }
        st.push_back(r);
        snapEval(st, std::string("apply ")+op);
    };

    for (auto& t: postfix_) {
        if (t.type==Tok::Number) { st.push_back(t.value); snapEval(st,"push"); }
        else if (t.type==Tok::Op) apply(t.op, t.unary);
        else throw std::runtime_error("Unexpected token");
    }
    if (st.size()!=1) throw std::runtime_error("Malformed result");
    snapEval(st,"done");
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

double DriftEngine::evaluateCached() {
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

void DriftEngine::pushOperand(double v) { runtimeVals_.push_back(v); lastWasValue_ = true; }
void DriftEngine::pushOperator(char op) { (void)op; } // 事件式这里用不到
bool  DriftEngine::reduce()     { return false; }
double DriftEngine::finalize()  { return 0.0; }
void   DriftEngine::reduceOnce(){}

} // namespace drift