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
#include "proj1.hpp"

using namespace std;

extern int choose_subtree_strategy;
extern int distance_method;
extern int using_rect_without_center;


int main(int argc, const char * argv[]) {
    // insert code here...
    
/*-------------------------R*TreeParamTest---------------------------*/
    
//    vector<pair<int, int> > params;
//    params.push_back(pair<int, int>(10, 2));
//    params.push_back(pair<int, int>(10, 3));
//    params.push_back(pair<int, int>(10, 4));
//    params.push_back(pair<int, int>(10, 5));
//    params.push_back(pair<int, int>(20, 5));
//    params.push_back(pair<int, int>(20, 8));
//    params.push_back(pair<int, int>(20, 10));
//    params.push_back(pair<int, int>(30, 10));
//    params.push_back(pair<int, int>(30, 15));
//    params.push_back(pair<int, int>(50, 10));
//    params.push_back(pair<int, int>(50, 20));
//    params.push_back(pair<int, int>(50, 25));
//    params.push_back(pair<int, int>(100, 50));
//    
//
//    Proj1Test<std::string, int, 9, double, 20, 8> test;
//    test.setFeaturePath("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/Color_Moment/color_moment_feature.txt");
//    test.setImageListPath("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/Color_Moment/imagelist.txt");
//    test.init();
//    test.MaxDiskAccessTestForExactMatch();

    
/*----------------------------Manual-Test----------------------------*/
    
    Proj1Test<std::string, double, 20, double, 10, 4> test;
    test.setFeaturePath("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/HOG/hog_sys_20_800600.txt");
    test.setImageListPath("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/HOG/imagelist.txt");
    test.init();
    
/* search performance test */
    
//    int p[] = {0, 1, 2, 3, 4, 5, 10, 20, 50, 100, 200, 500, 1000};
//    test.MaxDiskAccessTestForExactMatch();
//    int index = 0;
//    for (int i = 0; index < 13; i++) {
//        if (i == p[index]) {
//            test.MaxDiskAccessTestForExactMatch();
//            index++;
//        }
//        test.performRandomResinsert();
//    }
    
    
/* precision and recall test */
    
//    int p[] = {500, 200, 100, 50, 20, 5};
//    for (int i = 0; i < 6; i++) {
//        test.MaxPrecisionAndRecallNeighborSearchTest(p[i]);
//    }
    
/* query form file */
    
    vector<string> paths;
    vector<string> names;
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/bird_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/biulding_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/butterfly_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/flower_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/insect_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/piano_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/plane_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/watermill_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/wine_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/women_query.txt");
    paths.push_back("/Users/MarkLv/Documents/Tsinghua/2(2)/ADS/proj1/Features/queries/random_query.txt");
    names.push_back("bird");
    names.push_back("biulding");
    names.push_back("butterfly");
    names.push_back("flower");
    names.push_back("insect");
    names.push_back("piano");
    names.push_back("plane");
    names.push_back("watermill");
    names.push_back("wine");
    names.push_back("women");
    names.push_back("random");
    
    test.testQueryFromFile(paths[10], 11);
    
    
/* test for different 'ChooseSubtree method' and 'distance method' in R*tree */
    
//    for (int j = 1; j <= 3; j++) {
////        for (int k = 1; k <= 2; k++) {
//            choose_subtree_strategy = 1;
//            distance_method = j;
//            for (int i = 0; i < paths.size(); i++) {
//                cout << names[i] << "\t";
//                test.testQueryFromFile(paths[i], 500);
//                
//            }
//            cout << endl<<endl<<endl<<endl;
////        }
//    }

/* test for using hollow rect in R*tree */
    
//    for (int j = 1; j <= 2; j++) {
//        using_rect_without_center = j;
//
//        for (int i = 0; i < paths.size(); i++) {
//            cout << names[i] << "\t";
//            test.testQueryFromFile(paths[i], 500);
//        }
//    }
    
/* test for calculate max/min precision/recall for k-NN search */
    
//    test.MaxPrecisionAndRecallNeighborSearchTest(500);
    
    
    return 0;
}
