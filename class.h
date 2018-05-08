#ifndef CLASS_H
#define CLASS_H

#include <string>
#include "gmpfrxx.h"
#include "misc/vec/vec.h"
using namespace std;

class Bn_Ntk {
   public:
    string Name;
    Vec_Ptr_t *Node_Ary;
    Vec_Ptr_t *Pi_Ary;
    Vec_Ptr_t *Po_Ary;
    Vec_Ptr_t *Key_Ary;
    Vec_Ptr_t *NodeDFS_Ary;
    Bn_Ntk() {
        Node_Ary = Vec_PtrAlloc(0);
        Pi_Ary = Vec_PtrAlloc(0);
        Po_Ary = Vec_PtrAlloc(0);
        Key_Ary = Vec_PtrAlloc(0);
        NodeDFS_Ary = Vec_PtrAlloc(0);
    }
};

class Bn_Node {
   public:
    int Id;
    string Name;
    int Level;
    int Type;   // 0: pi, 1: keypi, 2: po, 3: internal
    int FType;  // 0: buff, 1: not, 2: and, 3: or, 4: nand, 5: nor, 6: xor, 7:
                // nxor, 8: mux
    mpfr_class Weight;
    Vec_Ptr_t *Fanin_Ary;
    Vec_Ptr_t *Fanout_Ary;
    bool flag;
    unsigned *Value;
    unsigned *Backup;
    Bn_Node() {
        Id = -1;
        Name = "";
        Level = -1;
        Type = -1;
        FType = -1;
        Weight = "0";
        Fanin_Ary = Vec_PtrAlloc(0);
        Fanout_Ary = Vec_PtrAlloc(0);
        flag = false;
    }
};

class Wire_Corruption {
   public:
    Bn_Node *ns;
    Bn_Node *nd;
    mpfr_class Corruption;
    Wire_Corruption() { Corruption = "0.0"; }
};
#endif
