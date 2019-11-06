/*
 NAME: Zewen Hua
 CLASS & SECTION: CSE6341 MonWedFri 11:30 - 12:25
 ASSIGNMENT NUMBER: Project 1
 COMPILER: gcc
 COMPILATION INSTRUCTION: g++ -g -Wall main.cpp -o pro1.out
 DESCRIPTION: This is a lexical analyzer, also known as scanner.
*/

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// scanner class
class Scanner{
public:
    int lite_num, nume_num, open_num, clos_num, nume_sum;
    char temp;
    bool error, not_end, eof, not_empty;
    string cur;
    string error_token;
    vector <char> cur_token;
    vector <string> Lite_seq;
    
    void getNextToken();
    void classifyToken();
    
    Scanner():lite_num(0),nume_num(0), open_num(0), clos_num(0), nume_sum(0),error(false), not_end(true), eof(false), not_empty(false){}
};

void Scanner::getNextToken(){
    char k;
    //check the first character of the token
    while(1){
        k = cin.get();
        //filter space, carriage return, line feed
        if (int(k) == 32 || int(k) == 13 || int(k) == 10) continue;
        //if file is empty
        else if (k == EOF && not_empty == false){
            eof = true;
            not_end = false;
            break;
        }
        else{
            temp = k;
            not_empty = true;
            if (temp == '('){
                open_num += 1;
                break;
            }
            else if(temp == ')'){
                clos_num += 1;
                break;
            }
            else{
                cur_token.push_back(temp);
                break;
            }
            
        }
    }
    while(eof == false){
        temp = cin.get();
        if (int(temp) == 32 || int(temp) == 13 || int(temp) == 10) break;
        else if (temp == EOF){
            not_end = false;
            break;
        }
        else if (temp == '('){
            open_num += 1;
            break;
        }
        else if(temp == ')'){
            clos_num += 1;
            break;
        }
        //normal token store in cur_token vector
        else if(temp >= '0' && temp <= '9'){
            cur_token.push_back(temp);
            continue;
        }
        else if(temp >= 'A' && temp <= 'Z'){
            cur_token.push_back(temp);
            continue;
        }
    }
}
// check literal, numeric, error atoms
void Scanner::classifyToken(){
    char first;
    string str = "";
    first = cur_token[0];
    if (first >= 'A' && first <= 'Z'){
        for (vector <char>::iterator iter = cur_token.begin(); iter != cur_token.end();++iter){str += *iter;}
        Lite_seq.push_back(str);
        lite_num += 1;
        cur_token.clear();
    }
    else if (first >= '0' && first <= '9'){
        for (vector <char>::iterator iter = cur_token.begin(); iter != cur_token.end();++iter){
            if (*iter >= 'A' && *iter <= 'Z'){error = true;}
            str += *iter;
        }
        if(!error){
            nume_sum += atoi(str.c_str());
            nume_num += 1;
            cur_token.clear();
        }
        else{
            error_token = str;
            cur_token.clear();
        }
    }
}

int main() {
    Scanner scan;
    while(scan.not_end){
        scan.getNextToken();
        if (scan.eof == false && scan.cur_token.size() != 0){
            scan.classifyToken();
        }
        if(scan.error) break;
    }
    //e rror token output
    if(scan.error){
        cout << "ERROR: Invalid token " << scan.error_token << endl;
    }
    // normal token output
    else{
        cout << "LITERAL ATOMS: " << scan.lite_num;
        for (int i = 0; i < scan.Lite_seq.size(); i++){
            cout << ", " << scan.Lite_seq[i];
        }
        cout << endl;
        cout << "NUMERIC ATOMS: " << scan.nume_num << ", " << scan.nume_sum << endl;
        cout << "OPEN PARENTHESES: " << scan.open_num << endl;
        cout << "CLOSING PARENTHESES: " << scan.clos_num << endl;
    }
}
