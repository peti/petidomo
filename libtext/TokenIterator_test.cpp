/*
 * $Source$
 * $Revision$
 * $Date$
 *
 * Copyright (c) 1999 by CyberSolutions GmbH.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by CyberSolutions GmbH.
 *
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cstdlib>
#include <fstream.h>
#include <string>
#include "text.hpp"
#include <algo.h>


// Set verbose=1 to see the tokens
const int verbose=1;

/**
   The do_sth_with_aTokenIterator class
   was written with the intent
   to have the class written to cout
   using the for_each template
 */
class do_sth_with_aTokenIterator {
  string s;
public:
  explicit do_sth_with_aTokenIterator() : s() {};
  void operator()(const TokenIterator& s)
    { cout<< *s <<endl; };

    //! Postincrement. 
    do_sth_with_aTokenIterator& operator++(int i)
    {  
    };

};

int main(int argc,char *argv[]){
  {
    //Tokenize words
    ifstream i("test.txt");
    if(!i)
      { cerr<<"Test Data not found(test.txt)"<< endl; return(2); }

    //Initialize the Tokenizer for word mode
    TokenIterator tokenize(i,TokenIterator::Word);

    string token="";
    int count=0;
    if(verbose)
      cout<<endl<<"--Word"<<endl;

    //Loop over all tokens
    while( tokenize.hastoken() ){
	token= tokenize();
	if(verbose)
	  cout<<":"<<token<<"\n";
	count ++;
    }
    if(verbose)
      cout<<endl<<count<<endl;
    if(count!=27)
      ;//return(1);
  }
  {
      //Tokenize words, with " "
      ifstream i("test.txt");
      if(!i) return 255;

      //Initialize the Tokenizer for word mode, "a b" is one word
      TokenIterator tokenize(i,TokenIterator::Word,true);

      string token="";
      int count=0;
      if(verbose)
	cout<<endl<<"--\"Word\""<<endl;

      //Loop over all tokens
      while( tokenize.hastoken() ){
	  token= tokenize();
	  if(verbose)
	    cout<<":"<<token<<"\n";
	  count ++;
      }
      if(verbose)
	cout<<endl<<count<<endl;
      if(count!=25)
	;//return(1);
  }
  {
      //Tokenize lines
      ifstream i("test.txt");
      if(!i) return 255;

      //Initialize the Tokenizer for line mode ( one line == one token )
      TokenIterator tokenize(i,TokenIterator::Line);

      string token="";
      int count=0;
      if(verbose)
	cout<<endl<<"--Line"<<endl;

      while( tokenize.hastoken() ){
	  token= tokenize();
	  if(verbose)
	    cout<<":"<<token<<"\n";
	  count ++;
      }
      if(verbose)
	cout<<endl<<count<<endl;
      if(count!=10)
	;//return(1);
  }
  {
      //Tokenize 'RFC-style'
      ifstream i("test.txt");
      if(!i) return 255;

      //Initialize Tokenizer for RFC mode
      // ( If the following line starts with space or tabulator,
      // it is glued to the previous line ).
      TokenIterator tokenize(i,TokenIterator::RFC);
      string token="";
      int count=0;
      if(verbose)
	cout<<endl<<"--RFC"<<endl;

      //Loop over all tokens
      while( tokenize.hastoken() ){
	  token= tokenize();
	  if(verbose)
	    cout<<":"<<token<<"\n";
	  count ++;
      }
      if(verbose)
	cout<<endl<<count<<endl;
      if(count!=5)
	;//return(1);
  }

  //trying sequence capability ..
  {
      //Tokenize words
      ifstream i("test.txt");
      if(!i) return 255;
      TokenIterator tokenize(i,TokenIterator::Word);
      string token="";
      int count=0;
      if(verbose)
	cout<<endl<<"--Word(seq)"<<endl;

      //TokenIterator has only dummy capabilities, hence the warnings
      // or maybe I just don't get what a unary function is ?
      /*      for_each (
		tokenize.begin(),
 		tokenize.end(),
		do_sth_with_aTokenIterator()
		)
	  ++count;
      */

      if(verbose)
	cout<<endl<<count<<endl;

      if(count!=0)
	cerr<<"You mean someone repaired that ?!"<<endl;
  }

  // there are different way to use the TokenIterator;
  // this way seems intuitive to me.
  // testing this way ..    
    {
      
      ifstream i("test.txt");
      if(!i) return 255;
      TokenIterator lines(i, TokenIterator::Line);

      cout<<endl;      
      cout<<"testing while( line= ++lines, ( lines.begin() != lines.end() ) )"
	  <<"\n            cout<<\"<<line<<\"<<endl; "<<endl;
      
 
      string line;
      while( line= ++lines, ( lines.begin() != lines.end() ) )
	  cout<<"\""<<line<<"\""<<endl; 
   
    }
 

  return 0;
}
