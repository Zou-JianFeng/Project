#include "hlLexer.h"
#include "hlParser.h"
#include <cassert>
#include <map>
#include <string>
#include <iostream>
#include <stdexcept>

using std::map;
using std::string;
using std::cout;
using std::runtime_error;

class ExprTreeEvaluator {
public:
    ExprTreeEvaluator() : father(nullptr) {}
    ExprTreeEvaluator(ExprTreeEvaluator *father) : father(father) {}
    int run(pANTLR3_BASE_TREE);
    int getVal(string name);
    int setVal(string name, int val);
private :
    map<string, int> memory;
    ExprTreeEvaluator *father;
};

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
const char* getText(pANTLR3_BASE_TREE tree);

int main(int argc, char* argv[]) {
    pANTLR3_INPUT_STREAM input;
    phlLexer lex;
    pANTLR3_COMMON_TOKEN_STREAM tokens;
    phlParser parser;

    assert(argc > 1);
    input = antlr3FileStreamNew((pANTLR3_UINT8)argv[1],ANTLR3_ENC_8BIT);
    lex = hlLexerNew(input);
    
    tokens = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT,
                                            TOKENSOURCE(lex));
    parser = hlParserNew(tokens);
    printf("get parser done!\n");
    fflush(stdout);
    hlParser_prog_return r = parser->prog(parser);
    printf("parser tree is done!\n");
    fflush(stdout);
    pANTLR3_BASE_TREE tree = r.tree;
    
    ExprTreeEvaluator eval;
    int rr = eval.run(tree);
    
    parser->free(parser);
    tokens->free(tokens);
    lex->free(lex);
    input->close(input);
    
    return 0;
}

int ExprTreeEvaluator::getVal(string name) {
    printf("get memory : %s\n", name.c_str());
    ExprTreeEvaluator *p = this;
    while (p != nullptr) {
        if (p->memory.find(name) != p->memory.end()) {
            return p->memory[name];
        }
        p = p->father;
    }
    throw runtime_error(name + " is Undeclared");
    return -1;
}

int ExprTreeEvaluator::setVal(string name, int val) {
    printf("set %s = %d\n", name.c_str(), val);
    this->memory[name] = val;
    return 1;
}

int ExprTreeEvaluator::run(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if(tok) {
        switch(tok->type) {
        case INT: {
            const char* s = getText(tree);
            if(s[0] == '~') {
                return -atoi(s+1);
            }
            else {
                return atoi(s);
            }
        }
        case ID: {
            string var(getText(tree));
            return this->getVal(var);
        }
        case BLOCK: {
            ExprTreeEvaluator newExpr(this);
            int k = tree->getChildCount(tree);
            int r = 0;
            for (int i = 0; i < k; i++) {
                r = newExpr.run(getChild(tree, i));
            }
            cout << "This Block result: " << r << '\n';
            return r;
        }
        case PLUS:
            return run(getChild(tree,0)) + run(getChild(tree,1));
        case MINUS:
            return run(getChild(tree,0)) - run(getChild(tree,1));
        case TIMES:
            return run(getChild(tree,0)) * run(getChild(tree,1));
        case DEF: {
            int k = tree->getChildCount(tree);
            for (int i = 0; i < k; i++) {
                string var(getText(getChild(tree, i)));
                int ans = getChild(tree, i)->getChildCount(getChild(tree, i));
                int n = 0;
                if (ans) {
                    n = run(getChild(getChild(tree,i), 0));
                }
                this->setVal(var, n);
            }
            return 0;
        }
        case ASSIGN: {
            string var(getText(getChild(tree,0)));
            int val = run(getChild(tree,1));
            this->setVal(var, val);
            return val;
        }
        case FOR: {
            int ind = tree->getChildCount(tree);
            run(getChild(tree, 0));
            while (run(getChild(tree, 1))) {
                for (int i = 3; i < ind; i++) {
                    run(getChild(tree, i));
                }
                run(getChild(tree, 2));
            }
            return 0;
        }
        case WHILE: {
            int k = tree->getChildCount(tree);
            while (run(getChild(tree, 0))) {
                for (int i = 1; i < k; i++) {
                    run(getChild(tree, i));
                }
            }
        }
        case MOD: 
            return run(getChild(tree, 0)) % run(getChild(tree, 1));
        case DIV:
            return run(getChild(tree, 0)) / run(getChild(tree, 1));
        case PRINT: {
            int ind = tree->getChildCount(tree);
            for (int i = 0; i < ind; i++) {
                printf("%d ", run(getChild(tree, i)));
            }
            printf("\n");
            return 1;
        }
        case IF: {
            int ind = tree->getChildCount(tree);
            int ans = run(getChild(tree, 0));
            if (ans) {
                return run(getChild(tree, 1));
            } else if (ind != 2) {
                return run(getChild(tree, 2));
            }
            return 0;
        }
        case OR:
            return run(getChild(tree, 0)) || run(getChild(tree, 1));
        case AND:
            return run(getChild(tree, 0)) && run(getChild(tree, 1));
        case GE:
            return run(getChild(tree, 0)) >= run(getChild(tree, 1));
        case NE:
            return run(getChild(tree, 0)) != run(getChild(tree, 1));
        case LITTLE:
            return run(getChild(tree, 0)) < run(getChild(tree, 1));
        case LE:
            return run(getChild(tree, 0)) <= run(getChild(tree, 1));
        case GT:
            return run(getChild(tree, 0)) > run(getChild(tree, 1));
        case EQ:
            return run(getChild(tree, 0)) == run(getChild(tree, 1));
        default:
            cout << "Unhandled token: #" << tok->type << '\n';
            return -1;
        }
    } else {
        int k = tree->getChildCount(tree);
        int r = 0;
        for(int i = 0; i < k; i++) {
            r = run(getChild(tree, i));
        }
        return r;
    }
}

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i) {
    assert(i < tree->getChildCount(tree));
    return (pANTLR3_BASE_TREE) tree->getChild(tree, i);
}

const char* getText(pANTLR3_BASE_TREE tree) {
    return (const char*) tree->getText(tree)->chars;
}
