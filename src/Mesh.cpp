//
// Created by edai on 21/05/18.
//

#include "Application.hpp"
#include "Mesh.hpp"


void Mesh::LoopSubdivisionOneStep()
{
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

void Mesh::Init()
{
    glist = glGenLists(1);
    glNewList(glist, GL_COMPILE);
    {
        glPushMatrix();
        for (int i = 0; i < faces.size(); i++)
        {
            glBegin(GL_TRIANGLES);
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
    glTranslatef(0, -2.0f, 5.5f);
    glColor3f(1.0, 0.33, 0.33);
    glScalef(0.05,0.05,0.05);
    glRotatef(rot, 0.0,1.0,0.0);
    glCallList(glist);
    glPopMatrix();
    rot = rot + 0.6f;
    if (rot > 360)
        rot -= 360;
}
