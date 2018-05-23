//
// Created by edai on 19/03/18.
//

#ifndef ASSIGNMENT_APPLICATION_HPP
#define ASSIGNMENT_APPLICATION_HPP

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#include <iostream>
#include <SOIL/SOIL.h>
#include <unistd.h>
#include <cstdlib>
#include <getopt.h>
#include <string>

#define DEFAULT_WIDTH 960
#define DEFAULT_HEIGHT 960
#define WINDOWPOS_X 500
#define WINDOWPOS_Y 50
#define WINDOW_TITLE "Valentin KAO - 2017280242"
#define DEFAULT_OBJ "resources/kitten.obj"
#define DEFAULT_ITERATIONS 2

struct Options
{
    std::string window_name;
    int         width;
    int         height;
    unsigned int        iterations;
    std::string obj_file;

public:
    Options()
    {
        width = DEFAULT_WIDTH;
        height = DEFAULT_HEIGHT;
        iterations = DEFAULT_ITERATIONS;
        obj_file = DEFAULT_OBJ;
    }
};

class Application
{
public:
    static bool Parse(Options *, int, char**);
    static int Start(int, char **);
    static int MeshProcess(Options *op);

protected:
    Application();
    ~Application();
};


#endif //ASSIGNMENT2_APPLICATION_HPP
