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

struct Node{
  string token ;
  bool empty ;
  Node *left ;
  Node *right ;
}; // node

typedef Node *Nodepoint ;

vector<string> gtokenlist ;
vector<string> gNewList ;

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

void ReadSExp( char &readChar ) {
  while ( readChar != '\n' ) {
    while ( IsWhiteSpace( readChar ) ) {
    // if is white space readchar, until not white space
      scanf( "%c", &readChar ) ;
    } // while

    if ( readChar == ';' ) { // 處理 comment 
      gCh = cin.peek() ;
      while ( gCh != '\n' ) {
        scanf( "%c", &readChar ) ;
        gCh = cin.peek() ;
      } // while

    } // if

    else if ( readChar == '(' ) {
      string temp ;
      temp = temp + readChar ;
      gtokenlist.push_back( temp ) ;
    } // else if 

    else if ( readChar == ')' ) {
      string temp ;
      temp = temp + readChar ;
      gtokenlist.push_back( temp ) ;
    } // else if

    else if ( readChar == '\"' ) { // deal with string
      string temp ;
      gCh = cin.peek() ;
      while ( gCh != '\n' ) {
        temp = temp + readChar ;
        scanf( "%c", &readChar ) ;
        gCh = cin.peek() ;
      } // while

      temp = temp + readChar ;
      gtokenlist.push_back( temp ) ;
    } // else if

    else if ( readChar == '\'' ) {
      string temp ;
      temp = temp + readChar ;
      gtokenlist.push_back( temp ) ;
    } // else if

    else { // others
      string temp  ;
      temp = temp + readChar ;
      gCh = cin.peek() ;
      while ( !IsSpecialChar( gCh ) && !IsWhiteSpace( gCh ) ) {
        scanf( "%c", &readChar ) ;
        temp = temp + readChar ;
        gCh = cin.peek() ;
      } // while

      gtokenlist.push_back( temp ) ;
    } // else

    scanf( "%c", &readChar ) ; 
  } // while
 
} // ReadSExp()

void DealDot ( Nodepoint &temp, string token ) { // 處理dot 
  if ( temp != NULL ) {
    DealDot( temp -> left, token ) ;
    if ( temp -> left != NULL && temp -> right == NULL ) {
      temp -> right  = new Node ;
      if ( token == "(" || token == ")" ) { // "(" 表示需 create new empty node 
        temp -> right -> empty = true ;
      } // if
    
      else {
        temp -> right -> token = token ;
        temp -> right -> empty = false ;
      } // else
    
    temp -> right -> left = NULL ;
    temp -> right -> right = NULL ;
    } // if
    
    DealDot( temp -> right, token ) ;
  } // if
} // DealDot()

void BuildTree( Nodepoint &temp, string token, bool dot ) {
  if ( temp == NULL ) {
    temp = new Node ;
    if ( token == "(" || token == ")" ) { // "(" 表示需 create new empty node 
      temp -> empty = true ;
    } // if
    
    else {
      temp -> token = token ;
      temp -> empty = false ;
    } // else
    
    temp -> left = NULL ;
    temp -> right = NULL ;
  } // if
  
  else if ( dot ) {
    DealDot ( temp,  token );
  } // else if
  
  else { // 一般token 
    if ( temp -> left == NULL || temp -> left -> empty == true )
      BuildTree( temp -> left, token, dot ) ;
    else {
      if ( temp -> right == NULL )  { // 右子樹為空，create a empty node 
        temp -> right = new Node ;
        temp -> right -> empty = true ;
        temp -> right -> left = NULL ;
        temp -> right -> right = NULL ;
      } // if
      
      BuildTree( temp -> right , token, dot ) ;
    } // else
  } // else
    
} // BuildTree()


