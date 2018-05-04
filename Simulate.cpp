#include "include.h"
using namespace std;

void GenerateRandomPatterns(Bn_Ntk *bNtk);
void Simulation(Vec_Ptr_t *NodeDFS_Ary);
void EvalBuff(Bn_Node *bNode);
void EvalNot(Bn_Node *bNode);
void EvalAnd(Bn_Node *bNode);
void EvalNand(Bn_Node *bNode);
void EvalOr(Bn_Node *bNode);
void EvalXor(Bn_Node *bNode);
void EvalNor(Bn_Node *bNode);
void EvalXnor(Bn_Node *bNode);
void EvalMux(Bn_Node *bNode);
void BackupValue(Vec_Ptr_t *Node_Ary);
extern int nWords;

//***********************************************
void GenerateRandomPatterns(Bn_Ntk *bNtk) {
    int j;
    Bn_Node *bNode;

    // SET initial random values of PIs
    Vec_PtrForEachEntry(Bn_Node *, bNtk->Pi_Ary, bNode, j) {
        for (int i = 0; i < nWords; ++i) {
            bNode->Value[i] = RANDOM_UNSIGNED;
        }
    }
    /*// SET initial random values of Keyinputs
      Vec_PtrForEachEntry(Bn_Node *, bNtk->Key_Ary, bNode, j) {
      for (int i = 0; i < nWords; ++i) {
      bNode->Value[i] = RANDOM_UNSIGNED;
      }
      }*/
}
//***********************************************
void Simulation(Vec_Ptr_t *Node_Ary) {
    int j;
    Bn_Node *bNode;

    // Simulation
    Vec_PtrForEachEntry(Bn_Node *, Node_Ary, bNode, j) {
        if ((bNode->Type == 3) || (bNode->Type == 2)) {
            switch (bNode->FType) {
                case 0:
                    EvalBuff(bNode);
                    break;
                case 1:
                    EvalNot(bNode);
                    break;
                case 2:
                    EvalAnd(bNode);
                    break;
                case 3:
                    EvalOr(bNode);
                    break;
                case 4:
                    EvalNand(bNode);
                    break;
                case 5:
                    EvalNor(bNode);
                    break;
                case 6:
                    EvalXor(bNode);
                    break;
                case 7:
                    EvalXnor(bNode);
                    break;
                case 8:
                    EvalMux(bNode);
                    break;
            }
        }
    }
}
//***********************************************
void EvalBuff(Bn_Node *bNode) {
    Bn_Node *Pi;

    Pi = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 0);

    for (int i = 0; i < nWords; ++i) bNode->Value[i] = Pi->Value[i];
}
//***********************************************
void EvalNot(Bn_Node *bNode) {
    Bn_Node *Pi;

    Pi = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 0);

    for (int i = 0; i < nWords; ++i) bNode->Value[i] = ~Pi->Value[i];
}
//***********************************************
void EvalAnd(Bn_Node *bNode) {
    int j;
    Bn_Node *Pi;

    memset(bNode->Value, 255, sizeof(unsigned) * nWords);

    Vec_PtrForEachEntry(Bn_Node *, bNode->Fanin_Ary, Pi, j) {
        for (int i = 0; i < nWords; ++i) {
            if (bNode->Value[i] == 0) continue;
            bNode->Value[i] = bNode->Value[i] & Pi->Value[i];
        }
    }
}
//***********************************************
void EvalNand(Bn_Node *bNode) {
    int j;
    Bn_Node *Pi;

    memset(bNode->Value, 255, sizeof(unsigned) * nWords);

    Vec_PtrForEachEntry(Bn_Node *, bNode->Fanin_Ary, Pi, j) {
        for (int i = 0; i < nWords; ++i) {
            if (bNode->Value[i] == 0) continue;
            bNode->Value[i] = bNode->Value[i] & Pi->Value[i];
        }
    }

    for (int i = 0; i < nWords; ++i) bNode->Value[i] = ~bNode->Value[i];
}
//***********************************************
void EvalOr(Bn_Node *bNode) {
    int j;
    Bn_Node *Pi;

    memset(bNode->Value, 0, sizeof(unsigned) * nWords);

    Vec_PtrForEachEntry(Bn_Node *, bNode->Fanin_Ary, Pi, j) {
        for (int i = 0; i < nWords; ++i) {
            if (bNode->Value[i] == 1) continue;
            bNode->Value[i] = bNode->Value[i] | Pi->Value[i];
        }
    }
}
//***********************************************
void EvalNor(Bn_Node *bNode) {
    int j;
    Bn_Node *Pi;

    memset(bNode->Value, 0, sizeof(unsigned) * nWords);

    Vec_PtrForEachEntry(Bn_Node *, bNode->Fanin_Ary, Pi, j) {
        for (int i = 0; i < nWords; ++i) {
            if (bNode->Value[i] == 1) continue;
            bNode->Value[i] = bNode->Value[i] | Pi->Value[i];
        }
    }

    for (int i = 0; i < nWords; ++i) bNode->Value[i] = ~bNode->Value[i];
}
//***********************************************
void EvalXor(Bn_Node *bNode) {
    Bn_Node *Pi1, *Pi2;

    Pi1 = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 0);
    Pi2 = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 1);

    for (int i = 0; i < nWords; ++i)
        bNode->Value[i] =
            (~Pi1->Value[i] & Pi2->Value[i]) | (Pi1->Value[i] & ~Pi2->Value[i]);
}
//***********************************************
void EvalXnor(Bn_Node *bNode) {
    Bn_Node *Pi1, *Pi2;

    Pi1 = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 0);
    Pi2 = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 1);

    for (int i = 0; i < nWords; ++i)
        bNode->Value[i] =
            (~Pi1->Value[i] & ~Pi2->Value[i]) | (Pi1->Value[i] & Pi2->Value[i]);
}
//***********************************************
void EvalMux(Bn_Node *bNode) {
    Bn_Node *Pi1, *Pi2, *Sel;

    Sel = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 0);
    Pi1 = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 1);
    Pi2 = (Bn_Node *)Vec_PtrEntry(bNode->Fanin_Ary, 2);

    for (int i = 0; i < nWords; ++i)
        bNode->Value[i] =
            (Pi1->Value[i] & ~Sel->Value[i]) | (Pi2->Value[i] & Sel->Value[i]);
}
//***********************************************
void BackupValue(Vec_Ptr_t *Node_Ary) {
    int j;
    Bn_Node *bNode;

    Vec_PtrForEachEntry(Bn_Node *, Node_Ary, bNode, j) {
        for (int i = 0; i < nWords; ++i) {
            bNode->Backup[i] = bNode->Value[i];
        }
    }
}
