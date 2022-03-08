# include <stdio.h>
# include <stdlib.h>
# include <iostream>
# include <vector>
# include <string>
# include <iomanip>
# include <stack> 
# include <string.h>
# include <ctype.h>
using namespace std; 
int gTestNum ;
char gCh ;
bool gEXIT = false ;
bool gReadEOF = false ;
int gLeft_num ;
int gRight_num ;

bool gFirstInput = false ;
bool gstart = false ;
bool gError = false ;
int gLine = 1 ;
int gColumn = 0 ; 

// ********************
// 用來記錄 換行字元的 num 
int gPrevious_line ;
int gPrevious_column ;
// ********************
struct Node{
  string token ;
  bool empty ;
  bool eval ;
  bool can_check_symbol ;
  Node *left ;
  Node *right ;
}; // node

struct Define {
  string name ;
  Node *head ;
}; // Define

vector<Define> gTable ;

typedef Node *Nodepoint ;
vector<string> gtokenlist ;
vector<string> gNewList ;


bool Scanner( char &readChar ) ;

bool IsDecimal( string str ) {
  int dotnum = 0 ;
  for ( int i = 0 ; i < strlen( str.c_str() ) ; i++ ) {
    if ( str[i] == '.' )
      dotnum++ ;
    else if ( !isdigit( str[i] ) )
      return false ;
  } // for
  
  if ( dotnum != 1 || strlen( str.c_str() ) < 2 )
    return false ;
  else
    return true ;
} // IsDecimal()

bool IsSpecialChar( char ch ) {
  if ( ch == '(' || ch == ')' || ch == '\'' || ch == '\"' || ch == ';' ) {
    return true ;
  } // if
  else 
    return false ;
} // IsSpecialChar()

bool IsWhiteSpace( char ch ) {
  if ( ch == ' ' || ch == '\t' || ch == '\n' ) 
    
    return true ;
  
  else
    return false ;
} // IsWhiteSpace()

bool AllIsDigit( string token ) {
  for ( int i = 0 ; i < strlen( token.c_str() ) ; i++ ) {
    if ( !isdigit( token[i] ) )
      return false ;
  } // for
  
  return true ;
} // AllIsDigit()

bool IsInteger( string token ) {
  for ( int i = 0 ; i < strlen( token.c_str() ) ; i++ ) {
    if ( !isdigit( token[i] ) ) {
      if ( i != 0 || ( token[i] != '+' && token[i] != '-' ) )
        return false ;
    } // if
  } // for
  
  return true ;
} // IsInteger()

bool IsNumber( string token ) {
  if ( IsInteger( token ) )
    return true ;
  // 前面會判斷integer 之後只要判斷小數的case 
  else if ( IsDecimal( token ) )
    return true ;
  else if ( token[0] == '+' ) { // +0.125  +.135456   +13.12.13  +asdff232    尚未考慮 '+.' 
    int dotnum = 0 ;
    string token_temp ;
    bool isDigit = true ; 
    for ( int j = 1 ; j < strlen( token.c_str() ) ; j++ ) {
      if ( !isdigit( token[j] ) && token[j] != '.' )
        isDigit = false ;
      if ( token[j] == '.' )
        dotnum++ ; 
      token_temp = token_temp + token[j] ;
    } // for
 
    if ( isDigit && dotnum < 2 )
      return true ;
    else
      return false ;
  } // else if
  
  else if ( token[0] == '-' ) {  
    int dotnum = 0 ;
    bool isDigit = true ; 
    for ( int j = 1 ; j < strlen( token.c_str() ) ; j++ ) {
      if ( !isdigit( token[j] ) && token[j] != '.' )
        isDigit = false ;
      if ( token[j] == '.' )
        dotnum++ ; 
    } // for

    if ( isDigit && dotnum == 1 ) 
      return true ;

    else
      return false ;
  } // else if
  
  else
    return false ;
} // IsNumber()

bool IsEqual( Nodepoint head1, Nodepoint head2 ) {
  if ( head1 != NULL && head2 != NULL ) {
    if ( head1 -> token == head2 -> token ) {
      if ( IsEqual( head1 -> left, head2 -> left ) && IsEqual( head1 -> right, head2 -> right ) )
        return true ;
      else 
        return false ;
    } // if
    
    else 
      return false ;
  } // if
  
  else if ( head1 == NULL && head2 == NULL ) {
    return true ;
  } // else if
  
  else
    return false ;
} // IsEqual()

bool IsSpecialtoken( string token ) {
  if ( token == "if" || token == "cond" )
    return true ;
  if ( token == "exit" )
    return true ;
  else if ( token == "+" || token == "-" )
    return true ;
  else if ( token == "*" || token == "/" )
    return true ;
  else if ( token == "not" )
    return true ;
  else if ( token == "and" || token == "or" )
    return true ;
  else if ( token == "begin" )
    return true ;
  else if ( token == ">" || token == ">=" )
    return true ;
  else if ( token == "<" || token == "<=" )
    return true ;
  else if ( token == "=" ) 
    return true ;
  else if ( token == "equal?" || token == "eqv?" )
    return true ;
  else if ( token == "string-append" )
    return true ;
  else if ( token == "string>?" || token == "string<?" )
    return true ;
  else if ( token == "string=?" )
    return true ; 
  else if ( token == "car" || token == "cdr" ) 
    return true ;
  else if ( token == "atom?" || token == "pair?" )
    return true ;
  else if ( token == "cons" ) 
    return true ;
  else if ( token == "list" || token == "list?" )
    return true ;
  else if ( token == "null?" )
    return true ;
  else if ( token == "integer?" )
    return true ;
  else if ( token == "real?" || token == "number?" )
    return true ;
  else if ( token == "string?" )
    return true ;
  else if ( token == "boolean?" )
    return true ;
  else if ( token == "symbol?" )
    return true ;
  else 
    return false ;
} // IsSpecialtoken()

bool IsSymbol( string token ) {
  if ( token == "'" || token == "quote"  )
    return false ;
  else if ( token == "" )
    return false ;
  else if ( token == "exit" )
    return false ;
  else if ( token == "#t" || token == "t" ) 
    return false ;
  else if ( token == "if" || token == "cond" )
    return false ;
  else if ( token[0] == '\"' ) 
    return false ;
  else if ( token == "+" || token == "-" )
    return false ;
  else if ( token == "*" || token == "/" )
    return false ;
  else if ( token == "not" )
    return false ;
  else if ( token == "and" || token == "or" )
    return false ;
  else if ( token == "begin" )
    return false ;
  else if ( token == ">" || token == ">=" )
    return false ;
  else if ( token == "<" || token == "<=" )
    return false ;
  else if ( token == "=" ) 
    return false ;
  else if ( token == "define" )
    return false ;
  else if ( token == "equal?" || token == "eqv?" )
    return false ;
  else if ( token == "string-append" )
    return false ;
  else if ( token == "string>?" || token == "string<?" )
    return false ;
  else if ( token == "string=?" )
    return false ;
  else if ( token == "nil" || token == "#f" )
    return false ; 
  else if ( token == "car" || token == "cdr" ) 
    return false ;
  else if ( token == "atom?" || token == "pair?" )
    return false ;
  else if ( token[0] == '+' || token[0] == '-' ) 
    return false ;
  else if ( AllIsDigit( token ) )
    return false ;
  
  else if ( token == "cons" ) 
    return false ;
  else if ( token == "list" || token == "list?" )
    return false ;
  else if ( token == "null?" )
    return false ;
  else if ( token == "integer?" )
    return false ;
  else if ( token == "real?" || token == "number?" )
    return false ;
  else if ( token == "string?" )
    return false ;
  else if ( token == "boolean?" )
    return false ;
  else if ( token == "symbol?" )
    return false ;

  else { // 這裡要處理正常小數的狀況 
    if ( IsDecimal( token ) ) 
      return false ;

    else { // 一般token 的狀況 
      return true ;
    } // else
  } // else
} // IsSymbol()