void PrintSExp( string token ) {
  if ( token == "#f"  ) {
    cout << "nil" ;
  } // else if

  else if ( token == "t" ) {
    cout << "#t" ;
  } // else if

  else if ( token == "+." ) {
    cout << "+." ;
  } // else if

  else if ( token == "-." ) {
    cout << "-." ;
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
    for ( int j = 1 ; j < strlen(token.c_str() ) ; j++ ) {
      if ( !isdigit( token[j] ) && token[j] != '.' )
        isDigit = false ;
      if ( token[j] == '.' )
        dotnum++ ; 
      token_temp = token_temp + token[j] ;
    } // for

    // 整數不能直接印要把+號拿掉才print，float 轉換後直接 print 
    if ( isDigit && dotnum < 2 ) {
      if ( dotnum == 1) { // float
        float float_temp = atof( token.c_str() ) ;
        printf ( "%.3f", float_temp ) ; 
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
      printf ( "%.3f", float_temp ) ; 
    } // if

    else
      cout << token ;
  } // else if

  else { // 這裡要處理正常小數的狀況 
    if ( IsDecimal( token ) ) {
      float float_temp = atof( token.c_str() ) ;
      printf ( "%.3f", float_temp ) ; 
    } // if

    else
      cout << token ;
  } // else

  cout  << endl ;
} // PrintSExp()

void PrintNode( Node *head, int &count ) {
  if (head != NULL ) {
    PrintNode( head -> left, count ) ;
    if ( head -> empty != true ) {
      PrintSExp( head -> token ) ;
    }
    PrintNode( head -> right, count ) ;
  } // 
} // PrintNode()

void Translate( int &j , int &n ) {
  if ( gtokenlist[j] == "(" ) { // start S-exp
    gNewList.insert( gNewList.begin() + n , "(" ) ;
    j++ ;
    n++ ;
    bool temp = false ;
    while ( gtokenlist[j] != ")" ) {
      Translate(j, n) ;
      if ( gtokenlist[j] == "." ) {
        gNewList.insert( gNewList.begin() + n , "." ) ;
        j++ ;
        n++ ;
        Translate(j, n) ;
        temp = true ;
      } // if
      
      else  { // gtokenlist[j] != "."
        if ( gtokenlist[j] == ")" ) {
          gNewList.insert( gNewList.begin() + n , "nil" ) ;
          gNewList.insert( gNewList.begin() + n , "." ) ;
          n = n + 2 ;
          temp = true ;
        } // if
        else {
       
          gNewList.insert( gNewList.begin() + n , ")" ) ;
          gNewList.insert( gNewList.begin() + n , "(" ) ;
          gNewList.insert( gNewList.begin() + n , "." ) ;
          n = n + 2 ; // 留下一個  )  在後面，等等要處理他 
        } // else
      } // else
    } // while
    
    gNewList.insert( gNewList.begin() + n , ")" ) ;
    j++ ;
    n = n + 2 ;
    if ( temp )
      n = n - 1 ;
  } // if
  
  else { // ATOM 
    gNewList.insert( gNewList.begin() + n , gtokenlist[j] ) ;
    j++ ;
    n++ ; 
  } // else
  
} // Translate()

void Execute() {
  Node *head = NULL ;
  bool dot = false ; // 要找某個node 左邊有東西 但右邊沒有 
  
  /*
  for ( int index = 0 ; index < gtokenlist.size() ; index++ ) {
    if (  gtokenlist[index] == "." )
      dot = true ;
    else {
      BuildTree( head, gtokenlist[index], dot ) ;
      dot = false ;
    } // else
  } // for
  
  Node *temp = head ;
  int count = 0 ;
  PrintNode( temp, count ) ;
  */
} // Execute()

int main() {
  cin >> gTestNum ; 
  char readChar ;
  scanf( "%c", &readChar ) ; // uTestNum -> '\n' read out
  cout << "Welcome to OurScheme!" << endl << endl ;
  cout << "> " ;
  while ( !gEXIT && scanf( "%c", &readChar ) != EOF ) {
    gtokenlist.clear() ;
    gNewList.clear() ;
    ReadSExp( readChar ) ;
    if ( gtokenlist.size() && gtokenlist[0] == "(" && gtokenlist[1] == "exit" && gtokenlist[2] == ")" )
      gEXIT = true ;
    else {
       // 雙括弧轉成nil 
      for ( int i = 0 ; i < gtokenlist.size() ; i++ ) {
        if ( i+1 < gtokenlist.size() ) {
          if( gtokenlist[i] == "(" && gtokenlist[i+1] == ")" ) {
            gtokenlist[i] = "nil" ;
            gtokenlist.erase(gtokenlist.begin()+i+1) ;
          } // if
        } // if
      } // for
      
      int j = 0 ;
      int n = 0 ;
      if ( gtokenlist.size() > 1 ) { // 表示不是ATOM 是個很長的exp 
        Translate(j, n ) ;
      } // if
      
      else 
        gNewList = gtokenlist ;
      
      Execute() ;
      cout << "> " ;
    } // else
  } // while
  
  if ( !gEXIT )
    cout << "ERROR (no more input) : END-OF-FILE encountered"  ;
  cout << endl << "Thanks for using OurScheme!"  ;
} // main()
