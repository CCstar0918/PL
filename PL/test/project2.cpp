// 10727109 林亞岑
// PL project 2
// OurScheme
# include <iostream> // cin, cout
# include <string>   // string, find_last_of, substr
# include <vector>   // vector
# include <sstream>
# include <iomanip>  // setprecision

using namespace std ;

enum TokenType {  // token type
  LEFT_PAREN,   // '('
  RIGHT_PAREN,  // ')'
  INT,          // e.g., '123', '+123', '-123'
  STRING,       // "string's (example)." (strings do not extend across lines)
  DOT,          // '.' (stand alone)
  FLOAT,        // '123.567', '123.', '.567', '+123.4', '-.123'
  NIL,          // 'nil' or '#f', but not 'NIL' nor 'nIL'
  T,            // 't' or '#t', but not 'T' nor '#T'
  QUOTE,        // '
  SYMBOL,       // not above
  NONE          // empty
}; // TokenType

struct Token {
  string name ;     // token名稱
  TokenType type ;  // token類別
}; // Token

struct TreeNode {
  bool isLeaf ;
  Token value ;
  TreeNode * left ;
  TreeNode * right ;
}; // TreeNode

struct Binder {
  string name ;
  TreeNode * binding ;
  // int numOfArg ; // 是function需要紀錄參數數量
}; // Binder

static int uTestNum = 0 ;

//   --exception-- {
// read error
class LPLost {} ; // 少了'('
class RPLost {} ; // 少了')'
class QuoteLost {} ; // 少了'\''
class EndFile {} ; // 沒有正常結尾，已無輸入
// eval error
class UnDefine {} ; // 未定義name
class BindingWrong {} ;  // binding錯誤(找不到對應function/symbol name)
class NonList {} ; // 最右子節點有東西 or 只是葉節點
class GramWrong {} ; // 文法錯誤
class RuningError {} ; // 執行時出的錯誤
// } --exception--

class CharCheck {
public:
  CharCheck() {
  } // CharCheck()

  ~CharCheck() {
  } // ~CharCheck()

  static bool IsDigit( char ch ) {
    if ( '0' <= ch && ch <= '9' )
      return true ;
    return false ;
  } // IsDigit()

  static bool IsSeparator( char ch ) {
    if ( ch == '(' || ch == ')' || ch == '\'' || ch == '\"' || ch == ';' )
      return true ;
    return false ;
  } // IsSeparator()

  static bool IsWhiteSpace( char ch ) {
    if ( ch == ' ' || ch == '\t' || ch == '\n' )
      return true ;
    return false ;
  } // IsWhiteSpace()

  static string IntToStr( int num ) {
    string str ;
    stringstream ss ;
    ss << num ;
    ss >> str ;
    return str ;
  } // IntToStr()

  static int StrToInt( string str ) {
    int integer ;
    stringstream ss ;
    ss << str ;
    ss >> integer ;
    return integer ;
  } // StrToInt()

  static string FloToStr( float num ) {
    string str ;
    stringstream ss ;
    ss << num ;
    ss >> str ;
    return str ;
  } // FloToStr()

  static float StrToFlo( string str ) {
    float floatNum ;
    stringstream ss ;
    ss << str ;
    ss >> floatNum ;
    return floatNum ;
  } // StrToFlo()

  static string RemoveQuote( string str ) {
    str = str.substr( 1, str.size()-2 ) ;
    return str ;
  } // RemoveQuote()
}; // class CharCheck

class TreeEditor {
private:

  static string DealString( string str ) {
    string temp = "" ;

    // 對'\'的處理
    for ( int i = 0 ; i < str.length() ; i++ ) {
      if ( str.at( i ) == '\\' ) {
        if ( str.at( i+1 ) == '\\' )
          temp = temp + '\\' ;
        else if ( str.at( i+1 ) == '\"' )
          temp = temp + '\"' ;
        else if ( str.at( i+1 ) == '\'' )
          temp = temp + '\'' ;
        else if ( str.at( i+1 ) == 't' )
          temp = temp + '\t' ;
        else if ( str.at( i+1 ) == 'n' )
          temp = temp + '\n' ;
        else {
          temp = temp + str.at( i ) ;
          temp = temp + str.at( i+1 ) ;
        } // else

        i++ ; // 已經處理掉了
      } // if
      else {
        temp = temp + str.at( i ) ;
      } // else

    } // for

    // \' 轉成quote
    if ( str == "\'" )
      temp = "quote" ;

    return temp ;
  } // DealString()

  static void PrintNode( TreeNode * cur ) {
    if ( cur->value.type == INT ) {
      cout << CharCheck::StrToInt( cur->value.name ) << "\n" ;
    } // if
    else if ( cur->value.type == FLOAT ) {
      cout << fixed << setprecision( 3 ) << CharCheck::StrToFlo( cur->value.name ) << "\n" ;
    } // else if
    else if ( cur->value.type == NIL ) {
      cout << "nil" << "\n" ;
    } // else if
    else if ( cur->value.type == T ) {
      cout << "#t" << "\n" ;
    } // else if
    else {
      cout << DealString( cur->value.name ) << "\n" ;
    } // else
  } // PrintNode()

  static void PrintSpace( bool need, int level ) {
    if ( need ) {
      for ( int space = 0 ; space < level * 2 ; space++ ) {
        cout << " " ;
      } // for
    } // if
  } // PrintSpace()

  static void Print( bool needSpace, int level, TreeNode * root ) {
    if ( root == NULL ) ;
    else if ( root->isLeaf ) { // 葉節點 直接印此節點
      PrintSpace( needSpace, level ) ;
      PrintNode( root ) ;
    } // else if
    else { // 中間節點

      // check left node
      if ( root->left == NULL )
        ;
      else if ( root->left->isLeaf ) { // 左節點是葉節點
        Print( needSpace, level, root->left ) ;
      } // else if
      else { // 左節點是中間節點
        PrintSpace( needSpace, level ) ;
        cout << "( " ;
        Print( false, level+1, root->left ) ; // '('後第一個TOKEN不需要空白
        PrintSpace( true, level ) ;
        cout << ")\n" ;
      } // else 左節點是中間節點

      // check right node
      if ( root->right == NULL )
        ;
      else if ( root->left != NULL && root->left->value.type == QUOTE ) { // 左邊是quote
        // 左邊是quote時, 把右邊括號起來印出
        if ( root->right->isLeaf ) { // 葉節點
          // 直接印
          Print( true, level, root->right ) ; // '('後第一個TOKEN不需要空白
        } // if 葉節點
        else { // 中間節點
          PrintSpace( true, level ) ;
          cout << "( " ;
          Print( false, level+1, root->right ) ; // '('後第一個TOKEN不需要空白
          PrintSpace( true, level ) ;
          cout << ")\n" ;
        } // else 中間節點
      } // else if 左邊是quote
      else if ( root->right->isLeaf ) { // 右節點是葉節點
        if ( root->right->value.type != NIL ) {
          PrintSpace( true, level ) ;
          cout << ".\n" ;
          Print( true, level, root->right ) ;
        } // if
      } // else if 右節點是葉節點
      else { // 右節點是中間節點
        Print( true, level, root->right ) ;
      } // else

    } // else 此點為中間節點
  } // Print()

public:

  TreeEditor() {
  } // TreeEditor()

  ~TreeEditor() {
  } // ~TreeEditor()

  static TreeNode * NewNode() { // 中間節點
    TreeNode * node = new TreeNode() ;
    node->isLeaf = false ;
    node->value.name = "" ;
    node->value.type = NIL ;
    node->left = NULL ;
    node->right = NULL ;
    return node ;
  } // NewNode()

  static TreeNode * NewNode( Token token ) { // 葉節點
    TreeNode * node = new TreeNode() ;
    node->isLeaf = true ;
    node->value.name = token.name ;
    node->value.type = token.type ;
    node->left = NULL ;
    node->right = NULL ;
    return node ;
  } // NewNode()

  static void DeleteTree( TreeNode * root ) {
    if ( root != NULL ) {
      DeleteTree( root->right ) ;
      DeleteTree( root->left ) ;
      delete root ;
      root = NULL ;
    } // if
  } // DeleteTree()