bool FindSymbol( string token, int &t ) {
  // 暫時不考慮 no define 的 stitution 
  while ( t < gTable.size() ) {
    if ( gTable[t].name == token )
      return true ;
    t++ ;
  } // while
  
  return false ;
} // FindSymbol()

void DeffineError( string token ) {
  if ( token == "#f"  ) {
    cout << "nil" ;
  } // if
  
  else if ( token == "t" ) {
    cout << "#t" ;
  } // else if
  
  else if ( token == "'" ) {
    cout << "quote" ;  
  } // else if
  
  else if ( token == "+" ) {
    cout << "+" ;
  } // else if
  
  else if ( token == "-" ) {
    cout << "-" ;
  } // else if
  
  else if ( token[0] == '\"' ) { // '\t' '\n' '\\'
    for ( int j = 0 ; j < strlen( token.c_str() ) ; j++ ) {
      if ( j + 1 < strlen( token.c_str() ) && token[j] == '\\' ) {
        if ( token[j+1] == '\\' || token[j+1] == '\"' ) {
          j = j + 1 ;
          cout << token[j] ;
        } // if

        else if ( token[j+1] == 'n' ) {
          cout << endl ;
          j++ ;
        } // else if

        else if ( token[j+1] == 't' ) {
          cout << '\t' ;
          j++ ;
        } // else if

        else // 如果都不是，print '\' 
          cout << token[j] ;
      } // if

      else {
        cout << token[j] ;
        if ( token[j] == '\"' && j != 0 )
          j = strlen( token.c_str() ) ;
      } // else
    } // for
  } // else if

  else if ( token[0] == '+' ) { // +0.125  +.135456   +13.12.13  +asdff232    尚未考慮 '+.' 
    int dotnum = 0 ;
    string token_temp ;
    bool isDigit = true ; 
    for ( int j = 1 ; j < strlen( token.c_str() ) ; j++ ) {
      if ( !isdigit( token[j] ) && token[j] != '.' )
        isDigit = false ;
      if ( token[j] == '.' )
        dotnum++ ; 
      token_temp = token_temp + token[j] ;
    } // for

    // 整數不能直接印要把+號拿掉才print，float 轉換後直接 print 
    if ( isDigit && dotnum < 2 ) {
      if ( dotnum == 1 ) { // float
        float float_temp = atof( token.c_str() ) ;
        printf( "%.3f", float_temp ) ; 
      } // if
      
      else // int
        cout << token_temp ;
    } // if

    else
      cout << token ;
  } // else if


  else if ( token[0] == '-' ) {  
    int dotnum = 0 ;
    bool isDigit = true ; 
    for ( int j = 1 ; j < strlen( token.c_str() ) ; j++ ) {
      if ( !isdigit( token[j] ) && token[j] != '.' )
        isDigit = false ;
      if ( token[j] == '.' )
        dotnum++ ; 
    } // for

    // 負的整數或其他東東可以直接輸出，如果是小數要轉成小數 
    if ( isDigit && dotnum == 1 ) {
      float float_temp = atof( token.c_str() ) ;
      printf( "%.3f", float_temp ) ; 
    } // if

    else
      cout << token ;
  } // else if

  else { // 這裡要處理正常小數的狀況 
    if ( IsDecimal( token ) ) {
      float float_temp = atof( token.c_str() ) ;
      printf( "%.3f", float_temp ) ; 
    } // if

    else
      cout << token ;
  } // else

  cout  << endl ;
} // DeffineError()

