//
// Created by edai on 21/05/18.
//

#include "Application.hpp"
#include "Mesh.hpp"

unsigned Mesh::GetFourthVertex(unsigned f, unsigned i0, unsigned i1)
{
    for (auto &index : edge2Faces.find(std::make_pair(i0, i1)) == edge2Faces.end() ?
                       edge2Faces[std::make_pair(i1, i0)] : edge2Faces[std::make_pair(i0, i1)])
    {
        for (unsigned i = 0; index != f && i < 3; i++)
        {
            unsigned n = faces[index].verts[i];
            if (n != i0 && n != i1)
                return n;
        }
    }
    return (0);
}

Vertex* Mesh::GetOddVertices(unsigned f, unsigned i0, unsigned i1, unsigned i2)
{
    Vertex *v = new Vertex();
    Vertex &v0 = verts[i0];
    Vertex &v1 = verts[i1];

    if (v0.isBoundary && v1.isBoundary)
    {
        v->pos = (v0.pos + v1.pos) * 0.5f;
        v->isBoundary = true;
    }
    else
    {
        unsigned i3 = GetFourthVertex(f, i0, i1);
        Vertex &v2 = verts[i2];
        Vertex &v3 = verts[i3];
        v->pos = 3.0/8.0 * (v0.pos + v1.pos) + 1.0/8.0 * (v2.pos + v3.pos);
    }
    //add faces
    v->adjVerts.push_back(i0);
    v->adjVerts.push_back(i1);
    return (v);
}

void Mesh::LoopSubdivisionOneStep()
{
    Mesh *simplified_mesh = new Mesh();
    std::vector<Vertex> *odd_vertexes = new std::vector<Vertex>();

    for (int i = 0; i < faces.size(); i++)
    {
        Face &f = faces[i];
        for (auto &v : f.verts)
        {
            odd_vertexes->push_back(*(GetOddVertices(i, f.verts[0], f.verts[1], f.verts[2])));
            odd_vertexes->push_back(*(GetOddVertices(i, f.verts[0], f.verts[2], f.verts[1])));
            odd_vertexes->push_back(*(GetOddVertices(i, f.verts[1], f.verts[2], f.verts[0])));
        }
    }
    delete odd_vertexes;
    return;
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
        for (std::list<unsigned>::iterator iter = verts[i].adjVerts.begin();
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
