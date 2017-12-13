#ifndef IMPLICITOCTREE_H
#define IMPLICITOCTREE_H

#include "abstractOctree.h"

class ImplicitOctree : public AbstractOctree
{
public :
    ImplicitOctree(int _height, BoundingBox _limit) : AbstractOctree(_height, _limit){}

    virtual void checkDistanceOnNode(TreeNode *node);

};

#endif // IMPLICITOCTREE_H