void PrintDefineError( Node *head, int count, bool dot_right, bool &first_token ) {
  // first_token 用來判斷說他是不是印第一個token
  // 是的話就不用印 white space 
  if ( head != NULL ) {
    
    if ( head -> left != NULL && head -> left -> empty == true ) { // 有另一條龍 
      if ( !first_token ) {
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      first_token = true ;
      cout << "( " ;
      count = count + 2 ;
    } // if
    
    PrintDefineError( head -> left, count, false, first_token ) ;
    
    if ( head -> left != NULL && head -> left -> empty == true ) { // 有另一條龍 
      count = count - 2 ;
      if ( !first_token ) {
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      cout << ")" << endl ;
    } // if
    
    // *************
    // empty == true || dot_right == true && token == nil
    if ( head -> empty != true && ( dot_right != true || head -> token != "nil" ) ) {
      if ( !first_token ) {
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      if ( dot_right == true ) {
        
        cout << "." << endl ;
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      DeffineError( head -> token ) ;
      first_token = false ;
    } // if
    // ***********
    
    PrintDefineError( head -> right, count, true, first_token ) ;
  } // if
} // PrintDefineError()

bool ReadSExp( char &readChar ) {
  while ( IsWhiteSpace( readChar ) || readChar == ';' ) { // 處理註解或是 white space  
    if ( readChar == ';' ) { // 讀到 換行字元 '\n' 
      while ( readChar != '\n' ) {
        // scanner 會幫我改變 readChar 裡的東西 
        if ( Scanner( readChar ) == false ) { // 發生 EOF error 
          gstart = true ;
          return false ;
        } // if
      } // while
    } // if
    
    else { // readChar == whiteSpace
      // read 一個 char 就丟回上面的 while 去判斷 
      if ( Scanner( readChar ) == false ) { // 發生 EOF error 
        gstart = true ;
        return false;
      } // if
    } // else
  } // while
  
  gFirstInput = false ;
  // ********************************************************
  if ( readChar == '(' ) {
    string temp ;
    temp = temp + readChar ;
    gtokenlist.push_back( temp ) ;
    return true ;
  } // if 

  else if ( readChar == ')' ) {
    string temp ;
    temp = temp + readChar ;
    gtokenlist.push_back( temp ) ;
    return true ;
  } // else if

  else if ( readChar == '\"' ) {
    string temp ;
    temp = temp + readChar ;
    if ( Scanner( readChar ) ) { // readChar read 到 雙引號 的下個字元 
      while ( readChar != '\"' ) { // 一直 read 到 右雙引號為止 
        if ( readChar == '\n' ) { 
          // 發現在右雙引號前 就 有換行字元 '\n'  => error 
          // 否則 接給string  
          cout <<  "ERROR (no closing quote) : END-OF-LINE encountered at Line " << 
                gPrevious_line << " Column " << gPrevious_column << endl << endl ; 
          return false ;
        } // if
        
        else {
          temp = temp + readChar ;
          char previous_char = readChar ;
          if ( !Scanner( readChar ) ) // 讀下一個進來 
            return false ; // error 
          if ( previous_char == '\\' && readChar == '\"' ) { // 跳脫 \" 處理  多讀一次 
            temp = temp + readChar ;
            if ( !Scanner( readChar ) ) 
              return false ; // error
          } // if
        } // else
      } // while
      // 出while 表示 string 讀完 readChar == '"' 
      temp = temp + readChar ;
      gtokenlist.push_back( temp ) ;
      return true ;
    } // if
    
    else // error 
      return false ;
  } // else if
  
  else if ( readChar == '\'' ) {
    string temp ;
    temp = temp + readChar ;
    gtokenlist.push_back( temp ) ;
    return true ;
  } // else if
  
  else {
    string temp ;
    temp = temp + readChar ;
    bool canRead = true ;
    if ( cin.peek() == EOF ) 
      canRead = false ;
    if ( canRead ) {
      gCh = cin.peek() ;
      while ( canRead && ( !IsSpecialChar( gCh ) && !IsWhiteSpace( gCh ) ) ) { // 如果下一個是正常char 就讀進來 接string 
        Scanner( readChar ) ;
        temp = temp + readChar ;
        if ( cin.peek() == EOF ) 
          canRead = false ;
        gCh = cin.peek() ;
      } // while
      // 出while 表示 下一個是特殊char 或是後面 EOF 
    } // if
    
    gtokenlist.push_back( temp ) ;
    return true ;
  } // else
 
} // ReadSExp()

void DealQuote( Nodepoint &head ) {
  if ( head != NULL ) {
    head -> eval = true ;
    DealQuote( head -> left ) ;
    DealQuote( head -> right ) ;
  } // if
} // DealQuote()

void BuildTree( Nodepoint &temp, int &index ) {
  if ( gNewList[index] == "(" ) {
    temp = new Node ;
    temp -> empty = true ;
    temp -> eval = false ;
    temp -> left = NULL ;
    temp -> right = NULL ;
    index ++ ;
    BuildTree( temp -> left, index ) ; 
    index++ ; // 這裡應該會是 dot 
    BuildTree( temp -> right, index ) ;
    index++ ; // 這裡是 右括號 ) 
  } // if
  
  else if ( gNewList[index] == "." ) {
    
  } // else if
  
  else { // 一般token 
    temp = new Node ;
    temp -> empty = false ;
    temp -> eval = false ;
    temp -> can_check_symbol = true ;
    temp -> token = gNewList[index] ;
    index++ ;
    temp -> left = NULL ;
    temp -> right = NULL ;
  } // else
 
} // BuildTree()

void PutInTable( string name, Nodepoint head ) {
  // 暫時不考慮重複 define 的stitution 
  bool defined = false ;
  for ( int i = 0 ; i < gTable.size() ; i++ ) {
    if ( gTable[i].name == name ) {
      gTable[i].head = head ;
      defined = true ;
    } // if
  } // for
  
  if ( defined == false ) {
    Define temp ;
    temp.name = name ;
    temp.head = head ;
    gTable.push_back( temp ) ;
  } // if
} // PutInTable()

void PrintSExp( Nodepoint head ) {
  if ( head -> token == "#f"  ) {
    cout << "nil" ;
  } // if
  
  else if (  head -> token == "'" ) {
    cout << "quote" ;  
  } // else if
  
  else if (  head -> token == "t" ) {
    cout << "#t" ;
  } // else if
  
  else if ( head -> token == "+" ) {
    if ( head -> eval == false )  // 是有意義的token 
      cout << "#<procedure " <<  head -> token << ">" ;
    else
      cout << head -> token ;
  } // else if
  
  else if ( head -> token == "-" ) {
    if ( head -> eval == false )  // 是有意義的token 
      cout << "#<procedure " <<  head -> token << ">" ;
    else
      cout << head -> token ;
  } // else if
  
  else if ( head -> token[0] == '\"' ) { // '\t' '\n' '\\'
    for ( int j = 0 ; j < strlen( head -> token.c_str() ) ; j++ ) {
      if ( j + 1 < strlen( head -> token.c_str() ) && head -> token[j] == '\\' ) {
        if ( head -> token[j+1] == '\\' || head -> token[j+1] == '\"' ) {
          j = j + 1 ;
          cout << head -> token[j] ;
        } // if

        else if ( head -> token[j+1] == 'n' ) {
          cout << endl ;
          j++ ;
        } // else if

        else if ( head -> token[j+1] == 't' ) {
          cout << '\t' ;
          j++ ;
        } // else if

        else // 如果都不是，print '\' 
          cout << head -> token[j] ;
      } // if

      else {
        cout << head -> token[j] ;
        if ( head -> token[j] == '\"' && j != 0 )
          j = strlen( head -> token.c_str() ) ;
      } // else
    } // for
  } // else if

  else if ( head -> token[0] == '+' ) { // +0.125  +.135456   +13.12.13  +asdff232    尚未考慮 '+.' 
    int dotnum = 0 ;
    string token_temp ;
    bool isDigit = true ; 
    for ( int j = 1 ; j < strlen( head -> token.c_str() ) ; j++ ) {
      if ( !isdigit( head -> token[j] ) && head -> token[j] != '.' )
        isDigit = false ;
      if ( head -> token[j] == '.' )
        dotnum++ ; 
      token_temp = token_temp +  head -> token[j] ;
    } // for

    // 整數不能直接印要把+號拿掉才print，float 轉換後直接 print 
    if ( isDigit && dotnum < 2 ) {
      if ( dotnum == 1 ) { // float
        float float_temp = atof( head -> token.c_str() ) ;
        printf( "%.3f", float_temp ) ; 
      } // if
      
      else // int
        cout << token_temp ;
    } // if

    else
      cout << head -> token ;
  } // else if

  else if ( head -> token[0] == '-' ) {  
    int dotnum = 0 ;
    bool isDigit = true ; 
    for ( int j = 1 ; j < strlen( head -> token.c_str() ) ; j++ ) {
      if ( !isdigit( head -> token[j] ) && head -> token[j] != '.' )
        isDigit = false ;
      if ( head -> token[j] == '.' )
        dotnum++ ; 
    } // for

    // 負的整數或其他東東可以直接輸出，如果是小數要轉成小數 
    if ( isDigit && dotnum == 1 ) {
      float float_temp = atof( head -> token.c_str() ) ;
      printf( "%.3f", float_temp ) ; 
    } // if

    else 
      cout << head -> token ;
  } // else if

  else { // 這裡要處理正常小數的狀況 
    if ( IsDecimal( head -> token ) ) {
      float float_temp = atof( head -> token.c_str() ) ;
      printf( "%.3f", float_temp ) ; 
    } // if

    else { // 一般token 的狀況 
      if ( IsSpecialtoken( head -> token ) && head -> eval == false )  // 是有意義的token 
        cout << "#<procedure " <<  head -> token << ">" ;
      else {
        cout << head -> token ;
      } // else 
    } // else
  } // else

  cout  << endl ;
} // PrintSExp()

void PrintNode( Node *head, int count, bool dot_right, bool &first_token ) {
  // first_token 用來判斷說他是不是印第一個token
  // 是的話就不用印 white space 
  if ( head != NULL ) {
    
    if ( head -> left != NULL && head -> left -> empty == true ) { // 有另一條龍 
      if ( !first_token ) {
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      first_token = true ;
      cout << "( " ;
      count = count + 2 ;
    } // if
    
    PrintNode( head -> left, count, false, first_token ) ;
    
    if ( head -> left != NULL && head -> left -> empty == true ) { // 有另一條龍 
      count = count - 2 ;
      if ( !first_token ) {
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      cout << ")" << endl ;
    } // if
    
    // *************
    // empty == true || dot_right == true && token == nil
    if ( head -> empty != true && ( dot_right != true || head -> token != "nil" ) ) {
      if ( !first_token ) {
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      if ( dot_right == true ) {
        
        cout << "." << endl ;
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      PrintSExp( head ) ;
      first_token = false ;
    } // if 
    // ***********
    
    PrintNode( head -> right, count, true, first_token ) ;
  } // if
} // PrintNode()

bool Check_Symbol( Nodepoint &head ) { // 指展開一格  
  if ( head != NULL ) {
    if ( head -> empty == false && IsSymbol( head -> token ) && head -> can_check_symbol == true ) {
      // 發現是symbol 去table找位置
      int t = 0 ;
      if ( FindSymbol( head -> token, t ) ) {
        head = gTable[t].head ;
        return true ;
      } // if
      
      else {
        if ( head -> token == "clean-environment" )
          cout << "ERROR (level of CLEAN-ENVIRONMENT)" << endl ;
        else if ( head -> token == "define" )
          cout << "ERROR (level of DEFINE)" << endl ; 
        else
          cout << "ERROR (unbound symbol) : " << head -> token << endl ;
          
        return false ;
      } // else
    } // if
    
    else if ( head -> token == "define" ) {
      cout << "ERROR (level of DEFINE)" << endl ; 
      return false ;
    } // else if
  } // if
  
  return true ;
} // Check_Symbol()

bool Check_AllSymbol( Nodepoint &head ) { // 此function 只用於 define , 注意quote 的情況 
  if ( head != NULL ) {
    if ( Check_AllSymbol( head -> left ) && Check_AllSymbol( head -> right ) ) {
      if ( Check_Symbol( head ) == false )
        return false ;
      head -> can_check_symbol = false ; // 不讓人展開 => 已經展開過了 不給check symbol 
      return true ;
    } // if
    
    return false ;
  } // if
  
  return true ;
} // Check_AllSymbol()

bool IsList( Nodepoint head ) {
  if ( head != NULL && head -> left != NULL ) {
    if ( head -> right -> empty == false && head -> right -> token != "nil" )
      return false ;
    if ( !IsList( head -> left ) || !IsList( head -> right ) )
      return false ;
    else 
      return true ;
  } // if
  
  return true ;
} // IsList()

void FindQuote( Nodepoint &head, bool check ) {
  if ( head != NULL ) {
    FindQuote( head -> left, check ) ;
    if ( head -> left != NULL && ( head -> left -> token == "'" || head -> left -> token == "quote" ) ) {
      // 左邊節點是 quote 則將右邊的所有節點都不能展開 
      check = false ;
    } // if
    
    if ( check == false )
      head -> can_check_symbol = false ;
    FindQuote( head -> right, check ) ;
  } // if
} // FindQuote()

bool Check_number_error( Nodepoint head, string operation ) {
  if ( IsNumber( head -> token ) == false ||  head -> empty == true ) { // 不是數字的情況 
    cout << "ERROR (" << operation << " with incorrect argument type) : " ;
    int count = 2 ;
    if ( head -> empty == true ) {
      bool first_token = true ;
      cout << "( " ;
      PrintDefineError( head, count, false, first_token ) ;
      cout << ")" << endl ;
    } // if
            
    else { // 一個token 
      bool first_token = true ;
      PrintDefineError( head, count, false, first_token ) ;
    } // else
    
    return false ;
  }  // if
  
  return true ;
} // Check_number_error()

bool Check_string_error( Nodepoint head, string operation ) {
  if ( head -> token[0] != '"' ) {
    cout << "ERROR (" << operation << " with incorrect argument type) : " ;
    int count = 2 ;
    if ( head -> empty == true ) {
      bool first_token = true ;
      cout << "( " ;
      PrintDefineError( head, count, false, first_token ) ;
      cout << ")" << endl ;
    } // if
            
    else { // 一個token 
      bool first_token = true ;
      PrintDefineError( head, count, false, first_token ) ;
    } // else
    
    return false ;
  }  // if
  
  return true ;
} // Check_string_error()

bool ReBuildTree( Nodepoint &head ) {
  if ( head != NULL && head -> left != NULL ) {
    if ( ReBuildTree( head -> left ) && Check_Symbol( head -> left ) ) {
      if ( head -> left -> token == "define" ) {
        cout << "ERROR (level of DEFINE)" << endl ;
        return false ;
      } // if
      
      else if ( head -> left -> token == "eqv?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ||
             head -> right -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : eqv?" << endl ;
          return false ;
        } // if
        
        // 分3種case 1. 都是symbol 2. 都不是  3. 一個是sybol 一個不是
        // string 不是symbol ，例： "HI", "HI" 這兩個出來要是 nil 
        if ( IsSymbol( head -> right -> left -> token ) && 
             IsSymbol( head -> right -> right -> left -> token ) ) {
          if ( !Check_Symbol( head -> right -> left ) || !Check_Symbol( head -> right -> right -> left ) )
            return false ;
          if ( head -> right -> left == head -> right -> right -> left )
            temp -> token = "#t" ;
          else 
            temp -> token = "nil" ;
        } // if 
        
        else if ( !IsSymbol( head -> right -> left -> token ) && 
                  !IsSymbol( head -> right -> right -> left -> token ) ) {
          // 如果不是數字 都 false 
          // 是數字的話再去比較 
          if ( !ReBuildTree( head -> right -> left ) || 
               !ReBuildTree( head -> right -> right -> left ) ) 
            return false ;
          if ( head -> right -> left -> empty == false && 
               head -> right -> right -> left -> empty == false &&
               IsNumber( head -> right -> left -> token ) && 
               IsNumber( head -> right -> right -> left -> token ) ) {
            if ( head -> right -> left -> token ==  head -> right -> right -> left -> token )
              temp -> token = "#t" ;
            else 
              temp -> token = "nil" ;
          } // if
          
          else if ( head -> right -> left == head -> right -> right -> left )
            temp -> token = "#t" ;
          else
            temp -> token = "nil" ;
          
        } // else if
        
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "cond" ) {
        Node *temp = new Node ;
        temp = head -> right ;
        Node *walk = temp -> left ;
        Check_Symbol( walk -> left ) ;
        if ( ReBuildTree( walk -> left ) ) {
          // 出while 的條件為不是nil 
          while ( walk -> left -> token == "nil" || walk -> left -> token == "#f" ) {
            temp = temp -> right ;
            walk = temp -> left ;
            if ( temp -> right -> token == "nil" && walk -> left -> token == "else" ) 
              walk -> left -> token = "#t" ;
            
            else { // walk -> right -> token == "nil"
              Check_Symbol( walk -> left ) ;
              ReBuildTree( walk -> left ) ;
            } // else
          } // while
          
          walk = walk -> right ;
          Check_Symbol( walk -> left ) ;
          ReBuildTree( walk -> left ) ;
          while ( walk -> right -> token != "nil" ) {
            walk = walk -> right ;
            Check_Symbol( walk -> left ) ;
            ReBuildTree( walk -> left ) ;
          } // while
          
          head = walk -> left ;
          return true ;
        } // if
        
        return false ;
      } // else if
      
      else if ( head -> left -> token == "cons" ) {
        Node *temp = new Node ;
        temp -> empty = true ;
        // check 是不是list 
        Node *w = head -> right ;
        while ( w != NULL ) {
          if ( w -> empty == false && w -> token != "nil" ) {
            int count = 2 ;
            cout << "ERROR (non-list) : " ;
            bool first_token = true ;
            cout << "( " ;
            PrintDefineError( head, count, false, first_token ) ;
            cout << ")" << endl ;
            return false ;
          } // if
          
          w = w -> right ; 
        } // while
        
        // check 參數個數是否正確 
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ||
             head -> right -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : cons" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) ||
             !ReBuildTree( head -> right -> left ) ||
             !Check_Symbol( head -> right -> right -> left ) || 
             !ReBuildTree( head -> right -> right -> left ) ) 
          return false ;
          
        temp -> left = head -> right -> left ;
        temp -> right = head -> right -> right -> left ;
        head = temp ;
        return true ;
      } // else if
      
      else if ( head -> left -> token == "list" ) {
        Node *temp = new Node ;
        temp -> empty = true ;
        // ******** list check
        Node *w = head -> right ;
        while ( w != NULL ) {
          if ( w -> empty == false && w -> token != "nil" ) {
            int count = 2 ;
            cout << "ERROR (non-list) : " ;
            bool first_token = true ;
            cout << "( " ;
            PrintDefineError( head, count, false, first_token ) ;
            cout << ")" << endl ;
            return false ;
          } // if
          
          w = w -> right ; 
        } // while
        
        Node *walk = head ;
        if ( walk -> right -> token == "nil" ) // list 是空的 ，後面沒有東西 
          head = head -> right ;
        else {
          while ( walk -> right -> token != "nil" ) {
            walk = walk -> right ;
            if ( Check_Symbol( walk -> left ) == false || ReBuildTree( walk -> left ) == false )
              return false ;
          } // while
          
          temp -> left = head -> right -> left ;
          temp -> right = head -> right -> right ;
          head = temp ; 
        } // else
        
        return true ;
      } // else if
      
      else if ( head -> left -> token == "'" || head -> left -> token == "quote" ) {
        // quote 裡的 東東 都要是eval => 把 eval 設成 true ; 
        Node *temp ;
        temp = head -> right -> left ; 
        head = temp ;  
        DealQuote( head ) ;
        return true ;
      } // else if
      
      else if ( head -> left -> token == "car" ) {
        Node *temp ;
        // check argument count
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : car" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        // insure have left and right 
        if ( head -> right -> left -> empty == false ) { // 表示不為空 直接接一個token 
          cout << "ERROR (car with incorrect argument type) : " << head -> right -> left -> token << endl ;
          return false ;
        } // if
        
        temp = head -> right -> left -> left ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "cdr" ) {
        Node *temp ;
        // check argument count
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : cdr" << endl ;
          return false ;
        } // if
        
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> empty == false ) { // 表示不為空 直接接一個token 
          cout << "ERROR (cdr with incorrect argument type) : " << head -> right -> left -> token << endl ;
          return false ;
        } // if
        
        temp = head -> right -> left -> right ;
        head = temp ;
        return true ;
      } // else if
      
      else if ( head -> left -> token == "atom?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : atom?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> empty == false ) 
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "pair?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : pair?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> empty == true ) 
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
        
      else if ( head -> left -> token == "list?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : list?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> empty == true && IsList( head -> right -> left ) ) 
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "null?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : null?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> token == "nil" || head -> right -> left -> token == "#f" ) 
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "integer?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : integer?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> empty == false && IsInteger( head -> right -> left -> token ) )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "real?" || head -> left -> token == "number?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : " << head -> left -> token << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> empty == false && IsNumber( head -> right -> left -> token ) )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "string?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : string?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> token[0] == '\"' )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "boolean?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : boolean?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> token == "nil" || head -> right -> left -> token == "#f" || 
             head -> right -> left -> token == "#t" )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "symbol?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : symbol?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> empty == false && IsSymbol( head -> right -> left -> token ) )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "+" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : +" << endl ;
          return false ;
        } // if
        // **********parameter END
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
          
        bool isInt = true ;
        // 為 false 表示error 裡面不是數字
        if ( Check_number_error( head -> right -> left, "+" ) == false )  
          return false ;
           
        // 先用float做宣告 然後持續判斷有沒有float ，如果沒有的話最後 assign 給 int 
        // 先判斷說他是什麼型別 integer 或是 float 在做宣告
        if ( IsInteger( head -> right -> left -> token ) == false ) // 不是integer ==> 是float 
          isInt = false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) ) 
          return false ;
        // 為 false 表示error 裡面不是數字
        if ( Check_number_error( walk -> left, "+" ) == false )  
          return false ;
          
        if ( IsInteger( walk -> left -> token ) == false )
          isInt = false ;
        ans = ans + atof( walk -> left -> token.c_str() ) ;
        
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, "+" ) == false )  
            return false ;
          if ( IsInteger( walk -> left -> token ) == false )
            isInt = false ;
          ans = ans + atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        char st[10] ;
        if ( isInt ) {
          int t_ans = ans ;
          sprintf( st, "%d", t_ans ) ;
        } // if
        
        else 
          sprintf( st, "%.3f", ans ) ;
        for ( int i = 0 ; i < strlen( st ) ; i++ )
          temp -> token = temp -> token + st[i] ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "-" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : -" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        bool isInt = true ;
        if ( Check_number_error( head -> right -> left, "-" ) == false )  
          return false ;
        
        if ( IsInteger( head -> right -> left -> token ) == false )
          isInt = false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) ) 
          return false ;
        if ( Check_number_error( walk -> left, "-" ) == false )  
          return false ;
        if ( IsInteger( walk -> left -> token ) == false )
          isInt = false ;
        ans = ans - atof( walk -> left -> token.c_str() ) ;
        
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, "-" ) == false )  
            return false ;
          if ( IsInteger( walk -> left -> token ) == false )
            isInt = false ;
          ans = ans - atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        char st[10] ;
        if ( isInt ) {
          int t_ans = ans ;
          sprintf( st, "%d", t_ans ) ;
        } // if
        
        else 
          sprintf( st, "%.3f", ans ) ;
        for ( int i = 0 ; i < strlen( st ) ; i++ )
          temp -> token = temp -> token + st[i] ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "*" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : *" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        bool isInt = true ;
        if ( Check_number_error( head -> right -> left, "*" ) == false )  
          return false ;
        if ( IsInteger( head -> right -> left -> token ) == false )
          isInt = false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) ) 
          return false ;
        if ( Check_number_error( walk -> left, "*" ) == false )  
          return false ;
        if ( IsInteger( walk -> left -> token ) == false )
          isInt = false ;
        ans = ans * atof( walk -> left -> token.c_str() ) ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, "*" ) == false )  
            return false ;
          if ( IsInteger( walk -> left -> token ) == false )
            isInt = false ;
          ans = ans * atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        char st[10] ;
        if ( isInt ) {
          int t_ans = ans ;
          sprintf( st, "%d", t_ans ) ;
        } // if
        
        else 
          sprintf( st, "%.3f", ans ) ;
        for ( int i = 0 ; i < strlen( st ) ; i++ )
          temp -> token = temp -> token + st[i] ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "/" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : /" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        bool isInt = true ;
        if ( Check_number_error( head -> right -> left, "/" ) == false )  
          return false ;
        if ( IsInteger( head -> right -> left -> token ) == false )
          isInt = false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) ) 
          return false ;
        if ( Check_number_error( walk -> left, "/" ) == false )  
          return false ;
        if ( IsInteger( walk -> left -> token ) == false )
          isInt = false ;
        if ( atof( walk -> left -> token.c_str() ) == 0.0 ) {
          cout << "ERROR (division by zero) : /" << endl ;
          return false ;
        } // if
        
        ans = ans / atof( walk -> left -> token.c_str() ) ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, "/" ) == false )  
            return false ;
          if ( IsInteger( walk -> left -> token ) == false )
            isInt = false ;
          if ( atof( walk -> left -> token.c_str() ) == 0.0 ) {
            cout << "ERROR (division by zero) : /" << endl ;
            return false ;
          } // if
          
          ans = ans / atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        char st[10] ;
        if ( isInt ) {
          int t_ans = ans ;
          sprintf( st, "%d", t_ans ) ;
        } // if
        
        else 
          sprintf( st, "%.3f", ans ) ;
        for ( int i = 0 ; i < strlen( st ) ; i++ )
          temp -> token = temp -> token + st[i] ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "not" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : not" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( head -> right -> left -> token == "nil" || head -> right -> left -> token == "#f" )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == ">" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : >" << endl ;
          return false ;
        } // if
        
        bool istrue = true ;
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_number_error( head -> right -> left, ">" ) == false )  
          return false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_number_error( walk -> left, ">" ) == false )  
          return false ;
        if ( ans <= atof( walk -> left -> token.c_str() ) )
          istrue = false ;
        ans = atof( walk -> left -> token.c_str() ) ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, ">" ) == false )  
            return false ;
          if ( ans <= atof( walk -> left -> token.c_str() ) )
            istrue = false ;
          ans = atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        if ( istrue )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == ">=" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : >=" << endl ;
          return false ;
        } // if
        
        bool istrue = true ;
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_number_error( head -> right -> left, ">=" ) == false )  
          return false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_number_error( walk -> left, ">=" ) == false )  
          return false ;
        if ( ans < atof( walk -> left -> token.c_str() ) )
          istrue = false ;
        ans = atof( walk -> left -> token.c_str() ) ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, ">=" ) == false )  
            return false ;
          if ( ans < atof( walk -> left -> token.c_str() ) )
            istrue = false ;
          ans = atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        if ( istrue )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "<" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : <" << endl ;
          return false ;
        } // if
        
        bool istrue = true ;
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_number_error( head -> right -> left, "<" ) == false )  
          return false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_number_error( walk -> left, "<" ) == false )  
          return false ;
        if ( ans >= atof( walk -> left -> token.c_str() ) )
          istrue = false ;
        ans = atof( walk -> left -> token.c_str() ) ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, "<" ) == false )  
            return false ;
          if ( ans >= atof( walk -> left -> token.c_str() ) )
            istrue = false ;
          ans = atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        if ( istrue )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "<=" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : <=" << endl ;
          return false ;
        } // if
        
        bool istrue = true ;
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_number_error( head -> right -> left, "<=" ) == false )  
          return false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_number_error( walk -> left, "<=" ) == false )  
          return false ;
        if ( ans > atof( walk -> left -> token.c_str() ) )
          istrue = false ;
        ans = atof( walk -> left -> token.c_str() ) ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, "<=" ) == false )  
            return false ;
          if ( ans > atof( walk -> left -> token.c_str() ) )
            istrue = false ;
          ans = atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        if ( istrue )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "=" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : =" << endl ;
          return false ;
        } // if
        
        bool istrue = true ;
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_number_error( head -> right -> left, "=" ) == false )  
          return false ;
        float ans = atof( head -> right -> left -> token.c_str() ) ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_number_error( walk -> left, "=" ) == false )  
          return false ;
        if ( ans != atof( walk -> left -> token.c_str() ) )
          istrue = false ;
        ans = atof( walk -> left -> token.c_str() ) ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_number_error( walk -> left, "=" ) == false )  
            return false ;
          if ( ans != atof( walk -> left -> token.c_str() ) )
            istrue = false ;
          ans = atof( walk -> left -> token.c_str() ) ;
        } // while 
        
        if ( istrue )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "string-append" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : string-append" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_string_error( head -> right -> left, "string-append" ) == false )  
          return false ;
        string ans = head -> right -> left -> token ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_string_error( walk -> left, "string-append" ) == false )  
          return false ;
        for ( int i = 1 ; i < strlen( walk -> left -> token.c_str() ) - 1 ; i++ ) 
          ans.insert( ans.end() - 1, walk -> left -> token[i] ) ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_string_error( walk -> left, "string-append" ) == false )  
            return false ;
          for ( int i = 1 ; i < strlen( walk -> left -> token.c_str() ) - 1 ; i++ ) 
            ans.insert( ans.end() - 1, walk -> left -> token[i] );
        } // while 
        
        temp -> token = ans ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "string>?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : string>?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_string_error( head -> right -> left, "string>?" ) == false )  
          return false ;
        bool istrue = true ;
        string ans = head -> right -> left -> token ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_string_error( walk -> left, "string>?" ) == false )  
          return false ;
        if ( ans <= walk -> left -> token )
          istrue = false ;
        ans = walk -> left -> token ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_string_error( walk -> left, "string>?" ) == false )  
            return false ;
          if ( ans <= walk -> left -> token )
            istrue = false ;
          ans = walk -> left -> token ;
        } // while 
        
        if ( istrue )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "string<?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : string<?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_string_error( head -> right -> left, "string<?" ) == false )  
          return false ;
        bool istrue = true ;
        string ans = head -> right -> left -> token ;
        
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_string_error( walk -> left, "string<?" ) == false )  
          return false ;
        if ( ans >= walk -> left -> token )
          istrue = false ;
        ans = walk -> left -> token ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_string_error( walk -> left, "string<?" ) == false )  
            return false ;
          if ( ans >= walk -> left -> token )
            istrue = false ;
          ans = walk -> left -> token ;
        } // while 
        
        if ( istrue )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "string=?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ) {
          cout << "ERROR (incorrect number of arguments) : string=?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) || !ReBuildTree( head -> right -> left ) ) 
          return false ;
        if ( Check_string_error( head -> right -> left, "string=?" ) == false )  
          return false ;
        bool istrue = true ;
        string ans = head -> right -> left -> token ;
        Node *walk = head -> right -> right ;
        if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
          return false ;
        if ( Check_string_error( walk -> left, "string=?" ) == false )  
          return false ;
        if ( ans != walk -> left -> token )
          istrue = false ;
        ans = walk -> left -> token ;
        while ( walk -> right -> token != "nil" ) {
          walk = walk -> right ;
          if ( !Check_Symbol( walk -> left ) || !ReBuildTree( walk -> left ) )
            return false ;
          if ( Check_string_error( walk -> left, "string=?" ) == false )  
            return false ;
          if ( ans != walk -> left -> token )
            istrue = false ;
          ans = walk -> left -> token ;
        } // while 
        
        if ( istrue )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ; 
        return true ;
      } // else if
      
      else if ( head -> left -> token == "equal?" ) {
        Node *temp = new Node ;
        if ( head -> right -> token == "nil" || 
             head -> right -> right -> token == "nil" ||
             head -> right -> right -> right -> token != "nil" ) {
          cout << "ERROR (incorrect number of arguments) : equal?" << endl ;
          return false ;
        } // if
        
        if ( !Check_Symbol( head -> right -> left ) ||
             !ReBuildTree( head -> right -> left ) ||
             !Check_Symbol( head -> right -> right -> left ) || 
             !ReBuildTree( head -> right -> right -> left ) ) 
          return false ;
        if ( IsEqual( head -> right -> left, head -> right -> right -> left ) )
          temp -> token = "#t" ;
        else 
          temp -> token = "nil" ;
        temp -> empty = false ;
        temp -> left = NULL ;
        temp -> right = NULL ;
        head = temp ;
        return true ; 
      } // else if
      
      else if ( head -> left -> token == "if" ) {
        Node *temp = new Node ;
        Check_Symbol( head -> right -> left ) ;
        if ( ReBuildTree( head -> right -> left ) ) {
          if ( head -> right -> left -> token != "nil" && head -> right -> left -> token != "#f" ) { // 取第一個 
            Check_Symbol( head -> right -> right -> left ) ;
            ReBuildTree( head -> right -> right -> left ) ;
            temp = head -> right -> right -> left ;
          } // if
          
          else {
            Check_Symbol( head -> right -> right -> right -> left ) ;
            ReBuildTree( head -> right -> right -> right -> left ) ;
            temp = head -> right -> right -> right -> left ;
          } // else
          
          head = temp ;
          return true ;
        } // if
        
        return false ;
      } // else if
    
      else if ( head -> left -> token == "begin" ) {
        Node *temp = new Node ;
        Check_Symbol( head -> right -> left ) ;
        if ( ReBuildTree( head -> right -> left ) ) {
          temp = head -> right ;
          Check_Symbol( temp -> left ) ;
          ReBuildTree( temp -> left ) ;
          while ( temp -> right -> token != "nil" ) {
            temp = temp -> right ;
            Check_Symbol( temp -> left ) ;
            ReBuildTree( temp -> left ) ; 
          } // while
          
          head = temp -> left ;
          return true ;
        } // if
        
        return false ;
      } // else if
      
      else if ( head -> left -> token == "and" ) {
        Node *temp = new Node ;
        Check_Symbol( head -> right -> left ) ;
        if ( ReBuildTree( head -> right -> left ) ) {
          if ( head -> right -> left -> token == "nil" || head -> right -> left -> token == "#f" ) {
            // 是 nil ， return nil
            temp -> token = "nil" ;
            temp -> empty = false ;
            temp -> left = NULL ;
            temp -> right = NULL ;
            head = temp ;
          } // if
          
          else if ( head -> right -> left -> token == "#t" ) {
            Node *walk = head -> right -> right ;
            Check_Symbol( walk -> left ) ;
            ReBuildTree( walk -> left ) ;
            while ( walk -> right -> token != "nil" ) {
              walk = walk -> right ;
              Check_Symbol( walk -> left ) ;
              ReBuildTree( walk -> left ) ;
            } // while
            
            head = walk -> left ;
          } // else if
          
          else { // 第一個得到的是 token， 一直往下找 找到 找到最尾 看是 #t or nil
            temp = head -> right -> left ;
            Node *walk = head -> right -> right ; 
            Check_Symbol( walk -> left ) ;
            ReBuildTree( walk -> left ) ;
            while ( walk -> right -> token != "nil" ) {
              walk = walk -> right ;
              Check_Symbol( walk -> left ) ;
              ReBuildTree( walk -> left ) ;
            } // while
            
            if ( walk -> left -> token == "#t" )
              head = temp ;
            else if ( walk -> left -> token == "#f" || walk -> left -> token == "nil" ) {
              temp = new Node ;
              temp -> token = "nil" ;
              temp -> empty = false ;
              temp -> left = NULL ;
              temp -> right = NULL ;
              head = temp ;
            } // else if
          } // else
          
          return true ;
        } // if
        
        return false ;
      } // else if
      
      else if ( head -> left -> token == "or" ) {
        Node *temp = new Node ;
        Check_Symbol( head -> right -> left ) ;
        if ( ReBuildTree( head -> right -> left ) ) {
          if ( head -> right -> left -> token == "nil" || head -> right -> left -> token == "#f" ) {
            // nil 回傳 最後一個 
            Node *walk = head -> right -> right ;
            Check_Symbol( walk -> left ) ;
            ReBuildTree( walk -> left ) ;
            while ( walk -> right -> token != "nil" ) {
              walk = walk -> right ;
              Check_Symbol( walk -> left ) ;
              ReBuildTree( walk -> left ) ;
            } // while
            
            head = walk -> left ;
          } // if
          
          else if ( head -> right -> left -> token == "#t" ) {
            // 回傳第一個 
            Node *walk = head -> right -> right ;
            Check_Symbol( walk -> left ) ;
            ReBuildTree( walk -> left ) ;
            head = walk -> left ;
          } // else if
          
          else { // 第一個得到的是 token， 一直往下找 找到 找到最尾 看是 #t or nil
            temp = head -> right -> left ; 
            Node *walk = head -> right -> right ;
            Node *temp2 ; 
            Check_Symbol( walk -> left ) ;
            ReBuildTree( walk -> left ) ;
            while ( walk -> right -> token != "nil" && walk -> right -> token != "#f" &&
                    walk -> right -> token != "#t" ) {
              temp2 = walk -> left ;
              walk = walk -> right ;
              Check_Symbol( walk -> left ) ;
              ReBuildTree( walk -> left ) ;
            } // while
            
            if ( walk -> left -> token == "#t" )
              head = temp ;
            else // walk -> left -> token == "nil"
              head = temp2 ;
          } // else
          
          return true ;
        } // if
        
        return false ;
      } // else if
      
      else {
        cout << "ERROR (attempt to apply non-function) : " << head -> left -> token << endl ;
        return false ;
      } // else 
    } // if
    
    return false ; // 沒進if 就回傳 false 
  } // if
  
  return true ;
} // ReBuildTree()

