#include "expr_util.h"

void RPNExpression::Push (TokenBase *t) { stack.push_back (t); }

TokenBase* RPNExpression::Pop () {
    TokenBase *t = stack.front();
    stack.erase (stack.begin());
    return t;
}

bool RPNExpression::Empty() const { return stack.empty (); }

void RPNExpression::Print() {
    for(int i = 0, size = stack.size(); i < size; i++)  {
        if (stack[i]->type == OP) {
            std::cout << static_cast<Token<std::string>*>(stack[i])->value << std::endl;
        }
        else if (stack[i]->type == NUM) {
            std::cout << static_cast<Token<double>*>(stack[i])->value << std::endl;
        }
        if (stack[i]->type == VAR) {
            std::cout << static_cast<Token<std::string>*>(stack[i])->value << std::endl;
        }
    }
}


/* 操作数的优先级 */
int ShuntingYard::FetchPrecedence(std::string op) const { return ShuntingYard::op_precedence[op]; }

/* 操作符栈顶的优先级 */
int ShuntingYard::StackPrecedence() const { 
    if (op_stack.empty()) { return -1; }
    return FetchPrecedence (op_stack.top ());
}

/* */ 
void ShuntingYard::HandleLeftParen() { op_stack.push ("("); }

/* 一直弹栈，直到遇到( */
void ShuntingYard::HandleRightParen() {
    while ("(" != op_stack.top ()) {
        rpn.Push(new Token<std::string>(op_stack.top(), OP));
        op_stack.pop();
    }
    op_stack.pop();
}

/* Consume operators with precedence >= than op then add op 
 * 在读到操作符时，如果此时栈顶操作符优先性大于或等于此操作符，弹出栈顶操作符直到发现优先级更低的元素位置
 * 值大代表优先级低
 * */
void ShuntingYard::HandleOperation(std::string op) {
    while (!op_stack.empty () &&
            FetchPrecedence(op) >= StackPrecedence()) {
        rpn.Push(new Token<std::string>(op_stack.top(), OP));
        op_stack.pop();
    }
    op_stack.push(op);
}


/**
 * @brief 中缀表达式转换成后缀表达式，具体算法可以网上了解
 *
 * @param infix
 *
 * @return 
 */
RPNExpression ShuntingYard::ToRpn(const std::string &infix) {
    const char* token = infix.c_str ();
    while (token && *token) {
        while (*token && isspace (*token)) { ++token; }
        if (! *token) { break; }
        if (isdigit (*token)) {
            char* next_token = 0;
            //到出现非数字或字符串结束时('\0')才结束转换)
            rpn.Push(new Token<double>(strtod(token, &next_token), NUM));
            token = next_token;
        } 
        else if (isvariablechar(*token)) {
            std::stringstream ss;
            ss << *token;
            ++token;
            while (isvariablechar(*token) || isdigit(*token)) {
                ss << *token;
                ++token;
            } 
            rpn.Push(new Token<std::string>(ss.str(), VAR));
        }
        else {
            if (*token == '(') {
                HandleLeftParen();
                token++;
            } else if (*token == ')') {
                HandleRightParen();
                token++;
            }
            else {
                std::stringstream ss;
                ss << *token;
                token++;
                while( !isvariablechar(*token) && !isdigit(*token) && !isspace(*token)) {
                    ss << *token;
                    token++;
                }
                std::string op_value = ss.str();
                HandleOperation(op_value);
            }
        }
    }
    while (!op_stack.empty()) {
        rpn.Push(new Token<std::string>(op_stack.top(), OP));
        op_stack.pop();
    }
    return rpn;
}


/**
 * @brief 静态变量
 */
std::unordered_map<std::string, int> ShuntingYard::op_precedence;

ShuntingYard::ShuntingYard (const std::string& infix) : expr(infix) { }

RPNExpression ShuntingYard::FetchRpn () { return ToRpn(expr); }


/**
 * @brief 静态函数，初始化操作符优先级
 *
 * @return 
 */
