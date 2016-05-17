//
//  proj1.hpp
//  RStar-TreeDemo
//
//  Created by 吕梦扬 on 16/5/17.
//  Copyright © 2016年 吕梦扬. All rights reserved.
//

#ifndef proj1_h
#define proj1_h

#include "RStarTree.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <array>

using namespace std;

template<class DATATYPE = std::string, class ELEMTYPE = int, int NUMDIMS = 9,class ELEMTYPEREAL = ELEMTYPE,int TMAXNODES = 8, int TMINNODES = TMAXNODES / 2>
class Proj1Test {
    string m_featurePath;
    string m_imageListPath;
    RStarTree<DATATYPE, ELEMTYPE, NUMDIMS, RSTAR_TREE_USING_MBR, ELEMTYPEREAL, TMAXNODES, TMINNODES> *t;
    map<std::string, std::array<ELEMTYPE, NUMDIMS> > features;
    map<std::string, int> imageTags;
    
public:
    Proj1Test() : t(NULL){}
    Proj1Test(string fPath, string iPath) : m_featurePath(fPath), m_imageListPath(iPath), t(NULL) {}
    void setFeaturePath(string path) {m_featurePath = path;}
    void setImageListPath(string path) {m_imageListPath = path;}
    void init() {
        
        if (!t) {
            t = new RStarTree<DATATYPE, ELEMTYPE, NUMDIMS, RSTAR_TREE_USING_MBR, ELEMTYPEREAL, TMAXNODES, TMINNODES>;
        } else {
            t->removeAll();
        }
        imageTags.clear();
        features.clear();
        
        fstream fef(m_featurePath);
        fstream imf(m_imageListPath);
        if (!fef.is_open() || !imf.is_open()) {
            cout << "error loading files\n";
            return;
        }
        int dim;
        fef >> dim;
        string tempstr;
        while (!fef.eof() && !imf.eof()) {
            ELEMTYPE temp[NUMDIMS];
            for (int i = 0; i < NUMDIMS; i++) {
                fef >> temp[i];
            }
            getline(imf, tempstr);
            if ((*(tempstr.end() - 1)) == '\r') {
                tempstr.erase(tempstr.end() - 1);
            }
            t->Insert(temp, temp, tempstr);
            proceedImageTag(tempstr);
            proceedImageFeature(tempstr, temp);
        }
    }
    int getDataCount() {return t->count();}
protected:
    string getImageTag(string imageName) {
        string::iterator it = imageName.begin();
        while ((*it) != '_') {
            it++;
        }
        string r;
        r.assign(imageName.begin(), it);
        return r;
    }
    void proceedImageTag(string imageName) {
        string tag = getImageTag(imageName);
        map<std::string, int>::iterator it = imageTags.find(tag);
        if (it == imageTags.end()) {
            imageTags[tag] = 1;
        } else {
            (*it).second += 1;
        }
    }
    void proceedImageFeature(string imageName, ELEMTYPE feature[NUMDIMS]) {
        std::array<ELEMTYPE, NUMDIMS> a;
        for (int i = 0; i < NUMDIMS; i++) {
            a[i] = feature[i];
        }
        features[imageName] = a;
    }
};


#endif /* proj1_h */
