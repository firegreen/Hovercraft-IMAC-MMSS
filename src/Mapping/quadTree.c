#include "Mapping/quadTree.h"
#include "Mapping/map.h"
#include <stdlib.h>

Node* makeNode(const Bounds2P* bounds){
    Node* retour = malloc(sizeof(Node));
    retour->center.x = (bounds->rightBottom.x - bounds->leftTop.x)/2.;
    retour->center.y = (bounds->leftTop.y - bounds->rightBottom.y)/2.;
    retour->bounds = *bounds;
    retour->nbObjects = 0;
    retour->childs[0] = NULL;
    return retour;
}

void setNodeChild(Node* node){
    Bounds2P bounds;
    bounds.leftTop = node->bounds.leftTop;
    bounds.rightBottom = node->center;
    node->childs[LEFT_TOP] = makeNode(&bounds);
    bounds.rightBottom.y = node->bounds.rightBottom.y;
    bounds.leftTop.y = node->center.y;
    node->childs[LEFT_BOTTOM] = makeNode(&bounds);
    bounds.rightBottom.x = node->bounds.rightBottom.x;
    bounds.leftTop.x = node->center.x;
    node->childs[RIGHT_BOTTOM] = makeNode(&bounds);
    bounds.rightBottom.y = node->center.y;
    bounds.leftTop.y = node->bounds.leftTop.y;
    node->childs[RIGHT_TOP] = makeNode(&bounds);
}

QuadTree* makeTree(const struct Map* map){
    QuadTree* retour = malloc(sizeof(QuadTree));
    Bounds2P bounds;
    bounds.rightBottom = map->bounds.rightBottom;
    bounds.leftTop = map->bounds.leftTop;
    retour->root = makeNode(&bounds);
    setNodeChild(retour->root);
    return retour;
}

void addObjectToNode(Node* node, struct Object* object, Point2D position);

void addObjectToTree(QuadTree* tree, struct Object* object, Point2D position){
    addObjectToNode(tree->root,object,position);
}

void putObjectToNodeChild(Node* node, struct Object* object, Point2D position){
    if(position.x > node->center.x)
        if(position.y > node->center.y)
            addObjectToNode(node->childs[RIGHT_TOP],object,position);
        else
            addObjectToNode(node->childs[RIGHT_BOTTOM],object,position);
    else
        if(position.y > node->center.y)
            addObjectToNode(node->childs[LEFT_TOP],object,position);
        else
            addObjectToNode(node->childs[LEFT_BOTTOM],object,position);
}

Chained_Object* makeChainedObject(struct Object* object, Chained_Object *next, Point2D position){
    Chained_Object* co;
    co = malloc(1 * sizeof(*co));
    if(co!=NULL){
        co->next = next;
        co->object = object;
        co->position = position;
    }
    return co;
}

void addObjectToNode(Node* node, struct Object* object, Point2D position){
    node->nbObjects++;
    if(node->childs[0]==NULL)
    {
        if(node->nbObjects<8){
            Chained_Object* co = makeChainedObject(object,node->objects,position);
            node->objects = co;
        }
        else{
            setNodeChild(node);
            Chained_Object* current = node->objects;
            while(current!=NULL){
                putObjectToNodeChild(node,current->object,current->position);
                current = current->next;
            }
            node->objects = NULL;
        }
    }
    putObjectToNodeChild(node,object,position);
}

Chained_Object* getNearestObjectsN(Node* node, Point2D p){
    if(node->childs[0]==NULL)
    {
        return node->objects;
    }
    if(p.x > node->center.x)
        if(p.y > node->center.y)
            return getNearestObjectsN(node->childs[RIGHT_TOP],p);
        else
            return getNearestObjectsN(node->childs[RIGHT_BOTTOM],p);
    else
        if(p.y > node->center.y)
            return getNearestObjectsN(node->childs[LEFT_TOP],p);
        else
            return getNearestObjectsN(node->childs[LEFT_BOTTOM],p);
}

Chained_Object* getNearestObjects(QuadTree* tree, Point2D p){
    return getNearestObjectsN(tree->root,p);
}
