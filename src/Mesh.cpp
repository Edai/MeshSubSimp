//
// Created by edai on 21/05/18.
//

#include "Application.hpp"
#include "Mesh.hpp"
#include <algorithm>


float beta(int n)
{
    if (n == 3)
        return (3.0f / 16.0f);
    else
        return (3.0f / (8.0f * n));
}

Vertex* Mesh::GetEvenVertex(unsigned i0, unsigned i1, std::vector<unsigned> &f)
{
    Vertex *v = &verts[i0];
    v->isComputed = true;

    if (v->isBoundary)
    {
        v->pos = 1.0/8.0  * (verts[i0].pos  + verts[i1].pos) +
                3.0/4.0 * v->pos;
    }
    else
    {
        float sum_beta = 0;
        for (auto i : v->adjVerts)
            sum_beta += beta(verts[i].adjVerts.size());
        v->pos *= (1.0 - v->adjVerts.size() * beta(v->adjVerts.size())) +
                sum_beta * beta(v->adjVerts.size());
    }
    return (v);
}

unsigned GetIndex(Face &f, unsigned i)
{
    auto it = std::find(f.verts, f.verts + 3, i);
    if (it != f.verts + 3)
    {
        std::cout << std::abs(std::distance(f.verts, it)) <<  " ";
        return std::abs(std::distance(f.verts, it));
    }
    return 42;
}

unsigned GetPosition(Face &face, unsigned i0, unsigned i1)
{
    unsigned indArr0 = GetIndex(face, i0);
    unsigned indArr1 = GetIndex(face, i1);
    std::cout << std::endl;
    if (indArr0 == 0 && indArr1 == 1)
        return (2);
    else if (indArr0 == 0 && indArr1 == 2)
        return (1);
    else if (indArr0 == 1 && indArr1 == 2)
        return (0);
}

Vertex* Mesh::GetOddVertex(unsigned i0, unsigned i1, std::vector<unsigned> &f)
{
    Vertex *v = new Vertex();
    Vertex &v0 = verts[i0];
    Vertex &v1 = verts[i1];

    if (v0.isBoundary && v1.isBoundary)
    {
        v->pos = (v0.pos + v1.pos) * 0.5f;
        v->isBoundary = true;
        if (f.size() != 1)
            std::cerr << "ERROR" << std::endl;
        faces[f[0]].oddsVerts[GetPosition(faces[f[0]], i0, i1)] = v;
    }
    else
    {
        if (f.size() != 2)
            std::cerr << "ERROR" << std::endl;
        unsigned i2 = faces[f[0]].vertOppositeTo(i0, i1);
        unsigned i3 = faces[f[1]].vertOppositeTo(i0, i1);
        Vertex &v2 = verts[i2];
        Vertex &v3 = verts[i3];
        v->pos = 3.0/8.0 * (v0.pos + v1.pos) + 1.0/8.0 * (v2.pos + v3.pos);
       // faces[f[0]].oddsVerts[GetPosition(faces[f[0]], i0, i1)] = v;
        std::cout << GetPosition(faces[f[0]], i0, i1) << std::endl;
      //  faces[f[1]].oddsVerts[GetPosition(faces[f[1]], i0, i1)] = v;
        std::cout << GetPosition(faces[f[1]], i0, i1) << std::endl;
    }
    v->adjVerts.push_back(i0);
    v->adjVerts.push_back(i1);
    return (v);
}

void Mesh::LoopSubdivisionOneStep()
{
    // Computer all odd vertices for each edge
    for (auto &e : edge2Faces)
    {
        unsigned i0 = e.first.first;
        unsigned i1 = e.first.second;
        verts.emplace_back(*GetOddVertex(i0, i1, e.second));
    }
    for (auto &f : faces)
    {
//        std::cout << f.oddsVerts[0] << " " << f.oddsVerts[1] << " " << f.oddsVerts[2] << std::endl;
    }
    for (auto &e : edge2Faces)
    {
        unsigned i0 = e.first.first;
        unsigned i1 = e.first.second;
        GetEvenVertex(i0, i1, e.second);
    }
}

void Mesh::Simplification()
{
    return;
}

