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

#ifndef __LIB_TEXT_HPP__
#define __LIB_TEXT_HPP__

#include <stdexcept>
#include <string>
#include <cstring>
#include <iterator>
#include <list>
#include <sys/types.h>
#include "../RegExp/RegExp.hpp"

/** \file text.hpp

    A library for text parsing and manipulation.

    This library contains a couple of useful functions for dealing
    with strings, most notably a regular expression class and a
    generic config file parser.
*/

//////////////////////////////////////////////////
//                tokenize()                    //
//////////////////////////////////////////////////

template<class T>
void tokenize(insert_iterator<T> & ii, const string & buffer,
	      const char * sep = " \t\r\n")
{
    string::size_type pos = 0;
    while(pos != string::npos) {
	string::size_type end_pos = buffer.find_first_of(sep, pos);
	string token = buffer.substr(pos, end_pos-pos);
	if (!token.empty()) {
	    *ii = token;
	    ++ii;
	    end_pos = buffer.find_first_not_of(sep, end_pos);
	}
	if (end_pos != string::npos)
	  end_pos = buffer.find_first_not_of(sep, end_pos);
	pos = end_pos;
    }
}

//////////////////////////////////////////////////
//               RegexTokenizer()               //
//////////////////////////////////////////////////


/** The RegexTokenizer extracts tokens from 'string' input.

    string or stream input has to be converted to string. This means
    the Tokenizer should be useful with large input which is divided
    into large chunks. A match is performed against a list of regular
    expressions. Each expression defines a match-separator pair.
    Regular Expressions are compiled with REG_EXTENDED flag.
*/

class RegexTokenizer: forward_iterator<RegexTokenizer, int> {
public:

    /** maximum number of registers, subexpressions */
    static const int N_pm=10;

    /** maximum length of a match */
    static const int N_substring=1024;

    /** the workspace */
    static char workspace[N_substring+1]; //+1 for trailing \0

    /** Modes (other than Custom) make the \a RegexTokenizer use a standard regular expression.

	\a Custom : The tokenizer uses the regular expression you specify.

	\a Word   : The tokenizer gives chunks of input separated by space and tabs.

	\a Line   : The tokenizer splits input at end of line.

	\a RFC    : The tokenizer splits input at end of line.
	         Lines may be continued by starting a new line with spaces or tabs.
		 These continuation characters are NOT stripped from the tokens.

     */
    enum Mode {Custom, Word, Line, RFC};

    /** RegexTokenizer is it''s own iterator. */
    typedef RegexTokenizer iterator;
private:
    string input;
    string result[N_pm];
    list<const char*>regex_src;// the source regexes needed for copy/begin/end
    list<regex_t>regex;        // not sure multiple regexes are a smart idea
    int whichregexwasmatched;
    regmatch_t pm[N_pm];
    int I_pm;                  // matched subexpressions
    int error;                 // result of regex calls
    int so,eo,previous_eo;     // positions
    //int matchMask;//bitset; which fields to return by the * operator
protected:
    Mode mode;
    void advance();
    void reset();
    int set(string _input,list<const char*> _regex);
public:
    /** default constructor. */
    RegexTokenizer();

    /** Tokenize a string in a mode. */
    RegexTokenizer(string _input,Mode _mode);

    /**  Tokenize a string according to a single regular expression. */
    RegexTokenizer(string _input,const char* oneregex);

    /** Tokenize a string according to several regular expressions.
	(If the first regular expression fails, the next one will be tried. )
     */
    RegexTokenizer(string _input,list<const char*> _regex);

    /** copy constructor */
    RegexTokenizer(const RegexTokenizer &r);

    //void selectFields(int m){ matchMask= m; }

    /** The begin state */
    RegexTokenizer begin() const;

    /** The end state */
    RegexTokenizer end() const;

    /**
	from Input Iterator
        Returns the current token.
    */
    const string operator*() const
    { return result[0]; };


    /** from Input Iterator
        Returns the i-th matched subexpression.
    */
    const string operator[](int i) const
    { return result[i]; };

    /** from Input Iterator
	PreIncrement
     */
    RegexTokenizer& operator++()
    { (*this).advance(); return *this; };

    /** from Input Iterator
	PostIncrement
     */
    RegexTokenizer& operator++(int i)
    { while(i>0){ (*this).advance(); --i; }; return *this; };

    /** Destructor */
    virtual ~RegexTokenizer();

