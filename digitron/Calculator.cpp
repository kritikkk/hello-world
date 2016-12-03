#include "std_lib_facilities.h"


const char number = '8';
const char quit = 'q';
const char print = ';';
const string prompt = ">";
const string result = "=";

////** Prototypes **////
double expression();
double term();
double primary();
void calculate();
void clean_up_mess();
double get_value(string s);
double set_value(string s, double d);

// ---------------------------------------------
// class Token
class Token{
public:
  char kind;
  double value;
  Token(char ch)
    :kind(ch), value(0) {}
  Token(char ch, double val)
  	:kind(ch), value(val) {}
};
// ---------------------------------------------
// class Token_stream
class Token_stream{
public:
	Token_stream();
	Token get();
	void putback(Token t);
  void ignore(char c);
private:
	bool full;
	Token buffer;
};
// ----------------------------------------------

Token_stream::Token_stream()
	:full(false), buffer(0)
{

}

void Token_stream::putback(Token t)
{
	if (full) error("putback into a full buffer");
	buffer = t;
	full = true;
}

Token Token_stream::get()
{
  if (full){
    // remove Token from buffer
    full = false;
    return buffer;
  }
  char ch;
  cin >> ch;

  switch (ch) {
    case print:
    case quit:
    case '(':
    case ')':
    case '+':
    case '-':
    case '*':
    case '/':
    case '%':
      return Token(ch); // left each character reperesents itself

    case '.':
    case '0':case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    {
      cin.putback(ch);    // put digit in input stream
      double val;
      cin >> val;       // read a float
      return Token(number, val);    // let '8' reperesents itself

    }
    default:
      error ("Bad token");
   }
}

void Token_stream::ignore(char c)
{
  // first look in buffer
  if (full && c == buffer.kind){
    full = false;
    return;
  }
  full = false;

  // now search input
  char ch = 0;
  while (cin >> ch) {
    if (ch == c) return;
  }
}

// ------------------------------------------------
// Class Variable
class Variable{
public:
  string name;
  double value;
  Variable(string n, double v)
    :name(n), value(v) {}
};

// global Variables
Token_stream ts;
vector<Variable> var_table;




////////////////////////////////////////////
////////////////////////////////////////////
// Main entry
int main()
{
	try{
    calculate();
    return 0;
	}

	catch (exception& e) {
		cerr << "error: " << e.what() << '\n';
    keep_window_open("");
		return 1;
	}

	catch (...) {
		cerr << "Unknown exception\n";
    keep_window_open();
		return 2;
	}
}


double set_value(string s, double d)
{
  // set the variable named s to d
  for(int i = 0; i < var_table.size(); ++i)
    if (var_table[i].name == s){
      var_table[i].value = d;
      return true;
  }
  error("set: undifined variable ", s);
}

double get_value(string s)
{
  // return the value of variable named s
  for (int i =0; i < var_table.size(); ++i)
    if (var_table[i].name == s)
      return var_table[i].value;
    error("get: undifined variable ", s);
}

void calculate()
{
  while(cin){
    try{
      cout << prompt;
      Token t = ts.get();

      while (t.kind == print) t=ts.get();   // discards all prints
      if (t.kind == quit) return;

      ts.putback(t);
      cout << result << expression() << endl;
   }
   catch(exception& e){
     cerr << e.what() << endl;
     clean_up_mess();
   }
 }
}

void clean_up_mess()
{
  ts.ignore(print);
}

double expression()
{
  double left = term();
  Token t = ts.get();
  while (true){
    switch (t.kind) {
      case '+':
        left += term();
        t = ts.get();
        break;

      case '-':
        left -= term();
        t = ts.get();
        break;

      default:
        ts.putback(t);
        return left;
    }
  }
}

double term()
{
  double left = primary();
  Token t = ts.get();
  while (true){
    switch (t.kind) {
      case '*':
        left *= primary();
        t = ts.get();
        break;
      case '/':
        {
          double d = primary();
          if (d == 0) error("divide by zero");
          left /= d;
          t = ts.get();
          break;
        }
      case '%':
        {
          double d = term();
          int i1 = int(left);
          if (i1 != left) error("left operator not int");
          int i2 = int(d);
          if(i2 != d) error("right operator not int");
          if(i2 == 0) error("%: divide by zero");
          left = i1 % i2;
          t = ts.get();
          break;
        }
      default:
        ts.putback(t);
        return left;
    }
  }
}

double primary()
{
  Token t = ts.get();
  switch (t.kind) {
    case '(':
      {
        double d = expression();
        t = ts.get();
        if (t.kind != ')') error("')' excepted");
        return d;
      }
    case number:
      return t.value;
    case '-':
      return - primary();
    case '+':
      return primary();
    default:
      error("primary expected");
  }
}
