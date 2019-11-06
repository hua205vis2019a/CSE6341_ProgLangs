#include <iostream>
#include <vector>
#include <string>

using namespace std;

typedef struct{
    int type; // 0 for error_token, 1 for (, 2 for ), 3 for lite_atom, 4 for num_atom, 5 for eof
    string atom;
    string error_token;
}Token;

struct Node{
    string val;
    int type; // 3 for lite_atom, 4 for num_atom
    bool isleaf;
    struct Node *left, *right;
    Node(): left(NULL), right(NULL), isleaf(false){}
};

class Scanner{
private:
    Token current;
public:
    bool error_input, error_parse, have_next;
    char temp, next;
    string error_token;
    
    void Init();
    Token GetCurrent();
    void MoveToNext();
    void NextToken();
    Token getNextToken();
    void Undefine();
    
    Node* car(Node *s);
    Node* cdr(Node *s);
    Node* cons(Node *s1, Node *s2);
    Node* atom(Node *s);
    Node* int_(Node *s);
    Node* null(Node *s);
    Node* eq(Node *s1, Node *s2);
    Node* plus(Node *s1, Node *s2);
    Node* minus(Node *s1, Node *s2);
    Node* times(Node *s1, Node *s2);
    Node* less(Node *s1, Node *s2);
    Node* greater(Node *s1, Node *s2);
    void CondHelper(Node *s);
    Node* CondEval(Node *s);
    Node* eval(Node *s);
    
    Scanner():error_input(false),error_parse(false), have_next(false){}
};

Scanner scan;
vector <char> cur_token;

void Scanner::Init(){
    NextToken();
    current = getNextToken();
}

Token Scanner::GetCurrent(){
    return current;
}

void Scanner::MoveToNext(){
    NextToken();
    current = getNextToken();
}

void Scanner::NextToken(){
    if(have_next){
        cur_token.push_back(next);
        have_next = false;
        return ;
    }
    char k;
    while(1){
        k = cin.get();
        if (int(k)==32||int(k)==13||int(k)==10) continue;
        else{
            temp = k;
            cur_token.push_back(temp);
            break;
        }
    }
    if(temp!='('&&temp!=')'&&temp!=EOF){
        while(true){
            temp = cin.get();
            if (int(temp) == 32 || int(temp) == 13 || int(temp) == 10) break;
            else if (temp == EOF||temp == '('||temp == ')'){
                have_next = true;
                next = temp;
                break;
            }
            //normal token store in cur_token vector
            else if((temp >= '0' && temp <= '9')||(temp >= 'A' && temp <= 'Z')){
                cur_token.push_back(temp);
                continue;
            }
        }
    }
}

Token Scanner::getNextToken(){
    char first;
    Token cur;
    string str = "";
    first = cur_token[0];
    if(first=='('){
        cur.type = 1;
        cur_token.clear();
    }
    else if(first==')'){
        cur.type = 2;
        cur_token.clear();
    }
    else if(first==EOF){
        cur.type = 5;
        cur_token.clear();
    }
    else if (first >= 'A' && first <= 'Z'){
        for (vector <char>::iterator iter = cur_token.begin(); iter != cur_token.end();++iter) str += *iter;
        cur.atom = str;
        cur.type = 3;
        cur_token.clear();
    }
    else if (first >= '0' && first <= '9'){
        for (vector <char>::iterator iter = cur_token.begin(); iter != cur_token.end();++iter){
            if (*iter >= 'A' && *iter <= 'Z') error_input = true;
            str += *iter;
        }
        if(!error_input){
            cur.type = 4;
            cur.atom = str;
            cur_token.clear();
        }
        else{
            cur.error_token = str;
            cur.type = 0;
            cur_token.clear();
        }
    }
    return cur;
}

void Scanner::Undefine(){
    cout << "ERROR: The function is undefined." << endl;
}

Node* Scanner::car(Node *s){
    if(s->isleaf){
        Undefine();
        exit(0);
    }
    else
        return s->left;
}

Node* Scanner::cdr(Node *s){
    if(s->isleaf){
        Undefine();
        exit(0);
    }
    else
        return s->right;
}

Node* Scanner::cons(Node *s1, Node *s2){
    Node *s_ = new Node;
    s_->left = s1;
    s_->right = s2;
    return s_;
}

