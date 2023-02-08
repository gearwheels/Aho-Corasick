#include "TTrie.h"

int main(){
    char buff[MAX_WORD_SIZE];
    bool space = false;
    unsigned long long len = 1; 
    unsigned long long numStr = 0;
    TTrie trie;
    unsigned short i = 0;

    char c = tolower(getchar());
    memset(buff, 0, MAX_WORD_SIZE);
    while (c > 0) { 
        while (c == '\t' || c == ' '){ 
            space = true;
            c = tolower(getchar());
        }
        i = 0;
        while(c >= 'a' && c <='z'){ 
            space = false;
            buff[i] = c;
            ++i;
            c = tolower(getchar());
        }
        if (c == '\t' || c == ' '){
            space = true;
            if(buff[0] != '\0'){
                trie.Create(buff);
            }
            ++len;
            memset(buff, 0, MAX_WORD_SIZE);
        }else if (c == '\n'){ 
            if(buff[0] != '\0'){
                trie.Create(buff);
            }
            ++numStr;
            if (space){
                --len;
            }
            trie.GetIter()->nonTerminal(numStr, len);
            memset(buff, 0, MAX_WORD_SIZE);
            trie.ResetItTTrie();
            len = 1;
            c = tolower(getchar());
            if (c == '\n') {
                trie.ResetItTTrie();
                break;
            }else{
                continue;
            }
        }else{
            std::cerr << "ERROR: input data (pattern).\n";
            return 0;
        }
        c = tolower(getchar());
        
    }

    trie.CreateLinks();

    std::vector<std::pair<unsigned long long int, unsigned long long int>> strWordVec; // вектор пар (номер строки, кол-во слов в строке)
    std::pair<unsigned long long int, unsigned long long int> strWordPair;
    strWordPair = std::make_pair<unsigned long long int, unsigned long long int>(1,0); // пара (номер строки, кол-во слов в строке)
    strWordVec.push_back(strWordPair);
    c = tolower(getchar());

    while (c > 0) { 
        while (c == '\t' || c == ' '){ 
            space = true;
            c = tolower(getchar());
        }
        i = 0;
        while(c >= 'a' && c <='z'){ 
            space = false;
            buff[i] = c;
            ++i;
            c = tolower(getchar());
        }
        if (c == '\t' || c == ' '){ 
            space = true;
            if(buff[0] != '\0'){ 
                ++strWordVec.back().second; 
                trie.Search(strWordVec, buff); // поиск
                memset(buff, 0, MAX_WORD_SIZE);
            }
        }else if (c == '\n'){
            if(buff[0] != '\0'){ 
                if (!space){  
                    ++strWordVec.back().second; // счетчик слов в строке
                }
                trie.Search(strWordVec, buff);// поиск
                memset(buff, 0, MAX_WORD_SIZE);
            }
            ++strWordPair.first; // счетчик строк 
            strWordVec.push_back(strWordPair); // добавляем новую пару в вектор 
        }else{
            std::cerr << "ERROR: input data (text).\n";
            return 0;
        }
        c = tolower(getchar());
    }
    return 0;
}