int ShuntingYard::InitOperationPrecedence() {
    op_precedence["()"] = 2; 
    op_precedence["**"]  = 3;
    op_precedence["*"]  = 5; op_precedence["/"]  = 5; op_precedence["%"] = 5;
    op_precedence["+"]  = 6; op_precedence["-"]  = 6;
    op_precedence["<<"] = 7; op_precedence[">>"] = 7;
    op_precedence["<"]  = 8; op_precedence["<="] = 8; op_precedence[">="] = 8; op_precedence[">"] = 8;
    op_precedence["=="] = 9; op_precedence["!="] = 9;
    op_precedence["&&"] = 13;
    op_precedence["||"] = 14;
    op_precedence["="] = 15;
    op_precedence[","] = 16;
    op_precedence["("]  = 17; 
}

/**
 * @brief 估算动态表达式的值
 *
 * @param ret_value
 * @param expr
 * @param vars
 *
 * @return 
 */
int Calculator::Eval(double& ret_value, const std::string& expr,
        std::unordered_map<std::string, double>& vars) { 
    ShuntingYard shunting(expr);
    RPNExpression rpn = shunting.FetchRpn();
    //rpn.Print();
    Flush();
    std::string key;
    while (!rpn.Empty()) { 
        TokenBase* token = rpn.Pop();
        if (token->type == NUM) {
            //operands.push(static_cast<Token<std::string>*>token)
            operands.push(static_cast<Token<double>*>(token)->value);
        }
        else if (token->type == VAR) {
            key = static_cast<Token<std::string>*>(token)->value;
            if (vars.find(key) == vars.end())
                return -1;
            operands.push(vars[key]);
        }
        else { //operation
            double right = operands.top();
            operands.pop();
            double left = operands.top();
            operands.pop();
            int right_i = static_cast<int>(right);
            int left_i = static_cast<int>(left);
            std::string op = static_cast<Token<std::string>*>(token)->value;
            if (!op.compare("+")) {
                operands.push(left + right);
            } else if (!op.compare("*")) {
                operands.push(left * right);
            } else if (!op.compare("-")) {
                operands.push(left - right);
            } else if (!op.compare("/")) {
                operands.push(left / right);
            } else if (!op.compare("<<")) {
                operands.push(left_i << right_i);
            } else if (!op.compare("**")) {
                operands.push(pow(left, right));
            } else if (!op.compare(">>")) {
                operands.push(left_i >> right_i);
            } else if (!op.compare("%")) {
                operands.push(left_i % right_i);
            } else if (!op.compare("<")) {
                operands.push(left_i < right_i);
            } else if (!op.compare(">")) {
                operands.push(left_i > right_i);
            } else if (!op.compare("<=")) {
                operands.push(left_i <= right_i);
            } else if (!op.compare(">=")) {
                operands.push(left_i >= right_i);
            } else if (!op.compare("==")) {
                operands.push(left_i == right_i);
            } else if (!op.compare("!=")) {
                operands.push(left_i != right_i);
            } else if (!op.compare("&&")) {
                operands.push(left_i && right_i);
            } else if (!op.compare("||")) {
                operands.push(left_i || right_i);
            } else {
                return -1;
            }
        }
        delete token;
    }
    ret_value = Result();
    return 0;
}

double Calculator::Result () const { return operands.top (); }
void Calculator::Flush () { 
    while (! operands.empty ()) { operands.pop (); }
}

//int main () {
//    ShuntingYard::InitOperationPrecedence();
//    Calculator c;
//    std::unordered_map<std::string, double> vars;
//    double ret;
//    std::cout << c.Eval(ret, "(20+10)*3/2-3", vars) << std::endl;
//    std::cout << ret << "\t" << static_cast<int>(ret == 42) << std::endl;
//
//    vars["uid"] = 434243291;
//    vars["cityid"] = 131;
//
//    c.Eval(ret, "uid % 10 == 0 && cityid == 131", vars);
//    std::cout << ret << "\t" << static_cast<int>(ret == 0)  << std::endl;
//
//    c.Eval(ret, "uid % 10 == 1 && cityid == 131", vars);
//    std::cout << ret << "\t" << static_cast<int>(ret == 1)  << std::endl;
//
//    c.Eval(ret, "uid % 10 == 1 && cityid != 131", vars);
//    std::cout << ret << "\t" << static_cast<int>(ret == 0)  << std::endl;
//
//    vars["cityid"] = 189;
//    c.Eval(ret, "uid % 10 == 1 && cityid != 131", vars);
//    std::cout << ret << "\t" << static_cast<int>(ret == 1)  << std::endl;
//    return 0;
//}
