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

#include "text.hpp"

// begin test section
// Set verbose to 1 if you want to see the parsed tokens
const int verbose=0;

int test1(){
  string input("a22bbb4444ccccc999999999dfgDFG");

  // Set up the tokenizer to match the input string
  // against a regular expression.
  // The entire match will be returned by *rt,
  // subexpressions by rt[1], rt[2], rt[3], ..
  RegexTokenizer rt(input,"([a-z]*)([^a-z]*)");

  int count=0;
  RegexTokenizer::iterator next= rt.begin();
  const RegexTokenizer::iterator last= rt.end();

  if(verbose)
    cout << "*** begin 1*** "<<endl;

  while(next!=last){
    ++next;// Preinc - processes input
    if(next!=last){
      if(verbose)
         cout << *next   // Entire match,
	      <<"="
	      <<next[1]  // first subexpression,
	      <<"+"
	      <<next[2]  // 2nd subexpr.
	      << endl;
    }
    count++;

  }
  if(verbose)
      cout << "--- end 1 ---"<<count<<endl;
  if(count != 5)
    return 1;

  return 0;
}

int test2(){
  string input("Word-Satz 1\n Satz 1a\nSatz 2\n\tSatz2a.");

  // Set up the tokenizer to match the input string
  // against a regular expression.
  // The entire match will be returned by *rt,
  // subexpressions by rt[1], rt[2], rt[3], ..
  RegexTokenizer rt(input,"([^ \t\n]*)([ \t\n]*)");
  int count=0;

  RegexTokenizer::iterator next= rt.begin();
  const RegexTokenizer::iterator last= rt.end();

  if(verbose)
      cout << "*** begin 2*** "<<endl;

  while(next!=last){
    ++next;// Preinc - processes input
    if(next!=last){
      if(verbose)
	cout <<next[1] // first matched subexpression
	     <<"["
	     <<next[2] // second matched subexpression
	     <<"]"<< endl;
    }
     count++;
  }

  if(verbose)
      cout << "--- end 2 ---"<<count<<endl;
  if(count != 8)
    return 1;

  return 0;
}

int testWord(){
  string input("Ein Satz aus vielen langen Wor-ten.\nUnd ein zweiter Satz.2\n3");

  // Set up the tokenizer to match the input string
  // against a regular expression that defines the word-wise tokenizing.
  // The expression used is "([^ \t\n]*)([ \t\n]*)" .
  // The entire match will be returned by *rt,
  // subexpressions by rt[1], rt[2], rt[3], ..
  RegexTokenizer rt(input,RegexTokenizer::Word);
  int count=0;

  RegexTokenizer::iterator next= rt.begin();
  const RegexTokenizer::iterator last= rt.end();

  if(verbose)
      cout << "*** begin Word*** "<<endl;

  while(next!=last){
    ++next;// Preinc - processes input
    if(next!=last){
      if(verbose)
	cout <<next[1]   // first matched subexpression,
	     <<"["
	     <<next[2]   // 2nd matched subexpr.
	     <<"]"<< endl;
    }
    count++;
  }

  if(verbose)
      cout << "--- end Word ---"<<count<<endl;
  if(count != 12)
    return 1;

  return 0;
}


int testLine(){
  string input("Line-Satz 1\n Satz 1a\nSatz 2\n\tSatz2a.");

  // Set up the tokenizer to match the input string
  // against a regular expression that defines line by line tokenizing.
  // The expression used is "^(.*)$\n" .
  // The entire match will be returned by *rt,
  // subexpressions by rt[1], rt[2], rt[3], ..
  RegexTokenizer rt(input,RegexTokenizer::Line);
  int count=0;

  RegexTokenizer::iterator next= rt.begin();
  const RegexTokenizer::iterator last= rt.end();

  if(verbose)
      cout << "*** begin Line*** "<<endl;

  while(next!=last){
    ++next;// Preinc - processes input
    if(next!=last){
  if(verbose)
          cout <<"'"
	       <<next[1]   // first matched subexpression
	       <<"'"<<"["
	       <<next[2]   // second matched subexpression
	       <<"]"<< endl;
    }
    count++;
  }

  if(verbose)
      cout << "--- end Line ---"<<count<<endl;
  if(count != 5)
    return 1;

  return 0;
}

int testRFC(){
  string input("RFC-Satz 1\n Satz 1a\nSatz 2\n\tSatz2a\n\tSatz2b.");

  // Set up the tokenizer to match the input string
  // against a regular expression that defines RFC-style tokenizing.
  // The expression used is "((^.*$)((\n)^[ \t]+.*$)*)(\n)?" .
  // Bug: whitespace that glues one line to the next is not removed.
  // (afaik, there not way to do this with a single regular expression).
  // The entire match will be returned by *rt,
  // subexpressions by rt[1], rt[2], rt[3], ..
  RegexTokenizer rt(input,RegexTokenizer::RFC);
  int count=0;

  RegexTokenizer::iterator next= rt.begin();
  const RegexTokenizer::iterator last= rt.end();

  if(verbose)
      cout << "*** begin RFC*** "<<endl;
  while(next!=last){
    ++next;// Preinc - processes input
    if(next!=last){
      if(verbose)
          cout <<"'"<<next[1]<<"'"<<"["<<next[2]<<"]"<<"["<<next[3]<<"]"<< endl;
      // first, second and third matched subexpression
    }
    count++;
  }

  if(verbose)
      cout << "--- end RFC ---"<<count<<endl;
  if(count != 3)
    return 1;

  return 0;
}

int main(int argc, char *argv[])
{
  return test1() || test2() || testWord() || testLine() || testRFC() ;
}
