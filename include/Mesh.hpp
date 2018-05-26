//
// Created by edai on 21/05/18.
//

#ifndef MESHSUBSIMP_MESH_HPP
#define MESHSUBSIMP_MESH_HPP

#include "SimpleMesh.h"

class Mesh
{
public:
    typedef std::map< std::pair<unsigned, unsigned>, std::vector<unsigned>> Edge2Faces;
    std::vector<Vertex> verts;
    std::vector<Face> faces;
    Edge2Faces edge2Faces;
    unsigned int glist;

public:
    void Init();
    void Render();

    void Load(const char* fileName);
    void Save(const char* fileName);
    Vector3D GetNormalTriangle(Face &);
    Vertex *GetOddVertex(unsigned, unsigned, std::vector<unsigned> &);
    void GetEvenVertex(unsigned i0);
    void LoopSubdivisionOneStep();
    void Simplification();
    void ResetComputing();
    unsigned int odd_GetFourthVertex(unsigned f, unsigned int i0, unsigned int i1);
};


#endif //MESHSUBSIMP_MESH_HPP
