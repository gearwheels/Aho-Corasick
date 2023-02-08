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
    if (this->children.find(TERMINAL) == this->children.end()){ 
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

    if (this->itTTrie->children.empty()){ 
        this->itTTrie = itTTrie->CreateChilde(symbols); 
    }else{
        iter = this->itTTrie->children.find(symbols);
        if (iter != this->itTTrie->children.end()){   
        itTTrie = iter->second;
            
        }else{ 
            this->itTTrie = itTTrie->CreateChilde(symbols);
        }
    }
}
bool TTrie::Go(const std::string &symbols){
    std::unordered_map<std::string, TTrieNode *>::iterator iter;
    if(!this->itTTrie->children.empty()){ 
        iter = this->itTTrie->children.find(symbols);
        if (iter != this->itTTrie->children.end()){ 
            itTTrie = iter->second;
        }else{  
            while (iter == this->itTTrie->children.end() && itTTrie != this->root){ 
                itTTrie = itTTrie->linkFail;
                iter = this->itTTrie->children.find(symbols);
            }
            if (iter != this->itTTrie->children.end()){ 
                itTTrie = iter->second; 
            }else if (itTTrie == this->root){ 
                iter = this->itTTrie->children.find(symbols); 
                if (iter != this->itTTrie->children.end()){ 
                    itTTrie = iter->second; 
                }
            }
        }
    }else{ 
        if(itTTrie->linkFail) {
            itTTrie = itTTrie->linkFail;
        } 
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
        if(iter == itTTrie1->children.end()){ 
            itTTrie1 = itTTrie1->exitPoint;
            continue;
        }
        auto ptrVec = strWordVec.end();
        --ptrVec;
        unsigned long long pattLen = iter->second->numLengt->second;
        while (ptrVec->second < pattLen){ 
            pattLen -= ptrVec->second; 
            ptrVec--; 
        }
        auto namberWord = ptrVec->second;
        namberWord -= pattLen - 1;
        printf("%llu,%llu,%llu\n", ptrVec->first, namberWord, iter->second->numLengt->first);
        itTTrie1 = itTTrie1->exitPoint;
    }
}

void TTrie::Search(const std::vector<std::pair<unsigned long long int, unsigned long long int>> &strWordVec, const std::string &symbols){
    std::unordered_map<std::string, TTrieNode *>::iterator iter;
    if (Go(symbols)){ 
        this->Answer(strWordVec, iter);
    }
}


void TTrie::CreateLinks(){
    TTrieNode *tmpNode = root;
    std::queue<TTrieNode *> queue;
    for (auto it = this->root->children.begin(); it != this->root->children.end(); it++){ 
        queue.push(it->second);
        it->second->linkFail = root; 
    }
    while (!queue.empty()) {
        tmpNode = queue.front();
        queue.pop();
        std::unordered_map<std::string, TTrieNode *>::iterator iter; 
        for (iter = tmpNode->children.begin(); iter != tmpNode->children.end(); ++iter) {
            TTrieNode *child = iter->second;
            TTrieNode *parentFail = tmpNode->linkFail; 
            std::string childsymbols = iter->first; 
            queue.push(child);
            while (true){ 
                if (childsymbols != TERMINAL){
                    std::unordered_map<std::string, TTrieNode *>::iterator existingNode = parentFail->children.find(childsymbols); 
                    if(existingNode != parentFail->children.end()){ 
                        if (existingNode->second != child){
                            child->linkFail = existingNode->second; 
                            if (existingNode->second->children.find(TERMINAL) != existingNode->second->children.end()){ 
                                child->exitPoint = existingNode->second; 
                            }else{
                                if (existingNode->second->exitPoint){ 
                                    child->exitPoint = existingNode->second->exitPoint;
                                }
                            }
                        }else{ 
                            child->linkFail = root; 
                        }
                        break;
                    }
                }else{
                    break;
                }
                if (parentFail == root) { 
                    child->linkFail = root; 
                    break;
                } else { 
                    parentFail = parentFail->linkFail;
                }
            }
        }
    }
}
#endif
