// Procedural City Generator
// Alex Huddleston
// 2016

#include <iostream>
#include <string.h>
#include <vector>
#include "../include/CompFab.h"
#include "../include/Mesh.h"

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
            minw = m.v[i].m_x;
        }
        if(m.v[i].m_y > maxw)
        {
            maxw = m.v[i].m_x;
        }
    }
    l = maxl - minl;
    w = maxw - minw;
}

int main(int argc, char **argv)
{
    unsigned int num = 16; //number of voxels (e.g. 16x16x16)
    
    if(argc < 3)
    {
        std::cout << "Usage: [executable] [template].obj output.obj [optional: -d for debugging output]" << std::endl;
        std::exit(1);
    }
    Mesh *test = new Mesh(argv[1], false);
    Mesh *output = new Mesh(test->v, test->t);
    double l = 0, w = 0;
    double *length = &l, *width = &w;
    findLW(*test, *length, *width);
    for(int i = 0; i < test->v.size(); i++)
    {
        output->v.push_back(*new CompFab::Vec3(test->v[i].m_x + *length + 1, test->v[i].m_y, test->v[i].m_z));
    }
    for(int k = 0; k < test->t.size(); k++)
    {
        output->t.push_back(*new CompFab::Vec3i(test->t[k].m_x + test->v.size(), test->t[k].m_y + test->v.size(), test->t[k].m_z + test->v.size()));
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
