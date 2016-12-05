// Procedural City Generator
// Alex Huddleston
// 2016

#include <iostream>
#include <string.h>
#include <vector>
#include "../include/CompFab.h"
#include "../include/Mesh.h"
#include "../include/ppm.h"
#include "../include/PerlinNoise.h"

#include <math.h>

#define PI 3.14159265


double distanceA2B(double ax, double ay, double bx, double by){
  return sqrt((bx-ax)*(bx-ax) + (by-ay)*(by-ay));
}

double jerfunc(double x, double y, PerlinNoise pnoise){
  double n = 0;
  double m = 0;

  //perlin noise
  n = 20 * pnoise.noise(x,y,0.8);
  n = n - floor(n);

  //Central distrobution
  //distance from point to center
  m = distanceA2B(x,y,0.5,0.5);

  return (m*0.15)+(n*0.85);

}

//A function to find the X and Y dimensions of the template obj
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
std::vector<CompFab::Vec3> createVec3d(int layers, double spacing, double length, double width)
{
    std::vector<CompFab::Vec3> *output = new std::vector<CompFab::Vec3>();
    
    double ls = length + spacing;
    double ws = width + spacing;
    
    // Will be used later to determine the direction of the translation matrix.
    // This is used to bypass needing to create a rotation matrix.
    // Should consider doing so anyway to speed up process, use less memory, and add modularization.
    double angle = 0.0;
    
    CompFab::Vec3 *temp = new CompFab::Vec3(-ls, -ws, 0);
    
    // Vec3 to hold our current translation matrix.
    CompFab::Vec3 *trans = new CompFab::Vec3(0, spacing, 0); 

    CompFab::Vec3 *coord = new CompFab::Vec3(0.5,0.5,0);
    
    // cl for current layer.
    for(int cl = 1; cl < layers; cl++)
    {
        // Constructor used to bypass needing to create a new operator override for multiplication.
        // Should also consider doing so anyway to speed up process, use less memory, and add modularization.
        *temp = CompFab::Vec3(-ls*cl, -ws*cl, 0);
        
        for(int c = 0; c < cl*8; c++)
        {
            angle = (c/(2*cl))*(0.5*PI);
            *trans = CompFab::Vec3(ls*cos(angle), ws*sin(angle), 0);
            *temp = *temp + *trans;
            
            output->push_back(*temp);

            *coord = *coord + mmult(trans, Vec3(1/(layers*2-1)/2),1/(layers*2-1)/2,0);


            *temp=mmult(temp,Vec3(1,1,n));
        }
    }
    
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
    
    // TODO: Modularize these.
    int layers = 10;
    double spacing = 1.0;
    
    // Create Mesh object from file, output to manipulate from template Mesh.
    Mesh *test = new Mesh(argv[1], false);
    Mesh *output = new Mesh(test->v, test->t);
    
    double l = 0, w = 0;
    double *length = &l, *width = &w;
    
    // Find the X and Y dimensions for the mesh. Assumes the mesh is facing upright.
    findLW(*test, *length, *width);
    
    // Calculate the translation matrices needed.
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
    for(int n = 1; n < pow((2*layers - 1), 2); n++)
    {
        int offset = test->v.size()*n;
        for(int k = 0; k < test->t.size(); k++)
        {
            output->t.push_back(CompFab::Vec3i(test->t[k].m_x +offset, test->t[k].m_y + offset, test->t[k].m_z + offset));
        }
    }
    
    // Debugging
    if(argc > 3)
    {
      
       // {
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