void Translate( int &j, int &n ) {
  if ( gtokenlist[j] == "(" ) { // start S-exp
    gNewList.insert( gNewList.begin() + n, "(" ) ;
    j++ ;
    n++ ;
    int temp = 0 ;
    while ( gtokenlist[j] != ")" ) {
      Translate( j, n ) ;
      if ( gtokenlist[j] == "." ) {
        gNewList.insert( gNewList.begin() + n, "." ) ;
        j++ ;
        n++ ;
        Translate( j, n ) ;
      } // if
      
      else  { // gtokenlist[j] != "."
        if ( gtokenlist[j] == ")" ) {
          gNewList.insert( gNewList.begin() + n, "nil" ) ;
          gNewList.insert( gNewList.begin() + n, "." ) ;
          n = n + 2 ;
        } // if
        else {
       
          gNewList.insert( gNewList.begin() + n, ")" ) ;
          gNewList.insert( gNewList.begin() + n, "(" ) ;
          gNewList.insert( gNewList.begin() + n, "." ) ;
          n = n + 2 ; // 留下一個  )  在後面，等等要處理他 
          temp ++ ;
        } // else
      } // else
    } // while
    
    for ( int a = 0 ; a < temp ; a++ )
      n++ ;
    gNewList.insert( gNewList.begin() + n, ")" ) ;
    j++ ;
    n = n + 1 ;
  } // if
  
  else { // ATOM 
    gNewList.insert( gNewList.begin() + n, gtokenlist[j] ) ;
    j++ ;
    n++ ; 
  } // else
  
} // Translate()

