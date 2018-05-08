#include "include.h"
using namespace std;

extern void Abc_Start();
extern void Abc_Stop();
extern Bn_Ntk *Io_ReadBench(const char *pFileName);
extern void GenerateRandomPatterns(Bn_Ntk *bNtk);
extern void Simulation(Vec_Ptr_t *NodeDFS_Ary);
extern void BackupValue(Vec_Ptr_t *Node_Ary);
extern Wire_Corruption *CorruptionCompute(Bn_Ntk *bNtk);
extern void Bn_NtkDFS(Bn_Ntk *bNtk);
extern void ComputeLevel(Vec_Ptr_t *NodeDFS_Ary);
extern void PreOrderTraversal(Vec_Ptr_t *NodePOT_Ary, Bn_Node *bNode);
extern void PrintBn_Ntk(Bn_Ntk *bNtk, const char *pFileName);
extern int Id;
void PrintCorruption(Bn_Ntk *bNtk);
int nWords = 32, nKeys = 64;
unsigned long long bitcount;
double nPatterns;
Vec_Ptr_t *correct_Po;

//***********************************************
int main(int argc, char const *argv[]) {
    mpfr_class::set_dprec(128);
    int j;
    unsigned *tmp;
    Bn_Ntk *bNtk;
    Bn_Node *bNode, *KeyPi, *KeyGate;
    Wire_Corruption *Key_loc;
    Vec_Ptr_t *NodePOT_Ary = Vec_PtrAlloc(0);
    // long Start_Time, End_Time;

    // Read bench
    bNtk = Io_ReadBench(argv[1]);
    // PrintBn_Ntk(bNtk);

    correct_Po = Vec_PtrAlloc(0);
    nPatterns = 32 * nWords;
    bitcount = pow(2, bNtk->Po_Ary->nSize) - 1;

    Vec_PtrForEachEntry(Bn_Node *, bNtk->Node_Ary, bNode, j) {
        bNode->Value = ALLOC(unsigned, nWords);
        bNode->Backup = ALLOC(unsigned, nWords);
    }

    GenerateRandomPatterns(bNtk);
    Simulation(bNtk->NodeDFS_Ary);
    BackupValue(bNtk->NodeDFS_Ary);

    Vec_PtrForEachEntry(Bn_Node *, bNtk->Po_Ary, bNode, j) {
        tmp = ALLOC(unsigned, nWords);
        for (int i = 0; i < nWords; ++i) tmp[i] = bNode->Value[i];
        Vec_PtrPush(correct_Po, tmp);
    }

    for (int i = 0; i < nKeys; ++i) {
        Key_loc = CorruptionCompute(bNtk);
        // KeyPi
        KeyPi = new Bn_Node();
        KeyPi->Id = Id++;
        KeyPi->Type = 1;   // 0: pi, 1: keypi, 2: po, 3: internal
        KeyPi->FType = 0;  // 0: buff, 1: not, 2: and, 3: or, 4: nand, 5: nor,
                           // 6: xor, 7: nxor, 8: mux
        KeyPi->Name = "ki" + to_string(i);
        KeyPi->Value = ALLOC(unsigned, nWords);
        KeyPi->Backup = ALLOC(unsigned, nWords);
        Vec_PtrPush(bNtk->Key_Ary, KeyPi);
        Vec_PtrPush(bNtk->Node_Ary, KeyPi);
        for (int k = 0; k < nWords; ++k) {
            KeyPi->Value[k] = RANDOM_UNSIGNED;
            KeyPi->Backup[k] = KeyPi->Value[k];
        }

        // KeyGate
        KeyGate = new Bn_Node();
        KeyGate->Id = Id++;
        KeyGate->Type = 3;   // 0: pi, 1: keypi, 2: po, 3: internal
        KeyGate->FType = 6;  // 0: buff, 1: not, 2: and, 3: or, 4: nand, 5: nor,
                             // 6: xor, 7: nxor, 8: mux
        KeyGate->Name = "kg" + to_string(i);
        KeyGate->Value = ALLOC(unsigned, nWords);
        KeyGate->Backup = ALLOC(unsigned, nWords);
        Vec_PtrPush(bNtk->Node_Ary, KeyGate);

        Vec_PtrPush(KeyPi->Fanout_Ary, KeyGate);
        Vec_PtrPush(KeyGate->Fanin_Ary, KeyPi);
        Vec_PtrPush(KeyGate->Fanin_Ary, Key_loc->ns);
        Vec_PtrPush(KeyGate->Fanout_Ary, Key_loc->nd);
        Vec_PtrRemove(Key_loc->ns->Fanout_Ary, Key_loc->nd);
        Vec_PtrPush(Key_loc->ns->Fanout_Ary, KeyGate);
        Vec_PtrRemove(Key_loc->nd->Fanin_Ary, Key_loc->ns);
        Vec_PtrPush(Key_loc->nd->Fanin_Ary, KeyGate);

        free(Key_loc);
        PreOrderTraversal(NodePOT_Ary, KeyGate);
        Simulation(NodePOT_Ary);
        BackupValue(NodePOT_Ary);
    }

    Bn_NtkDFS(bNtk);
    ComputeLevel(bNtk->NodeDFS_Ary);
    PrintCorruption(bNtk);
    PrintBn_Ntk(bNtk, argv[2]);

    Vec_PtrForEachEntry(Bn_Node *, bNtk->Node_Ary, bNode, j) {
        free(bNode->Value);
        free(bNode->Backup);
        delete bNode;
    }

    delete bNtk;
    return 0;
}