  static TreeNode * Build( vector<Token> tokens, int & index, TreeNode * root ) {
    TreeNode * cur = NULL ;
    if ( tokens.size() <= index )
      ; // stop
    else if ( tokens.at( index ).type == RIGHT_PAREN
              || tokens.at( index ).type == DOT ) // ")" or "."
      cout << "error : loss LEFT_PAREN \n" ;
    else if ( tokens.at( index ).type == QUOTE ) { // "\'"
      root = NewNode() ;
      root->left = NewNode( tokens.at( index ) ) ; // QUOTE
      index++ ;
      root->right = Build( tokens, index, root->right ) ;
    } // else if
    else if ( tokens.at( index ).type == LEFT_PAREN ) { // "("
      root = NewNode() ;
      cur = root ;

      index++ ;
      cur->left = Build( tokens, index, cur->left ) ; // first

      while ( tokens.at( index ).type != DOT
              && tokens.at( index ).type != RIGHT_PAREN ) { // 非"." or ")"
        cur->right = NewNode() ;
        cur = cur->right ;
        cur->left = Build( tokens, index, cur->left ) ;
      } // while

      if ( tokens.at( index ).type == DOT ) {
        index++ ;
        cur->right = Build( tokens, index, cur->right ) ;
      } // if
      else {
        cur->right = NewNode() ;
        cur->right->isLeaf = true ;
      } // else


      if ( tokens.at( index ).type == RIGHT_PAREN ) {
        index++ ;
      } // if
      else {
        cout << "error : loss RIGHT_PAREN \n" ;
      } // else

    } // else if
    else { // 只有內容
      root = NewNode( tokens.at( index ) ) ;
      index++ ;
    } // else

    return root ;
  } // Build()

  static void PrintSExp( TreeNode * expr ) {
    if ( expr == NULL )
      ;
    else if ( expr->isLeaf )
      Print( false, 0, expr ) ;
    else {
      cout << "( " ;
      Print( false, 1, expr ) ;
      cout << ")\n" ;
    } // else
  } // PrintSExp()

}; // class TreeEditor

class Evaluater {
private:
  vector<Binder> mSymList ;  // 當stack用，新增在第一個，搜尋從第一個
  vector<Binder> mFuncList ; // 當stack用，新增在第一個，搜尋從第一個
  vector<Binder> mPrim ; // 存primitive binding, 但不會預先放好, 只是為了不重覆新增堆積在memory

  bool mUseDefine ;
  bool mWillExit ; 
  //  --Primitive-- {
  // 1
  TreeNode * CONS( TreeNode * root ) {
    if ( !ArguNumCheck( false, root, "=", 2 ) ) { 
      cout << "ERROR (incorrect number of arguments) : cons\n" ;
      throw GramWrong() ;
    } // if 

    TreeNode * curOld = root->right ;
    TreeNode * curNew = NULL ;
    TreeNode * rootNew = NULL ;

    rootNew = TreeEditor::NewNode() ;
    rootNew->left = Eval( false, curOld->left ) ;

    curOld = curOld->right ;
    rootNew->right = Eval( false, curOld->left ) ; 

    return rootNew ;
  } // CONS()

  TreeNode * LIST( TreeNode * root ) {
    if ( !ArguNumCheck( false, root, ">=", 0 ) ) {
      cout << "ERROR (incorrect number of arguments) : list\n" ;
      throw GramWrong() ;
    } // if   

    TreeNode * curOld = root ;
    TreeNode * curNew = TreeEditor::NewNode() ;
    TreeNode * rootNew = curNew ;

    curOld = curOld->right ;
    while ( !curOld->isLeaf ) {
      curNew->left = Eval( false, curOld->left ) ;
      curNew->right = TreeEditor::NewNode() ;
      curNew = curNew->right ;
      curOld = curOld->right ;
    } // while

    curNew->isLeaf = true ;
    return rootNew ;
  } // LIST()

  // 2
  TreeNode * SymQUOTE( TreeNode * root ) {
    if ( !ArguNumCheck( false, root, "=", 1 ) ) {
      cout << "ERROR (incorrect number of arguments) : quote\n" ;
      throw GramWrong() ;
    } // if  

    return root->right->left ;
  } // SymQUOTE()

  // 3
  TreeNode * DEFINE( bool isTopLevel, TreeNode * root ) {
    if ( !isTopLevel ) {
      cout << "ERROR (level of DEFINE)\n" ;
      throw GramWrong() ;
    } // if
    else if ( !ArguNumCheck( false, root, "=", 2 ) ) { 
      cout << "ERROR (DEFINE format) : " ; // argument number wrong
      TreeEditor::PrintSExp( root ) ;
      throw GramWrong() ;
    } // else if 

    TreeNode * curOld = root ;
    Binder newBinder ;
    // mSmlist 用來存放定義的sumbol 
    int iSym = 0 ; // index of mSymList
    bool correctFormat = false ;

    curOld = curOld->right ;
    // curOld = A
    if ( curOld->left->value.type == SYMBOL ) { // 有要被定義的symbl
      newBinder.name = curOld->left->value.name ;
      iSym = Find( mSymList, newBinder.name ) ; // 檢查是否為已定義value
      // 檢查A 是否已經定義過 
      curOld = curOld->right ;
      // curOld 現在在 B 的位置 
      // B如果定義過會在Eval 回傳 
      newBinder.binding = Eval( false, curOld->left ) ;

      if ( iSym != -1 ) { // 已被定義過->重定義
        correctFormat = true ;
        mSymList[iSym].binding = newBinder.binding ;
      } // if 覆蓋舊定義
      else if ( !IsPrimitive( newBinder.name ) ) { // 不是使用primitive名稱
        correctFormat = true ;
        mSymList.insert( mSymList.begin(), newBinder ) ; // 新增在最前面
      } // else if 新增定義
    } // if 有symbol name

    if ( correctFormat ) {
      mUseDefine = true ;
      cout << newBinder.name << " defined\n" ;
      return NULL ;
    } // if
    else {
      cout << "ERROR (DEFINE format) : " ;
      TreeEditor::PrintSExp( root ) ;
      throw GramWrong() ;
    } // else

    return NULL ;
  } // DEFINE()

  // 4
  TreeNode * CAR( TreeNode * root ) { // left node
    if ( !ArguNumCheck( false, root, "=", 1 ) ) {
      cout << "ERROR (incorrect number of arguments) : car\n" ;
      throw GramWrong() ;
    } // if 

    TreeNode * curOld = root->right ;
    TreeNode * binding = NULL ;

    binding = Eval( false, curOld->left ) ;
    if ( !binding->isLeaf ) {
      return binding->left ;
    } // if 
    else { // just a leaf
      cout << "ERROR (car with incorrect argument type) : " ;
      TreeEditor::PrintSExp( binding ) ;
      throw GramWrong() ;
    } // else 
  } // CAR()

  TreeNode * CDR( TreeNode * root ) { // right node
    if ( !ArguNumCheck( false, root, "=", 1 ) ) {
      cout << "ERROR (incorrect number of arguments) : cdr\n" ;
      throw GramWrong() ;
    } // if 

    TreeNode * curOld = root->right ;
    TreeNode * binding = NULL ;

    binding = Eval( false, curOld->left ) ;
    if ( !binding->isLeaf ) {
      return binding->right ;
    } // if 
    else { // just a leaf
      cout << "ERROR (cdr with incorrect argument type) : " ;
      TreeEditor::PrintSExp( binding ) ;
      throw GramWrong() ;
    } // else 
  } // CDR()

