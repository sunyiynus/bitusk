
#include <stack>

#include "bcode.hpp"

using namespace bitusk;

string Bcode::extractSegment(position& pos, const char de)
{
    string tmp;
    try{
        while (*pos != de) {
            tmp.push_back(*pos++);
        }
		pos++;
    } catch ( exception& ec){
	}
    return tmp;
}


int Bcode::parseInt(position& pos)
{
		string tmp;
		int result = 0;
		tmp = extractSegment(pos, 'e');
		try{
				result = stoi(tmp);
		} catch (exception& ec){
		}
		return result;
}


string Bcode::parseStr(position& pos)
{
    string tmp;
    int str_len;
    try{
        tmp = extractSegment(pos, ':');
        str_len = stoi(tmp);
        tmp.clear();
        tmp = string(pos,pos + str_len);
    } catch (exception& ec){
    }

    pos+=str_len;
    return tmp;
}


bool Bcode::findPair(std::string::iterator& pos) {
  std::stack<char> stk;
  stk.push(*pos++);
  while( !stk.empty()) {
    switch (*pos) {
      case 'l':
          findPair(pos);
          break;
      case 'd':
          findPair(pos);
          break;
      case 'i':
          parseInt(++pos);
          break;
      case 'e':
          stk.pop();
          pos++;
          break;
      default:
          if( isdigit(*pos) ) {
            parseStr( pos );
            break;
          }
          break;
    }
  }
  return true;
}


