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


#include "text.hpp"

char RegexTokenizer::workspace[RegexTokenizer::N_substring+1]="";

RegexTokenizer::RegexTokenizer(){
}

void RegexTokenizer::reset(){
  input= string();
  int i=N_pm; while(--i>0){ pm[i].rm_so=-1; pm[i].rm_eo=-1; }
  so= 0;
  eo= 0;
  previous_eo= -1;
  error= 0;
}

int RegexTokenizer::set(string _input,list<const char*> _regex){
  reset();
  input= _input;
 
  list<const char*>::iterator first= _regex.begin();
  list<const char*>::iterator last = _regex.end();

  while(first!=last){

    regex_t re;
    int i;

    //REG_EXTENDED
    //use extended regular expressions
    //REG_NEWLINE
    //makes ^...$ work to match newline/endofline

    i= regcomp (&re, *first, REG_EXTENDED|REG_NEWLINE);
    if(i)
      return i;
    regex.push_back(re);
    regex_src.push_back(*first);
    ++first;
  }
}

RegexTokenizer::RegexTokenizer(string _input,Mode _mode){
  mode= _mode;
  //create a list
  list<const char*>alist;
  switch(_mode){
  case Word: 
    alist.push_back("([^ \t\n]*)([ \t\n]*)");
    break;
  case Line: 
    alist.push_back("^(.*)$\n"); 
    break;
  case RFC: 
    alist.push_back("((^.*$)((\n)^[ \t]+.*$)*)(\n)?"); 
    //this works, but output is confusing
    // that is, how to remove the glue ?
    break;
  case Custom: 
    //break;
  default:
    cerr<<"RegexTokenizer mode constructor called with pointless mode."<<endl;
  }
  set(_input,alist);
}

RegexTokenizer::RegexTokenizer(string _input,const char* oneregex){
  //create a list
  list<const char*>alist;
  alist.push_back(oneregex);
  set(_input,alist);
}

RegexTokenizer::RegexTokenizer(string _input,list<const char*> _regex){
  set(_input,_regex);
}

RegexTokenizer::RegexTokenizer(const RegexTokenizer &r){
  //cerr<<"(copy constructor)"<<endl;
  set(r.input,r.regex_src);

  // result= r.result; "ANSI C++ fobids ..."
  memcpy(&result[0], &r.result[0], N_pm*sizeof(result[0]) );

  whichregexwasmatched= r.whichregexwasmatched;

  // pm= r.pm;
  memcpy(&pm[0], &r.pm[0], N_pm*sizeof(pm[0]) );

  I_pm= r.I_pm;
  error= r.error;
  so= r.so;
  eo= r.eo;
  previous_eo= r.previous_eo;
  mode= r.mode; 
}


RegexTokenizer RegexTokenizer::begin() const
    {
      //cerr<<"(begin)"<<endl;
      RegexTokenizer RT(*this);
      RT.error= 0;
      RT.so= 0;
      RT.eo= 0;
      RT.previous_eo= -1;
      return RT;
    }

RegexTokenizer RegexTokenizer::end() const
    {
      //cerr<<"(end)"<<endl;
      RegexTokenizer RT(*this);
      RT.error= 1;
      RT.so= input.length();
      RT.eo= input.length();
      RT.previous_eo= RT.eo;
      return RT;
    }

void RegexTokenizer::advance(){
  //try all patterns until one matches

  //cerr<<"advance"<<endl;
  //wonder where to get the string from ?
  //using a char * buffer is ugly, but there is no regex for string
  // (no regex stuff which I'm aware of at the time of writing (1999) )
  if(eo < (signed int)input.size()){
    // there is no c_substr(eo,N_substring)  ;-(
    string sWorkspace(input,eo,N_substring);
    // waste of time, but I´m not sure when sWorkspace.c_str() gets freed;
    strncpy(workspace, sWorkspace.c_str(), N_substring) ;
  }
  else
    workspace[0]='\0';

  result[0]= string();

  if(
     error == 0 && /* regex ok ? */ 
     *workspace != 0 && /* check end buffer */
     previous_eo < eo /* make sure we finish */
     )
    {/* while matches found */
      //cerr<<"go over regex's supplied"<<endl;
      list<regex_t>::iterator first= regex.begin();
      list<regex_t>::iterator last = regex.end();
      error= 1;
  
      previous_eo= eo;
      while(error && result[0].empty() && first!=last){//check for empty buffer
	{
	  //cerr<<endl <<"matching "<< workspace + eo<< endl;

	  /* substring found between pm.rm_so and pm.rm_eo */
	  /* This call to regexec() finds the next match */
	  error = regexec(&*first, workspace, N_pm, &pm[0], 0); 
	  ++first;
	}

	if(!error){
	  int final_so= eo;
	  int final_eo= eo;
	  //Go over the members of pm to see submatches	  
	  int i;
	  i=N_pm; while(--i>0){ result[i]= string(); }
	  i=0;
	  while(i<N_pm &&
		pm[i].rm_so>=0 && pm[i].rm_eo>0 &&
		pm[i].rm_so<N_substring && pm[i].rm_eo<=N_substring
		){
	    int local_so= previous_eo+pm[i].rm_so;
	    int local_eo= previous_eo+pm[i].rm_eo;
	    if(i==0)
	      {
		final_so= local_so;
		final_eo= local_eo;
	      }
	    result[i]= input.substr(local_so, local_eo-local_so);
	    //cout <<"match["<<i<<"]{"<<pm[i].rm_so<<","<<pm[i].rm_eo<<"}";
	    //cout <<"("<< local_so <<","<< local_eo <<"): " << result[i] << endl;
	    
	    i++;
	  }
	  so= final_so;
	  eo= final_eo;
	  I_pm= i;
	}
	else{
	  (void)regerror(error, &*first, workspace, N_substring);
	}
      }
    }else{
      //if the final match has been passed,
      //signal end (to make != operator work ?PS)
      // like in *this= end();
      so= input.length();
      eo= input.length();
      previous_eo= eo;
      
    }

}

RegexTokenizer::~RegexTokenizer(){
  list<regex_t>::iterator first= regex.begin();
  list<regex_t>::iterator last = regex.end();

  while(first!=last){ 
    //cerr<<"freeing "<<&*first<<endl;
    (void) regfree (&*first);
    ++first;
  }
}

ostream& operator<<(ostream &o, const RegexTokenizer &r){
  o<<"("<<&r<<" "<<r.previous_eo<<"-"<<r.so<<"/"<<r.eo<<" ?"<<r.error<<")["<<r.input<<"]"<<endl;
  return o;
}