Node* Scanner::atom(Node *s){
    Node *s_ = new Node;
    s_->type = 3;
    s_->isleaf = true;
    if(s->isleaf)
        s_->val = "T";
    else
        s_->val = "NIL";
    return s_;
}

Node* Scanner::int_(Node *s){
    Node *s_ = new Node;
    s_->type = 3;
    s_->isleaf = true;
    if(s->isleaf && s->type == 4)
        s_->val = "T";
    else
        s_->val = "NIL";
    return s_;
}

Node* Scanner::null(Node *s){
    Node *s_ = new Node;
    s_->type = 3;
    s_->isleaf = true;
    if(s->isleaf && s->val=="NIL")
        s_->val = "T";
    else
        s_->val = "NIL";
    return s_;
}

Node* Scanner::eq(Node *s1, Node *s2){
    Node *s_ = new Node;
    s_->type = 3;
    s_->isleaf = true;
    if(s1->isleaf==false || s2->isleaf==false){
        Undefine();
        exit(0);
    }
    else if(s1->val==s2->val)
        s_->val = "T";
    else
        s_->val = "NIL";
    return s_;
}

Node* Scanner::plus(Node *s1, Node *s2){
    if(s1->isleaf==false || s2->isleaf==false || s1->type != 4 || s2->type != 4){
        Undefine();
        exit(0);
    }
    else{
        Node *s_ = new Node;
        s_->isleaf = true;
        s_->type = 4;
        s_->val = to_string(atoi(s1->val.c_str()) + atoi(s2->val.c_str()));
        return s_;
    }
}

Node* Scanner::minus(Node *s1, Node *s2){
    if(s1->isleaf==false || s2->isleaf==false || s1->type != 4 || s2->type != 4){
        Undefine();
        exit(0);
    }
    else{
        Node *s_ = new Node;
        s_->isleaf = true;
        s_->type = 4;
        s_->val = to_string(atoi(s1->val.c_str()) - atoi(s2->val.c_str()));
        return s_;
    }
}

Node* Scanner::times(Node *s1, Node *s2){
    if(s1->isleaf==false || s2->isleaf==false || s1->type != 4 || s2->type != 4){
        Undefine();
        exit(0);
    }
    else{
        Node *s_ = new Node;
        s_->isleaf = true;
        s_->type = 4;
        s_->val = to_string(atoi(s1->val.c_str()) * atoi(s2->val.c_str()));
        return s_;
    }
}

Node* Scanner::less(Node *s1, Node *s2){
    if(s1->isleaf==false || s2->isleaf==false || s1->type != 4 || s2->type != 4){
        Undefine();
        exit(0);
    }
    else{
        Node *s_ = new Node;
        s_->isleaf = true;
        s_->type = 3;
        if(atoi(s1->val.c_str())<atoi(s2->val.c_str()))
            s_->val = "T";
        else
            s_->val = "NIL";
        return s_;
    }
}

Node* Scanner::greater(Node *s1, Node *s2){
    if(s1->isleaf==false || s2->isleaf==false || s1->type != 4 || s2->type != 4){
        Undefine();
        exit(0);
    }
    else{
        Node *s_ = new Node;
        s_->isleaf = true;
        s_->type = 3;
        if(atoi(s1->val.c_str())>atoi(s2->val.c_str()))
            s_->val = "T";
        else
            s_->val = "NIL";
        return s_;
    }
}

void Scanner::CondHelper(Node *s){
    if(car(s)->isleaf==true || (car(s)->isleaf==false && cdr(cdr(car(s)))->isleaf==false)){
        Undefine();
        exit(0);
    }
    else if(cdr(s)->isleaf==false) CondHelper(cdr(s));
}

Node* Scanner::CondEval(Node *s){
    if(eval(car(car(s)))->val!="NIL") return eval(car(cdr(car(s))));
    else if(eval(car(car(s)))->val=="NIL" && cdr(s)->isleaf==false) return CondEval(cdr(s));
    else{
        Undefine();
        exit(0);
    }
}

