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
// �ΨӰO�� ����r���� num 
int gPrevious_line ;
int gPrevious_column ;
// ********************
struct Node{
  string token ;
  bool empty ;
  Node *left ;
  Node *right ;
}; // node

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

bool ReadSExp( char &readChar ) {
  while ( IsWhiteSpace( readChar ) || readChar == ';' ) { // �B�z���ѩάO white space  
    if ( readChar == ';' ) { // Ū�� ����r�� '\n' 
      while ( readChar != '\n' ) {
        // scanner �|���ڧ��� readChar �̪��F�� 
        if ( Scanner( readChar ) == false ) { // �o�� EOF error 
          return false ;
        } // if
      } // while
    } // if
    
    else { // readChar == whiteSpace
      // read �@�� char �N��^�W���� while �h�P�_ 
      if ( Scanner( readChar ) == false ) { // �o�� EOF error 
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
    if ( Scanner( readChar ) ) { // readChar read �� ���޸� ���U�Ӧr�� 
      while ( readChar != '\"' ) { // �@�� read �� �k���޸����� 
        if ( readChar == '\n' ) { 
          // �o�{�b�k���޸��e �N ������r�� '\n'  => error 
          // �_�h ����string  
          cout <<  "ERROR (no closing quote) : END-OF-LINE encountered at Line " << 
                gPrevious_line << " Column " << gPrevious_column << endl << endl ; 
          return false ;
        } // if
        
        else {
          temp = temp + readChar ;
          char previous_char = readChar ;
          if ( !Scanner( readChar ) ) // Ū�U�@�Ӷi�� 
            return false ; // error 
          if ( previous_char == '\\' && readChar == '\"' ) { // ���� \" �B�z  �hŪ�@�� 
            temp = temp + readChar ;
            if ( !Scanner( readChar ) ) 
              return false ; // error
          } // if
        } // else
      } // while
      // �Xwhile ��� string Ū�� readChar == '"' 
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
      while ( canRead && ( !IsSpecialChar( gCh ) && !IsWhiteSpace( gCh ) ) ) { // �p�G�U�@�ӬO���`char �NŪ�i�� ��string 
        Scanner( readChar ) ;
        temp = temp + readChar ;
        if ( cin.peek() == EOF ) 
          canRead = false ;
        gCh = cin.peek() ;
      } // while
      // �Xwhile ��� �U�@�ӬO�S��char �άO�᭱ EOF 
    } // if
    
    gtokenlist.push_back( temp ) ;
    return true ;
  } // else
 
} // ReadSExp()


void BuildTree( Nodepoint &temp, int &index ) {
  if ( gNewList[index] == "(" ) {
    temp = new Node ;
    temp -> empty = true ;
    temp -> left = NULL ;
    temp -> right = NULL ;
    index ++ ;
    BuildTree( temp -> left, index ) ; 
    index++ ; // �o�����ӷ|�O dot 
    BuildTree( temp -> right, index ) ;
    index++ ; // �o�̬O �k�A�� ) 
  } // if
  
  else if ( gNewList[index] == "." ) {
    
  } // else if
  
  else { // �@��token 
    temp = new Node ;
    temp -> empty = false ;
    temp -> token = gNewList[index] ;
    index++ ;
    temp -> left = NULL ;
    temp -> right = NULL ;
  } // else
 
} // BuildTree()


void PrintSExp( string token ) {
  if ( token == "#f"  ) {
    cout << "nil" ;
  } // if
  
  else if ( token == "'" ) {
    cout << "quote" ;  
  } // else if
  
  else if ( token == "t" ) {
    cout << "#t" ;
  } // else if
  
  else if ( token == "+" ) {
    cout << "+" ;
  } // else if
  
  else if ( token == "-" ) {
    cout << "-" ;
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

        else // �p�G�����O�Aprint '\' 
          cout << token[j] ;
      } // if

      else {
        cout << token[j] ;
        if ( token[j] == '\"' && j != 0 )
          j = strlen( token.c_str() ) ;
      } // else
    } // for
  } // else if

  else if ( token[0] == '+' ) { // +0.125  +.135456   +13.12.13  +asdff232    �|���Ҽ{ '+.' 
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

    // ��Ƥ��ઽ���L�n��+�������~print�Afloat �ഫ�᪽�� print 
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

    // �t����ƩΨ�L�F�F�i�H������X�A�p�G�O�p�ƭn�ন�p�� 
    if ( isDigit && dotnum == 1 ) {
      float float_temp = atof( token.c_str() ) ;
      printf( "%.3f", float_temp ) ; 
    } // if

    else
      cout << token ;
  } // else if

  else { // �o�̭n�B�z���`�p�ƪ����p 
    if ( IsDecimal( token ) ) {
      float float_temp = atof( token.c_str() ) ;
      printf( "%.3f", float_temp ) ; 
    } // if

    else
      cout << token ;
  } // else

  cout  << endl ;
} // PrintSExp()

