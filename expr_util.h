#ifndef src_utils_expr_util_h
#define src_utils_expr_util_h

#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <cstdlib>
#include <map>
#include <stdexcept>
#include <sstream>
#include <stdint.h>
#include <math.h>
#include <unordered_map>

#define isvariablechar(c) (isalpha(c) || c == '_')

enum tokType {
    OP, VAR, NUM
};

class Calculator;


struct TokenBase { 
    uint8_t type;
    virtual ~TokenBase() {}
};
/*
   Concrete 'token' of an RPN expression. 
   Operators are of type Token< char >
   Operands are of type Token< double >
 */
template<class T> class Token : public TokenBase {
    public:
        Token (T _value, uint8_t _type) : value(_value) {
            this->type = _type; 
        }
        T value;
};


/**
 * @brief 逆波兰后缀表达式的结构体
 * 真正的计算在calculator
 */
class RPNExpression {
    public:
        void Push (TokenBase *t);
        TokenBase* Pop ();
        bool Empty() const;
        void Print();
    private:
        std::vector<TokenBase*> stack;
};

/*
   Shuntingyard 算法把中缀表达式转换成后缀表达式
 */
class ShuntingYard {
    private:
        const std::string expr;
        RPNExpression rpn;
        std::stack<std::string> op_stack;

        /* 操作数的优先级 */
        int FetchPrecedence(std::string op) const;

        /* 操作符栈顶的优先级 */
        int StackPrecedence() const;

        /* */ 
        void HandleLeftParen();

        /* 一直弹栈，直到遇到( */
        void HandleRightParen();

        /* Consume operators with precedence >= than op then add op 
         * 在读到操作符时，如果此时栈顶操作符优先性大于或等于此操作符，弹出栈顶操作符直到发现优先级更低的元素位置
         * 值大代表优先级低
         * */
        void HandleOperation(std::string op) ;

        /* 中缀表达式转换后缀表达式，具体算法可以网上找 */
        RPNExpression ToRpn(const std::string &infix);
    public:
        static std::unordered_map<std::string, int> op_precedence;
        ShuntingYard (const std::string& infix);
        RPNExpression FetchRpn();
        static int InitOperationPrecedence(); 
};


class Calculator {
    public:

        /**
         * @brief 估算动态表达式的值
         *
         * @param ret_value 外部传入引用变量，相当于返回结果
         * @param expr 字符串动态表达式
         * @param vars 变量对应的值集合
         *
         * @return 0正确，其他异常
         */
        int Eval(double& ret_value, const std::string& expr,
                    std::unordered_map<std::string, double>& vars);
    private:
        std::stack<double> operands;
        double Result () const ;
        void Flush () ;
};

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


#endif
