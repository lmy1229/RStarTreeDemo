//
//  RStarTree.hpp
//  RStar-TreeDemo
//
//  Created by 吕梦扬 on 16/5/10.
//  Copyright © 2016年 吕梦扬. All rights reserved.
//

#ifndef RStarTree_hpp
#define RStarTree_hpp

#include <stdio.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <cmath>

//#define NDEBUG
#include <assert.h>

using namespace std;

#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a > b ? b : a)
#define MAX_INDEX(a, b) (a > b ? 1 : 2)
#define MIN_INDEX(a, b) (a > b ? 2 : 1)

#define RSTAR_TREE_TEMPLETE template<class DATATYPE, class ELEMTYPE, int NUMDIMS, int FLAG, class ELEMTYPEREAL, int TMAXNODES, int TMINNODES>
#define RSTAR_TREE_QUAL RStarTree<DATATYPE, ELEMTYPE, NUMDIMS, FLAG, ELEMTYPEREAL, TMAXNODES, TMINNODES>

#define ACCESS_NODE(count) count++;

#define RSTAR_TREE_USING_MBR 1
#define RSTAR_TREE_USING_SPHERICAL 2
#define RSTAR_TREE_NO_NEED_TO_RELEASE_DATA 4

#define SEARCH_FLAG_EXACTLY 1
#define SEARCH_FLAG_INTERSECT 2
#define SEARCH_FLAG_COVER_QUERY 3
#define SEARCH_FLAG_COVERED_BY_QUERY 4
#define SEARCH_FLAG_NEAREST_DATA(count) (count+5)
#define SEARCH_NORMAL_FLAG(f) ((f >= 1) && (f <= 4))
#define VALIDATE_FLAG(f) (f >= 1)

#define RSTAR_TREE_REINSERT_P 0.30

template<class DATATYPE = double, class ELEMTYPE = int, int NUMDIMS = 9,int FLAG = RSTAR_TREE_USING_MBR,class ELEMTYPEREAL = ELEMTYPE,int TMAXNODES = 8, int TMINNODES = TMAXNODES / 2>
class RStarTree {
    
#pragma mark - DEFINITIONS
    
public:
    struct generalNode;
    struct Node;
    struct Rect;
    void (*release)(DATATYPE);
    
protected:
    struct LinkedListNode;
    struct leafNode;
    struct bindingData;
    
    Node *root;
    ELEMTYPEREAL unitSphericalVolume;
    int nodeCount;
    
    enum {
        MAXNODE = TMAXNODES,
        MINNODE = TMINNODES,
    };
    
public:
    RStarTree();
    ~RStarTree();
    
#pragma mark Interfaces
    void Insert(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], const DATATYPE& data);
    void Delete(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], int flag);
    int Search(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], int flag, vector<DATATYPE> &rData);
    int NeighborSearch(const ELEMTYPE center[NUMDIMS], int searchCount, vector<DATATYPE> &rData);
    void removeAll();
    int count();

protected:
    