void Execute() {
  Node *head = NULL ;
  int index = 0 ;
  BuildTree( head, index ) ;
   
  Node *temp = head ;
  int count = 2 ;
  
  
  FindQuote( temp, true ) ;
  
  if ( head -> left != NULL && head -> left -> token == "define" ) {
    bool err = false ;
    Node *temp_node = new Node ;
    if ( head -> right -> token == "nil" || head -> right -> right -> token == "nil" ||
         head -> right -> right -> right -> token != "nil" )
      err = true ;
    if ( err == false && IsSymbol( head -> right -> left -> token ) ) { // 看是不是symbol 不是的話 輸出error 
      if ( Check_Symbol( head -> right -> right -> left ) ) {
        temp_node = head -> right -> right -> left ;
        if ( Check_AllSymbol( temp_node ) ) {
          PutInTable( head -> right -> left -> token, temp_node ) ; 
          cout << head -> right -> left -> token << " defined" << endl ;
        } // if
      } // if
    } // if
    
    else { // error
      cout << "ERROR (DEFINE format) : " ;
      bool first_token = true ;
      cout << "( " ;
      PrintDefineError( temp, count, false, first_token ) ;
      cout << ")" << endl ;
    } // else
  } // if
  
  else if ( head -> left != NULL && head -> left -> token == "exit" ) {
    cout << "ERROR (incorrect number of arguments) : exit" << endl ;
  } // else if
  
  else if ( Check_Symbol( temp ) && ReBuildTree( temp ) ) {
    bool first_token = true ;
    if ( temp -> empty == true ) {
      cout << "( " ;
      PrintNode( temp, count, false, first_token ) ;
      cout << ")" << endl ;
    } // if
    
    else {
      PrintNode( temp, count, false, first_token ) ;
    } // else
  } // else if
  
} // Execute()


