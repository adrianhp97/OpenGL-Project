// Author: Tommy Hinks (thinks@hotmail.com) 2005
#ifndef PARTICLELIST_H
#define PARTICLELIST_H
#include "node.h"
class ParticleList {
public:
 ParticleList();
 ~ParticleList();

 void insert (float [3], float [3]);
 void del (Node *);
 bool empty ();
 int getNodeCount() { return nodeCount; }
 Node* getHeader() { return header; }
private:
 Node *header;
 int nodeCount;
};
#endif 