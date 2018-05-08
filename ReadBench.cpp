#include "include.h"
using namespace std;

Bn_Ntk *Io_ReadBench(const char *pFileName);
void Bn_NtkDFS(Bn_Ntk *bNtk);
void SubBn_NtkDFS(Bn_Ntk *bNtk, Bn_Node *bNode);
void ComputeLevel(Vec_Ptr_t *NodeDFS_Ary);
int MaxLevel(Bn_Node *bNode);
void PrintBn_Ntk(Bn_Ntk *bNtk, const char *pFileName);
int Id = 0;
int gatecount = 0;

//***********************************************
Bn_Ntk *Io_ReadBench(const char *pFileName) {
    int nOutputs = 0;
    Bn_Node *bNode, *head;
    map<string, Bn_Node *> All_Node;
    map<string, Bn_Node *>::iterator iter;
    Bn_Ntk *bNtk = new Bn_Ntk();
    bNtk->Name = pFileName;

    ifstream inFile(pFileName, ios::in);

    if (!inFile) {
        cout << "The file can not be opened\n";
        exit(1);
    }

    while (!inFile.eof()) {
        string line;
        getline(inFile, line);

        if (line.empty() || line[0] == '#') continue;

        char *token = strtok((char *)line.c_str(), " =(,)");

        if (strcmp(token, "INPUT") == 0) {  // PI
            token = strtok(NULL, " =(,)");
            bNode = new Bn_Node();
            bNode->Id = Id++;
            bNode->Type = 0;   // 0: pi, 1: keypi, 2: po, 3: internal
            bNode->FType = 0;  // 0: buff, 1: not, 2: and, 3: or, 4: nand, 5:
                               // nor, 6: xor, 7: nxor, 8: mux
            bNode->Name = token;
            Vec_PtrPush(bNtk->Pi_Ary, bNode);
            Vec_PtrPush(bNtk->Node_Ary, bNode);
            All_Node[bNode->Name] = bNode;
            continue;
        } else if (strcmp(token, "OUTPUT") == 0) {  // PO
            token = strtok(NULL, " =(,)");
            bNode = new Bn_Node();
            bNode->Id = Id++;
            bNode->Type = 2;   // 0: pi, 1: keypi, 2: po, 3: internal
            bNode->FType = 0;  // 0: buff, 1: not, 2: and, 3: or, 4: nand, 5:
                               // nor, 6: xor, 7: nxor, 8: mux
            bNode->Name = token;

            iter = All_Node.find(token);
            if (iter != All_Node.end()) {
                if (strcmp(iter->second->Name.c_str(), token) != 0) {
                    cout << "hash name problem : " << token << ", "
                         << iter->second->Name << endl;
                    exit(1);
                }
                Vec_PtrPush(iter->second->Fanout_Ary, bNode);
                Vec_PtrPush(bNode->Fanin_Ary, iter->second);
            }

            Vec_PtrPush(bNtk->Po_Ary, bNode);
            Vec_PtrPush(bNtk->Node_Ary, bNode);
            All_Node[bNode->Name] = bNode;
            continue;
        } else {  // internal
            iter = All_Node.find(token);
            if (iter != All_Node.end()) {
                if (strcmp(iter->second->Name.c_str(), token) != 0) {
                    cout << "hash name problem : " << token << ", "
                         << iter->second->Name << endl;
                    exit(1);
                }
                head = iter->second;
            } else {
                // new an internal node
                bNode = new Bn_Node();
                bNode->Id = Id++;
                bNode->Type = 3;  // 0: pi, 1: keypi, 2: po, 3: internal
                bNode->Name = token;
                Vec_PtrPush(bNtk->Node_Ary, bNode);
                All_Node[bNode->Name] = bNode;
                head = bNode;
            }

            token = strtok(NULL, " =(,)");
            // head node type
            if ((strcmp(token, "buf") == 0) || (strcmp(token, "BUF") == 0))
                head->FType = 0;
            if ((strcmp(token, "not") == 0) || (strcmp(token, "NOT") == 0))
                head->FType = 1;
            if ((strcmp(token, "and") == 0) || (strcmp(token, "AND") == 0))
                head->FType = 2;
            if ((strcmp(token, "or") == 0) || (strcmp(token, "OR") == 0))
                head->FType = 3;
            if ((strcmp(token, "nand") == 0) || (strcmp(token, "NAND") == 0))
                head->FType = 4;
            if ((strcmp(token, "nor") == 0) || (strcmp(token, "NOR") == 0))
                head->FType = 5;
            if ((strcmp(token, "xor") == 0) || (strcmp(token, "XOR") == 0))
                head->FType = 6;
            if ((strcmp(token, "xnor") == 0) || (strcmp(token, "XNOR") == 0))
                head->FType = 7;
            if ((strcmp(token, "mux") == 0) || (strcmp(token, "MUX") == 0))
                head->FType = 8;

            token = strtok(NULL, " =(,)");

            while (token != NULL) {
                iter = All_Node.find(token);
                if (iter != All_Node.end()) {
                    if (strcmp(iter->second->Name.c_str(), token) != 0) {
                        cout << "hash name problem : " << token << ", "
                             << iter->second->Name << endl;
                        exit(1);
                    }
                    Vec_PtrPush(head->Fanin_Ary, iter->second);
                    Vec_PtrPush(iter->second->Fanout_Ary, head);
                } else {
                    // new an internal node
                    bNode = new Bn_Node();
                    bNode->Id = Id++;
                    bNode->Type = 3;
                    bNode->Name = token;
                    Vec_PtrPush(bNtk->Node_Ary, bNode);
                    All_Node[bNode->Name] = bNode;
                    Vec_PtrPush(head->Fanin_Ary, bNode);
                    Vec_PtrPush(bNode->Fanout_Ary, head);
                }
                token = strtok(NULL, " =(,)");
            }
            ++gatecount;
        }
    }
    inFile.close();

    Bn_NtkDFS(bNtk);
    // Compute Level
    ComputeLevel(bNtk->NodeDFS_Ary);

    return bNtk;
}
//***********************************************
void Bn_NtkDFS(Bn_Ntk *bNtk) {
    int i;
    Bn_Node *bNode;

    Vec_PtrFree(bNtk->NodeDFS_Ary);
    bNtk->NodeDFS_Ary = Vec_PtrAlloc(0);

    Vec_PtrForEachEntry(Bn_Node *, bNtk->Po_Ary, bNode, i) {
        if (!bNode->flag) {
            bNode->flag = true;
            SubBn_NtkDFS(bNtk, bNode);
        }
    }

    Vec_PtrForEachEntry(Bn_Node *, bNtk->NodeDFS_Ary, bNode, i) {
        bNode->flag = false;
    }
}
//***********************************************
void SubBn_NtkDFS(Bn_Ntk *bNtk, Bn_Node *bNode) {
    int i;
    Bn_Node *fanin;

    Vec_PtrForEachEntry(Bn_Node *, bNode->Fanin_Ary, fanin, i) {
        if (!fanin->flag) {
            fanin->flag = true;
            SubBn_NtkDFS(bNtk, fanin);
        }
    }
    Vec_PtrPush(bNtk->NodeDFS_Ary, bNode);
}
//***********************************************
void ComputeLevel(Vec_Ptr_t *NodeDFS_Ary) {
    int i;
    Bn_Node *bNode;

    Vec_PtrForEachEntry(Bn_Node *, NodeDFS_Ary, bNode, i) {
        if (bNode->Type == 0 || bNode->Type == 1) {
            bNode->Level = 0;
            continue;
        }
        bNode->Level = MaxLevel(bNode);
    }
}
//***********************************************
int MaxLevel(Bn_Node *bNode) {
    int level = 0, i;
    Bn_Node *fanin;

    Vec_PtrForEachEntry(Bn_Node *, bNode->Fanin_Ary, fanin, i) {
        if (fanin->Level > level) level = fanin->Level;
    }

    return level + 1;
}
//***********************************************
void PrintBn_Ntk(Bn_Ntk *bNtk, const char *pFileName) {
    /*int i, j;
      Bn_Node *bNode, *fanin, *fanout;
      cout << "Name      Id      Level      Type      FType\n";
      Vec_PtrForEachEntry(Bn_Node *, bNtk->Node_Ary, bNode, i) {
      cout << "---------------------------------------------\n";
      cout << bNode->Name << "   " << bNode->Id <<  "   " << bNode->Level <<  "
      " << bNode->Type <<  "   " << bNode->FType << endl;
      cout << "Input:\n";
      Vec_PtrForEachEntry(Bn_Node *, bNode->Fanin_Ary, fanin, j) {
      cout << fanin->Name <<  "   " << fanin->Id <<  "   " << fanin->Level <<  "
      " << fanin->Type <<  "   " << fanin->FType << endl;
      }
      cout << "Output:\n";
      Vec_PtrForEachEntry(Bn_Node *, bNode->Fanout_Ary, fanout, j) {
      cout << fanout->Name <<  "   " << fanout->Id <<  "   " << fanout->Level <<
      "   " << fanout->Type <<  "   " << fanout->FType << endl;
      }
      }*/

    int i, j;
    Bn_Node *bNode, *fanin;

    ofstream outFile(pFileName, ios::out);

    if (!outFile) {
        cout << "The file can not be opened\n";
        exit(1);
    }

    // Print PIs
    Vec_PtrForEachEntry(Bn_Node *, bNtk->Pi_Ary, bNode, i) {
        outFile << "INPUT(" << bNode->Name << ")\n";
    }
    // Print KeyPIs
    Vec_PtrForEachEntry(Bn_Node *, bNtk->Key_Ary, bNode, i) {
        outFile << "KEYINPUT(" << bNode->Name << ")\n";
    }
    // Print POs
    Vec_PtrForEachEntry(Bn_Node *, bNtk->Po_Ary, bNode, i) {
        outFile << "OUTPUT(" << bNode->Name << ")\n";
    }
    // Print Internal
    Vec_PtrForEachEntry(Bn_Node *, bNtk->Node_Ary, bNode, i) {
        if ((bNode->Type != 2) && (bNode->Type != 3))
            continue;
        else {
            outFile << bNode->Name << " = ";
            switch (bNode->FType) {
                case 0:
                    outFile << "BUF(";
                    break;
                case 1:
                    outFile << "NOT(";
                    break;
                case 2:
                    outFile << "AND(";
                    break;
                case 3:
                    outFile << "OR(";
                    break;
                case 4:
                    outFile << "NAND(";
                    break;
                case 5:
                    outFile << "NOR(";
                    break;
                case 6:
                    outFile << "XOR(";
                    break;
                case 7:
                    outFile << "XNOR(";
                    break;
                case 8:
                    outFile << "MUX(";
                    break;
            }
            Vec_PtrForEachEntry(Bn_Node *, bNode->Fanin_Ary, fanin, j) {
                if (j != (bNode->Fanin_Ary->nSize - 1))
                    outFile << fanin->Name << ", ";
                else
                    outFile << fanin->Name << ")\n";
            }
        }
    }

    outFile.close();
}
