#include "text.hpp"
#include <fstream.h>

/**
  Example using the Custom Mode of the TokenIterator
 */
class MyCustomTokenIterator: public TokenIterator{
public:

    MyCustomTokenIterator(string inputStr, bool b=false)
	: TokenIterator(inputStr,TokenIterator::Custom, b){
	eoltoken= '\n';
	separator= ":\n";
	whitespace= "";
    };

    MyCustomTokenIterator(istream &inputStr, bool b=false)
	: TokenIterator(inputStr,TokenIterator::Custom, b){
	eoltoken= '\n';
	separator= ":\n";
	whitespace= "";
    };
};


int main(int argc, char* argv[]){
    ifstream infile("/etc/passwd");
    MyCustomTokenIterator tokenize(infile);
    while(!infile.eof()){
	string user= tokenize();
	string password= tokenize();
	string userid = tokenize();
	string groupid= tokenize();
	string description= tokenize();
	string home = tokenize();
	string shell= tokenize();
	
	if(password=="x")
	  password="shadowed";

	cout << "----"<<endl;
	cout << "user       :" <<user <<endl;
	cout << "password   :" <<password <<endl;
	cout << "userid     :" <<userid <<endl;
	cout << "groupid    :" <<groupid <<endl;
	cout << "description:" <<description <<endl;
	cout << "home       :" <<home <<endl;
	cout << "shell      :" <<shell <<endl;

	while( !infile.eof() && tokenize.thesep!= tokenize.eolToken() )
	  {
	      string trailing_garbage = tokenize();
	      cout<<"\\:"<< trailing_garbage;
	  }
	cout<<endl;
    }
    return 0;
}