#pragma mark Sorting Functors
    struct SortByLeftEdge : public std::binary_function<const Node * const, const Node * const, bool> {
        const int m_axis;
        explicit SortByLeftEdge (const int axis) : m_axis(axis) {}
        
        bool operator() (const Node * const n1, const Node * const n2) const {
            return n1->boundingRect.mins[m_axis] < n2->boundingRect.mins[m_axis];
        }
    };
    struct SortByRightEdge : public std::binary_function<const Node * const, const Node * const, bool> {
        const int m_axis;
        explicit SortByRightEdge (const int axis) : m_axis(axis) {}
        
        bool operator() (const Node * const n1, const Node * const n2) const {
            return n1->boundingRect.maxs[m_axis] < n2->boundingRect.maxs[m_axis];
        }
    };
    struct SortByLeftThenRightEdge : public std::binary_function<const Node * const, const Node * const, bool> {
        const int m_axis;
        explicit SortByLeftThenRightEdge (const int axis) : m_axis(axis) {}
        
        bool operator() (const Node * const n1, const Node * const n2) const {
            if (n1->boundingRect.mins[m_axis] < n2->boundingRect.mins[m_axis]) {
                return true;
            } else if (n1->boundingRect.mins[m_axis] == n2->boundingRect.mins[m_axis]) {
                return n1->boundingRect.maxs[m_axis] < n2->boundingRect.maxs[m_axis];
            } else {
                return false;
            }
        }
    };
    struct SortByAreaEnlargement : public std::binary_function<const Node * const, const Node * const, bool> {
        Rect center;
        explicit SortByAreaEnlargement (const Rect &rect) {rectCopy(center, rect);}
        
        bool operator() (const Node * const n1, const Node * const n2) const {
            ELEMTYPEREAL v1 = rectangularVolume(n1->boundingRect);
            ELEMTYPEREAL v2 = rectangularVolume(n2->boundingRect);
//            return (rectVolume(boundingRect(n1->boundingRect, center)) - v1) < (rectVolume(boundingRect(n2->boundingRect, center)) - v2);
            ELEMTYPEREAL dv1 = rectangularVolume(boundingRect(n1->boundingRect, center)) - v1;
            ELEMTYPEREAL dv2 = rectangularVolume(boundingRect(n2->boundingRect, center)) - v2;
            if (dv1 < dv2) {
                return true;
            } else if (dv1 == dv2) {
                return (dv1 + v1) < (dv2 + v2);
            } else {
                return false;
            }
        }
    };
    struct SortByDistanceFromCenter : public std::binary_function<const Node * const, const Node * const, bool> {
        Rect center;
        explicit SortByDistanceFromCenter (const Rect &newRect) {rectCopy(center, newRect);}
        
        bool operator() (const Node * const n1, const Node * const n2) const {
            return squaredDistanceBetweenRects(n1->boundingRect, center) < squaredDistanceBetweenRects(n2->boundingRect, center);
        }
    };
    struct SortByOverlapEnlargement : public std::binary_function<const Node * const, const Node * const, bool> {
        Rect center;
        explicit SortByOverlapEnlargement (const Rect &newRect) {center.copyFrom(newRect);}
        
        bool operator() (const Node * const n1, const Node * const n2) const {
            return intersectVolume(n1->boundingRect, center) > intersectVolume(n2->boundingRect, center);
        }
    };
    
#pragma mark utilities
protected:
    Node* newNode();
    leafNode* newLeaf(const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS], const DATATYPE &data);
    void initRect(Rect &rect, const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS]);
    void initRectWithCenterAndRadius(Rect &rect, const ELEMTYPE center[NUMDIMS], ELEMTYPE r);
    static int cover(const Rect &rect1, const Rect &rect2);
    static bool isRectsEqual(const Rect &rect1, const Rect &rect2);
    static bool intersect(const Rect &rect1, const Rect &rect2);
    static bool intersectWithoutCenter(const Rect &rect1, const Rect &rect1Center, const Rect &rect2);
    static ELEMTYPEREAL intersectVolume(const Rect &rect1, const Rect &rect2);
    static ELEMTYPEREAL overlapsVolume(Node *node, int index);
    static Rect boundingRect(const Rect &rect1, const Rect &rect2);
    static void rectCopy(Rect &dest, const Rect src);
    static ELEMTYPEREAL rectangularVolume(const Rect &rect);
    ELEMTYPEREAL sphericalVolume(const Rect &rect);
    ELEMTYPEREAL rectVolume(const Rect &rect);
    static ELEMTYPE rectMargin(const Rect &rect);
    static ELEMTYPE squaredDistanceBetweenRects(const Rect &rect1, const Rect &rect2);
    void resizeNode(Node *node);

#pragma mark insert
    Node* chooseSubtree(leafNode *lnode, Node *node);
    Node* insertInternalNode(leafNode *lnode, Node *node, bool firstInsert = true);
    Node* OverflowTreatment(Node *level, bool firstInsert);
    Node* splitNode(Node *node);
    void reinsert(Node *node);
