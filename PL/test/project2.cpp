// 10727109 �L�ȧ�
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
  string name ;     // token�W��
  TokenType type ;  // token���O
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
  // int numOfArg ; // �Ofunction�ݭn�����ѼƼƶq
}; // Binder

static int uTestNum = 0 ;

//   --exception-- {
// read error
class LPLost {} ; // �֤F'('
class RPLost {} ; // �֤F')'
class QuoteLost {} ; // �֤F'\''
class EndFile {} ; // �S�����`�����A�w�L��J
// eval error
class UnDefine {} ; // ���w�qname
class BindingWrong {} ;  // binding���~(�䤣�����function/symbol name)
class NonList {} ; // �̥k�l�`�I���F�� or �u�O���`�I
class GramWrong {} ; // ��k���~
class RuningError {} ; // ����ɥX�����~
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

    // ��'\'���B�z
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

        i++ ; // �w�g�B�z���F
      } // if
      else {
        temp = temp + str.at( i ) ;
      } // else

    } // for

    // \' �নquote
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
    else if ( root->isLeaf ) { // ���`�I �����L���`�I
      PrintSpace( needSpace, level ) ;
      PrintNode( root ) ;
    } // else if
    else { // �����`�I

      // check left node
      if ( root->left == NULL )
        ;
      else if ( root->left->isLeaf ) { // ���`�I�O���`�I
        Print( needSpace, level, root->left ) ;
      } // else if
      else { // ���`�I�O�����`�I
        PrintSpace( needSpace, level ) ;
        cout << "( " ;
        Print( false, level+1, root->left ) ; // '('��Ĥ@��TOKEN���ݭn�ť�
        PrintSpace( true, level ) ;
        cout << ")\n" ;
      } // else ���`�I�O�����`�I

      // check right node
      if ( root->right == NULL )
        ;
      else if ( root->left != NULL && root->left->value.type == QUOTE ) { // ����Oquote
        // ����Oquote��, ��k��A���_�ӦL�X
        if ( root->right->isLeaf ) { // ���`�I
          // �����L
          Print( true, level, root->right ) ; // '('��Ĥ@��TOKEN���ݭn�ť�
        } // if ���`�I
        else { // �����`�I
          PrintSpace( true, level ) ;
          cout << "( " ;
          Print( false, level+1, root->right ) ; // '('��Ĥ@��TOKEN���ݭn�ť�
          PrintSpace( true, level ) ;
          cout << ")\n" ;
        } // else �����`�I
      } // else if ����Oquote
      else if ( root->right->isLeaf ) { // �k�`�I�O���`�I
        if ( root->right->value.type != NIL ) {
          PrintSpace( true, level ) ;
          cout << ".\n" ;
          Print( true, level, root->right ) ;
        } // if
      } // else if �k�`�I�O���`�I
      else { // �k�`�I�O�����`�I
        Print( true, level, root->right ) ;
      } // else

    } // else ���I�������`�I
  } // Print()