bool S_Expression( char &readChar, int &index ) {
  if ( gtokenlist[index] == "(" ) {
    if ( Scanner( readChar ) == false ) {
      gstart = true ;
      return false ;
    } // if
    
    if ( ReadSExp( readChar ) == false ) {
      gstart = true ;
      return false ;
    } // if
    
    index++ ;
    if ( gtokenlist[index] == ")" ) {
      return true ;
    } // if
    // ******************************* 
    // ( <exp>
    while ( 1 ) {
      if ( S_Expression( readChar, index ) == false )
        return false ;
      if ( Scanner( readChar ) == false ) {
        gstart = true ;
        return false ;
      } // if
      
      if ( ReadSExp( readChar ) == false ) {
        gstart = true ;
        return false ;
      } // if
      
      index++ ;
      // ( <exp>  -> next maybe <exp> dot or )
      if ( gtokenlist[index] == "." ) {
        if ( Scanner( readChar ) == false ) {
          gstart = true ;
          return false ;
        } // if
        
        if ( ReadSExp( readChar ) == false ) {
          gstart = true ;
          return false ; 
        } // if
        
        index++ ;
        if ( S_Expression( readChar, index ) == false ) // 處理中應該是 <exp> ********** => ( 1 . 2 
          return false ;
        if ( Scanner( readChar ) == false ) {
          gstart = true ;
          return false ;
        } // if
        
        if ( ReadSExp( readChar ) == false ) {
          gstart = true ;
          return false ;
        } // if
        
        index++ ;
        if ( gtokenlist[index] != ")" ) { // 預期應該是 右括號  )  
          cout << "ERROR (unexpected token) : ')' expected when token at Line " << 
                  gLine << " Column " << gColumn -  ( gtokenlist[index].size() - 1 ) <<
                  " is >>" << gtokenlist[index] << "<<" << endl << endl ;
          // skip the line
          while ( readChar != '\n' ) {
            if ( !Scanner( readChar ) ) 
              return false;
          } // while
          
          return false ;
        } // if
        
        else 
          return true ;
      } // if
      
      else if ( gtokenlist[index] == ")" )
        return true ;
    } // while
  } // if
  
  else if ( gtokenlist[index] == "'" ) {
    if ( Scanner( readChar ) == false ) {
      gstart = true ;
      return false ;
    } // if
    
    if ( ReadSExp( readChar ) == false ) {
      gstart = true ;
      return false ;
    } // if
    
    index++ ;
    return S_Expression( readChar, index ) ;
  } // else if
  
  else if ( gtokenlist[index] == "." ) { // dot 開頭 , should error
    cout << "ERROR (unexpected token) : atom or '(' expected when token at Line " << 
          gLine << " Column " << gColumn << " is >>.<<" << endl << endl ;
    // skip the line
    while ( readChar != '\n' ) {
      if ( !Scanner( readChar ) )
        return false ;
    } // while
    
    return false ;
  } // else if
  
  else if ( gtokenlist[index] == ")" ) {
    cout << "ERROR (unexpected token) : atom or '(' expected when token at Line " << 
            gLine << " Column " << gColumn << " is >>)<<" << endl << endl ;
    while ( readChar != '\n' ) {
      if ( !Scanner( readChar ) )
        return false ;
    } // while
      
    return false ;
  } // else if
  
  else { // 一般token 
    return true ;
  } // else
  
} // S_Expression()