Node* Scanner::eval(Node *s){
    if(s->isleaf && s->val=="T")
        return s;
    if(s->isleaf && s->val=="NIL")
        return s;
    if(int_(s)->val=="T")
        return s;
    if(s->isleaf){
        Undefine();
        exit(0);
    }
    Node *s1 = new Node;
    Node *s2 = new Node;
    string op = car(s)->val;
    if(op=="PLUS" || op=="MINUS" || op=="TIMES" || op=="LESS" || op=="GREATER" || op=="EQ" || op=="CONS"){
        s1 = car(cdr(s));
        s2 = car(cdr(cdr(s)));
        if(cdr(cdr(cdr(s)))->isleaf==false){
            Undefine();
            exit(0);
        }
        else{
            if(op=="PLUS") return plus(eval(s1),eval(s2));
            else if(op=="MINUS") return minus(eval(s1),eval(s2));
            else if(op=="TIMES") return times(eval(s1),eval(s2));
            else if(op=="LESS") return less(eval(s1),eval(s2));
            else if(op=="GREATER") return greater(eval(s1),eval(s2));
            else if(op=="EQ") return eq(eval(s1),eval(s2));
            else if(op=="CONS") return cons(eval(s1),eval(s2));
        }
    }
    if(op=="ATOM" || op=="INT" || op=="NULL" || op=="CAR" || op=="CDR" || op=="QUOTE"){
        s1 = car(cdr(s));
        if(cdr(cdr(s))->isleaf==false){
            Undefine();
            exit(0);
        }
        else{
            if(op=="ATOM") return atom(eval(s1));
            else if(op=="INT") return int_(eval(s1));
            else if(op=="NULL") return null(eval(s1));
            else if(op=="CAR") return car(eval(s1));
            else if(op=="CDR") return cdr(eval(s1));
            else if(op=="QUOTE") return s1;
        }
    }
    if(op=="COND"){
        s1 = cdr(s);
        CondHelper(s1);
        return CondEval(s1);
    }
    else{
        Undefine();
        exit(0);
    }
}

void ParseStart(Scanner &scanner);
void ParseExpr(Scanner &scanner, Node *par);
void Print(Node *curr);

void ParseStart(Scanner &scan){
    while(scan.GetCurrent().type!=5){
        Node *root = new Node;
        ParseExpr(scan, root);
        Print(scan.eval(root->left));
        cout << endl;
    }
}

void ParseExpr(Scanner &scan, Node *par){
    if(scan.GetCurrent().type == 3){
        Node *lnode = new Node;
        lnode->type = 3;
        lnode->val = scan.GetCurrent().atom;
        lnode->isleaf = true;
        par->left = lnode;
        scan.MoveToNext();
    }
    else if(scan.GetCurrent().type == 4){
        Node *lnode = new Node;
        lnode->type = 4;
        lnode->val = scan.GetCurrent().atom;
        lnode->isleaf = true;
        par->left = lnode;
        scan.MoveToNext();
    }
    else if(scan.GetCurrent().type == 1){
        scan.MoveToNext();
        Node *lnode = new Node;
        Node *rnode = new Node;
        par->left = lnode;
        par->right = rnode;
        while(scan.GetCurrent().type != 2 && scan.GetCurrent().type != 5){
            ParseExpr(scan, lnode);
            Node *rnode = new Node;
            lnode->right = rnode;
            lnode = lnode->right;
        }
        if(scan.GetCurrent().type == 2){
            lnode->val = "NIL";
            lnode->isleaf = true;
            scan.MoveToNext();
        }
        else if(scan.GetCurrent().type == 5){
            cout << "ERROR: More open parenthesis than closing parenthesis" << endl;
            exit(0);
        }
    }
    else if(scan.GetCurrent().type == 0){
        cout << "ERROR: Invalid token " << scan.GetCurrent().error_token << endl;
        exit(0);
    }
    else{
        cout << "ERROR: More closing parenthesis than open parenthesis" << endl;
        exit(0);
    }
}

void Print(Node *curr){
    if(curr->isleaf) cout << curr->val;
    else{
        cout << "(";
        while(curr->isleaf==false){
            Print(curr->left);
            curr = curr->right;
            if(!curr->isleaf){
                cout << " ";
            }
        }
        if(curr->val!="NIL"){
            cout << " . ";
            cout << curr->val;
        }
        cout << ")";
    }
}

int main(){
    scan.Init();
    ParseStart(scan);
    return 0;
}