    /** compare not equal */
    bool operator != (const RegexTokenizer &R) const{// const & I say, const
	return  so != R.so || eo != R.eo || previous_eo != R.previous_eo;
    }

    /** compare two RegexTokenizers */
    bool operator == (const RegexTokenizer &R) const{
	return !( *this != R );
    }

    /** print the current state of the RegexTokenizer */
    friend ostream& operator<<(ostream &o,const RegexTokenizer &r);
};


//////////////////////////////////////////////////
//                 TokenIterator                //
//////////////////////////////////////////////////


/** The TokenIterator extracts tokens from string or stream input.

    There are four main modes and a custom mode. In all modes, the
    backslash works as an escape character for the next character i.e.
    'one\\\\backslash' is read as 'one\backslash'.

    Description of the main modes:

    1. Words separated by whitespace, with "whitespace" consisting of
    tabulators and the blank.
    \code
    TokenIterator tokenize(inputStr,TokenIterator::Word);
    \endcode

    2. Words separated by whitespace, "one word" is one token.
    whitespace is defined to be only tabulators and the blank.
    \code
    TokenIterator tokenize(inputStr,TokenIterator::Word,true);
    \endcode

    3. Each line is a token.
    Escaped newlines will become part of the token.
    example:
    \code
    TokenIterator tokenize(inputStr,TokenIterator::Line);
    \endcode

    4. RFC style:
    Whitespace at start of next line appends next line.
    The use of escaping the newline to append the next line,
    like in Makefiles, is NOT part of this mode.
    example:
    \code
    TokenIterator tokenize(inputStr,TokenIterator::RFC);
    \endcode

    5. The Custom Mode: The custom mode is intended for reading from
    data that is in almost human-readable-format, like /etc/passwd.
    Separating elements are not returned as Tokens, but are stored in
    thesep and previoussep. In /etc/passwd ':' is the separator,
    while newlines separate records.
    \code
    class MyCustomTokenIterator: public TokenIterator{
        public:

        MyCustomTokenIterator(string inputStr, bool b=false)
	    : TokenIterator(inputStr,TokenIterator::Custom, b){
	    eoltoken= '\n';
	    separator= ":\n";
        };

        MyCustomTokenIterator(istream &inputStr, bool b=false)
	    : TokenIterator(inputStr,TokenIterator::Custom, b){
	    eoltoken= '\n';
	    separator= ":\n";
        };
   \endcode
   See \a CustomTokenIterator.cpp for the full example.

    Bugs (Custom Mode): Does not recognize a separator preceded by whitespace
    Instead, the tokenizer will collapse a series of whitespace, but
    will offer it as a separator in thesep.
    This is probably not what you want.
*/


class TokenIterator:istream_iterator<string,int> {

private:
    istream *i;
    bool ismyistream;
    string buffer;

    static TokenIterator finalIterator;

    static string mooncheese;

public:
    /** \relates TokenIterator
	The modes allowed as arguments.
     */
    enum Mode {Word, Line, RFC, Custom};

    typedef TokenIterator iterator;

protected:
    int brace;
    int braceoftoken;
    string bracestack;
    bool braces;
    Mode mode;

    const char *whitespace;  // ALL whitespace must be listed here
    const char *separator;   // separators
    const char *continuation;// lists continuation
    const char *leftbrace;   // leftbrace[i] matches rightbrace[i]
    const char *rightbrace;  // supports multiple levels of braces
    char escapechar;// escapechar is the escape char; default \ .
    char eoltoken;  // use this instead of end of line
    char whitetoken;// use this instead of whitespace

    void setMode(Mode m);
    void reset();

public:
    /**
      Returns one token each call.
      An empty token does NOT signal the end of the input.
    */
    virtual string operator()();

    /** Dummy constructor */
    /** constructs an Iterator that has reached end */
    TokenIterator();

    /** Constructor used to tokenize a string s,
	using \a Mode m (default is Words),
	by default without braces.
    */
    TokenIterator(string s, Mode m=Word, bool braces=false);

    /** Constructor used to tokenize from an input stream,
	using \a Mode m (default is Words),
	by default without braces.

	The input stream is consumed, which is why
	the TokenIterator doesn''t offer backward iterator capabilities.
    */
    TokenIterator(istream &is, Mode m=Word, bool braces=false);


