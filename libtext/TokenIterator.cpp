/*
 * $Source$
 * $Revision$
 * $Date$
 *
 * Copyright (c) 1999 by CyberSolutions GmbH, Germany.
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
#include <cstring>
#include <strstream>

#include "text.hpp"

static int mystrpos(const char *c,char s){
  int i=0;
  while(c[i])
    { 
      if(c[i]==s){
	  return i; 
      }
      i++;
    };
  if(!c[i])
    return -1;
  else
    return -1;
}

string TokenIterator::mooncheese= string("The Moon is A green cheese (sheesh!).");

void TokenIterator::reset(){
  i= (istream*)0;
  brace= 0;  bracestack[0]='\0';
  braceoftoken= 0;
  thesep= '\0'; previoussep= '\0';
  eoltoken= '\n';
  whitetoken= ' ';
  buffer= mooncheese;
}

void TokenIterator::setMode(Mode m){
  mode= m;
  switch(mode){
  case Word:
    whitespace=" \t";
    separator="";
    continuation="";
    leftbrace="\"";
    rightbrace="\"";
    escapechar = '\\';
    break;
  case Line:
    whitespace="";
    separator="";
    continuation="";
    leftbrace="";
    rightbrace="";
    escapechar = '\\';
    break;
  case RFC:
    whitespace="";
    separator="";
    continuation=" \t";
    leftbrace="";
    rightbrace="";
    escapechar ='\\';
    break;
  default:
    whitespace = " \t";
    separator  = ",;:+-=/\\@";
    continuation="";
    leftbrace  = "\"([{<";
    rightbrace = "\")]}>";
    escapechar = '\\';
  }
}

TokenIterator::TokenIterator(){
  reset();
  braces= false;
  setMode(Word);
  ismyistream= false;
}



TokenIterator TokenIterator::finalIterator = TokenIterator();

//TokenIterator::TokenIterator(string s, Mode m=Word, bool b=false){
TokenIterator::TokenIterator(string s, Mode m, bool b){
  reset();
  braces= b;
  setMode(m);
  ismyistream= true;
  i= new istrstream(s.c_str());
  //++(*this);// read first value (not done; makes this unwieldly)
}

//TokenIterator::TokenIterator(istream &is, Mode m=Word, bool b=false){
TokenIterator::TokenIterator(istream &is, Mode m, bool b){
  reset();
  braces= b;
  setMode(m);
  ismyistream= false;
  i= &is;
  //++(*this);// read first value (not done; makes this unwieldly)
}

TokenIterator::~TokenIterator(){
  if(ismyistream)
    delete i;
}


TokenIterator::iterator& TokenIterator::begin() const
{
    if(  i && i->good() && !i->eof()  )  
	return *const_cast<TokenIterator*> (this);
    else	  
	return finalIterator;
};


//! from Input Iterator
//! Returns the next object in the stream.
TokenIterator::operator string() const
{
    return buffer; 
};


//! from Input Iterator
//! Returns the next object in the stream.
TokenIterator::operator string()
{
    if( buffer== mooncheese )
	(*this)();  
    return buffer; 
};


//! from Input Iterator
//! Returns the next object in the stream.
const string TokenIterator::operator*() const
{
    return buffer; 
};


//! from Input Iterator
//! Returns the next object in the stream.
const string TokenIterator::operator*()
{
    if( buffer== mooncheese )
	(*this)();  
    return buffer; 
};


//! from Input Iterator
//! Preincrement.
TokenIterator& TokenIterator::operator++()
{ 
    (*this)(); return *this;    
};

    //! from Input Iterator
    //! Postincrement. 
    //! this works .. almost


TokenIterator& TokenIterator::operator++(int i)
{  
 static TokenIterator t = *this; 
 while(i>0){ --i; (*this)++;  } 
 return t; 
};


/** compare not equal */
bool TokenIterator::operator != (TokenIterator &R) const{// const & I say, const
    // note: const TokenIterator &R will create a copy of R :-(
    // this can't work; have to allow use of const in the above
    // has to be compared differently( endflags .. ! )
	
    return &R!= this;
}

/** compare two Tokenizers */
bool TokenIterator::operator == (TokenIterator &R) const{
    // note: const TokenIterator &R will create a copy of R :-(
    // this can't work; have to allow use of const in the above
    // has to be compared differently( endflags .. ! )

    return !( *this != R );
}

/** need this for foreach template */
bool TokenIterator::operator ! (void) const{
    return !( i && i->good() && !i->eof() );
}

/** need this for fun */
bool TokenIterator::hastoken(void) const{
     return  i && i->good() && !i->eof();
}



inline bool linefeed(char c, istream *i){
  if(c=='\r'){
    char d;
    if( i->get(d) ){
      if(d=='\n')
	;/* dos line feed */
      else
	i->unget();
    }
    return true;
  }else if(c=='\n'){
    char d;
    if( i->get(d) ){
      if(d=='\r')
	;/* carriage return after line feed(?) */
      else
	i->unget();
    }
    return true;
  }
  return false;
}


string TokenIterator::operator()(){
  char c= 0;
  int pos;

  previoussep= thesep;
  buffer= string("");

  while( i->get(c) ){

    if(c==escapechar){

      char d;// special translations need to be plugged in here

      if( i->get(d) ){
	if( brace && linefeed(d,i) )
	  buffer+= '\n';
	else
	  buffer+= d;
	}
      }
   
    else if( linefeed(c,i) ){

      thesep= eoltoken;
      {
	switch(mode){
	case Word:
	    if( previoussep!=whitetoken || buffer.length() )// space" = "
	      return buffer;
	    break;
	case Line:
	  return buffer;
	  break;
	case RFC:
	  { 
	    char d;
	    if( i->get(d) ){
	      if(!strchr(continuation,d) ){
		i->unget();
		return buffer;
	      }else
		i->unget();
	    
	    }

	    do{
	      if(!i->get(d)){ return buffer; }
	    }while( strchr(continuation,d) );

	    //should "A\n \tB" be returned as one token "AB" or as "A B" ?
	    // currently, "AB" is returned
	    i->unget();// unget

	  }
	  break;
	default:
	  return buffer;
	}
      }
    }
    else if( !(brace) && strchr(whitespace,c) ){ // brace>0 implies braces==true

      if(buffer.length()){
	thesep= whitetoken;
	return buffer;// send token
      }else
	previoussep= whitetoken;// !?
	;/* skip */

    }else if(strchr(separator,c)){
      thesep= c;
      if( previoussep!=whitetoken || buffer.length() )// space" = "
	return buffer;// send token

    }else if(brace>0 && bracestack[brace]==c){

      /* closing brace */
      braceoftoken= brace;
      brace--; /* pop stack of braces */

      thesep= c;
      return buffer;// send token
      
    }else if( braces && (pos=mystrpos(leftbrace,c), pos>=0) ){//pos>0

      /* opening brace */
      braceoftoken= brace;
      bracestack[++brace]= rightbrace[pos];
      if( previoussep!=whitetoken || buffer.length() ){// space" = "
	  thesep= c;
	  return buffer;// send token
      }
    }else{
      /* normal, append to token */
      buffer+= c;
    }
  }
  return buffer;
}


LexxStyleTokenIterator::LexxStyleTokenIterator(TokenIterator *Tbase){
  state=0;
  base= Tbase;
}

LexxStyleToken&  LexxStyleTokenIterator::operator()(){
 state= !state;
  thetoken.ttype= (LexxStyleToken::Tokentype)state;
  if(state){
    thetoken.Tstring= (*base)();     
  }else{
    thetoken.Tchar= base->thesep;     
  }
  return thetoken;
}