#pragma mark remove
#pragma mark search
    bool accept(const Rect &query, const Rect &rect, int flag);
    void searchInternalNode(Node *node, const Rect &query, vector<pair<DATATYPE, ELEMTYPE> > &result, int flag, int &diskAccessCount);
    void searchInternalNodeWithoutCenter(Node *node, const Rect &query, const Rect &queryCenter, vector<pair<DATATYPE, ELEMTYPE> > &result, int &diskAccessCount);
#pragma mark removeAll
    void removeAllNode(Node *node);
    void removeLeaf(leafNode *node);
#pragma mark count
    void countNode(const Node *node , int &c);
    
public:
    
    struct generalNode {
        Rect boundingRect;
    };
    
    struct Node: public generalNode {
        int childCount() {return (int)childs.size();}
        int level;  // level of this node, 0 for leaf node
        vector<Node*> childs;
    };
    
    struct Rect {
        ELEMTYPE mins[NUMDIMS];
        ELEMTYPE maxs[NUMDIMS];
        
        Rect() {
            reset();
        }
        
        void stretch(const Rect &rect) {
            for (int i = 0; i < NUMDIMS; i++) {
                mins[i] = MIN(mins[i], rect.mins[i]);
                maxs[i] = MAX(maxs[i], rect.maxs[i]);
            }
        }
        
        void reset() {
            for (int i = 0; i < NUMDIMS; i++) {
                mins[i] = maxs[i] = 0;
            }
        }
        
        void copyFrom(const Rect &rect) {
            for (int i = 0; i < NUMDIMS; i++) {
                maxs[i] = rect.maxs[i];
                mins[i] = rect.mins[i];
            }
        }
    };
protected:
    struct leafNode: public generalNode {
        DATATYPE data;
    };
    
    struct LinkedListNode {
        Node *node;
        LinkedListNode *next;
        LinkedListNode *prev;
    };
    
    
};

#pragma mark - IMPLEMENTATIONS

RSTAR_TREE_TEMPLETE
RSTAR_TREE_QUAL::RStarTree() {
    assert(MAXNODE > MINNODE);
    assert(MINNODE > 0);
    
    // Precomputed volumes of the unit spheres for the first few dimensions
    const float UNIT_SPHERE_VOLUMES[] = {
        0.000000f, 2.000000f, 3.141593f, // Dimension  0,1,2
        4.188790f, 4.934802f, 5.263789f, // Dimension  3,4,5
        5.167713f, 4.724766f, 4.058712f, // Dimension  6,7,8
        3.298509f, 2.550164f, 1.884104f, // Dimension  9,10,11
        1.335263f, 0.910629f, 0.599265f, // Dimension  12,13,14
        0.381443f, 0.235331f, 0.140981f, // Dimension  15,16,17
        0.082146f, 0.046622f, 0.025807f, // Dimension  18,19,20
    };
    
    root = newNode();
    unitSphericalVolume = (ELEMTYPEREAL)UNIT_SPHERE_VOLUMES[NUMDIMS];
    
    nodeCount = 0;
}

RSTAR_TREE_TEMPLETE
RSTAR_TREE_QUAL::~RStarTree() {
    
    removeAll();
    nodeCount = 0;
    delete root;
    root = NULL;
}

#pragma mark Utilities

RSTAR_TREE_TEMPLETE
typename RSTAR_TREE_QUAL::Node* RSTAR_TREE_QUAL::newNode() {
    Node *node = new Node;
    
    assert(node);
    
    node->level = 0;
    for (int i = 0; i < NUMDIMS; i++) {
        node->boundingRect.maxs[i] = 0;
        node->boundingRect.mins[i] = 0;
    }
//    for (int i = 0; i < TMAXNODES + 1; i++) {
//        node->childs[i] = NULL;
//    }
    return node;
}

