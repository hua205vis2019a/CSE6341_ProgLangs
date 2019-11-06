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
    
    int length(Node *s);
    void pncheck(Node *s);
    
    bool bound(string x, vector <Node*> z);
    Node* getval(string x, vector <Node*> z);
    
    void CondHelper(Node *s);
    Node* evcon(Node *x);
    Node* evlist(Node *x);
    void addpairs(Node *xlist, Node *ylist);
    Node* apply(Node *f, Node *x);
    
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
    
    Node* eval(Node *exp);
    
    Scanner():error_input(false),error_parse(false), have_next(false){}
};

Scanner scan;
vector <char> cur_token;
vector <Node*> alist;
vector <Node*> dlist;

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
    if(s==NULL || (s->isleaf && s->val=="NIL"))
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

int Scanner::length(Node *s){
    if(s==NULL||s->isleaf) return 0;
    else return 1+length(cdr(s));
}

void Scanner::CondHelper(Node *s){
    if(car(s)->isleaf==true || (car(s)->isleaf==false && cdr(cdr(car(s)))->isleaf==false)){
        Undefine();
        exit(0);
    }
    else if(cdr(s)->isleaf==false) CondHelper(cdr(s));
}

bool Scanner::bound(string x, vector <Node*> a){
    for (Node* element: a)
        if(element->left->val==x) return true;
    return false;
}

Node* Scanner::getval(string x, vector <Node*> a){
    for (int i = a.size()-1; i>=0;i--)
        if(a[i]->left->val==x) return cdr(a[i]);
    cout << "ERROR: getval error" << endl;
    exit(0);
}

Node* Scanner::evcon(Node *x){
    if(null(x)->val=="T"){
        cout <<"ERROR: evcon function error" << endl;
        exit(0);
    }
    else if(eval(car(car(x)))->val=="T") return eval(car(cdr(car(x))));
    else return evcon(cdr(x));
}

Node* Scanner::evlist(Node *x){
    if(x==NULL||null(x)->val=="T"){
        Node *s = new Node;
        s->type = 3;
        s->isleaf = true;
        s->val = "NIL";
        return s;
    }
    else
        return cons(eval(car(x)), evlist(cdr(x)));
}

void Scanner::addpairs(Node *xlist, Node *ylist){
    if(length(xlist)!=length(ylist)){
        cout << "ERROR: ADDPAIRS function xlist and ylist have different length" << endl;
        exit(0);
    }
    else{
        if(length(xlist)==0) return ;
        else{
            alist.push_back(cons(car(xlist),car(ylist)));
            addpairs(cdr(xlist),cdr(ylist));
        }
    }
}

Node* Scanner::apply(Node *f, Node *x){
    if(atom(f)->val=="T"&&f->type==3){
        string op = f->val;
        if(op=="PLUS" || op=="MINUS" || op=="TIMES" || op=="LESS" || op=="GREATER" || op=="EQ" || op=="CONS"){
            if(length(x)!=2){
                Undefine();
                exit(0);
            }
            else{
                if(op=="PLUS") return plus(eval(car(x)),eval(car(cdr(x))));
                else if(op=="MINUS") return minus(eval(car(x)),eval(car(cdr(x))));
                else if(op=="TIMES") return times(eval(car(x)),eval(car(cdr(x))));
                else if(op=="LESS") return less(eval(car(x)),eval(car(cdr(x))));
                else if(op=="GREATER") return greater(eval(car(x)),eval(car(cdr(x))));
                else if(op=="EQ") return eq(eval(car(x)),eval(car(cdr(x))));
                else if(op=="CONS") return cons(eval(car(x)),eval(car(cdr(x))));
            }
        }
        else if(op=="ATOM" || op=="INT" || op=="NULL" || op=="CAR" || op=="CDR"){
            if(length(x)!=1){
                Undefine();
                exit(0);
            }
            else{
                if(op=="CAR") return car(eval(car(x)));
                else if(op=="CDR") return cdr(eval(car(x)));
                else if(op=="ATOM") return atom(eval(car(x)));
                else if(op=="INT") return int_(eval(car(x)));
                else if(op=="NULL") return null(eval(car(x)));
            }
        }
        else{
            addpairs(car(getval(f->val,dlist)), x);
            return eval(cdr(getval(f->val,dlist)));
        }
    }
    cout << "ERROR: function name error" << endl;
    exit(0);
}

void Scanner::pncheck(Node *s){
    if(s->isleaf) return ;
    else{
        if(!car(s)->isleaf){
            cout << "ERROR: Parameter could not be a list" << endl;
            exit(0);
        }
        string pn = car(s)->val;
        if(pn=="T" || pn=="NIL" || pn=="CAR" || pn=="CDR" || pn=="CONS" || pn=="ATOM" || pn=="EQ" || pn=="NULL" || pn=="INT" || pn=="PLUS" || pn=="MINUS" || pn=="TIMES" || pn=="LESS" || pn=="GREATER" || pn=="COND" || pn=="QUOTE" || pn=="DEFUN"){
            cout << "ERROR: Illegal parameter name" << endl;
            exit(0);
        }
        else
            pncheck(cdr(s));
    }
}

Node* Scanner::eval(Node *exp){
    if(atom(exp)->val=="T"){
        if(exp->val=="T") return exp;
        else if(exp->val=="NIL") return exp;
        else if(int_(exp)->val=="T") return exp;
        else if(bound(exp->val, alist)){
            return getval(exp->val, alist);}
        else{
            Undefine();
            exit(0);
        }
    }
    else{
        if(car(exp)->val=="QUOTE"){
            if(length(exp)!=2){
                cout <<"ERROR: QUOTE function wrong length" << endl;
                exit(0);
            }
            return car(cdr(exp));
        }
        else if(car(exp)->val=="COND"){
            CondHelper(cdr(exp));
            return evcon(cdr(exp));
        }
        else if(car(exp)->val=="DEFUN"){
            if(length(exp)!=4){
                cout << "ERROR: Wrong DEFUN structure" << endl;
                exit(0);
            }
            Node *s1 = new Node;
            Node *s2 = new Node;
            s1 = car(cdr(exp));
            s2 = car(cdr(cdr(exp)));
            string fn = s1->val;
            if(s1->type!=3 || fn=="T" || fn=="NIL" || fn=="CAR" || fn=="CDR" || fn=="CONS" || fn=="ATOM" || fn=="EQ" || fn=="NULL" || fn=="INT" || fn=="PLUS" || fn=="MINUS" || fn=="TIMES" || fn=="LESS" || fn=="GREATER" || fn=="COND" || fn=="QUOTE" || fn=="DEFUN"){
                cout << "ERROR: Illegal DEFUN name" << endl;
                exit(0);
            }
            if(s2->isleaf && s2->val!="NIL"){
                cout << "ERROR: parameters should be in a list" << endl;
                exit(0);
            }
            pncheck(s2);
            dlist.push_back(cons(car(cdr(exp)),cons(car(cdr(cdr(exp))),car(cdr(cdr(cdr(exp)))))));
            return 0;
        }
        else{
            return apply(car(exp), evlist(cdr(exp)));
        }
    }
}

void ParseStart(Scanner &scanner);
void ParseExpr(Scanner &scanner, Node *par);
void Print(Node *curr);

void ParseStart(Scanner &scan){
    while(scan.GetCurrent().type!=5){
        Node *root = new Node;
        ParseExpr(scan, root);
        if(scan.length(root->left)>1 && scan.car(root->left)->val=="DEFUN"){
            scan.eval(root->left);
        }
        else{
            Print(scan.eval(root->left));
            cout << endl;
        }
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
