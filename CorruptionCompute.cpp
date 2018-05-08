#include "include.h"
using namespace std;

Wire_Corruption *CorruptionCompute(Bn_Ntk *bNtk);
void PreOrderTraversal(Vec_Ptr_t *NodePOT_Ary, Bn_Node *bNode);
void SubPreOrderTraversal(Vec_Ptr_t *NodePOT_Ary, Bn_Node *bNode);
extern void Simulation(Vec_Ptr_t *Node_Ary);
extern double nPatterns;
extern unsigned long long bitcount;
extern Vec_Ptr_t *correct_Po;
extern int nWords;

//***********************************************
Wire_Corruption *CorruptionCompute(Bn_Ntk *bNtk) {
    int j, k, l, NP = 0;
    mpfr_class corruption, max_corruption, MD;
    max_corruption = "-1", MD = "0";
    Bn_Node *bNode, *fanout, *Po, *subNode;
    Wire_Corruption *wire = new Wire_Corruption();
    Vec_Ptr_t *NodePOT_Ary = Vec_PtrAlloc(0);
    unsigned *ODCTmp = ALLOC(unsigned, nWords);
    unsigned *Tmp = ALLOC(unsigned, 1);

    Vec_PtrForEachEntry(Bn_Node *, bNtk->Node_Ary, bNode, j) {
        if (bNode->Type == 2)
            continue;
        else {
            for (int i = 0; i < nWords; ++i)
                bNode->Value[i] =
                    ~bNode->Value[i];  // change the Value of bnode

            Vec_PtrForEachEntry(Bn_Node *, bNode->Fanout_Ary, fanout, k) {
                NP = 0;
                MD = "0";
                memset(ODCTmp, 0, sizeof(unsigned) * nWords);
                PreOrderTraversal(NodePOT_Ary, fanout);
                Simulation(NodePOT_Ary);
                Vec_PtrForEachEntry(Bn_Node *, bNtk->Po_Ary, Po, l) {
                    for (int i = 0; i < nWords; ++i) {
                        Tmp[0] = Po->Value[i] ^ ((unsigned *)Vec_PtrGetEntry(
                                                    correct_Po, l))[i];
                        for (int k = 0; k < 32; ++k)
                            if (GetBit(&Tmp[0], k) == 1) MD += Po->Weight;
                        ODCTmp[i] = ODCTmp[i] | Tmp[0];
                    }
                }
                Vec_PtrForEachEntry(Bn_Node *, NodePOT_Ary, subNode, l) {
                    for (int i = 0; i < nWords; ++i)  // restore
                        subNode->Value[i] = subNode->Backup[i];
                }
                for (int i = 0; i < nWords; ++i)
                    for (int m = 0; m < 32; ++m)
                        if (GetBit(&ODCTmp[i], m) == 1) NP++;
                corruption = ((NP / nPatterns) * (MD / nPatterns));

                if (corruption > max_corruption) {
                    wire->ns = bNode;
                    wire->nd = fanout;
                    max_corruption = corruption;
                    wire->Corruption = max_corruption;
                }
            }
            // restore
            for (int i = 0; i < nWords; ++i) bNode->Value[i] = ~bNode->Value[i];
        }
    }
    mpfr_printf("max_wire_corruption: %.40Rf\n", max_corruption.get_mpfr_t());

    free(ODCTmp);
    free(Tmp);
    return wire;
}
//***********************************************
void PreOrderTraversal(Vec_Ptr_t *NodePOT_Ary, Bn_Node *bNode) {
    int i;

    Vec_PtrClear(NodePOT_Ary);
    SubPreOrderTraversal(NodePOT_Ary, bNode);

    Vec_PtrForEachEntry(Bn_Node *, NodePOT_Ary, bNode, i) {
        bNode->flag = false;
    }
}
//***********************************************.
void SubPreOrderTraversal(Vec_Ptr_t *NodePOT_Ary, Bn_Node *bNode) {
    int i;
    Bn_Node *fanout;

    if (!bNode->flag) {
        bNode->flag = true;
        Vec_PtrPush(NodePOT_Ary, bNode);
        Vec_PtrForEachEntry(Bn_Node *, bNode->Fanout_Ary, fanout, i) {
            SubPreOrderTraversal(NodePOT_Ary, fanout);
        }
    }
}
