#pragma once
#include <string>
#include <vector>
#include <stdexcept>

namespace drift {

// ======= Token 结构 =======
struct Tok {
    enum Type { Number, Op, LParen, RParen } type;
    double value = 0.0;
    char op = 0;
    int prec = 0;
    bool rightAssoc = false;
    bool unary = false;

    Tok(Type t = Number) : type(t) {}
    Tok(Type t, double v) : type(t), value(v) {}
};

// ======= 快照结构，用于调试或可视化 =======
struct Snapshot {
    std::vector<std::string> outputQueue;
    std::vector<std::string> opStack;
    std::vector<std::string> evalStack;
    std::string note;
};

// ======= 主引擎类 =======
class DriftEngine {
public:
    // 解析 + 计算 API
    bool setExpression(const std::string& expr);
    double evaluate(const std::string& expr);
    double evaluateCached();
    std::string toPostfixString() const;
    void clear();

    // 事件式 API（用于赛车漂移动态构造表达式）
    void pushOperand(double v);
    void pushOperator(char op);
    bool reduce();        // 执行一次规约（若可规约）
    void reduceOnce();    // 强制规约一步
    double finalize();    // 完成整个计算

    // 快照（用于可视化或调试）
    const std::vector<Snapshot>& steps() const { return steps_; }

private:
    // 内部结构
    std::string expr_;
    std::vector<Tok> infix_;
    std::vector<Tok> postfix_;
    std::vector<Snapshot> steps_;

    std::vector<Tok>    runtimeOps_;
    std::vector<double> runtimeVals_;
    bool lastWasValue_ = false;

    // 内部方法
    void tokenize(const std::string& s, std::vector<Tok>& out);
    void infixToPostfix();
    double evalPostfix();

    // 工具函数
    static bool isOp(char c);
    static int precedence(char c, bool unary, bool& rightAssoc);
    static std::string tokToString(const Tok& t);

    // 快照记录函数
    void snapShunting(const std::vector<Tok>& out, const std::vector<Tok>& ops, const std::string& note);
    void snapEval(const std::vector<double>& st, const std::string& note);
};

} // namespace drift