void Mesh::Load(const char* fileName)
{
    std::ifstream input(fileName);
    std::stringstream sin;
    std::string curLine;
    Vector3D p;
    unsigned v[3];
    unsigned tcoord[3];
    char t;

    while (getline(input, curLine))
    {
        if (curLine.substr(0, 2) == "v ")
        {
            sin.clear(); sin << curLine;
            sin >> t >> p.x >> p.y >> p.z;
            Vertex v;
            v.pos = p; v.isBoundary = false;
            verts.push_back(v);
        }
        else if (curLine.substr(0, 2) == "f ")
        {
            if (curLine.find("/") != std::string::npos)
                sscanf(curLine.c_str(), "f %d/%d %d/%d %d/%d", &v[0], &tcoord[0], &v[1], &tcoord[1], &v[2], &tcoord[2]);
            else
                sscanf(curLine.c_str(), "f %d %d %d", &v[0], &v[1], &v[2]);

            --v[0]; --v[1]; --v[2];
            Face f; f.verts[0] = v[0]; f.verts[1] = v[1]; f.verts[2] = v[2];
            faces.push_back(f);

            verts[v[0]].adjVerts.push_back(v[1]); verts[v[0]].adjVerts.push_back(v[2]);
            verts[v[1]].adjVerts.push_back(v[0]); verts[v[1]].adjVerts.push_back(v[2]);
            verts[v[2]].adjVerts.push_back(v[0]); verts[v[2]].adjVerts.push_back(v[1]);

            for (unsigned i = 0; i < 3; ++i)
            {
                unsigned v0 = fmin(v[i], v[(i+1)%3]);
                unsigned v1 = fmax(v[i], v[(i+1)%3]);
                Edge2Faces::iterator iter = edge2Faces.find(std::make_pair(v0, v1));
                if (iter == edge2Faces.end())
                {
                    std::vector<unsigned> curFaces;
                    curFaces.push_back(faces.size()-1);
                    edge2Faces[std::make_pair(v0, v1)] = curFaces;
                }
                else
                {
                    edge2Faces[std::make_pair(v0, v1)].push_back(faces.size()-1);
                }
            }
        }
    }

    for (unsigned i = 0; i < verts.size(); ++i)
    {
        std::set<unsigned> rmRedunt;
        for (std::vector<unsigned>::iterator iter = verts[i].adjVerts.begin();
             iter != verts[i].adjVerts.end(); ++iter)
            rmRedunt.insert(*iter);
        verts[i].adjVerts.clear();
        for (std::set<unsigned>::iterator iter = rmRedunt.begin();
             iter != rmRedunt.end(); ++iter)
            verts[i].adjVerts.push_back(*iter);
    }

    for (Edge2Faces::iterator iter = edge2Faces.begin(); iter != edge2Faces.end(); ++iter)
    {
        if (iter->second.size() == 1)
        {
            verts[iter->first.first].isBoundary = true;
            verts[iter->first.second].isBoundary = true;
        }
    }

}

void Mesh::Save(const char* fileName)
{
    std::ofstream output(fileName);
    for (unsigned i = 0; i < verts.size(); ++i)
        output << "v " << verts[i].pos.x << " " << verts[i].pos.y << " " << verts[i].pos.z << std::endl;
    for (unsigned i = 0; i < faces.size(); ++i)
        output << "f " << faces[i].verts[0]+1 << " " << faces[i].verts[1]+1 << " " << faces[i].verts[2]+1 << std::endl;
    output.close();
}

Vector3D Mesh::GetNormalTriangle(Face &f)
{
    Vector3D u = verts[f.verts[1]].pos - verts[f.verts[0]].pos;
    Vector3D v = verts[f.verts[2]].pos - verts[f.verts[0]].pos;

    return (u ^ v);
}

void Mesh::Init()
{
    glist = glGenLists(1);
    glNewList(glist, GL_COMPILE);
    {
        glPushMatrix();
        for (int i = 0; i < faces.size(); i++)
        {
            glBegin(GL_TRIANGLES);
            auto n = GetNormalTriangle(faces[i]);
            glNormal3f(n.x, n.y, n.z);
            for (int j = 0; j < 3; j++)
                glVertex3f(verts[faces[i].verts[j]].pos.x, verts[faces[i].verts[j]].pos.y, verts[faces[i].verts[j]].pos.z);
        }
        glEnd();
    }
    glPopMatrix();
    glEndList();
}

void Mesh::Render()
{
    static GLfloat rot = 0.0;

    glPushMatrix();
    glTranslatef(0, -2.0f, 6.5f);
    glColor4f(1.0, 0.33, 0.33, 1.0);
    glScalef(0.05,0.05,0.05);
    glRotatef(rot, 0.0,1.0,0.0);
    glCallList(glist);
    glPopMatrix();

    rot = rot + 0.6f;
    if (rot > 360)
        rot -= 360;
}
//        Face &f = faces[i];
//        unsigned indexes[] = {f.verts[0], f.verts[1], f.verts[2]};
//        Face nfaces[3];
//        if (!f.isComputed)
//        {
//            oddVex[0] = GetOddVertex(i, f.verts[0], f.verts[1], f.verts[2]);
//            oddVex[1] = GetOddVertex(i, f.verts[0], f.verts[2], f.verts[1]);
//            oddVex[2] = GetOddVertex(i, f.verts[1], f.verts[2], f.verts[0]);
//
//            if (!verts[f.verts[0]].isComputed)
//                evenVex[0] = GetEvenVertex(i, f.verts[0], f.verts[1], f.verts[2]);
//            if (!verts[f.verts[1]].isComputed)
//                evenVex[1] = GetEvenVertex(i, f.verts[1], f.verts[0], f.verts[2]);
//            if (!verts[f.verts[2]].isComputed)
//                evenVex[2] = GetEvenVertex(i, f.verts[2], f.verts[0], f.verts[1]);
//            verts.push_back(*oddVex[0]); // verts.size() - 3
//            verts.push_back(*oddVex[1]); // verts.size() - 2
//            verts.push_back(*oddVex[2]); // verts.size() - 1
//
//            ApplyIndexesFaces(f, indexes[0], u1, u0);
//            ApplyIndexesFaces(nfaces[0], indexes[2], u1, u2);
//            ApplyIndexesFaces(nfaces[1], u0, u1, u2);
//            ApplyIndexesFaces(nfaces[2], indexes[1], u0, u2);
//            faces.push_back(nfaces[0]);
//            faces.push_back(nfaces[1]);
//            faces.push_back(nfaces[2]);