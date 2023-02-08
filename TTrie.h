#ifndef TTRIE_H
#define TTRIE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <string>
#include <cstring>
#include <queue>
#include <new>
#include <ctime>

const unsigned short MAX_WORD_SIZE = 17;

const std::string TERMINAL = "$";

class TTrieNode{
public:
    friend class TTrie;
    TTrieNode(){
        numLengt = nullptr ; 
        linkFail = nullptr;
        exitPoint = nullptr;
    }
    void nonTerminal (unsigned long long numStr, unsigned long long len);
    ~TTrieNode() {
    };
private:
    void myFree();
    TTrieNode* CreateChilde(const std::string simbol);
    std::unordered_map<std::string, TTrieNode *> children;// еще будем хранить $ чтобы знать что закончилось слово 
    TTrieNode *linkFail;
    TTrieNode *exitPoint;
    std::pair<unsigned long long, unsigned long long> *numLengt;
};

void TTrieNode::myFree(){
    if (!this->children.empty()){
        for (std::unordered_map<std::string, TTrieNode *>::iterator iter = this->children.begin(); iter != this->children.end(); ++iter){
            if (iter->second != nullptr){
                iter->second->myFree();
                delete iter->second;
            }
        }
            this->children.clear();
    }
    delete this->numLengt;
    this->numLengt = nullptr;
    this->linkFail = nullptr;
    this->exitPoint = nullptr;
}

TTrieNode* TTrieNode::CreateChilde(const std::string simbol){
    auto tmpPair = std::make_pair(simbol,new TTrieNode);
    this->children.insert(tmpPair);
    return tmpPair.second;
    
}

void TTrieNode::nonTerminal(unsigned long long numStr, unsigned long long len){
    if (this->children.find(TERMINAL) == this->children.end()){ // если нет терм символа создаем его 
        std::pair<std::string, TTrieNode *> tmpPair = std::make_pair(TERMINAL,new TTrieNode);
        std::pair<unsigned long long, unsigned long long> *tmpPairInt = new std::pair<unsigned long long, unsigned long long>;
        *tmpPairInt = std::make_pair(numStr, len);
        tmpPair.second->numLengt = tmpPairInt;
        this->children.insert(tmpPair);
    }
}

class TTrie{
public:
    TTrie(){
        this->root = new TTrieNode;
        itTTrie = this->root;
    }
    void Create(const std::string &symbols);
    void Search(const std::vector<std::pair<unsigned long long int, unsigned long long int>> &strWordVec, const std::string &symbols);
    void CreateLinks();
    void ResetItTTrie ();
    ~TTrie() {
        this->root->myFree();
        this->itTTrie = nullptr; 
        delete this->root;
    };
    TTrieNode* GetIter(){
        return this->itTTrie;
    }
private:
    TTrieNode *root;
    TTrieNode *itTTrie;
    bool Go(const std::string & symbols);
    void Answer(const std::vector<std::pair<unsigned long long int, unsigned long long int>> &strWordVec, std::unordered_map<std::string,TTrieNode *>::iterator &iter);
};

void TTrie::ResetItTTrie(){
    itTTrie = this->root;
}

void TTrie::Create(const std::string &symbols){

    std::unordered_map<std::string, TTrieNode *>::iterator iter;

    if (this->itTTrie->children.empty()){ // если нода пуста то добавляем к ней в дети входящий символы 
        this->itTTrie = itTTrie->CreateChilde(symbols); 
    }else{
        iter = this->itTTrie->children.find(symbols);
        if (iter != this->itTTrie->children.end()){ // если в табл детей нашли ребенка с такими же символами  
        itTTrie = iter->second;
            // то переставляем итератор на него 
        }else{ // иначе добавляем ребенка и переводим итератор на него 
            this->itTTrie = itTTrie->CreateChilde(symbols);
        }
    }
}
bool TTrie::Go(const std::string &symbols){
    std::unordered_map<std::string, TTrieNode *>::iterator iter;
    if(!this->itTTrie->children.empty()){// если есть дети 
        iter = this->itTTrie->children.find(symbols); //ищем строку переданную в ф-ию 
        if (iter != this->itTTrie->children.end()){ // если находим
            itTTrie = iter->second; // передвигаем итератор на нее 
        }else{ // если нет такой строки 
            // то нужно передвинуть итератор на linkfail 
            while (iter == this->itTTrie->children.end() && itTTrie != this->root){ // проходим по всем linkFail либо находим заданную строку либо доходим до корня
                itTTrie = itTTrie->linkFail;
                iter = this->itTTrie->children.find(symbols);
            }
            if (iter != this->itTTrie->children.end()){ // 1) находим заданный символ
                itTTrie = iter->second; // передвигаем итератор на него
            }else if (itTTrie == this->root){ // 2) доходим до корня
                iter = this->itTTrie->children.find(symbols); // ищем в нем заданный символ
                if (iter != this->itTTrie->children.end()){ //  находим заданный символ
                    itTTrie = iter->second; // передвигаем итератор на него
                }
            }
        }
    }else{ // если нет детей, хотя  скорее всего будет хотя бы $
        // можно поставить итератор на linkfail
        if(itTTrie->linkFail) {
            itTTrie = itTTrie->linkFail;
        } // если itTTrie->linkFail == nullptr значит это либо корень 
        // либо $ но его мы не сможем получить т к он не из словаря 
    }
    if (itTTrie == this->root){
        return false;
    }else{
        return true;
    }
}

