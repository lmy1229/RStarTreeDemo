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

protected:
    string m_featurePath;
    string m_imageListPath;
    RStarTree<DATATYPE, ELEMTYPE, NUMDIMS, RSTAR_TREE_USING_MBR | RSTAR_TREE_NO_NEED_TO_RELEASE_DATA, ELEMTYPEREAL, TMAXNODES, TMINNODES> *t;
    map<std::string, std::array<ELEMTYPE, NUMDIMS> > features;
    map<std::string, int> imageTags;
    
public:
    struct neighborSearchParam {
        string imagename;
        int count;
        double precision;
        double recall;
        vector<DATATYPE> searchResult;
        
        void print() {
            cout << imagename << '\t' << precision << '\t' << recall <<endl;
        }
    };
    
public:
    Proj1Test() : t(NULL){}
    Proj1Test(string fPath, string iPath) : m_featurePath(fPath), m_imageListPath(iPath), t(NULL) {}
    void setFeaturePath(string path) {m_featurePath = path;}
    void setImageListPath(string path) {m_imageListPath = path;}
    void init() {
        
        if (!t) {
            t = new RStarTree<DATATYPE, ELEMTYPE, NUMDIMS, RSTAR_TREE_USING_MBR | RSTAR_TREE_NO_NEED_TO_RELEASE_DATA, ELEMTYPEREAL, TMAXNODES, TMINNODES>;
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
            ELEMTYPE temp2[NUMDIMS];
            for (int i = 0; i < NUMDIMS; i++) {
                fef >> temp[i];
                temp2[i] = temp[i] + 1;
            }
            getline(imf, tempstr);
            if ((*(tempstr.end() - 1)) == '\r') {
                tempstr.erase(tempstr.end() - 1);
            }
            t->Insert(temp, temp2, tempstr);
            proceedImageTag(tempstr);
            proceedImageFeature(tempstr, temp);
        }
    }
    void performRandomResinsert() {
        t->randomReinsert();
    }
    int getDataCount() {return t->count();}
    int MaxDiskAccessTestForExactMatch() {
        typename std::map<std::string, std::array<ELEMTYPE, NUMDIMS> >::iterator it = features.begin();
        int maxAccess = 0;
        string maxString = "";
        
        int totalAccess = 0;
        
        while (it != features.end()) {
            string name = (*it).first;
            int r = ExactMatch(name);
            totalAccess += r;
            if (r > maxAccess) {
                maxAccess = r;
                maxString = name;
            }
            it++;
        }
        cout << maxString << ": " << maxAccess << endl;
        cout << "average: " << totalAccess / t->count()<<endl;
        return maxAccess;
    }
    void MaxPrecisionAndRecallNeighborSearchTest(int count) {
        typename std::map<std::string, std::array<ELEMTYPE, NUMDIMS> >::iterator it = features.begin();
        string maxString = "";
        
        double maxp = 0, maxr = 0;
        string maxpName, maxrName;
        double minp = 10, minr = 10;
        string minpName, minrName;
        
        double avp = 0, avr = 0;
        
        while (it != features.end()) {
            string name = (*it).first;
            neighborSearchParam p;
            p.imagename = name;
            p.count = count;
            int r = neighborSearchTest(name, count, p);
            
            avp += p.precision;
            avr += p.recall;
            
            if (p.precision > maxp) {
                maxp = p.precision;
                maxpName = name;
            }
            if (p.recall > maxr) {
                maxr = p.recall;
                maxrName = name;
            }
            
            if (p.precision < minp) {
                minp = p.precision;
                minpName = name;
            }
            if (p.recall < minr) {
                minr = p.recall;
                minrName = name;
            }
//            p.print();
            it++;
        }
        
        cout << "max precision:" << maxp << "\t"<<maxpName <<endl << "max recall:" << maxr << "\t"<<maxrName<<endl;
        cout << "min precision:" << minp << "\t"<<minpName <<endl << "min recall:" << minr << "\t"<<minrName<<endl;
        cout << "avp: " << avp / (double)getDataCount() << "avr: " << avr / (double)getDataCount()<<endl;
    }
    int neighborSearchTest(string name, int count, neighborSearchParam &param) {
        typename map<std::string, std::array<ELEMTYPE, NUMDIMS> >::iterator it;
        it = features.find(name);
        string tag = getImageTag(name);
        param.imagename = name;
        param.count = count;
        if (it != features.end()) {
            std::array<ELEMTYPE, NUMDIMS> f = (*it).second;
            ELEMTYPE feature[NUMDIMS];
            
            for (int i = 0; i < NUMDIMS; i++) {
                feature[i] = f[i];
            }
            vector<DATATYPE>().swap(param.searchResult);
            int diskAccessCount = t->NeighborSearch(feature, count, param.searchResult);
            
            int correctCount = 0;
            for (int i = 0; i < param.searchResult.size(); i++) {
                string ttag = getImageTag(param.searchResult[i]);
                if (ttag == tag) {
                    correctCount++;
                }
            }
            
            param.precision = (double)correctCount / (double)param.searchResult.size();
            param.recall = (double)correctCount / (double)imageTags[tag];
            
            return diskAccessCount;
        } else {
            return -1;
        }
    }
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
    int ExactMatch(string imageName) {
        typename std::map<std::string, std::array<ELEMTYPE, NUMDIMS> >::iterator it;
        it = features.find(imageName);
        if (it == features.begin()) {
            return 0;
        } else {
            array<ELEMTYPE, NUMDIMS> a = (*it).second;
            ELEMTYPE query[NUMDIMS];
            for (int i = 0; i < NUMDIMS; i++) {
                query[i] = a[i];
            }
            vector<DATATYPE> result;
            int r = t->Search(query, query, result);
//            cout << imageName << ": " << r << endl;
            return r;
        }
    }
};
#endif /* proj1_h */