  // 5
  TreeNode * TrueOrFalse( TreeNode * root, string testType ) {
    if ( !ArguNumCheck( false, root, "=", 1 ) ) {
      cout << "ERROR (incorrect number of arguments) : " ;
      cout << testType << "\n" ;
      throw GramWrong() ;
    } // if

    Token ans ;
    ans.name = "" ;
    TreeNode * cur = root->right ;
    
    cur = Eval( false, cur->left ) ;
    if ( testType == "atom?" ) {
      if ( cur->isLeaf ) 
        ans.type = T ;
      else 
        ans.type = NIL ;
    } // if 
    else if ( testType == "pair?" ) {
      if ( !cur->isLeaf ) 
        ans.type = T ;
      else 
        ans.type = NIL ;
    } // else if 
    else if ( testType == "list?" ) {
      int count = 0 ;
      while ( !cur->isLeaf ) {
        count++ ; // 紀錄有幾個左子節點(list有幾個內容)
        cur = cur->right ;
      } // while

      if ( count >= 0 && cur->value.type != NIL ) // 最右項不是NIL
        ans.type = NIL ;
      else if ( count == 0 ) // just a leaf
        ans.type = NIL ;
      else 
        ans.type = T ;
    } // else if 
    else if ( testType == "null?" ) {
      if ( cur->isLeaf && cur->value.type == NIL ) 
        ans.type = T ;
      else 
        ans.type = NIL ;
    } // else if 
    else if ( testType == "integer?" ) {
      if ( cur->isLeaf && cur->value.type == INT ) 
        ans.type = T ;
      else 
        ans.type = NIL ;
    } // else if 
    else if ( testType == "real?" ) {
      if ( !cur->isLeaf )
        ans.type = NIL ;
      else if ( cur->value.type == INT ) 
        ans.type = T ;
      else if ( cur->value.type == FLOAT ) 
        ans.type = T ;
      else 
        ans.type = NIL ;
    } // else if 
    else if ( testType == "number?" ) {
      if ( !cur->isLeaf )
        ans.type = NIL ;
      else if ( cur->value.type == INT ) 
        ans.type = T ;
      else if ( cur->value.type == FLOAT ) 
        ans.type = T ;
      else 
        ans.type = NIL ;
    } // else if 
    else if ( testType == "string?" ) {
      if ( cur->isLeaf && cur->value.type == STRING ) 
        ans.type = T ;
      else 
        ans.type = NIL ;
    } // else if 
    else if ( testType == "boolean?" ) {
      if ( !cur->isLeaf )
        ans.type = NIL ;
      else if ( cur->value.type == T ) 
        ans.type = T ;
      else if ( cur->value.type == NIL ) 
        ans.type = T ;
      else 
        ans.type = NIL ;
    } // else if 
    else if ( testType == "symbol?" ) {
      if ( !cur->isLeaf )
        ans.type = NIL ;
      else {
        int iPrim = Find( mPrim, cur->value.name ) ;
        if ( iPrim != -1 ) 
          ans.type = NIL ;
        else if ( cur->value.type == SYMBOL ) 
          ans.type = T ;
        else 
          ans.type = NIL ;
      } // else 
    } // else if 
    else
      ans.type = NIL ; 

    return TreeEditor::NewNode( ans ) ;
  } // TrueOrFalse()