void PrintCorruption(Bn_Ntk *bNtk) {
    int j;
    Bn_Node *bNode;
    // unsigned * Po_value;
    unsigned long long HD = 0;
    mpfr_class MD;
    /*unsigned *Tmp = ALLOC(unsigned, 1);
      Vec_PtrForEachEntry(Bn_Node *, bNtk->Po_Ary, bNode, j) {
      for (int i = 0; i < nWords; ++i) {
      Tmp[0] = bNode->Value[i] ^ bNode->Backup[i];
      for (int k = 0; k < 32; ++k)
      if (GetBit(&Tmp[0], k) == 1)
      MD += bNode->Weight;
      }
      }
      cout << "MD: " << MD << "  bitcount: " << bitcount << "  nPatterns: " <<
      nPatterns << endl;
      cout << "Corruption value: " << fixed  <<  setprecision(3) << (double)(MD
      / (bitcount * nPatterns)) << endl;*/
    mpfr_class avg1;
    avg1 = "0";
    double avg2 = 0;
    for (int i = 0; i < nWords; ++i) {
        for (int k = 0; k < 32; ++k) {
            MD = "0", HD = 0;
            Vec_PtrForEachEntry(Bn_Node *, bNtk->Po_Ary, bNode, j) {
                // cout << GetBit(&bNode->Value[i], k);
                if (GetBit(&bNode->Value[i], k) !=
                    GetBit(&((unsigned *)Vec_PtrGetEntry(correct_Po, j))[i],
                           k)) {
                    MD += bNode->Weight;
                    HD++;
                }
            }
            /*cout << endl;
              Vec_PtrForEachEntry(unsigned *, correct_Po, Po_value, j) {
              cout << GetBit(&Po_value[i], k);
              }
              cout << endl;
              cout << "corruption_value: " << fixed  <<  setprecision(3) <<
              ((double)MD / (bitcount)) << endl;
              cout << "hamming_distance: " << fixed  <<  setprecision(3) <<
              ((double)HD / bNtk->Po_Ary->nSize) << endl;*/
            avg1 += MD;
            avg2 += ((double)HD / bNtk->Po_Ary->nSize);
        }
    }
    cout << "HD: " << avg1 / nPatterns << endl;
    cout << "HD: " << avg2 / nPatterns << endl;
}