RSTAR_TREE_TEMPLETE
typename RSTAR_TREE_QUAL::leafNode* RSTAR_TREE_QUAL::newLeaf(const ELEMTYPE *min, const ELEMTYPE *max, const DATATYPE &data) {
    leafNode *node = new leafNode;
    
    assert(node);
    
    initRect(node->boundingRect, min, max);
    node->data = data;
    return node;
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::initRect(Rect &rect, const ELEMTYPE min[NUMDIMS], const ELEMTYPE max[NUMDIMS]) {
    for (int i = 0; i < NUMDIMS; ++i)
    {
        rect.mins[i] = min[i];
        rect.maxs[i] = max[i];
    }
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::initRectWithCenterAndRadius(RStarTree::Rect &rect, const ELEMTYPE *center, ELEMTYPE r) {
    for (int i = 0; i < NUMDIMS; i++) {
        rect.maxs[i] = center[i] + r;
        rect.mins[i] = center[i] - r;
    }
}


RSTAR_TREE_TEMPLETE
int RSTAR_TREE_QUAL::cover(const Rect &rect1, const Rect &rect2) {
    int f = MAX_INDEX(rect1.mins[0], rect2.mins[0]);
    for (int i = 0; i < NUMDIMS; ++i)
    {
        int temp1 = MAX_INDEX(rect1.mins[i], rect2.mins[i]);
        int temp2 = MAX_INDEX(rect1.maxs[i], rect2.maxs[i]);
        if (temp1 == temp2 && temp1 == f)
        {
            continue;
        } else {
            return 0;
        }
    }
    return f;
    // return 1 means that rect1 fully covers rect2
    // return 2 means that rect2 fully covers rect1
}

RSTAR_TREE_TEMPLETE
bool RSTAR_TREE_QUAL::intersect(const Rect &rect1, const Rect &rect2) {
    for (int i = 0; i < NUMDIMS; ++i)
    {
        ELEMTYPE width = MAX(rect1.maxs[i], rect2.maxs[i]) - MIN(rect1.mins[i], rect2.mins[i]);
        if (width > (rect1.maxs[i] - rect1.mins[i]) + (rect2.maxs[i] - rect2.mins[i]))
        {
            return false;
        }
    }
    return true;
}

RSTAR_TREE_TEMPLETE
bool RSTAR_TREE_QUAL::intersectWithoutCenter(const RStarTree::Rect &rect1, const RStarTree::Rect &rect1Center, const RStarTree::Rect &rect2) {
    
    return intersect(rect1, rect2) && (cover(rect1Center, rect2) != 1);
    
}

RSTAR_TREE_TEMPLETE
ELEMTYPEREAL RSTAR_TREE_QUAL::intersectVolume(const Rect &rect1, const Rect &rect2) {
    
    ELEMTYPE v = 1;
    for (int i = 0; i < NUMDIMS; i++) {
        ELEMTYPE width = MAX(rect1.maxs[i], rect2.maxs[i]) - MIN(rect1.mins[i], rect2.mins[i]);
        if (width < (rect1.maxs[i] - rect1.mins[i]) + (rect2.maxs[i] - rect2.mins[i]))
        {
            v = v * ((rect1.maxs[i] - rect1.mins[i]) + (rect2.maxs[i] - rect2.mins[i]) - width);
        } else {
            return (ELEMTYPEREAL)0;
        }
    }
    return v;
}

RSTAR_TREE_TEMPLETE
ELEMTYPEREAL RSTAR_TREE_QUAL::overlapsVolume(RStarTree::Node *node, int index) {
    
    ELEMTYPEREAL v = (ELEMTYPEREAL)0;
    
    for (int i = 0; i < node->childCount(); i++) {
        if (i != index) {
            v = v + intersectVolume(node->childs[i]->boundingRect, node->childs[index]->boundingRect);
        }
    }
    return v;
}

RSTAR_TREE_TEMPLETE
bool RSTAR_TREE_QUAL::isRectsEqual(const Rect &rect1, const Rect &rect2) {
    for (int i = 0; i < NUMDIMS; ++i)
    {
        if (rect1.maxs[i] != rect2.maxs[i] || rect1.mins[i] != rect2.mins[i])
        {
            return false;
        }
    }
    return true;
}

RSTAR_TREE_TEMPLETE
typename RSTAR_TREE_QUAL::Rect RSTAR_TREE_QUAL::boundingRect(const Rect &rect1, const Rect &rect2) {
    Rect rect;
    for (int i = 0; i < NUMDIMS; ++i)
    {
        rect.maxs[i] = MAX(rect1.maxs[i], rect2.maxs[i]);
        rect.mins[i] = MIN(rect1.mins[i], rect2.mins[i]);
    }
    return rect;
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::rectCopy(Rect &dest, const RStarTree::Rect src) {
    for (int i = 0; i < NUMDIMS; ++i)
    {
        dest.maxs[i] = src.maxs[i];
        dest.mins[i] = src.mins[i];
    }
}

RSTAR_TREE_TEMPLETE
ELEMTYPEREAL RSTAR_TREE_QUAL::rectangularVolume(const RStarTree::Rect &rect) {
    ELEMTYPEREAL v = 1;
    for (int i = 0; i < NUMDIMS; ++i)
    {
        v = v * (rect.maxs[i] - rect.mins[i]);
    }
    return v;
}

RSTAR_TREE_TEMPLETE
ELEMTYPEREAL RSTAR_TREE_QUAL::sphericalVolume(const RStarTree::Rect &rect) {
    
    ELEMTYPEREAL v;
    ELEMTYPEREAL r = 0;
    for (int i = 0; i < NUMDIMS; ++i)
    {
        r = r + (rect.maxs[i] - rect.mins[i]) * (rect.maxs[i] - rect.mins[i]);
    }
    r = sqrt(r);
    v = (ELEMTYPEREAL)pow(r, NUMDIMS) * unitSphericalVolume;
    return v;
}

RSTAR_TREE_TEMPLETE
ELEMTYPEREAL RSTAR_TREE_QUAL::rectVolume(const RStarTree::Rect &rect) {
    
    if (FLAG == RSTAR_TREE_USING_MBR)
    {
        return rectangularVolume(rect);
    } else if (FLAG == RSTAR_TREE_USING_SPHERICAL) {
        return sphericalVolume(rect);
    }
    return 0;
}

RSTAR_TREE_TEMPLETE
ELEMTYPE RSTAR_TREE_QUAL::rectMargin(const RStarTree::Rect &rect) {
    ELEMTYPE m = (ELEMTYPE)0;
    for (int i = 0; i < NUMDIMS; i++) {
        m += rect.maxs[i] - rect.mins[i];
    }
    return m;
}

RSTAR_TREE_TEMPLETE
ELEMTYPE RSTAR_TREE_QUAL::squaredDistanceBetweenRects(const RStarTree::Rect &rect1, const RStarTree::Rect &rect2) {
    ELEMTYPE v = 0;
    for (int i = 0; i < NUMDIMS; i++) {
        ELEMTYPE temp = (rect1.mins[i] + rect1.maxs[i]) / 2 - (rect2.mins[i] + rect2.maxs[i]) / 2;
        v = v + temp * temp;
    }
    return v;
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::resizeNode(Node *node) {
    
    if (node->childCount() == 0) {
        rectCopy(node->boundingRect, Rect());
        return;
    }
    
    Rect trect;
    rectCopy(trect, node->childs[0]->boundingRect);
    for (int i = 1; i < node->childCount(); i++) {
        for (int j = 0; j < NUMDIMS; j++) {
            trect.maxs[j] = MAX(node->childs[i]->boundingRect.maxs[j], trect.maxs[j]);
            trect.mins[j] = MIN(node->childs[i]->boundingRect.mins[j], trect.mins[j]);
        }
    }
    rectCopy(node->boundingRect, trect);
    return;
}

#pragma mark Insert

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::Insert(const ELEMTYPE *min, const ELEMTYPE *max, const DATATYPE &data) {
    
    leafNode *lnode = newLeaf(min, max, data);
    insertInternalNode(lnode, root);
    nodeCount++;
}

RSTAR_TREE_TEMPLETE
typename RSTAR_TREE_QUAL::Node* RSTAR_TREE_QUAL::chooseSubtree(RStarTree::leafNode *lnode, RStarTree::Node *node) {
    if (node->level == 1) {
        return static_cast<Node *>(* std::min_element(node->childs.begin(), node->childs.end(), SortByOverlapEnlargement(lnode->boundingRect)));
    } else {
        return static_cast<Node *>(* std::min_element(node->childs.begin(), node->childs.end(), SortByAreaEnlargement(lnode->boundingRect)));
    }
}

RSTAR_TREE_TEMPLETE
typename RSTAR_TREE_QUAL::Node* RSTAR_TREE_QUAL::insertInternalNode(RStarTree::leafNode *lnode, RStarTree::Node *node, bool firstInsert) {
    rectCopy(node->boundingRect, boundingRect(node->boundingRect, lnode->boundingRect));
    if (node->level == 0) {
        // if this node is already at bottom level, then add the leaf to this node.
        node->childs.push_back((Node *)lnode);
    } else {
        // else invoke ChooseSubtree and then insert leaf to that node.
        Node *tempNode = insertInternalNode(lnode, chooseSubtree(lnode, node), firstInsert);
        if (!tempNode) {
            return NULL;
        } else {
            node->childs.push_back(tempNode);
        }
    }
    
    if (node->childCount() > MAXNODE) {
        return OverflowTreatment(node, firstInsert);
    } else {
        return NULL;
    }
    // return NULL means that nothing special happened
    // return value is not NULL means that this node needs to be splited and then add the returned node into the parent node.
}

RSTAR_TREE_TEMPLETE
typename RSTAR_TREE_QUAL::Node* RSTAR_TREE_QUAL::OverflowTreatment(RStarTree::Node *level, bool firstInsert) {
    
    if (level != root && firstInsert) {
        reinsert(level);
        return NULL;
    }
    
    Node * splitItem = splitNode(level);
    
    if (level == root) {
        Node *newRoot = newNode();
        newRoot->level = root->level + 1;
        
        newRoot->childs.push_back(root);
        newRoot->childs.push_back(splitItem);
        
        resizeNode(newRoot);
        
        root = newRoot;
        return NULL;
    }
    
    return splitItem;
    
}

RSTAR_TREE_TEMPLETE
typename RSTAR_TREE_QUAL::Node* RSTAR_TREE_QUAL::splitNode(RStarTree::Node *node) {
    
    Node *nn = newNode();
    nn->level = node->level;
    
    const int n_items = (int)node->childs.size();
    const int distribution_count = n_items - 2 * MINNODE + 1;
    
    int split_axis = NUMDIMS+1, split_edge = 0, split_index = 0;
    ELEMTYPE split_margin = 0;
    
    Rect r1, r2;
    
    assert(n_items == MAXNODE + 1);
    assert(distribution_count > 0);
    assert(MINNODE + distribution_count - 1 <= n_items);
    
    // choose split axis
    for (int axis = 0; axis < NUMDIMS; axis++) {
        ELEMTYPE margin = 0;
        ELEMTYPEREAL overlap = 0, dist_area, dist_overlap;
        int dist_edge = 0, dist_index = 0;
        
        dist_area = dist_overlap = std::numeric_limits<ELEMTYPEREAL>::max();
        
        for (int edge = 0; edge <= 1; edge++) {
            if (edge == 0) {
                std::sort(node->childs.begin(), node->childs.end(), SortByLeftEdge(axis));
            } else {
                std::sort(node->childs.begin(), node->childs.end(), SortByRightEdge(axis));
            }
            
            for (int k = 0; k < distribution_count; k++) {
                ELEMTYPEREAL area = 0;
                
                r1.reset();
                for (int i = 0; i < MINNODE + k + 1; i++) {
                    r1.stretch(node->childs[i]->boundingRect);
                }
                
                r1.reset();
                for (int i = MINNODE + k + 1; i < node->childCount(); i++) {
                    r1.stretch(node->childs[i]->boundingRect);
                }
                
                margin += rectMargin(r1) + rectMargin(r2);
                area   += rectVolume(r1) + rectVolume(r2);
                overlap = intersectVolume(r1, r2);
                
                if (overlap < dist_overlap || (overlap == dist_overlap && area < dist_area)) {
                    dist_edge = edge;
                    dist_index = MINNODE + k;
                    dist_overlap = overlap;
                    dist_area = area;
                }
            }
        }
        
        if (split_axis == NUMDIMS + 1 || split_margin > margin) {
            split_axis = axis;
            split_margin = margin;
            split_edge = dist_edge;
            split_index = dist_index;
        }
    }
    
    // distribute items into two groups
    if (split_edge == 0) {
        std::sort(node->childs.begin(), node->childs.end(), SortByLeftEdge(split_axis));
    } else if (split_axis != NUMDIMS - 1){
        std::sort(node->childs.begin(), node->childs.end(), SortByRightEdge(split_axis));
    }
    
    nn->childs.assign(node->childs.begin() + split_index, node->childs.end());
    node->childs.erase(node->childs.begin() + split_index, node->childs.end());
    
    resizeNode(node);
    resizeNode(nn);
    
    return nn;
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::reinsert(RStarTree::Node *node) {
    vector<generalNode *> removed_item;
    
    const int n_items = node->childCount();
    const int p = (int)((double)n_items * RSTAR_TREE_REINSERT_P) > 0 ? (int)((double)n_items * RSTAR_TREE_REINSERT_P) : 1;
    
    assert(n_items == MAXNODE + 1);
    
    std::partial_sort(node->childs.begin(), node->childs.end() - p , node->childs.end(), SortByDistanceFromCenter(node->boundingRect));
    
    removed_item.assign(node->childs.end() - p, node->childs.end());
    node->childs.erase(node->childs.end() - p, node->childs.end());
    
    resizeNode(node);

    for (int i = 0; i < removed_item.size(); i++) {
        insertInternalNode(static_cast<leafNode *>(removed_item[i]), root, false);
    }
}

#pragma mark Remove

#pragma mark Search

RSTAR_TREE_TEMPLETE
int RSTAR_TREE_QUAL::Search(const ELEMTYPE *min, const ELEMTYPE *max, int flag, vector<DATATYPE> &rData) {
    
    Rect query;
    initRect(query, min, max);
    
    vector<pair<DATATYPE, ELEMTYPE> > result;
    int count = 0;
    searchInternalNode(root, query, result, flag, count);
    
    vector<DATATYPE>().swap(rData);
    for (unsigned long i = 0; i < result.size(); i++) {
        rData.push_back(result[i].first);
    }
    return count;
}

RSTAR_TREE_TEMPLETE
int RSTAR_TREE_QUAL::NeighborSearch(const ELEMTYPE *center, int searchCount, vector<DATATYPE> &rData) {
    
    Rect query, queryCenter;
    ELEMTYPE r1 = 10, r2 = 0;
    initRectWithCenterAndRadius(query, center, r1);
    initRectWithCenterAndRadius(queryCenter, center, r1);
    
    vector<pair<DATATYPE, ELEMTYPE> > result;
    int count = 0;
    
    while (true) {
        searchInternalNodeWithoutCenter(root, query, queryCenter, result, count);
        if (result.size() >= searchCount) {
            break;
        } else {
            r2 = r1;
            r1 = r1 * 2;
            initRectWithCenterAndRadius(query, center, r1);
            initRectWithCenterAndRadius(queryCenter, center, r1);
        }
    }
    vector<DATATYPE>().swap(rData);
    for (unsigned long i = 0; i < result.size(); i++) {
        rData.push_back(result[i].first);
    }
    return count;
    
}

RSTAR_TREE_TEMPLETE
bool RSTAR_TREE_QUAL::accept(const Rect &query, const Rect &rect, int flag) {
    assert(SEARCH_NORMAL_FLAG(flag));
    
    switch(flag) {
        case SEARCH_FLAG_EXACTLY:
            return isRectsEqual(rect, query);
        case SEARCH_FLAG_INTERSECT:
            return intersect(rect, query);
        case SEARCH_FLAG_COVER_QUERY:
            return cover(rect, query) == 1;
        case SEARCH_FLAG_COVERED_BY_QUERY:
            return cover(rect, query) == 2;
        default:
            return false;
    }
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::searchInternalNode(RStarTree::Node *node, const Rect &query, vector<pair<DATATYPE, ELEMTYPE> > &result, int flag, int &diskAccessCount) {
    assert(node);
    if (node->level == 0) {
        for (int i = 0; i < node->childCount(); i++) {
//            leafNode *l = static_cast<leafNode *>(node->childs[i]);
            leafNode *l = (leafNode *)node->childs[i];
            if (accept(query, l->boundingRect, flag)) {
                ACCESS_NODE(diskAccessCount)
                pair<DATATYPE, ELEMTYPEREAL> p;
                p.first = l->data;
                p.second = squaredDistanceBetweenRects(query, l->boundingRect);
                result.push_back(p);
            }
        }
    } else {
        for (int i = 0; i < node->childCount(); i++) {
            if (accept(query, node->childs[i]->boundingRect, SEARCH_FLAG_INTERSECT)) {
                ACCESS_NODE(diskAccessCount);
                searchInternalNode(node->childs[i], query, result, flag, diskAccessCount);
            }
        }
    }
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::searchInternalNodeWithoutCenter(RStarTree::Node *node, const RStarTree::Rect &query, const RStarTree::Rect &queryCenter, vector<pair<DATATYPE, ELEMTYPE> > &result, int &diskAccessCount) {
    assert(node);
    if (node->level == 0) {
        for (int i = 0; i < node->childCount(); i++) {
            //            leafNode *l = static_cast<leafNode *>(node->childs[i]);
            leafNode *l = (leafNode *)node->childs[i];
            if (intersectWithoutCenter(query, queryCenter, l->boundingRect)) {
                ACCESS_NODE(diskAccessCount)
                pair<DATATYPE, ELEMTYPEREAL> p;
                p.first = l->data;
                p.second = squaredDistanceBetweenRects(query, l->boundingRect);
                result.push_back(p);
            }
        }
    } else {
        for (int i = 0; i < node->childCount(); i++) {
            if (intersectWithoutCenter(query, queryCenter, node->childs[i]->boundingRect)) {
                ACCESS_NODE(diskAccessCount);
                searchInternalNodeWithoutCenter(node->childs[i], query, queryCenter, result, diskAccessCount);
            }
        }
    }
}

#pragma mark Remove All

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::removeAll() {
    assert(release || (FLAG & RSTAR_TREE_NO_NEED_TO_RELEASE_DATA));
    
    removeAllNode(root);
    root = newNode();
    
    nodeCount = 0;
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::removeAllNode(RStarTree::Node *node) {
    
    assert(node);
    
    if (node->level == 0) {
        for (int i = 0; i < node->childCount(); i++) {
            removeLeaf((leafNode *)node->childs[i]);
            node->childs[i] = NULL;
        }
    } else {
        for (int i = 0; i < node->childCount(); i++) {
            removeAllNode(node->childs[i]);
            node->childs[i] = NULL;
        }
    }
    
    delete node;
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::removeLeaf(RStarTree::leafNode *node) {
    if (FLAG & RSTAR_TREE_NO_NEED_TO_RELEASE_DATA) {
        delete node;
    } else {
        release(node->data);
        delete node;
    }
}

#pragma mark count
RSTAR_TREE_TEMPLETE
int RSTAR_TREE_QUAL::count() {
//    int c = 0;
//    countNode(root, c);
//    return c;
    
    return nodeCount;
}

RSTAR_TREE_TEMPLETE
void RSTAR_TREE_QUAL::countNode(const RStarTree::Node *node, int &c) {
    
    if (node == NULL) {
        return;
    } else {
        if (node->level == 0) {
            c = c + node->childCount();
        } else {
            for (int i = 0; i < node->childCount(); i++) {
                countNode(node->childs[i], c);
            }
        }
    }
    
}

#endif /* RStarTree_hpp */
