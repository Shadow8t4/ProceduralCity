// Procedural City Generator
// Alex Huddleston
// 2016

#include <iostream>
#include <string.h>
#include <vector>
#include "../include/CompFab.h"
#include "../include/Mesh.h"
#include <math.h>

#define PI 3.14159265

void findLW(Mesh &m, double &l, double &w)
{
    double minl, maxl, minw, maxw;
    for(int i = 0; i < m.t.size(); i++)
    {
        if(m.v[i].m_x < minl)
        {
            minl = m.v[i].m_x;
        }
        if(m.v[i].m_x > maxl)
        {
            maxl = m.v[i].m_x;
        }
        if(m.v[i].m_y < minw)
        {
            minw = m.v[i].m_y;
        }
        if(m.v[i].m_y > maxw)
        {
            maxw = m.v[i].m_y;
        }
    }
    l = maxl - minl;
    w = maxw - minw;
}

// Calculate translation matrices and add them to the vector of translation matrices provided.
std::vector<CompFab::Vec3> createVec3d(int layers, double spacing, double length, double width)
{
    std::vector<CompFab::Vec3> *output = new std::vector<CompFab::Vec3>();
    
    CompFab::Vec3 *temp = new CompFab::Vec3(-(length + spacing), -(width + spacing), 0);
    CompFab::Vec3 *trans = new CompFab::Vec3(0, spacing, 0);
    
    for(int cl = 1; cl < layers; cl++)
    {
        *temp = CompFab::Vec3(-(length + spacing)*cl, -(width + spacing)*cl, 0);
        
        for(int c = 0; c < cl*8; c++)
        {
            /*
            double cosine = cos(((2*PI)/(cl*8))*c);
            double sine = sin(((2*PI)/(cl*8))*c);
            //
            double xcoord = 1;
            double angle = tan((2*PI)/(cl*8))*c;
            double ycoord = 1;
            if(cosine != 0)
            {
                ycoord = sine/cosine;
            }
            if(sine != 0)
            {
                xcoord = cosine/sine;
            }
            //
            double x = cosine;//(sqrt(1 - (sine*sine)/2));
            if(x > 0)
            {
                x = floor(x);
            }
            else
            {
                x = ceil(x);
            }
            double y = sine;//(sqrt(1-(cosine*cosine)/2));
            if(y > 0)
            {
                y = floor(x);
            }
            else
            {
                y = ceil(x);
            }
            */
            *trans = CompFab::Vec3(spacing*cos(floor((c/(2*cl)))*(0.5*PI)) + length*cos(floor((c/(2*cl)))*(0.5*PI)), spacing*sin(floor((c/(2*cl)))*(0.5*PI)) + width*sin(floor((c/(2*cl)))*(0.5*PI)), 0);
            *temp = *temp + *trans;
            
            output->push_back(*temp);
        }
    }
    
    /*
    temp->push_back(CompFab::Vec3(length,0,0));
    temp->push_back(CompFab::Vec3(length,width,0));
    temp->push_back(CompFab::Vec3(0,width,0));
    temp->push_back(CompFab::Vec3(-length,width,0));
    temp->push_back(CompFab::Vec3(-length,0,0));
    temp->push_back(CompFab::Vec3(-length,-width,0));
    temp->push_back(CompFab::Vec3(0,-width,0));
    temp->push_back(CompFab::Vec3(length,-width,0));
    */
    
    return *output;
}

int main(int argc, char **argv)
{
    
    // Error checking.
    if(argc < 3)
    {
        std::cout << "Usage: [executable] [template].obj output.obj [optional: -d for debugging output]" << std::endl;
        std::exit(1);
    }
    
    // Modularize this later.
    double spacing = 1.0;
    
    // Create Mesh object from file, output to manipulate from template Mesh.
    Mesh *test = new Mesh(argv[1], false);
    Mesh *output = new Mesh(test->v, test->t);
    
    int layers = 10;
    double l = 0, w = 0;
    double *length = &l, *width = &w;
    
    // Find dimensions for the mesh. Assumes the mesh is facing upright.
    findLW(*test, *length, *width);
    
    std::vector<CompFab::Vec3> d = createVec3d(layers, spacing, *length, *width);
    
    // Duplicating template, will later be replaced with a much more robust procedural generation function.
    for(int i = 0; i < d.size(); i++)
    {
        for(int j = 0; j < test->v.size(); j++)
        {
            output->v.push_back(CompFab::Vec3(test->v[j] + d[i]));
        }
    }
    
    // Copying needed triangle data.
    for(int n = 1; n < (2*layers - 1)*(2*layers - 1); n++)
    {
        for(int k = 0; k < test->t.size(); k++)
        {
            output->t.push_back(CompFab::Vec3i(test->t[k].m_x + test->v.size()*n, test->t[k].m_y + test->v.size()*n, test->t[k].m_z + test->v.size()*n));
        }
    }
    
    // Debugging
    if(argc > 3)
    {
        if(strcmp(argv[3], "-d") == 0)
        {
            for(int j = 0; j < output->v.size(); j++)
            {
                std::cout << output->v[j].m_x << " " << output->v[j].m_y << " " << output->v[j].m_z << std::endl;
                std::cout << output->t[j].m_x << " " << output->t[j].m_y << " " << output->t[j].m_z << std::endl;
                std::cout << std::endl;
            }
        }
        else
        {
            std::cout << "Usage: [executable] [template].obj output.obj [optional: -d for debugging output]" << std::endl;
        }
    }
    
    output->save(argv[2]);
    
    std::cout << "Success." << std::endl;
}
