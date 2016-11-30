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

// A function to find the X and Y dimensions of the template obj
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

// Calculate translation matrices and output them as a vector of Vec3s.
std::vector<CompFab::Vec3> createVec3d(std::vector<CompFab::Vec3> &t, int layers, double spacing, double length, double width)
{
    std::vector<CompFab::Vec3> *output = new std::vector<CompFab::Vec3>();
    
    double ls = length + spacing;
    double ws = width + spacing;
    
    // Will be used later to determine the direction of the translation matrix.
    double angle = 0.0;
    
    CompFab::Vec3 *temp = new CompFab::Vec3(-ls, -ws, 0);
    
    // Vec3 to hold our current translation matrix.
    CompFab::Vec3 *trans = new CompFab::Vec3(ls, ws, 0);
    
    //Vec3 to hold our current height multiplication materx.
    CompFab::Vec3 *mult = new CompFab::Vec3(1,1,1);
    
    //push_back initial templat's mesh, multiplied by the height multiplication matrix.
    for(int i = 0; i < t.size(); i++)
    {
        output->push_back(mmult(*mult, t[i]));
    }
    
    // cl for current layer.
    for(int cl = 1; cl < layers; cl++)
    {
        // Constructor used to re-initialize temp.
        *temp = CompFab::Vec3(-ls*cl, -ws*cl, 0);
        
        // Used to change the multiplciation matrix per layer.
        mult->m_z = 1.0/(cl +1);
        
        
        for(int c = 0; c < cl*8; c++)
        {
            angle = (c/(2*cl))*(0.5*PI);
            *trans = CompFab::Vec3(ls*cos(angle), ws*sin(angle), 0);
            *temp += *trans;
            for(int j = 0; j < t.size(); j++)
            {
                output->push_back(mmult(*mult, t[j]) + *temp);
            }
        }
    }
    
    return *output;
}
std::vector<CompFab::Vec3i> createVec3id(std::vector<CompFab::Vec3i> &t, std::vector<CompFab::Vec3> &v, int layers)
{
    std::vector<CompFab::Vec3i> *output = new std::vector<CompFab::Vec3i>();
    *output = t;
    
    for(int n = 1; n < pow((2*layers - 1), 2); n++)
    {
        CompFab::Vec3i *offset = new CompFab::Vec3i(v.size()*n);
        for(int k = 0; k < t.size(); k++)
        {
            output->push_back(t[k] + *offset);
        }
    }
    
    return *output;
}

int main(int argc, char **argv)
{
    
    // Error checking.
    if(argc < 3)
    {
        std::cout << "Usage: [executable] [template].obj output.obj [mode (-g, etc.)] [optional: -d for debugging output]" << std::endl;
        std::exit(1);
    }
    
    // Initialization of needed output variables.
    std::vector<CompFab::Vec3> vv;
    std::vector<CompFab::Vec3i> tt;
    Mesh *output = new Mesh();
    
    // Debugging
    if(argc > 3)
    {
        if(strcmp(argv[3], "-g") == 0 || strcmp(argv[3], "-d") == 0)
        {
            // TODO: Modularize these.
            int layers = 10;
            double spacing = 1;
            
            // Create Mesh object from file, output to manipulate from template Mesh.
            Mesh *test = new Mesh(argv[1], false);
            
            double l = 0, w = 0;
            double *length = &l, *width = &w;
            
            // Find the X and Y dimensions for the mesh. Assumes the mesh is facing upright.
            findLW(*test, *length, *width);
            
            // Calculate the translation matrices needed and apply them.
            vv = createVec3d(test->v, layers, spacing, *length, *width);
            
            // Copying needed triangle data.
            tt = createVec3id(test->t, test->v, layers);
            
            // Using contructor to create new output Mesh.
            *output = Mesh(vv, tt);
        }
        
        if(strcmp(argv[argc - 1], "-d") == 0)
        {
            for(int j = 0; j < output->v.size(); j++)
            {
                std::cout << output->v[j].m_x << " " << output->v[j].m_y << " " << output->v[j].m_z << std::endl;
            }
            std::cout << std::endl;
            for(int k = 0; k < output->t.size(); k++)
            {
                std::cout << output->t[k].m_x << " " << output->t[k].m_y << " " << output->t[k].m_z << std::endl;
            }
        }
    }
    
    output->save(argv[2]);
    
    std::cout << "Success." << std::endl;
}
