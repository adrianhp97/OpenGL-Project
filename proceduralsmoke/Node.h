// Author: Tommy Hinks (thinks@hotmail.com) 2005
#ifndef NODE_H
#define NODE_H
// Class defined in h-file because of it's simplicity.
// Member-functions are kept at a minimum for better performance.
class Node
{
public:
 // Constructors
 Node(float p[3], float v[3], Node *nextPtr)
 {
 pos[0] = p[0];
 pos[1] = p[1];
 pos[2] = p[2];
 vel[0] = v[0];
 vel[1] = v[1];
 vel[2] = v[2];

 age = 0.0f;
 next = nextPtr;
 }

 Node()
 {
 age = 0.0f;
 next = 0;
 }

 // ? Get ?
 // (No index checking)
 float getPos(int n) { return pos[n]; }
 float getVel(int n) { return vel[n]; }

 // Vector version, for glVertex3fv
 float* getPos() { return pos; }

 float getAge() { return age; }
 void setAge(float t) { age = t; }

 Node* getNext() { return next; }

 // ! Set !
 void setPos(float f1, float f2, float f3)
 {
 pos[0] = f1;
 pos[1] = f2;
 pos[2] = f3;
 } 
 void setVel(int n, float value)
 {
 vel[n] = value;
 }

private:
 Node* next; // Pointer to next node in list
 float pos[3]; // Store position
 float vel[3]; // Store velocity
 float age; // How long the particle has lived

 friend class ParticleList;
};
#endif 