void TTrie::Answer(const std::vector<std::pair<unsigned long long int, unsigned long long int>> &strWordVec, std::unordered_map<std::string, TTrieNode *>::iterator &iter){
    auto itTTrie1 = this->itTTrie;
    while (itTTrie1){
        iter = itTTrie1->children.find(TERMINAL);
        if(iter == itTTrie1->children.end()){ // если итератор на последней букве слова и эта буква не является терминальной вершиной
            itTTrie1 = itTTrie1->exitPoint;
            continue;
        }
        auto ptrVec = strWordVec.end();
        --ptrVec;// итератор на вектор номер строки и коичество слов 
        unsigned long long pattLen = iter->second->numLengt->second;
        while (ptrVec->second < pattLen){ // пока взятый нами член вектора
            // по второму полю (кол во слов в строке) меньше длины паттерна 
            pattLen -= ptrVec->second; //отнимаем длину строки от длины патерна 
            ptrVec--; // перемещаем итератор назад 
        }
        auto namberWord = ptrVec->second;
        namberWord -= pattLen - 1;
        printf("%llu,%llu,%llu\n", ptrVec->first, namberWord, iter->second->numLengt->first);
        itTTrie1 = itTTrie1->exitPoint;
    }
}

void TTrie::Search(const std::vector<std::pair<unsigned long long int, unsigned long long int>> &strWordVec, const std::string &symbols){
    std::unordered_map<std::string, TTrieNode *>::iterator iter;
    if (Go(symbols)){ // если Go true то itTTrie != root 
        this->Answer(strWordVec, iter);
    }
}


void TTrie::CreateLinks(){
    TTrieNode *tmpNode = root;
    std::queue<TTrieNode *> queue;
    for (auto it = this->root->children.begin(); it != this->root->children.end(); it++){ // инициализирунм очередь первыми членами бора
        queue.push(it->second);
        it->second->linkFail = root; 
    }
    while (!queue.empty()) {// проходим по очереди
        tmpNode = queue.front();
        queue.pop();
        std::unordered_map<std::string, TTrieNode *>::iterator iter; // итератор для прохода по unordered_map 
        for (iter = tmpNode->children.begin(); iter != tmpNode->children.end(); ++iter) {
            TTrieNode *child = iter->second; // узел ребенка 
            TTrieNode *parentFail = tmpNode->linkFail; // узел на который ссылкой неудач указывает родительский узел 
            std::string childsymbols = iter->first; // символ узла ребенка 
            queue.push(child);
            while (true){ // смотрим ссылки неудач и точки выходов узлов с идентичным символом
                if (childsymbols != TERMINAL){
                    std::unordered_map<std::string, TTrieNode *>::iterator existingNode = parentFail->children.find(childsymbols); // проверяем есть ли символ ребенка в родительской ссылке неудач
                    if(existingNode != parentFail->children.end()){ // если есть 
                        if (existingNode->second != child){ // если это не тот же самый узел 
                            child->linkFail = existingNode->second; // делаем ссылку неудач на него
                            if (existingNode->second->children.find(TERMINAL) != existingNode->second->children.end()){ // проверяем не является ли найденый символ концом слова
                                child->exitPoint = existingNode->second; // если является делаем ссылку выхода на него
                            }else{ // если не является концом слова
                                if (existingNode->second->exitPoint){ // проверяем есть ли у него ссылка на конец слова
                                    child->exitPoint = existingNode->second->exitPoint;// если есть ссылаемся на нее
                                }
                            }
                        }else{ // если это тот же самый узел 
                            child->linkFail = root; // делаем ссылку неудач на корень 
                        }
                        break;
                    }
                }else{
                    break;
                }
                if (parentFail == root) { // если ссылка неудач родителя была на корень 
                    child->linkFail = root; // делаем ссылку неудач ребенка на корень 
                    break;
                } else { // иначе переходим по ссылке неудач родителя и ищем символ ребенка там
                    // это происходит если мы не нашли символ ребенка в детях по ссылке неудач и при этом у родителя есть активная ссылка (не nullptr) неудач
                    parentFail = parentFail->linkFail;
                }
            }
        }
    }
}
#endif
