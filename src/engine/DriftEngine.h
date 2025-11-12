#pragma once
#include <vector>
#include <string>
#include <stdexcept>
#include <cmath>
#include <cctype>
#include <optional>

namespace drift {

struct Tok {
    enum Type { Number, Op, LParen, RParen } type{};
    double      value{0};
    char        op{0};
    int         prec{0};
    bool        rightAssoc{false};
    bool        unary{false};
};

struct Snapshot {
    std::vector<std::string> outputQueue; // (仅做调试用，可忽略)
    std::vector<std::string> opStack;
    std::vector<std::string> evalStack;
    std::string              note;
};

class DriftEngine {
public:
    // 解析 + 求值（一次性）
    bool   setExpression(const std::string& expr);
    double evaluate(const std::string& expr);
    double evaluateCached();

    // DSL/事件式累积（本项目里主要用一次性 evaluate）
    void   pushOperand(double v);
    void   pushOperator(char op);
    bool   reduce();
    double finalize();

    // 维护
    void   clear();

    // 调试（可选）
    std::string toPostfixString() const { return {}; }
    const std::vector<Snapshot>& steps() const { return steps_; }

private:
    // 词法 + 语法
    static bool  isOp(char c);
    static int   precedence(char c, bool unary, bool& rightAssoc);
    static std::string tokToString(const Tok& t);

    void tokenize(const std::string& s, std::vector<Tok>& out);
    void infixToPostfix();
    double evalPostfix();

    // 可视化快照（可选）
    void snapShunting(const std::vector<Tok>& out, const std::vector<Tok>& ops, const std::string& note);
    void snapEval(const std::vector<double>& st, const std::string& note);
    void reduceOnce();

private:
    std::string expr_;
    std::vector<Tok> infix_;
    std::vector<Tok> postfix_;
    std::vector<Snapshot> steps_;

    // 事件式（可选）
    std::vector<double> runtimeVals_;
    std::vector<Tok>    runtimeOps_;
    bool lastWasValue_ = false;
};

} // namespace drift