    /** A begin function returning bool.
	\a begin and \a end functions have been crafted to
	work with this way of using iterators:
	\code
	ifstream is(somefilename);
	TokenIterator tokenize(is);

	while( tokenize->begin() != tokenize->end() ){
	    string token= tokenize();
	    ...
	}
	\endcode
     */
    iterator& begin() const;


    /** A end function returning an iterator. See \a begin .
     */
    inline iterator& end() const{ return finalIterator; };


    virtual ~TokenIterator();

    //! from Input Iterator
    //! Returns the current object in the stream.
    operator string() const;


    //! from Input Iterator
    //! Returns the current object in the stream,
    //! and the next object if the stream hasn't been read yet
    operator string();


    //! from Input Iterator
    //! Returns the current object in the stream.
    const string operator*() const;


    //! from Input Iterator
    //! Returns the current object in the stream,
    //! and the next object if the stream hasn't been read yet
    const string operator*();


    //! from Input Iterator
    //! Preincrement.
    TokenIterator& operator++();

    //! from Input Iterator
    //! Postincrement.
    //! this works .. almost
    TokenIterator& operator++(int i);


    /** compare not equal */
    bool operator != (TokenIterator &R) const;


    /** compare two Tokenizers */
    bool operator == (TokenIterator &R) const;


    /** need this for foreach template */
    bool operator ! (void) const;


    /** Introducing an implicit conversion to bool is not */
    /** good because it creates an ambiguity, */
    /** since bool may be converted implicitly to int and String. */
    bool hastoken (void) const;



    /** contains the separator that ended the token */
    char thesep;

    /** holds the separator that preceded the token */
    char previoussep;

    /** when using braces (in custom mode),
	check this to get the number of unclosed braces. */
    inline int bracingdepth() const{ return braceoftoken; };

    /** use this to compare with instead of end of line \\n */
    inline char eolToken() const{ return eoltoken; };

    // use this to compare with instead of space */
    inline char whiteToken() const{ return whitetoken; };
};
/** \example TokenIterator_test.cpp */
/** \example CustomTokenIterator.cpp  */


/**
   The LexxStyleToken is returned by the \a LexxStyleTokenIterator
\code
   struct LexxStyleToken{
    enum Tokentype {T1_separator, T1_string};
    Tokentype ttype;
    string Tstring;
    char Tchar;
    };
\endcode
*/
struct LexxStyleToken{
    enum Tokentype {T1_separator, T1_string};
    Tokentype ttype;
    string Tstring;
    char Tchar;
};

/**
   The \a LexxStyleToken iterator is a wrapper around the
   \a TokenIterator . It returns the separators and the parts
   of the string that are separated by the separators
   in alteration.
*/
class LexxStyleTokenIterator{
private:
    TokenIterator *base;
    int state;
public:
    /**
       Return the current token,
       without proceeding to the next token.
     */
    LexxStyleToken thetoken;

    /**
       Wrap the TokenIterator in the LexxStyleTokenIterator.
     */
    LexxStyleTokenIterator(TokenIterator *Tbase);

    /**
       Return the next token.
     */
    LexxStyleToken& operator()();
};


/**
   \a crop_token removes leading and trailing whitespace from a token.
   Example:
   \code
   cout << crop_token( " \thead tail \t" ) << endl; // prints "head tail"
   \endcode
*/

inline string crop_token(const string &s, const string whitespace=string(" /t") ){
    size_t left = s.find_first_not_of(whitespace.c_str());
    size_t right= s.find_last_not_of(whitespace.c_str());
    return string(s,left,right-left+1);
};


/** \a text_escape escapes newlines and escape characters
    inside a string such that it may be read by the \a TokenIterator
    in \a TokenIterator::Line or \a TokenIterator::Word Mode.
*/
inline string text_escape(const string &lines)
{
    unsigned int count= 0;

    //
    // count how many characters have to be escaped
    //
    for( unsigned int i=0;  i<lines.size(); ++i )
	if( lines[i]=='\n' || lines[i]=='\\' )
	    ++count;


    string result("");
    result.reserve( lines.size()+count+1 );

    //
    // escape characters
    //
      {

	for( unsigned int i=0;  i<lines.size(); ++i )
	    {
	      if( lines[i]=='\n' || lines[i]=='\\' )
		  result += '\\';
	      result += lines[i];
	    }

      }

    return result;
}

#endif // !defined(__LIB_TEXT_HPP__)