  // 6
  TreeNode * Arithmetic( bool firstAri, TreeNode * root, string operType ) {
    if ( !firstAri ) ;
    else if ( operType == "not" ) {
      if ( !ArguNumCheck( false, root, "=", 1 ) ) {
        cout << "ERROR (incorrect number of arguments) : " ;
        cout << operType << "\n" ;
        throw GramWrong() ;
      } // if 
    } // else if 
    else if ( !ArguNumCheck( false, root, ">=", 2 ) ) {
      cout << "ERROR (incorrect number of arguments) : " ;
      cout << operType << "\n" ;
      throw GramWrong() ;
    } // else if

    TreeNode * parameter = NULL ;
    TreeNode * result = NULL ;
    TreeNode * evaled = NULL ;
    TreeNode * cur = NULL ;
    Token ans ;
    ans.name = "" ;
    ans.type = NIL ;

    if ( firstAri ) {
      parameter = root->right ;
    } // if
    else {
      parameter = root ;
    } // else 

    if ( parameter == NULL ) ;

    else if ( parameter->isLeaf && parameter->value.type == NIL ) ; // eval stop

    else if ( operType == "+" || operType == "*" || 
              operType == "-" || operType == "/" ||
              operType == "+-" || operType == "*/" ) { 
      float sumF = 0.0 ;
      int sumI = 0 ;

      string mainOT = operType ; // 為了"-"與"/"遞迴
      if ( operType == "+-" ) {
        mainOT = "-" ;
        operType = "+" ;
      } // if 
      else if ( operType == "*/" ) {
        mainOT = "/" ;
        operType = "*" ;
      } // else if 

      evaled = Eval( false, parameter->left ) ; // deal first number
      if ( evaled == NULL ) {
        cout << "ERROR(" << mainOT << " format) : " ;
        throw GramWrong() ;
      } // if 
      else if ( evaled->isLeaf && evaled->value.type == INT ) {
        ans.name = evaled->value.name ;
        ans.type = INT ;
      } // else if 
      else if ( evaled->isLeaf && evaled->value.type == FLOAT ) {
        ans.name = evaled->value.name ;
        ans.type = FLOAT ;
      } // else if 
      else {
        cout << "ERROR (" << mainOT << " with incorrect argument type) : " ;
        TreeEditor::PrintSExp( evaled ) ;
        throw GramWrong() ;
      } // else 

      // 遞迴往下處理後面的數字
      if ( operType == "+" || operType == "*" ) {
        evaled = Arithmetic( false, parameter->right, operType ) ; // second (遞迴往下)
      } // if 
      else if ( operType == "-" ) { 
        evaled = Arithmetic( false, parameter->right, "+-" ) ; // second (遞迴往下加)
      } // else if 
      else { // "/"
        evaled = Arithmetic( false, parameter->right, "*/" ) ; // second (遞迴往下乘)
      } // else 

      if ( firstAri && evaled == NULL ) { // 只有一個argument
        cout << "ERROR (incorrect number of arguments) : " ;
        cout << mainOT << "\n" ;
        throw GramWrong() ;
      } // if 
      else if ( evaled == NULL ) { // 遞迴到底了
        result = TreeEditor::NewNode( ans ) ;
      } // else if
      else { // deal first number and other
        if ( ans.type == FLOAT || evaled->value.type == FLOAT ) { // float
          sumF = CharCheck::StrToFlo( ans.name ) ;

          if ( operType == "+" ) {
            sumF = sumF + CharCheck::StrToFlo( evaled->value.name ) ;
          } // if 
          else if ( operType == "-" ) { 
            sumF = sumF - CharCheck::StrToFlo( evaled->value.name ) ;
          } // else if 
          else if ( operType == "*" ) { 
            sumF = sumF * CharCheck::StrToFlo( evaled->value.name ) ;
          } // else if 
          else { // "/"
            if ( CharCheck::StrToFlo( evaled->value.name ) == 0.0 ) {
              cout << "ERROR (division by zero) : /\n" ;
              throw RuningError() ;
            } // if 
            else {
              sumF = sumF / CharCheck::StrToFlo( evaled->value.name ) ;
            } // else
          } // else 

          ans.name = CharCheck::FloToStr( sumF ) ;
          ans.type = FLOAT ;
          result = TreeEditor::NewNode( ans ) ;
        } // if float
        else { 
          sumI = CharCheck::StrToInt( ans.name ) ;

          if ( operType == "+" ) {
            sumI = sumI + CharCheck::StrToInt( evaled->value.name ) ;
          } // if 
          else if ( operType == "-" ) { 
            sumI = sumI - CharCheck::StrToInt( evaled->value.name ) ;
          } // else if 
          else if ( operType == "*" ) { 
            sumI = sumI * CharCheck::StrToInt( evaled->value.name ) ;
          } // else if 
          else { // "/"
            if ( CharCheck::StrToInt( evaled->value.name ) == 0 ) {
              cout << "ERROR (division by zero) : /\n" ;
              throw GramWrong() ;
            } // if 
            else {
              sumI = sumI / CharCheck::StrToInt( evaled->value.name ) ;
            } // else
          } // else 

          ans.name = CharCheck::IntToStr( sumI ) ;
          ans.type = INT ;
          result = TreeEditor::NewNode( ans ) ;
        } // else int
      } // else deal first number and other
    } // else if Arithmetic +,-,*,/

    else if ( operType == "not" ) {
      evaled = Eval( false, parameter->left ) ;
      if ( evaled->isLeaf && evaled->value.type == NIL ) // False
        ans.type = T ;
      else // 非false值的
        ans.type = NIL ;

      result = TreeEditor::NewNode( ans ) ;
    } // else if "not"

    else if ( operType == "and" || operType == "or" ) {
      // argument(>=2)
      // and : 第一個argument為nil，回傳其值；否則回傳第二個argument
      // or : 第一個argument不為nil，回傳其值；否則回傳第二個argument
      bool haveResult = false ;
      cur = parameter ;

      while ( !haveResult && !cur->isLeaf ) {

        evaled = Eval( false, cur->left ) ;
        if ( evaled == NULL ) ;
        else if ( evaled->isLeaf && evaled->value.type == NIL ) { // evaled is nil
          if ( operType == "and" ) {
            if ( !haveResult ) {
              result = evaled ;
              haveResult = true ;
            } // if 
          } // if 
          else { // "or"
            // do nothing
          } // else 

        } // else if 
        else { // evaled is not nil
          if ( operType == "and" ) {
            // do nothing
          } // if 
          else { // "or"
            if ( !haveResult ) {
              result = evaled ;
              haveResult = true ;
            } // if 
          } // else 
        } // else 

        cur = cur->right ;
      } // while

      // last argument
      if ( !haveResult ) {
        result = evaled ;
        haveResult = true ;
      } // if 

    } // else if "and" "or"

    else if ( operType == ">" || operType == ">=" || 
              operType == "<" || operType == "<=" || operType == "=" ) {
      // argument(>=2)
      // 全部用float去比較
      float preNum = 0.0 ;
      evaled = Eval( false, parameter->left ) ; // deal first token
      if ( evaled == NULL ) {
        cout << "ERROR(" << operType << " format) : " ;
        throw GramWrong() ;
      } // if 
      else if ( ( evaled->isLeaf ) && 
                ( evaled->value.type == INT || evaled->value.type == FLOAT ) ) {
        preNum = CharCheck::StrToFlo( evaled->value.name ) ; // 前一個的數字
      } // else if 
      else {
        cout << "ERROR (" << operType << " with incorrect argument type) : " ;
        TreeEditor::PrintSExp( evaled ) ;
        throw GramWrong() ;
      } // else 
      
      cur = parameter->right ;
      
      ans.type = T ; // 先假設是對的
      while ( !cur->isLeaf ) {
        evaled = Eval( false, cur->left ) ;
        
        if ( evaled == NULL ) ;
        else if ( ( evaled->isLeaf ) &&
                  ( evaled->value.type == INT || evaled->value.type == FLOAT ) ) {
          if ( operType == ">" ) {
            if ( preNum > CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // 目前為止都是true
                ans.type = T ;
              else // 曾經不是true
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // if 
          else if ( operType == ">=" ) {
            if ( preNum >= CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // 目前為止都是true
                ans.type = T ;
              else // 曾經不是true
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
          else if ( operType == "<" ) {
            if ( preNum < CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // 目前為止都是true
                ans.type = T ;
              else // 曾經不是true
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
          else if ( operType == "<=" ) {
            if ( preNum <= CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // 目前為止都是true
                ans.type = T ;
              else // 曾經不是true
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
          else { // "="
            if ( preNum == CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // 目前為止都是true
                ans.type = T ;
              else // 曾經不是true
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
        } // else if is number
        else { // 不是number
          cout << "ERROR (" << operType << " with incorrect argument type) : " ;
          TreeEditor::PrintSExp( evaled ) ;
          throw GramWrong() ;
        } // else 

        preNum = CharCheck::StrToFlo( evaled->value.name ) ;
        cur = cur->right ;
      } // while

      result = TreeEditor::NewNode( ans ) ;
    } // else if number compare

    else if ( operType == "string-append" ) {
      // argument(>=2)
      evaled = Eval( false, parameter->left ) ; // deal first number
      if ( evaled == NULL ) {
        cout << "ERROR(" << operType << " format) : " ;
        throw GramWrong() ;
      } // if 
      else if ( evaled->isLeaf && evaled->value.type == STRING ) {
        ans.name = evaled->value.name ;
        ans.type = STRING ;
      } // else if 
      else {
        cout << "ERROR (" << operType << " with incorrect argument type) : " ;
        TreeEditor::PrintSExp( evaled ) ;
        throw GramWrong() ;
      } // else 
      
      cur = parameter->right ;
      
      while ( !cur->isLeaf ) {
        evaled = Eval( false, cur->left ) ;
        
        if ( evaled == NULL ) ;
        else if ( evaled->isLeaf && evaled->value.type == STRING ) {
          ans.name = CharCheck::RemoveQuote( ans.name ) ;
          ans.name = ans.name + CharCheck::RemoveQuote( evaled->value.name ) ;
          ans.name = "\"" + ans.name + "\"" ;
          result = TreeEditor::NewNode( ans ) ; 
        } // else if is string
        else { // 不是string
          cout << "ERROR (" << operType << " with incorrect argument type) : " ;
          TreeEditor::PrintSExp( evaled ) ;
          throw GramWrong() ;
        } // else 

        cur = cur->right ;
      } // while

      result = TreeEditor::NewNode( ans ) ;
    } // else if "string-append"

    else if ( operType == "string>?" || operType == "string<?" ||
              operType == "string=?" ) {
      // argument(>=2)
      string preStr = "" ;
      evaled = Eval( false, parameter->left ) ; // deal first token
      if ( evaled == NULL ) {
        cout << "ERROR(" << operType << " format) : " ;
        throw GramWrong() ;
      } // if 
      else if ( evaled->isLeaf && evaled->value.type == STRING ) {
        preStr = evaled->value.name ;
      } // else if 
      else {
        cout << "ERROR (" << operType << " with incorrect argument type) : " ;
        TreeEditor::PrintSExp( evaled ) ;
        throw GramWrong() ;
      } // else 
      
      cur = parameter->right ;
      
      ans.type = T ; // 先假設是對的
      while ( !cur->isLeaf ) {
        evaled = Eval( false, cur->left ) ;
        
        if ( evaled == NULL ) ;
        else if ( evaled->isLeaf && evaled->value.type == STRING ) {
          if ( operType == "string>?" ) {
            if ( preStr > evaled->value.name ) {
              if ( ans.type == T ) // 目前為止都是true
                ans.type = T ;
              else // 曾經不是true
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // if 
          else if ( operType == "string<?" ) {
            if ( preStr < evaled->value.name ) {
              if ( ans.type == T ) // 目前為止都是true
                ans.type = T ;
              else // 曾經不是true
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
          else { // "string=?"
            if ( preStr == evaled->value.name ) {
              if ( ans.type == T ) // 目前為止都是true
                ans.type = T ;
              else // 曾經不是true
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else 
        } // else if is string
        else { // 不是string
          cout << "ERROR (" << operType << " with incorrect argument type) : " ;
          TreeEditor::PrintSExp( evaled ) ;
          throw GramWrong() ;
        } // else 

        preStr = evaled->value.name ;
        cur = cur->right ;
      } // while

      result = TreeEditor::NewNode( ans ) ;
    } // else if string compare

    return result ;
  } // Arithmetic()

  // 7
  TreeNode * EQV( TreeNode * root ) {
    if ( !ArguNumCheck( false, root, "=", 2 ) ) {
      cout << "ERROR (incorrect number of arguments) : eqv?\n" ;
      throw GramWrong() ;
    } // if

    // 指向同一個object
    TreeNode * cur = root->right ; 
    TreeNode * p1 = NULL ; // argument1
    TreeNode * p2 = NULL ; // argument2
    Token ans ;
    ans.name = "" ;
    ans.type = NIL ;

    p1 = Eval( false, cur->left ) ;

    cur = cur->right ;
    p2 = Eval( false, cur->left ) ;

    cur = cur->right ;
    if ( p1 == p2 ) { // 指向同一個object
      ans.type = T ;
    } // if 
    else if ( p1->isLeaf && p2->isLeaf ) { // ATOM
      if ( p1->value.type == p2->value.type ) {
        // T, NIL, QUOTE存在名稱不止一種可能，故只需比較type
        // String就算內容一樣不算eqv
        if ( p1->value.type == T ) {
          ans.type = T ;
        } // if 
        else if ( p1->value.type == NIL ) {
          ans.type = T ;
        } // else if 
        else if ( p1->value.type == QUOTE ) {
          ans.type = T ;
        } // else if 
        else if ( p1->value.type == STRING ) {
          ans.type = NIL ;
        } // else if 
        else if ( p1->value.name == p2->value.name ) { // SYMBOL
          ans.type = T ;
        } // else if 
        else {
          ans.type = NIL ;
        } // else 
      } // if 
      else 
        ans.type = NIL ;
    } // else if 
    else {
      ans.type = NIL ;
    } // else ATOM

    return TreeEditor::NewNode( ans ) ;
  } // EQV()

  TreeNode * EQUAL( TreeNode * root ) {
    if ( !ArguNumCheck( false, root, "=", 2 ) ) {
      cout << "ERROR (incorrect number of arguments) : equal?\n" ;
      throw GramWrong() ;
    } // if

    // 結構相同
    TreeNode * cur = root->right ; 
    TreeNode * p1 = NULL ; // argument1
    TreeNode * p2 = NULL ; // argument2
    Token ans ;
    ans.name = "" ;
    ans.type = NIL ;

    p1 = Eval( false, cur->left ) ;

    cur = cur->right ;
    p2 = Eval( false, cur->left ) ;

    cur = cur->right ;
    if ( SameStruct( p1, p2 ) )
      ans.type = T;
    else
      ans.type = NIL ;

    return TreeEditor::NewNode( ans ) ;
  } // EQUAL()

  bool SameStruct( TreeNode * p1, TreeNode * p2 ) {
    TreeNode * curP1 = NULL ; 
    TreeNode * curP2 = NULL ; 

    if ( p1 == NULL && p2 == NULL ) {
      return true ;
    } // if 
    else if ( p1 == NULL || p2 == NULL ) {
      return false ;
    } // else if 
    else if ( p1->isLeaf && p2->isLeaf ) {
      if ( p1->value.type == p2->value.type ) {
        // T, NIL, QUOTE存在名稱不止一種可能，故只需比較type
        if ( p1->value.type == T ) {
          return true ;
        } // if 
        else if ( p1->value.type == NIL ) {
          return true ;
        } // else if 
        else if ( p1->value.type == QUOTE ) {
          return true ;
        } // else if 
        else if ( p1->value.name == p2->value.name ) {
          return true ;
        } // else if 
        else {
          return false ; 
        } // else 
      } // if
      else 
        return false ;
    } // else if 
    else if ( p1->isLeaf || p2->isLeaf ) {
      return false ;
    } // else if 
    else {
      if ( SameStruct( p1->left, p2->left ) && SameStruct( p1->right, p2->right ) ) 
        return true ;
      else 
        return false ;
    } // else 
  } // SameStruct()

  // 8 
  TreeNode * BEGIN( TreeNode * root ) {
    if ( !ArguNumCheck( false, root, ">=", 1 ) ) {
      cout << "ERROR (incorrect number of arguments) : begin\n" ;
      throw GramWrong() ;
    } // if

    // argument(>=1)
    // 取list尾端的值
    TreeNode * cur = root->right ;
    TreeNode * result = NULL ;

    while ( !cur->isLeaf ) {
      result = Eval( false, cur->left ) ; 
      cur = cur->right ;
    } // while

    return result ;
  } // BEGIN()

  // 9
  TreeNode * IF( TreeNode * root ) {
    if ( !ArguNumCheck( false, root, "=", 2 ) && !ArguNumCheck( false, root, "=", 3 ) ) {
      cout << "ERROR (incorrect number of arguments) : if\n" ;
      throw GramWrong() ;
    } // if

    // argument(2~3)
    TreeNode * cur = root->right ;
    TreeNode * result = NULL ;
    TreeNode * cond = NULL ; // 條件
    bool meetCond = false ; // 是否符合條件

    cond = Eval( false, cur->left ) ; // bool
    if ( cond == NULL ) {
      cout << "ERROR(IF format) : " ;
      throw GramWrong() ;
    } // if 
    else if ( cond->isLeaf && cond->value.type == NIL ) {
      meetCond = false ;
    } // else if
    else { // 非false的任何數值
      meetCond = true ;
    } // else 

    cur = cur->right ;
    if ( meetCond ) { // 條件成立，回傳第一個結果
      result = Eval( false, cur->left ) ; 
    } // if
    else { // 條件不成立，回傳第二個結果
      cur = cur->right ;
      if ( cur == NULL ) ;
      else if ( cur->isLeaf ) {
        cout << "ERROR (no return value) : " ;
        TreeEditor::PrintSExp( root ) ;
        throw GramWrong() ;
      } // else if 
      else {
        result = Eval( false, cur->left ) ; 
      } // else
    } // else 

    return result ;
  } // IF()

  TreeNode * COND( TreeNode * root ) {
    TreeNode * cur = root->right ;
    TreeNode * result = NULL ;
    TreeNode * statement = NULL ; // 第n組的內容
    TreeNode * cond = NULL ; // 第n組中的條件
    bool haveAns = false ; // 已滿足條件得到結果
    bool wrongFormat = false ;

    if ( !ArguNumCheck( false, root, ">=", 1 ) ) {
      cout << "ERROR (COND format) : " ;
      TreeEditor::PrintSExp( root ) ;
      throw GramWrong() ;
    } // if
    else {
      while ( !cur->isLeaf ) {
        statement = cur->left ;

        try {
          if ( !ArguNumCheck( true, statement, ">=", 1 ) ) { // 此組格式不符合
            cout << "ERROR (COND format) : " ;
            TreeEditor::PrintSExp( root ) ;
            throw GramWrong() ;
          } // if 
        } // try
        catch ( NonList error ) {
          wrongFormat = true ;
        } // catch 

        cur = cur->right ;
      } // while 
    } // else

    cur = root->right ;
    while ( !haveAns && !wrongFormat && !cur->isLeaf ) {
      cond = NULL ;
      statement = cur->left ;
      
      if ( cur->right->isLeaf ) { // 最後一組條件判斷
        if ( cur->right->value.type != NIL ) { // 右子節點還有東西
          wrongFormat = true ;
        } // if 
        else if ( statement->left->value.name == "else" ) {
          if ( !haveAns ) { // 如果還沒有值，將此值賦予
            haveAns = true ;
            try {
              result = BEGIN( statement ) ;
            } // try 
            catch ( NonList error ) {
              wrongFormat = true ;
            } // catch 
          } // if 
        } // else if is "else"
        else { // 不是"else"
          cond = Eval( false, statement->left ) ;
        } // else 
      } // if 最後一組條件判斷
      else {
        cond = Eval( false, statement->left ) ;
      } // else if
      
      if ( !haveAns && cond != NULL && 
           ! ( cond->isLeaf && cond->value.type == NIL ) ) { // 非false值即為true

        try {
          result = BEGIN( statement ) ;
        } // try 
        catch ( NonList error ) {
          wrongFormat = true ;
        } // catch 

        haveAns = true ;
      } // if 

      cur = cur->right ;
    } // while

    if ( wrongFormat ) {
      cout << "ERROR (COND format) : " ;
      TreeEditor::PrintSExp( root ) ;
      throw GramWrong() ;
    } // if 
    else if ( !haveAns ) {
      cout << "ERROR (no return value) : " ;
      TreeEditor::PrintSExp( root ) ;
      throw RuningError() ; 
    } // else if 

    return result ;
  } // COND()

  // 10
  TreeNode * CLEAN_ENVIRONMENT( TreeNode * root, bool isTopLevel ) {
    if ( !isTopLevel ) {
      cout << "ERROR (level of CLEAN-ENVIRONMENT)\n" ;
      throw GramWrong() ;
    } // if
    else if ( !ArguNumCheck( false, root, "=", 0 ) ) {
      cout << "ERROR (incorrect number of arguments) : clean-environment\n" ;
      throw GramWrong() ;
    } // else if 
    else {
      mSymList.clear() ;
      mFuncList.clear() ;
      cout << "environment cleaned\n" ;
    } // else

    return NULL ;
  } // CLEAN_ENVIRONMENT()

  TreeNode * EXIT( TreeNode * root, bool isTopLevel ) {
    if ( !isTopLevel ) {
      cout << "ERROR (level of EXIT)\n" ;
      throw GramWrong() ;
    } // if
    else if ( !ArguNumCheck( false, root, "=", 0 ) ) {
      cout << "ERROR (incorrect number of arguments) : exit\n" ;
      throw GramWrong() ;
    } // else if 
    else {
      mWillExit = true ;
    } // else 

    return NULL ;
  } // EXIT()
  //  } --Primitive--

  bool IsPrimitive( string name ) {
    // 1
    if ( name == "cons" ) return true ;
    else if ( name == "list" ) return true ;
    // 2
    else if ( name == "quote" ) return true ;
    // 3
    else if ( name == "define" ) return true ;
    // 4
    else if ( name == "car" ) return true ;
    else if ( name == "cdr" ) return true ;
    // 5
    else if ( name == "atom?" ) return true ;
    else if ( name == "pair?" ) return true ;
    else if ( name == "list?" ) return true ;
    else if ( name == "null?" ) return true ;
    else if ( name == "integer?" ) return true ;
    else if ( name == "real?" ) return true ;
    else if ( name == "number?" ) return true ;
    else if ( name == "string?" ) return true ;
    else if ( name == "boolean?" ) return true ;
    else if ( name == "symbol?" ) return true ;
    // 6
    else if ( name == "+" ) return true ;
    else if ( name == "-" ) return true ;
    else if ( name == "*" ) return true ;
    else if ( name == "/" ) return true ;
    else if ( name == "not" ) return true ;
    else if ( name == "and" ) return true ;
    else if ( name == "or" ) return true ;
    else if ( name == ">" ) return true ;
    else if ( name == ">=" ) return true ;
    else if ( name == "<" ) return true ;
    else if ( name == "<=" ) return true ;
    else if ( name == "=" ) return true ;
    else if ( name == "string-append" ) return true ;
    else if ( name == "string>?" ) return true ;
    else if ( name == "string<?" ) return true ;
    else if ( name == "string=?" ) return true ;
    // 7
    else if ( name == "eqv?" ) return true ;
    else if ( name == "equal?" ) return true ;
    // 8
    else if ( name == "begin" ) return true ;
    // 9
    else if ( name == "if" ) return true ;
    else if ( name == "cond" ) return true ;
    // 10
    else if ( name == "clean-environment" ) return true ;

    else if ( name == "exit" ) return true ;

    else return false ;
  } // IsPrimitive()

  TreeNode * DealPrimitive( bool isTopLevel, TreeNode * root ) {
    string name = root->left->value.name ;
    return DealPrimitive( isTopLevel, root, name ) ; 
  } // DealPrimitive()

  TreeNode * DealPrimitive( bool isTopLevel, TreeNode * root, string name ) {
    // 1
    if ( name == "cons" ) return CONS( root ) ;
    else if ( name == "list" ) return LIST( root ) ;
    // 2
    else if ( name == "quote" ) return SymQUOTE( root ) ;
    // 3
    else if ( name == "define" ) return DEFINE( isTopLevel, root ) ;
    // 4
    else if ( name == "car" ) return CAR( root ) ;
    else if ( name == "cdr" ) return CDR( root ) ;
    // 5
    else if ( name == "atom?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "pair?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "list?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "null?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "integer?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "real?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "number?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "string?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "boolean?" ) return TrueOrFalse( root, name ) ; 
    else if ( name == "symbol?" ) return TrueOrFalse( root, name ) ; 
    // 6
    else if ( name == "+" ) return Arithmetic( true, root, name ) ;
    else if ( name == "-" ) return Arithmetic( true, root, name ) ;
    else if ( name == "*" ) return Arithmetic( true, root, name ) ;
    else if ( name == "/" ) return Arithmetic( true, root, name ) ;
    else if ( name == "not" ) return Arithmetic( true, root, name ) ;
    else if ( name == "and" ) return Arithmetic( true, root, name ) ;
    else if ( name == "or" ) return Arithmetic( true, root, name ) ;
    else if ( name == ">" ) return Arithmetic( true, root, name ) ;
    else if ( name == ">=" ) return Arithmetic( true, root, name ) ;
    else if ( name == "<" ) return Arithmetic( true, root, name ) ;
    else if ( name == "<=" ) return Arithmetic( true, root, name ) ;
    else if ( name == "=" ) return Arithmetic( true, root, name ) ;
    else if ( name == "string-append" ) return Arithmetic( true, root, name ) ;
    else if ( name == "string>?" ) return Arithmetic( true, root, name ) ;
    else if ( name == "string<?" ) return Arithmetic( true, root, name ) ;
    else if ( name == "string=?" ) return Arithmetic( true, root, name ) ;
    // 7
    else if ( name == "eqv?" ) return EQV( root ) ;
    else if ( name == "equal?" ) return EQUAL( root ) ;
    // 8
    else if ( name == "begin" ) return BEGIN( root ) ;
    // 9
    else if ( name == "if" ) return IF( root ) ;
    else if ( name == "cond" ) return COND( root ) ;
    // 10
    else if ( name == "clean-environment" ) return CLEAN_ENVIRONMENT( root, isTopLevel ) ;

    else if ( name == "exit" ) return EXIT( root, isTopLevel ) ;

    return NULL ;
  } // DealPrimitive()

  TreeNode * PrimBinding( string key ) {
    int iPrim = Find( mPrim, key ) ;
    if ( iPrim != -1 ) // 已存在mPrim中
      return mPrim[iPrim].binding ;
    
    Token token ;
    Binder binder ;
    token.type = SYMBOL ;
    token.name = "#<procedure " + key + ">" ;
    binder.name = token.name ;
    binder.binding = TreeEditor::NewNode( token ) ;
    
    mPrim.insert( mPrim.begin(), binder ) ; // 新增在最前面
    
    return mPrim[0].binding ;
  } // PrimBinding()

  bool ArguNumCheck( bool willCatchError, TreeNode * root, string relation, int arguNum ) {
    TreeNode * cur = root ;
    int count = 0 ;
    
    while ( !cur->isLeaf ) {
      count++ ;
      cur = cur->right ;
    } // while

    count-- ; // 把root層減掉

    if ( count >= 0 && cur->value.type != NIL ) {
      if ( willCatchError ) { // 只丟error不印
        throw NonList() ;
      } // if 
      else {
        cout << "ERROR (non-list) : " ;
        TreeEditor::PrintSExp( root ) ;
        throw NonList() ;
      } // else 
    } // if 

    if ( relation == ">=" ) {
      if ( count >= arguNum ) 
        return true ;
      else 
        return false ;
    } // if 
    else if ( relation == "=" ) {
      if ( count == arguNum ) 
        return true ;
      else 
        return false ;
    } // else if 

    return false ;
  } // ArguNumCheck()

  int Find( vector<Binder> list, string key ) {
    for ( int i = 0 ; i < list.size() ; i++ ) {
      if ( list[i].name == key )
        return i ;
    } // for

    return -1 ;
  } // Find()

  TreeNode * Eval( bool isTopLevel, TreeNode * root ) {
    // project2 start
    int iFunc = 0, iSym = 0, iPrim = 0 ;
    string bindingName = "" ;
    TreeNode * newRoot = NULL ;
    if ( root == NULL )
      return NULL ; // root is Leaf node
    else if ( root->isLeaf ) { // isLeaf 表示 not empty
      // 已定義或未定義symbol , 或 單純數值
      if ( root->value.type == SYMBOL ) {
        iSym = Find( mSymList, root->value.name ) ;
        if ( iSym != -1 ) { // defined symbol
          return mSymList[iSym].binding ;
          // 定義過 解開回傳 
        } // if defined symbol
        //-----------------------OK 
        else if ( IsPrimitive( root->value.name ) ) {
          return PrimBinding( root->value.name ) ;
        } // else if
        //------------------------OK 
        else { // undefined symbol
          cout << "ERROR (unbound symbol) : " << root->value.name << "\n" ;
          throw UnDefine() ; 
        } // else undefined symbol
      } // if symbol
      else { // not symbol
        return root ; // 直接回傳
      } // else not symbol
    } // else if root is Leaf node
    
    // root->leaf is leaf 
    // function node + argument list
    else if ( root->left->value.type == QUOTE ) {
      return root->right ;
    } // else if 
    // 在最外面將quote處理 直接回傳right 
    else if ( root->left->value.type == SYMBOL ) { // Symbol開頭( function )
      iFunc = Find( mFuncList, root->left->value.name ) ;
      iSym = Find( mSymList, root->left->value.name ) ;
      iPrim = Find( mPrim, root->left->value.name ) ;
      if ( iFunc != -1 ) {
        return root ; // 目前還沒自訂function
      } // if
      else if ( iPrim != -1 ) { // #<procedure xxx>
        bindingName = mPrim[iPrim].name ;
        bindingName = bindingName.substr( 12, bindingName.size()-13 ) ; // 擷取prime name
        return DealPrimitive( isTopLevel, root, bindingName ) ;
      } // else if 
      else if ( iSym != -1 ) { // defined symbol
        newRoot = TreeEditor::NewNode() ;
        newRoot->left = Eval( false, root->left ) ;
        newRoot->right = root->right ;

        if ( newRoot->left->isLeaf ) {
          return Eval( isTopLevel, newRoot ) ;
        } // if
        else {
          cout << "ERROR (attempt to apply non-function) : " ;
          TreeEditor::PrintSExp( newRoot->left ) ;
          throw BindingWrong() ; 
        } // else 
      } // else if defined symbol
      else if ( IsPrimitive( root->left->value.name ) ) {
        return DealPrimitive( isTopLevel, root ) ;
      } // else if
      else { // 找不到此function名稱
        cout << "ERROR (unbound symbol) : " << root->left->value.name << "\n" ;
        throw UnDefine() ; 
      } // else 
    } // else if is function name start
    else if ( root->left->isLeaf ) { // 左子節點是葉節點
      // function node 非 SYMBOL&QUOYE
      ArguNumCheck( false, root, ">=", -1 ) ; // 先檢查是否為list

      cout << "ERROR (attempt to apply non-function) : " ;
      TreeEditor::PrintSExp( root->left ) ;
      throw BindingWrong() ; 
    } // else if
    // root->leaf is middle node
    else if ( isTopLevel ) { // 重新計算
      newRoot = TreeEditor::NewNode() ;
      newRoot->left = Eval( false, root->left ) ;
      newRoot->right = root->right ;

      if ( newRoot->left->isLeaf ) {
        return Eval( true, newRoot ) ;
      } // if
      else {
        cout << "ERROR (attempt to apply non-function) : " ;
        TreeEditor::PrintSExp( newRoot->left ) ;
        throw BindingWrong() ; 
      } // else 
    } // else if 
    else {
      newRoot = TreeEditor::NewNode() ;
      newRoot->left = Eval( false, root->left ) ;
      newRoot->right = root->right ;
      return Eval( false, newRoot ) ;
    } // else

    return root ;
  } // Eval()

public:
  Evaluater() {
    mSymList.clear() ;
    mFuncList.clear() ;
    mPrim.clear() ;
    mUseDefine = false ;
    mWillExit = false ;
  } // Evaluater()

  ~Evaluater() {
    mSymList.clear() ;
    mFuncList.clear() ;
    mPrim.clear() ;
  } // ~Evaluater()

  TreeNode * EvalSExp( TreeNode * inSExp ) {
    TreeNode * resultSExp = NULL ;
    mUseDefine = false ;
    try {
      resultSExp = Eval( true, inSExp ) ;
    } // try
    catch ( UnDefine error ) { 
      // 名稱未知->忽略此指令
      return NULL ;
    } // catch 
    catch ( GramWrong error ) {
      // 文法錯誤->忽略此指令
      return NULL ;
    } // catch GramWrong
    catch ( BindingWrong error ) {
      // 找不到binding->忽略此指令
      return NULL ;
    } // catch BindingWrong
    catch ( RuningError error ) {
      // 執行錯誤->忽略此指令
      return NULL ;
    } // catch RuningError
    catch ( NonList error ) {
      // 無法處理->忽略此指令
      return NULL ;
    } // catch NonList

    return resultSExp ;
  } // EvalSExp()

  bool IsExit() {
    return mWillExit ;
  } // IsExit()

  bool UseDefine() {
    return mUseDefine ;
  } // UseDefine()
}; // class Evaluater

class Scanner {
private:
  string mNextToken ;
  string mPreToken ;
  char mNextChar ;

  int mLine ; // 所有文件中的第幾行
  int mPTEndLine ; // preToken End Line in mLine

  TokenType CheckTokenType( string token ) {
    TokenType type = NONE ;
    if ( token == "" )
      type = NONE ;
    else if ( token == "(" )
      type = LEFT_PAREN ;
    else if ( token == ")" )
      type = RIGHT_PAREN ;
    else if ( token.at( 0 ) == '\"' ) {
      if ( token.at( token.length()-1 ) == '\"' )
        type = STRING ;
      else
        type = SYMBOL ;
    } // else if
    else if ( token == "." )
      type = DOT ;
    else if ( token == "nil" || token == "#f" || token == "()" )
      type = NIL ;
    else if ( token == "t" || token == "#t" )
      type = T ;
    else if ( token == "\'" )
      type = QUOTE ;
    else {
      int i = 0 ;
      int dotNum = 0 ;
      bool haveDigit = false ;
      if ( token.at( i ) == '+' || token.at( i ) == '-' )
        i++ ;

      for ( ; i < token.length() ; i++ ) {
        if ( CharCheck::IsDigit( token.at( i ) ) )
          haveDigit = true ;
        else if ( token.at( i ) == '.' )
          dotNum++ ;
        else // 當有一個非數字或非dot的字元出現
          type = SYMBOL ;
      } // for

      // 純數字+dot
      if ( type != NONE ) // 已被設定
        ;
      else if ( !haveDigit )
        type = SYMBOL ;
      else if ( dotNum == 0 ) // 沒有dot
        type = INT ;
      else if ( dotNum == 1 ) // 1個dot
        type = FLOAT ;
      else // 1個以上的dot
        type = SYMBOL ;
    } // else

    return type ;
  } // CheckTokenType()

  Token DealTokenThenClear( string & tokenName ) { // 依tokenName產生一個Token, 並清除tokenName
    Token token ;
    token.type = CheckTokenType( tokenName ) ;
    token.name = tokenName ;
    mPreToken = tokenName ;
    tokenName = "" ;
    return token ;
  } // DealTokenThenClear()

public:
  Scanner() {
    mLine = 0 ;
    mPTEndLine = 0 ;
    mNextToken = "" ;
    mPreToken = "" ;
    mNextChar = '\0' ;
  } // Scanner()

  ~Scanner() {
    mLine = 0 ;
    mPTEndLine = 0 ;
    mNextToken = "" ;
    mPreToken = "" ;
    mNextChar = '\0' ;
  } // ~Scanner()

  Token GetToken( bool isFirstT, int & line, int & column ) {
    bool done = false, errorQL = false ;
    Token token ;

    // 如果有已讀入但尚未處理的，先處理
    if ( mNextToken != "" ) { // 已有讀入token沒被處理
      token = DealTokenThenClear( mNextToken ) ;
      mPTEndLine = mLine ; // 記錄此token結束的line在哪
      done = true ;
    } // if

    // 跳過white space, 直到非white space或END-OF-FILE
    // 從非white space開始繼續讀入，直到separator
    while ( !done && !errorQL && !cin.eof() ) { // 讀檔沒有狀況且沒讀到結尾

      if ( mNextChar == '\0' )
        ;
      else if ( CharCheck::IsWhiteSpace( mNextChar ) || cin.eof() ) { // white space
        if ( mNextToken != "" ) { // 成立token
          token = DealTokenThenClear( mNextToken ) ;
          mPTEndLine = mLine ; // 記錄此token結束的line在哪
          done = true ;
        } // if

        if ( !done ) { // 來處理此char資訊
          column++ ;
          if ( mNextChar == '\n' ) {

            // 若在上一個S-exp結束後還沒有東西，不要計算此line讀到的
            if ( isFirstT && mPTEndLine == mLine )
              line = 1 ;
            else
              line++ ;

            isFirstT = false ;
            column = 0 ;
            mLine++ ;
          } // if
        } // if not done

      } // else if white space
      else if ( CharCheck::IsSeparator( mNextChar ) ) { // separator
        if ( mNextChar == ';' ) { // comment 無效字元
          // 遇到註解優先處理掉
          while ( mNextChar != '\n' && !cin.eof() ) {
            mNextChar = cin.get();
          } // while

          // 若在上一個S-exp結束後還沒有東西，不要計算此line讀到的
          if ( isFirstT && mPTEndLine == mLine )
            line = 1 ;
          else
            line++ ;

          isFirstT = false ;
          column = 0 ;
          mLine++ ;

        } // if comment
        else if ( mNextToken != "" ) { // 前面有token成立
          token = DealTokenThenClear( mNextToken ) ;
          mPTEndLine = mLine ; // 記錄此token結束的line在哪
          done = true ;
        } // else if token成立
        else if ( mNextChar == '\"' ) { // string
          column++ ;
          mNextToken = mNextChar ; // "

          mNextChar = cin.get() ; // 讀'內文的第一個char
          while ( mNextChar != '\"' && mNextChar != '\n' && !cin.eof() ) {
            column++ ;
            mNextToken = mNextToken + mNextChar ;
            if ( mNextChar == '\\' ) { // 特殊字元
              mNextChar = cin.get() ; // 直接讀進下一個字元
              column++ ;
              mNextToken = mNextToken + mNextChar ;
            } // if

            mNextChar = cin.get() ;
          } // while

          column++ ; // end char
          if ( mNextChar == '\"' ) { // if no problenm
            mNextToken = mNextToken + mNextChar ;
            mNextChar = '\0' ;
            token = DealTokenThenClear( mNextToken ) ;
            mPTEndLine = mLine ; // 記錄此token結束的line在哪
            done = true ;
          } // if
          else { // error ---- throw QuoteLost !
            mNextChar = '\0' ;
            mNextToken = "" ;
            errorQL = true ;
          } // else

        } // else if string
        else { // 前面沒有token 處理separator
          column++ ;
          mNextToken = mNextChar ;
          mNextChar = '\0' ;
          token = DealTokenThenClear( mNextToken ) ;
          mPTEndLine = mLine ; // 記錄此token結束的line在哪
          done = true ;
        } // else
      } // else if separator
      else { // 非white space && 非separator
        column++ ;
        mNextToken = mNextToken + mNextChar ;
      } // else

      if ( !done && !errorQL )
        mNextChar = cin.get() ;
    } // while

    if ( !done && mNextToken != "" ) {
      token = DealTokenThenClear( mNextToken ) ;
      mPTEndLine = mLine ; // 記錄此token結束的line在哪
      done = true ;
    } // if

    // deal output
    if ( errorQL )
      throw QuoteLost() ;

    if ( cin.eof() ) {
      if ( done )
        return token ;

      throw EndFile() ;
    } // if 

    return token ;
  } // GetToken()

  Token PeekToken( bool isFirstT, int & line, int & column ) {
    Token token ;
    if ( mNextToken == "" ) { // 沒有現有token
      token = GetToken( isFirstT, line, column ) ; // 保存讀入的新token
      mNextToken = token.name ; // 存好mNextToken
    } // if
    else { // 已有現成token
      token.name = mNextToken ;
      token.type = CheckTokenType( mNextToken ) ;
    } // else

    return token ;
  } // PeekToken()

  Token NowToken() {
    string tmpName = mPreToken ;
    return DealTokenThenClear( tmpName ) ;
  } // NowToken()

  void SkipThisLine() {
    while ( mNextChar != '\n' && !cin.eof() ) {
      mNextChar = cin.get() ;
    } // while

    mNextChar = '\0' ;
    mLine++ ;
  } // SkipThisLine()

}; // class Scanner

class Parser {
private:
  Scanner * mScanner ;
  vector<Token> mTokens ; // data list

  //   --------Syntax of OurScheme--------- {
  bool S_EXP( bool isFirst, int & line, int & column ) {
    // <S-exp> ::= <ATOM>
    //             | LEFT-PAREN <S-exp> { <S-exp> } [ DOT <S-exp> ] RIGHT-PAREN
    //             | QUOTE <S-exp>
    Token curToken = mScanner->PeekToken( isFirst, line, column ) ;

    if ( curToken.type == SYMBOL || curToken.type == INT || curToken.type == FLOAT
         || curToken.type == STRING || curToken.type == NIL || curToken.type == T ) {
      return ATOM( line, column ) ;
    } // if
    else if ( curToken.type == QUOTE ) {
      mTokens.push_back( mScanner->GetToken( false, line, column ) ) ; // QUOTE
      return S_EXP( false, line, column ) ;
    } // else if
    else if ( curToken.type == DOT || curToken.type == RIGHT_PAREN ) {
      mScanner->GetToken( false, line, column ) ; // 讀掉有問題的token
      throw LPLost() ; // error ------------ throw LPLost
    } // else if
    else if ( curToken.type == LEFT_PAREN ) {
      if ( ATOM( line, column ) ) { // 在裡面已Get LEFT_PAREN
        return true ;
      } // if
      else {
        curToken.name = "(" ;
        curToken.type = LEFT_PAREN ;
        mTokens.push_back( curToken ) ;
        if ( S_EXP( false, line, column ) ) { // <S-exp>
          curToken = mScanner->PeekToken( false, line, column ) ;
          while ( curToken.type != DOT && curToken.type != RIGHT_PAREN
                  && S_EXP( false, line, column ) ) {
            curToken = mScanner->PeekToken( false, line, column ) ; // { <S-exp> }
          } // while

          curToken = mScanner->PeekToken( false, line, column ) ;
          if ( curToken.type == DOT ) { // [ DOT <S-exp> ]
            mTokens.push_back( mScanner->GetToken( false, line, column ) ) ; // DOT
            if ( !S_EXP( false, line, column ) ) // <S-exp>
              return false ;
          } // if

          curToken = mScanner->PeekToken( false, line, column ) ;
          if ( curToken.type == RIGHT_PAREN ) { // RIGHT_PAREN
            mTokens.push_back( mScanner->GetToken( false, line, column ) ) ; // RIGHT_PAREN
            return true ;
          } // if
          else {
            mScanner->GetToken( false, line, column ) ; // 讀掉有問題的token
            throw RPLost() ; // error ------------ throw RPLost
          } // else
        } // if
      } // else
    } // else if  LEFT_PAREN 開頭

    return false ;
  } // S_EXP()

  bool ATOM( int & line, int & column ) {
    // <ATOM>  ::= SYMBOL | INT | FLOAT | STRING
    //             | NIL | T | LEFT-PAREN RIGHT-PAREN
    Token curToken = mScanner->PeekToken( false, line, column ) ;

    if ( curToken.type == SYMBOL || curToken.type == INT || curToken.type == FLOAT
         || curToken.type == STRING || curToken.type == NIL ||  curToken.type == T ) {
      mTokens.push_back( mScanner->GetToken( false, line, column ) ) ;
      return true ;
    } // if
    else if ( curToken.type == LEFT_PAREN ) {
      mScanner->GetToken( false, line, column ) ; // LEFT_PAREN
      curToken = mScanner->PeekToken( false, line, column ) ;
      if ( curToken.type == RIGHT_PAREN ) {
        mScanner->GetToken( false, line, column ) ; // RIGHT_PAREN
        curToken.name = "()" ;
        curToken.type = NIL ;
        mTokens.push_back( curToken ) ;
        return true ;
      } // if
      else
        return false ;
    } // else if

    return false ;
  } // ATOM()
  // } --------Syntax of OurScheme---------

public:
  Parser() {
    mScanner = new Scanner ;
    mTokens.clear() ;
  } // Parser()

  ~Parser() {
    delete mScanner ;
    mTokens.clear() ;
  } // ~Parser()

  TreeNode * ReadSExp( TreeNode * expr ) {
    // initial
    int line = 1, column = 0 ;
    int index = 0 ; // 用來建樹的參數
    // TreeEditor::DeleteTree( expr ) ;
    expr = NULL ;
    mTokens.clear() ;
    try {
      if ( S_EXP( true, line, column ) ) { // 取一個 S-Exp
        expr = TreeEditor::Build( mTokens, index, expr ) ; // 建樹
      } // if
    } // try
    catch ( LPLost error ) {
      cout << "ERROR (unexpected token) : " ;
      cout << "atom or '(' expected when token at" ;
      cout << " Line " << line << " Column " << column ;
      cout << " is >>" << mScanner->NowToken().name << "<<\n" ;
      mScanner->SkipThisLine() ;
      return NULL ;
    } // catch LPLost
    catch ( RPLost error ) {
      cout << "ERROR (unexpected token) : ')' " ;
      cout << "expected when token at" ;
      cout << " Line " << line << " Column " << column ;
      cout << " is >>" << mScanner->NowToken().name << "<<\n" ;
      mScanner->SkipThisLine() ;
      return NULL ;
    } // catch RPLost
    catch ( QuoteLost error ) {
      cout << "ERROR (no closing quote) : END-OF-LINE encountered at" ;
      cout << " Line " << line << " Column " << column << "\n" ;
      return NULL ;
    } // catch QuoteLost
    
    return expr ;
  } // ReadSExp()

}; // Parser

int main() {
  Parser parser ;
  Evaluater evaluater ;
  TreeNode * inSExp = NULL ; // 'expr' is a pointer that points to a linked list data structure
  TreeNode * resultSExp = NULL ;
  char ch ;
  cin >> uTestNum ;
  ch = cin.get() ; // read '\n'

  cout << "Welcome to OurScheme!\n" ;

  bool exit = false ;
  bool eof = false ;
  while ( !exit ) { // !(user entered '(exit)') && !(END-OF-FILE encountered)
    cout << "\n> " ;
    try {
      inSExp = parser.ReadSExp( inSExp ) ;
    } // try
    catch( EndFile error ) {
      cout << "ERROR (no more input) : END-OF-FILE encountered" ;
      eof = true ;
    } // catch EndFile

    if ( eof ) // end of file
      exit = true ;
    else { 
      resultSExp = evaluater.EvalSExp( inSExp ) ;
      
      if ( evaluater.IsExit() ) // exit
        exit = true ;
      else if ( evaluater.UseDefine() ) 
        ; // 不輸出也不刪除
      else {
        TreeEditor::PrintSExp( resultSExp ) ; // "pretty print" this data structure.
        
        TreeEditor::DeleteTree( inSExp ) ; 
      } // else 

    } // else

  } // while

  cout << "\nThanks for using OurScheme!" ;
} // main()