void PrintNode( Node *head, int count, bool dot_right, bool &first_token ) {
  // first_token �ΨӧP�_���L�O���O�L�Ĥ@��token
  // �O���ܴN���ΦL white space 
  if ( head != NULL ) {
    
    if ( head -> left != NULL && head -> left -> empty == true ) { // ���t�@���s 
      if ( !first_token ) {
        for ( int i = 0 ; i < count ; i++ )
          cout << " " ;
      } // if
      
      first_token = true ;
      cout << "( " ;
      count = count + 2 ;
    } // if
    
    PrintNode( head -> left, count, false, first_token ) ;
    
    if ( head -> left != NULL && head -> left -> empty == true ) { // ���t�@���s 
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
      
      PrintSExp( head -> token ) ;
      first_token = false ;
    } // if
    // ***********
    
    PrintNode( head -> right, count, true, first_token ) ;
  } // if
} // PrintNode()

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
          n = n + 2 ; // �d�U�@��  )  �b�᭱�A�����n�B�z�L 
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
  /*
  for ( int i = 0 ; i < gNewList.size() ; i++ ) {
    cout << gNewList[i] ;
  } // for
  cout << endl ;
  */
  bool first_token = true ;
  if ( gNewList.size() > 1 ) {
    cout << "( " ;
    PrintNode( temp, count, false, first_token ) ;
    cout << ")" << endl ;
  } // if
  
  else 
    PrintNode( temp, count, false, first_token ) ;
  
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
        if ( S_Expression( readChar, index ) == false ) // �B�z�����ӬO <exp> ********** => ( 1 . 2 
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
        if ( gtokenlist[index] != ")" ) { // �w�����ӬO �k�A��  )  
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
  
  else if ( gtokenlist[index] == "." ) { // dot �}�Y , should error
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
  
  else { // �@��token 
    return true ;
  } // else
  
} // S_Expression()

bool S_Head( char &readChar ) { 
  if ( ReadSExp( readChar ) == true ) {
    int index = 0 ;
    return S_Expression( readChar, index ) ;
  } // if
  
  else // Ū�쪺�F�観���D
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
      while ( left_num != right_num ) { // left == right �Nstop
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
      while ( left_num != right_num ) { // left == right �Nstop
        if ( gtokenlist[temp_index] == "(" )
          left_num++ ;
        else if ( gtokenlist[temp_index] == ")" )  
          right_num++ ;
        temp_index++ ;
        
      } // while
      
      gtokenlist.insert( gtokenlist.begin() + temp_index, ")" ) ;
      i = t ;
    } // else if
    
    else { // �@��Ttoken 
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
        // ���A���নnil 
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
        if ( gtokenlist.size() > 1 ) { // ��ܤ��OATOM �O�ӫܪ���exp 
          Translate( j, n ) ;
        } // if
        
        else 
          gNewList = gtokenlist ;
        // ***end translate
        if ( gNewList.size() == 5 && gNewList[0] == "(" && gNewList[1] == "exit" && 
             gNewList[2] == "." && gNewList[3] == "nil" && gNewList[4] == ")" )
          gEXIT = true ;
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

bool Scanner( char &readChar ) { // �کI�s�L �L�N�n���ڤ@��char�A�p�G return false �A ��ܨS���U�@��char => �o��EOF �N�n��error 
  // �o�̭n�p�� line and column 
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
