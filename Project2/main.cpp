#include <iostream>
#include <vector>
#include <string>

using namespace std;

typedef struct{
    int type; // 1 for (, 2 for ), 3 for atom, 0 for error_token
    string atom;
    string error_token;
}Token;

struct Node{
    string val;
    bool isleaf;
    struct Node *left, *right;
    Node(): left(NULL), right(NULL), isleaf(false){}
};

class Scanner{
private:
    Token current;
public:
    bool error_input, error_parse, not_end, eof, not_empty, have_next;
    char temp, next;
    string error_token;

    void Init();
    Token GetCurrent();
    void MoveToNext();
    void NextToken();
    Token getNextToken();
    
    Scanner():error_input(false),error_parse(false), not_end(true), eof(false), not_empty(false), have_next(false){}
};

Scanner scan;
vector <char> cur_token;

void Scanner::Init(){
    NextToken();
    if(eof == false || cur_token.size()!=0){
        current = getNextToken();
    }
}

Token Scanner::GetCurrent(){
    return current;
}

void Scanner::MoveToNext(){
    NextToken();
    if(eof == false || cur_token.size()!=0){
        current = getNextToken();
    }
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
        else if(k==EOF&&not_empty==false){
            eof = true;
            not_end = false;
            break;
        }
        else{
            temp = k;
            not_empty = true;
            cur_token.push_back(temp);
            break;
        }
    }
    if(temp!='('&&temp!=')'){
        while(eof == false){
            temp = cin.get();
            if (int(temp) == 32 || int(temp) == 13 || int(temp) == 10) break;
            else if (temp == EOF){
                //eof = true;
                break;
            }
            else if (temp == '('||temp == ')'){
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
            cur.type = 3;
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

void ParseStart(Scanner &scanner);
void ParseExpr(Scanner &scanner, Node *par);
void Print(Node *curr);

void ParseStart(Scanner &scan){
    while(!scan.eof || cur_token.size()!= 0){
        Node *root = new Node;
        ParseExpr(scan, root);
        Print(root->left);
        cout << endl;
    }
}

void ParseExpr(Scanner &scan, Node *par){
    if(scan.GetCurrent().type == 3){
        Node *lnode = new Node;
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
        while(scan.GetCurrent().type != 2 && !scan.eof){
            ParseExpr(scan, lnode);
            Node *rnode = new Node;
            lnode->right = rnode;
            lnode = lnode->right;
        }
        if(!scan.eof){
        lnode->val = "NIL";
        lnode->isleaf = true;
        scan.MoveToNext();
        }
        else{
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
        Print(curr->left);
        cout << " . ";
        Print(curr->right);
        cout << ")";
    }
}

int main(){
    scan.Init();
    ParseStart(scan);
    return 0;
}