bool S_Head( char &readChar ) { 
  if ( ReadSExp( readChar ) == true ) {
    int index = 0 ;
    return S_Expression( readChar, index ) ;
  } // if
  
  else // 讀到的東西有問題
    return false ; 
} // S_Head() ;

void Check_quote( int &i ) {
  if ( gtokenlist[i] == "'" ) {
    if ( gtokenlist[i+1] == "(" ) {
      gtokenlist.insert( gtokenlist.begin() + i, "(" ) ; // ex: '(12) ==> ('(12)
      i++ ;
      int left_num = 1 ;
      int right_num = 0 ;
      int temp_index = i + 2 ;
      while ( left_num != right_num ) { // left == right 就stop
        if ( gtokenlist[temp_index] == "(" )
          left_num++ ;
        else if ( gtokenlist[temp_index] == ")" )  
          right_num++ ;
        temp_index++ ;
        
      } // while
      
      gtokenlist.insert( gtokenlist.begin() + temp_index, ")" ) ;
    } // if
      
    else if ( gtokenlist[i+1] == "'" ) {
      int t = i+1 ;
      Check_quote( t ) ;
      gtokenlist.insert( gtokenlist.begin() + i, "(" ) ; // ex: '(12) ==> ('(12)
      i++ ;
      t++ ;
      int left_num = 1 ;
      int right_num = 0 ;
      int temp_index = i + 2 ;
      while ( left_num != right_num ) { // left == right 就stop
        if ( gtokenlist[temp_index] == "(" )
          left_num++ ;
        else if ( gtokenlist[temp_index] == ")" )  
          right_num++ ;
        temp_index++ ;
        
      } // while
      
      gtokenlist.insert( gtokenlist.begin() + temp_index, ")" ) ;
      i = t ;
    } // else if
    
    else { // 一般Ttoken 
      gtokenlist.insert( gtokenlist.begin() + i, "(" ) ;
      i++ ;
      gtokenlist.insert( gtokenlist.begin() + i+2, ")" ) ;
    } // else
  } // if
} // Check_quote()

