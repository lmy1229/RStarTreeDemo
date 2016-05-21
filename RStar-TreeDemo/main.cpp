//
//  main.cpp
//  RStar-TreeDemo
//
//  Created by 吕梦扬 on 16/5/10.
//  Copyright © 2016年 吕梦扬. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include "RStarTree.hpp"
#include "proj1.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    // insert code here...
    
//    RStarTree<std::string, int, 9, RSTAR_TREE_USING_MBR | RSTAR_TREE_NO_NEED_TO_RELEASE_DATA, double, 10, 4> t;
////
//    ifstream inf("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/Color_Moment/color_moment_feature.txt");
//    ifstream datainf("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/Color_Moment/imagelist.txt");
//    assert(inf.is_open() && datainf.is_open());
//    int m;
//    inf>>m;
//    while (!inf.eof()) {
//        int temp[9];
//        for (int i = 0; i < 9; i++) {
//            inf>>temp[i];
//        }
//        string a;
//        getline(datainf, a);
//        t.Insert(temp, temp, a);
//    }
//
//    int searchl[9] = {7315, 7328, 8919, 3086, 1739, 1572, 3794, 2097, 2402};
////    int searchl[9] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
//    int searchr[9] = {10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000};
//
//    vector<string> result;
//    
//    int count = t.Search(searchl, searchl, result, SEARCH_FLAG_INTERSECT);
//    cout<<count<<endl<<result.size()<<endl;
    
/*------------------------------HOG_NEW------------------------------*/
    
    Proj1Test<std::string, double, 20, double, 10, 4> test;
    test.setFeaturePath("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/HOG/hog_20_new.txt");
    test.setImageListPath("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/HOG/imagelist.txt");
    test.init();
    test.MaxDiskAccessTestForExactMatch();
    for (int i = 0; i < 100; i++) {
        for (int i = 0; i < 10; i++) {
            test.performRandomResinsert();
        }
        test.MaxDiskAccessTestForExactMatch();
    }
//    Proj1Test<std::string, double, 20, double, 10, 4>::neighborSearchParam param;
//    cout << test.neighborSearchTest("n03767203_4464.JPEG", 500, param)<<endl;

//    test.MaxPrecisionAndRecallNeighborSearchTest(500);
    
/*-----------------------------ColorMoment----------------------------*/
    
//    Proj1Test<std::string, int, 9, double, 10, 4> test;
//    test.setFeaturePath("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/Color_Moment/color_moment_feature.txt");
//    test.setImageListPath("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/Color_Moment/imagelist.txt");
//    test.init();
//    test.MaxDiskAccessTestForExactMatch();
//    test.performRandomResinsert();
//    test.MaxDiskAccessTestForExactMatch();
//    test.performRandomResinsert();
//    test.MaxDiskAccessTestForExactMatch();
//    test.performRandomResinsert();
//    test.MaxDiskAccessTestForExactMatch();
//    test.performRandomResinsert();
//    test.MaxDiskAccessTestForExactMatch();
//    test.performRandomResinsert();
//    test.MaxDiskAccessTestForExactMatch();
//    test.performRandomResinsert();
//    test.MaxDiskAccessTestForExactMatch();
//    for (int i = 0; i < 100; i++) {
//        for (int i = 0; i < 10; i++) {
//            test.performRandomResinsert();
//        }
//        test.MaxDiskAccessTestForExactMatch();
//    }
//    Proj1Test<std::string, int, 9, double, 10, 4>::neighborSearchParam param;
//    cout << test.neighborSearchTest("n03767203_4464.JPEG", 500, param)<<endl;
//    test.MaxPrecisionAndRecallNeighborSearchTest(500);
    
    return 0;
}
