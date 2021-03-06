#ifndef QUADTREE_H
#define QUADTREE_H

#include "Object2D/geometry.h"
struct Map;

typedef enum NodePosition {LEFT_TOP=0,LEFT_BOTTOM,RIGHT_TOP,RIGHT_BOTTOM} NodePosition;

struct Object;
struct Chained_Object{
    struct Object* object;
    Point2D position;
    struct Chained_Object* next;
};
typedef struct Chained_Object Chained_Object;

typedef struct Node{
    Chained_Object* objects;
    int nbObjects;
    Point2D center;
    Bounds2P bounds;
    struct Node* childs[4];
} Node;

typedef struct QuadTree{
    Node* root;
} QuadTree;

Node* makeNode(const Bounds2P* bounds);
QuadTree* makeTree(const struct Map* map);
Chained_Object* makeChainedObject(struct Object* object, Chained_Object *next, Point2D position);
void freeAllChaineObject(struct Chained_Object** object);
void addObjectToTree(QuadTree* tree, struct Object* object, Point2D position);
Chained_Object* getNearestObjects(QuadTree* tree, Point2D p);

#endif // QUADTREE_H