int main() {
  cin >> gTestNum ; 
  char readChar ;
  scanf( "%c", &readChar ) ; // uTestNum -> '\n' read out
  cout << "Welcome to OurScheme!" << endl << endl ;
  while ( !gEXIT && Scanner( readChar )  ) {
    cout << "> " ;
    gstart = false ;
    gError = false ;
    gtokenlist.clear() ;
    gNewList.clear() ;
    if ( S_Head( readChar ) ) {
      if ( gReadEOF == false ) { 
        // check quote**********
        for ( int i = 0 ; i < gtokenlist.size() ; i++ ) {
          Check_quote( i ) ;
        } // for
        
        // end check quote**********
        // 雙括弧轉成nil 
        for ( int i = 0 ; i < gtokenlist.size() ; i++ ) {
          if ( gtokenlist[i] == "#f" )
            gtokenlist[i] = "nil" ;
          if ( i+1 < gtokenlist.size() ) {
            if ( gtokenlist[i] == "(" && gtokenlist[i+1] == ")" ) {
              gtokenlist[i] = "nil" ;
              gtokenlist.erase( gtokenlist.begin()+i+1 ) ;
            } // if
          } // if
        } // for
                
        // ***start translate
        int j = 0 ;
        int n = 0 ;
        if ( gtokenlist.size() > 1 ) { // 表示不是ATOM 是個很長的exp 
          Translate( j, n ) ;
        } // if
        
        else 
          gNewList = gtokenlist ;
        // ***end translate
        if ( gNewList.size() == 5 && gNewList[0] == "(" && gNewList[1] == "exit" && 
             gNewList[2] == "." && gNewList[3] == "nil" && gNewList[4] == ")" )
          gEXIT = true ;
          
        else if ( gNewList.size() == 5 && gNewList[0] == "(" && gNewList[1] == "clean-environment" && 
                  gNewList[2] == "." && gNewList[3] == "nil" && gNewList[4] == ")" ) {
          gTable.clear() ;
          cout << "environment cleaned" << endl << endl ;
        } // else if
          
        else if ( gtokenlist.size() > 0 ) {
          Execute() ;
          cout << endl ;
        } // if
      } // if
    } // if
    
    
    gLine = 1 ;
    gColumn = 0 ;
    gFirstInput = true ;
  } // while

  if ( !gEXIT ) {
    if ( gstart == false )
      cout << "> " ;
    cout << "ERROR (no more input) : END-OF-FILE encountered"   ;
  } // if
  
  cout << endl << "Thanks for using OurScheme!"  ;
} // main()

bool Scanner( char &readChar ) { // 我呼叫他 他就要給我一個char，如果 return false ， 表示沒有下一個char => 發生EOF 就要報error 
  // 這裡要計算 line and column 
  if ( cin.peek() != EOF ) {
    scanf( "%c", &readChar ) ;
    gColumn++ ;
    if ( readChar == '\n' ) {
      gPrevious_line  = gLine ;
      gPrevious_column = gColumn ;
      gLine++ ;
      gColumn = 0 ;  
      if ( gFirstInput == true ) {
        gLine = 1 ;
        gColumn = 0 ;
        gFirstInput = false ;
      } // if
    } // if
    
    return true ;
  } // if
  
  else {
    gReadEOF = true ;
    return false ;
  } // else 
} // Scanner()
