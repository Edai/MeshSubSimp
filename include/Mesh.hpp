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

public:
    void Load(const char* fileName);
    void Save(const char* fileName);

    void LoopSubdivisionOneStep();
    void Simplification();

};


#endif //MESHSUBSIMP_MESH_HPP