public:

  TreeEditor() {
  } // TreeEditor()

  ~TreeEditor() {
  } // ~TreeEditor()

  static TreeNode * NewNode() { // �����`�I
    TreeNode * node = new TreeNode() ;
    node->isLeaf = false ;
    node->value.name = "" ;
    node->value.type = NIL ;
    node->left = NULL ;
    node->right = NULL ;
    return node ;
  } // NewNode()

  static TreeNode * NewNode( Token token ) { // ���`�I
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
              && tokens.at( index ).type != RIGHT_PAREN ) { // �D"." or ")"
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
    else { // �u�����e
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
  vector<Binder> mSymList ;  // ��stack�ΡA�s�W�b�Ĥ@�ӡA�j�M�q�Ĥ@��
  vector<Binder> mFuncList ; // ��stack�ΡA�s�W�b�Ĥ@�ӡA�j�M�q�Ĥ@��
  vector<Binder> mPrim ; // �sprimitive binding, �����|�w����n, �u�O���F�����зs�W��n�bmemory

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
    // mSmlist �ΨӦs��w�q��sumbol 
    int iSym = 0 ; // index of mSymList
    bool correctFormat = false ;

    curOld = curOld->right ;
    // curOld = A
    if ( curOld->left->value.type == SYMBOL ) { // ���n�Q�w�q��symbl
      newBinder.name = curOld->left->value.name ;
      iSym = Find( mSymList, newBinder.name ) ; // �ˬd�O�_���w�w�qvalue
      // �ˬdA �O�_�w�g�w�q�L 
      curOld = curOld->right ;
      // curOld �{�b�b B ����m 
      // B�p�G�w�q�L�|�bEval �^�� 
      newBinder.binding = Eval( false, curOld->left ) ;

      if ( iSym != -1 ) { // �w�Q�w�q�L->���w�q
        correctFormat = true ;
        mSymList[iSym].binding = newBinder.binding ;
      } // if �л\�©w�q
      else if ( !IsPrimitive( newBinder.name ) ) { // ���O�ϥ�primitive�W��
        correctFormat = true ;
        mSymList.insert( mSymList.begin(), newBinder ) ; // �s�W�b�̫e��
      } // else if �s�W�w�q
    } // if ��symbol name

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
        count++ ; // �������X�ӥ��l�`�I(list���X�Ӥ��e)
        cur = cur->right ;
      } // while

      if ( count >= 0 && cur->value.type != NIL ) // �̥k�����ONIL
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

      string mainOT = operType ; // ���F"-"�P"/"���j
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

      // ���j���U�B�z�᭱���Ʀr
      if ( operType == "+" || operType == "*" ) {
        evaled = Arithmetic( false, parameter->right, operType ) ; // second (���j���U)
      } // if 
      else if ( operType == "-" ) { 
        evaled = Arithmetic( false, parameter->right, "+-" ) ; // second (���j���U�[)
      } // else if 
      else { // "/"
        evaled = Arithmetic( false, parameter->right, "*/" ) ; // second (���j���U��)
      } // else 

      if ( firstAri && evaled == NULL ) { // �u���@��argument
        cout << "ERROR (incorrect number of arguments) : " ;
        cout << mainOT << "\n" ;
        throw GramWrong() ;
      } // if 
      else if ( evaled == NULL ) { // ���j�쩳�F
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
      else // �Dfalse�Ȫ�
        ans.type = NIL ;

      result = TreeEditor::NewNode( ans ) ;
    } // else if "not"

    else if ( operType == "and" || operType == "or" ) {
      // argument(>=2)
      // and : �Ĥ@��argument��nil�A�^�Ǩ�ȡF�_�h�^�ǲĤG��argument
      // or : �Ĥ@��argument����nil�A�^�Ǩ�ȡF�_�h�^�ǲĤG��argument
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
      // ������float�h���
      float preNum = 0.0 ;
      evaled = Eval( false, parameter->left ) ; // deal first token
      if ( evaled == NULL ) {
        cout << "ERROR(" << operType << " format) : " ;
        throw GramWrong() ;
      } // if 
      else if ( ( evaled->isLeaf ) && 
                ( evaled->value.type == INT || evaled->value.type == FLOAT ) ) {
        preNum = CharCheck::StrToFlo( evaled->value.name ) ; // �e�@�Ӫ��Ʀr
      } // else if 
      else {
        cout << "ERROR (" << operType << " with incorrect argument type) : " ;
        TreeEditor::PrintSExp( evaled ) ;
        throw GramWrong() ;
      } // else 
      
      cur = parameter->right ;
      
      ans.type = T ; // �����]�O�諸
      while ( !cur->isLeaf ) {
        evaled = Eval( false, cur->left ) ;
        
        if ( evaled == NULL ) ;
        else if ( ( evaled->isLeaf ) &&
                  ( evaled->value.type == INT || evaled->value.type == FLOAT ) ) {
          if ( operType == ">" ) {
            if ( preNum > CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // �ثe����Otrue
                ans.type = T ;
              else // ���g���Otrue
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // if 
          else if ( operType == ">=" ) {
            if ( preNum >= CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // �ثe����Otrue
                ans.type = T ;
              else // ���g���Otrue
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
          else if ( operType == "<" ) {
            if ( preNum < CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // �ثe����Otrue
                ans.type = T ;
              else // ���g���Otrue
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
          else if ( operType == "<=" ) {
            if ( preNum <= CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // �ثe����Otrue
                ans.type = T ;
              else // ���g���Otrue
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
          else { // "="
            if ( preNum == CharCheck::StrToFlo( evaled->value.name ) ) {
              if ( ans.type == T ) // �ثe����Otrue
                ans.type = T ;
              else // ���g���Otrue
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
        } // else if is number
        else { // ���Onumber
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
        else { // ���Ostring
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
      
      ans.type = T ; // �����]�O�諸
      while ( !cur->isLeaf ) {
        evaled = Eval( false, cur->left ) ;
        
        if ( evaled == NULL ) ;
        else if ( evaled->isLeaf && evaled->value.type == STRING ) {
          if ( operType == "string>?" ) {
            if ( preStr > evaled->value.name ) {
              if ( ans.type == T ) // �ثe����Otrue
                ans.type = T ;
              else // ���g���Otrue
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // if 
          else if ( operType == "string<?" ) {
            if ( preStr < evaled->value.name ) {
              if ( ans.type == T ) // �ثe����Otrue
                ans.type = T ;
              else // ���g���Otrue
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else if 
          else { // "string=?"
            if ( preStr == evaled->value.name ) {
              if ( ans.type == T ) // �ثe����Otrue
                ans.type = T ;
              else // ���g���Otrue
                ans.type = NIL ;
            } // if 
            else 
              ans.type = NIL ;
          } // else 
        } // else if is string
        else { // ���Ostring
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

    // ���V�P�@��object
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
    if ( p1 == p2 ) { // ���V�P�@��object
      ans.type = T ;
    } // if 
    else if ( p1->isLeaf && p2->isLeaf ) { // ATOM
      if ( p1->value.type == p2->value.type ) {
        // T, NIL, QUOTE�s�b�W�٤���@�إi��A�G�u�ݤ��type
        // String�N�⤺�e�@�ˤ���eqv
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

    // ���c�ۦP
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
        // T, NIL, QUOTE�s�b�W�٤���@�إi��A�G�u�ݤ��type
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
    // ��list���ݪ���
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
    TreeNode * cond = NULL ; // ����
    bool meetCond = false ; // �O�_�ŦX����

    cond = Eval( false, cur->left ) ; // bool
    if ( cond == NULL ) {
      cout << "ERROR(IF format) : " ;
      throw GramWrong() ;
    } // if 
    else if ( cond->isLeaf && cond->value.type == NIL ) {
      meetCond = false ;
    } // else if
    else { // �Dfalse������ƭ�
      meetCond = true ;
    } // else 

    cur = cur->right ;
    if ( meetCond ) { // ���󦨥ߡA�^�ǲĤ@�ӵ��G
      result = Eval( false, cur->left ) ; 
    } // if
    else { // ���󤣦��ߡA�^�ǲĤG�ӵ��G
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
    TreeNode * statement = NULL ; // ��n�ժ����e
    TreeNode * cond = NULL ; // ��n�դ�������
    bool haveAns = false ; // �w��������o�쵲�G
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
          if ( !ArguNumCheck( true, statement, ">=", 1 ) ) { // ���ծ榡���ŦX
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
      
      if ( cur->right->isLeaf ) { // �̫�@�ձ���P�_
        if ( cur->right->value.type != NIL ) { // �k�l�`�I�٦��F��
          wrongFormat = true ;
        } // if 
        else if ( statement->left->value.name == "else" ) {
          if ( !haveAns ) { // �p�G�٨S���ȡA�N���Ƚᤩ
            haveAns = true ;
            try {
              result = BEGIN( statement ) ;
            } // try 
            catch ( NonList error ) {
              wrongFormat = true ;
            } // catch 
          } // if 
        } // else if is "else"
        else { // ���O"else"
          cond = Eval( false, statement->left ) ;
        } // else 
      } // if �̫�@�ձ���P�_
      else {
        cond = Eval( false, statement->left ) ;
      } // else if
      
      if ( !haveAns && cond != NULL && 
           ! ( cond->isLeaf && cond->value.type == NIL ) ) { // �Dfalse�ȧY��true

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
    if ( iPrim != -1 ) // �w�s�bmPrim��
      return mPrim[iPrim].binding ;
    
    Token token ;
    Binder binder ;
    token.type = SYMBOL ;
    token.name = "#<procedure " + key + ">" ;
    binder.name = token.name ;
    binder.binding = TreeEditor::NewNode( token ) ;
    
    mPrim.insert( mPrim.begin(), binder ) ; // �s�W�b�̫e��
    
    return mPrim[0].binding ;
  } // PrimBinding()

  bool ArguNumCheck( bool willCatchError, TreeNode * root, string relation, int arguNum ) {
    TreeNode * cur = root ;
    int count = 0 ;
    
    while ( !cur->isLeaf ) {
      count++ ;
      cur = cur->right ;
    } // while

    count-- ; // ��root�h�

    if ( count >= 0 && cur->value.type != NIL ) {
      if ( willCatchError ) { // �u��error���L
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
    else if ( root->isLeaf ) { // isLeaf ��� not empty
      // �w�w�q�Υ��w�qsymbol , �� ��¼ƭ�
      if ( root->value.type == SYMBOL ) {
        iSym = Find( mSymList, root->value.name ) ;
        if ( iSym != -1 ) { // defined symbol
          return mSymList[iSym].binding ;
          // �w�q�L �Ѷ}�^�� 
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
        return root ; // �����^��
      } // else not symbol
    } // else if root is Leaf node
    
    // root->leaf is leaf 
    // function node + argument list
    else if ( root->left->value.type == QUOTE ) {
      return root->right ;
    } // else if 
    // �b�̥~���Nquote�B�z �����^��right 
    else if ( root->left->value.type == SYMBOL ) { // Symbol�}�Y( function )
      iFunc = Find( mFuncList, root->left->value.name ) ;
      iSym = Find( mSymList, root->left->value.name ) ;
      iPrim = Find( mPrim, root->left->value.name ) ;
      if ( iFunc != -1 ) {
        return root ; // �ثe�٨S�ۭqfunction
      } // if
      else if ( iPrim != -1 ) { // #<procedure xxx>
        bindingName = mPrim[iPrim].name ;
        bindingName = bindingName.substr( 12, bindingName.size()-13 ) ; // �^��prime name
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
      else { // �䤣�즹function�W��
        cout << "ERROR (unbound symbol) : " << root->left->value.name << "\n" ;
        throw UnDefine() ; 
      } // else 
    } // else if is function name start
    else if ( root->left->isLeaf ) { // ���l�`�I�O���`�I
      // function node �D SYMBOL&QUOYE
      ArguNumCheck( false, root, ">=", -1 ) ; // ���ˬd�O�_��list

      cout << "ERROR (attempt to apply non-function) : " ;
      TreeEditor::PrintSExp( root->left ) ;
      throw BindingWrong() ; 
    } // else if
    // root->leaf is middle node
    else if ( isTopLevel ) { // ���s�p��
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
      // �W�٥���->���������O
      return NULL ;
    } // catch 
    catch ( GramWrong error ) {
      // ��k���~->���������O
      return NULL ;
    } // catch GramWrong
    catch ( BindingWrong error ) {
      // �䤣��binding->���������O
      return NULL ;
    } // catch BindingWrong
    catch ( RuningError error ) {
      // ������~->���������O
      return NULL ;
    } // catch RuningError
    catch ( NonList error ) {
      // �L�k�B�z->���������O
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

  int mLine ; // �Ҧ���󤤪��ĴX��
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
        else // ���@�ӫD�Ʀr�ΫDdot���r���X�{
          type = SYMBOL ;
      } // for

      // �¼Ʀr+dot
      if ( type != NONE ) // �w�Q�]�w
        ;
      else if ( !haveDigit )
        type = SYMBOL ;
      else if ( dotNum == 0 ) // �S��dot
        type = INT ;
      else if ( dotNum == 1 ) // 1��dot
        type = FLOAT ;
      else // 1�ӥH�W��dot
        type = SYMBOL ;
    } // else

    return type ;
  } // CheckTokenType()

  Token DealTokenThenClear( string & tokenName ) { // ��tokenName���ͤ@��Token, �òM��tokenName
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

    // �p�G���wŪ�J���|���B�z���A���B�z
    if ( mNextToken != "" ) { // �w��Ū�Jtoken�S�Q�B�z
      token = DealTokenThenClear( mNextToken ) ;
      mPTEndLine = mLine ; // �O����token������line�b��
      done = true ;
    } // if

    // ���Lwhite space, ����Dwhite space��END-OF-FILE
    // �q�Dwhite space�}�l�~��Ū�J�A����separator
    while ( !done && !errorQL && !cin.eof() ) { // Ū�ɨS�����p�B�SŪ�쵲��

      if ( mNextChar == '\0' )
        ;
      else if ( CharCheck::IsWhiteSpace( mNextChar ) || cin.eof() ) { // white space
        if ( mNextToken != "" ) { // ����token
          token = DealTokenThenClear( mNextToken ) ;
          mPTEndLine = mLine ; // �O����token������line�b��
          done = true ;
        } // if

        if ( !done ) { // �ӳB�z��char��T
          column++ ;
          if ( mNextChar == '\n' ) {

            // �Y�b�W�@��S-exp�������٨S���F��A���n�p�⦹lineŪ�쪺
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
        if ( mNextChar == ';' ) { // comment �L�Ħr��
          // �J������u���B�z��
          while ( mNextChar != '\n' && !cin.eof() ) {
            mNextChar = cin.get();
          } // while

          // �Y�b�W�@��S-exp�������٨S���F��A���n�p�⦹lineŪ�쪺
          if ( isFirstT && mPTEndLine == mLine )
            line = 1 ;
          else
            line++ ;

          isFirstT = false ;
          column = 0 ;
          mLine++ ;

        } // if comment
        else if ( mNextToken != "" ) { // �e����token����
          token = DealTokenThenClear( mNextToken ) ;
          mPTEndLine = mLine ; // �O����token������line�b��
          done = true ;
        } // else if token����
        else if ( mNextChar == '\"' ) { // string
          column++ ;
          mNextToken = mNextChar ; // "

          mNextChar = cin.get() ; // Ū'���媺�Ĥ@��char
          while ( mNextChar != '\"' && mNextChar != '\n' && !cin.eof() ) {
            column++ ;
            mNextToken = mNextToken + mNextChar ;
            if ( mNextChar == '\\' ) { // �S��r��
              mNextChar = cin.get() ; // ����Ū�i�U�@�Ӧr��
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
            mPTEndLine = mLine ; // �O����token������line�b��
            done = true ;
          } // if
          else { // error ---- throw QuoteLost !
            mNextChar = '\0' ;
            mNextToken = "" ;
            errorQL = true ;
          } // else

        } // else if string
        else { // �e���S��token �B�zseparator
          column++ ;
          mNextToken = mNextChar ;
          mNextChar = '\0' ;
          token = DealTokenThenClear( mNextToken ) ;
          mPTEndLine = mLine ; // �O����token������line�b��
          done = true ;
        } // else
      } // else if separator
      else { // �Dwhite space && �Dseparator
        column++ ;
        mNextToken = mNextToken + mNextChar ;
      } // else

      if ( !done && !errorQL )
        mNextChar = cin.get() ;
    } // while

    if ( !done && mNextToken != "" ) {
      token = DealTokenThenClear( mNextToken ) ;
      mPTEndLine = mLine ; // �O����token������line�b��
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
    if ( mNextToken == "" ) { // �S���{��token
      token = GetToken( isFirstT, line, column ) ; // �O�sŪ�J���stoken
      mNextToken = token.name ; // �s�nmNextToken
    } // if
    else { // �w���{��token
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
      mScanner->GetToken( false, line, column ) ; // Ū�������D��token
      throw LPLost() ; // error ------------ throw LPLost
    } // else if
    else if ( curToken.type == LEFT_PAREN ) {
      if ( ATOM( line, column ) ) { // �b�̭��wGet LEFT_PAREN
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
            mScanner->GetToken( false, line, column ) ; // Ū�������D��token
            throw RPLost() ; // error ------------ throw RPLost
          } // else
        } // if
      } // else
    } // else if  LEFT_PAREN �}�Y

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
    int index = 0 ; // �Ψӫؾ𪺰Ѽ�
    // TreeEditor::DeleteTree( expr ) ;
    expr = NULL ;
    mTokens.clear() ;
    try {
      if ( S_EXP( true, line, column ) ) { // ���@�� S-Exp
        expr = TreeEditor::Build( mTokens, index, expr ) ; // �ؾ�
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
        ; // ����X�]���R��
      else {
        TreeEditor::PrintSExp( resultSExp ) ; // "pretty print" this data structure.
        
        TreeEditor::DeleteTree( inSExp ) ; 
      } // else 

    } // else

  } // while

  cout << "\nThanks for using OurScheme!" ;
} // main()

