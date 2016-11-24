//Computational Fabrication Assignment #1
// Created by David Levin 2014
// Modified by Shinjiro Sueda 2016

#include <iostream>
#include <vector>
#include "../include/CompFab.h"
#include "../include/Mesh.h"

//Triangle list (global)
typedef std::vector<CompFab::Triangle> TriangleList;

TriangleList g_triangleList;
CompFab::VoxelGrid *g_voxelGrid;

bool loadMesh(char *filename, unsigned int num)
{
    g_triangleList.clear();
    
    Mesh *tempMesh = new Mesh(filename, true);
    
    CompFab::Vec3 v1, v2, v3;
    
    //copy triangles to global list
    for(unsigned int tri =0; tri<tempMesh->t.size(); ++tri)
    {
        v1 = tempMesh->v[tempMesh->t[tri][0]];
        v2 = tempMesh->v[tempMesh->t[tri][1]];
        v3 = tempMesh->v[tempMesh->t[tri][2]];
        g_triangleList.push_back(CompFab::Triangle(v1,v2,v3));
    }
    
    //Create Voxel Grid
    CompFab::Vec3 bbMax, bbMin;
    BBox(*tempMesh, bbMin, bbMax);
    
    //Build Voxel Grid
    double bbX = bbMax[0] - bbMin[0];
    double bbY = bbMax[1] - bbMin[1];
    double bbZ = bbMax[2] - bbMin[2];
    double spacing;
    
    if(bbX > bbY && bbX > bbZ)
    {
        spacing = bbX/(num-1);
    } else if(bbY > bbX && bbY > bbZ) {
        spacing = bbY/(num-1);
    } else {
        spacing = bbZ/(num-1);
    }
    
    g_voxelGrid = new CompFab::VoxelGrid(bbMin, num, num, num, spacing);
    
    delete tempMesh;
    
    return true;
    
}

void saveVoxelsToObj(const char * outfile)
{
    Mesh box;
    Mesh mout;
    int nx = g_voxelGrid->m_numX;
    int ny = g_voxelGrid->m_numY;
    int nz = g_voxelGrid->m_numZ;
    double spacing = g_voxelGrid->m_spacing;
    
    CompFab::Vec3 hspacing(0.5*spacing, 0.5*spacing, 0.5*spacing);
    
    for (int ii = 0; ii < nx; ii++) {
        for (int jj = 0; jj < ny; jj++) {
            for (int kk = 0; kk < nz; kk++) {
                if(!g_voxelGrid->isInside(ii,jj,kk)){
                    continue;
                }
                CompFab::Vec3 coord(0.5f + ((double)ii)*spacing, 0.5f + ((double)jj)*spacing, 0.5f+((double)kk)*spacing);
                CompFab::Vec3 box0 = coord - hspacing;
                CompFab::Vec3 box1 = coord + hspacing;
                makeCube(box, box0, box1);
                mout.append(box);
            }
        }
    }
    
    mout.save_obj(outfile);
}

// Code referenced is sourced in README.txt
bool triangleIntersection(CompFab::Vec3 V1, CompFab::Vec3 V2, CompFab::Vec3 V3, CompFab::Vec3 O, CompFab::Vec3 D)
{
	CompFab::Vec3 e1, e2;  //Edge1, Edge2
	CompFab::Vec3 P, Q, T;
	double det, inv_det, u, v;
	double t;

	//Find vectors for two edges sharing V1
	e1 = V2 - V1;
	e2 = V3 - V1;
	//Begin calculating determinant - also used to calculate u parameter
	P = D % e2;
	//if determinant is near zero, ray lies in plane of triangle or ray is parallel to plane of triangle
	det = e1 * P;
	//NOT CULLING
	if (det > -EPSILON && det < EPSILON) return false;
	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	T = O - V1;

	//Calculate u parameter and test bound
	u = (T * P) * inv_det;
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) return false;

	//Prepare to test v parameter
	Q = T % e1;

	//Calculate V parameter and test bound
	v = (D * Q) * inv_det;
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) return false;

	t = (e2 * Q) * inv_det;

	if (t > EPSILON) { //ray intersection
		return true;
	}

	// No hit, no win
	return false;
}

int castRays(CompFab::Vec3 t_origin)
{

	// 6 separate counter variables for the 6 raycasts. Additional coutner to count how many of them counted a voxel as being "inside"
	int rc = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0;

	// Calculating triangle intersections for each triangle with all 6 raycasts.
	for (int i = 0; i < g_triangleList.size(); i++)
	{
		if (triangleIntersection(g_triangleList.at(i).m_v1, g_triangleList.at(i).m_v2, g_triangleList.at(i).m_v3, t_origin, CompFab::Vec3(1, 0, 0)))
		{
			++c1;
		}
		if (triangleIntersection(g_triangleList.at(i).m_v1, g_triangleList.at(i).m_v2, g_triangleList.at(i).m_v3, t_origin, CompFab::Vec3(-1, 0, 0)))
		{
			++c2;
		}
		if (triangleIntersection(g_triangleList.at(i).m_v1, g_triangleList.at(i).m_v2, g_triangleList.at(i).m_v3, t_origin, CompFab::Vec3(0, 1, 0)))
		{
			++c3;
		}
		if (triangleIntersection(g_triangleList.at(i).m_v1, g_triangleList.at(i).m_v2, g_triangleList.at(i).m_v3, t_origin, CompFab::Vec3(0, -1, 0)))
		{
			++c4;
		}
		if (triangleIntersection(g_triangleList.at(i).m_v1, g_triangleList.at(i).m_v2, g_triangleList.at(i).m_v3, t_origin, CompFab::Vec3(0, 0, 1)))
		{
			++c5;
		}
		if (triangleIntersection(g_triangleList.at(i).m_v1, g_triangleList.at(i).m_v2, g_triangleList.at(i).m_v3, t_origin, CompFab::Vec3(0, 0, -1)))
		{
			++c6;
		}
	}

	// Determining which triangle intersections were valid.
	if (c1 % 2 == 1) ++rc;
	if (c2 % 2 == 1) ++rc;
	if (c3 % 2 == 1) ++rc;
	if (c4 % 2 == 1) ++rc;
	if (c5 % 2 == 1) ++rc;
	if (c6 % 2 == 1) ++rc;

	return rc;
}

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
    
    if(argc < 4)
    {
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
        for(int j = 0; j < output->v.size(); j++)
        {
            std::cout << output->v[j].m_x << " " << output->v[j].m_y << " " << output->v[j].m_z << std::endl;
            std::cout << output->t[j].m_x << " " << output->t[j].m_y << " " << output->t[j].m_z << std::endl;
            std::cout << std::endl;
        }
        output->save(argv[2]);
    }
    else
    {
        num = atoi(argv[3]);
        
        // The loadMesh() function loads the mesh and then creates:
        // - g_triangleList: The list of triangles in the input OBJ mesh
        // - g_voxelGrid: The VoxelGrid object, with all voxels marked as unoccupied
        std::cout<<"Load Mesh : "<<argv[1]<<"\n";
        loadMesh(argv[1], num);
        
        std::cout<<"Voxelizing into "<<num<<"x"<<num<<"x"<<num<<"\n";
        
        // Below, write a triple for-loop to go through all the voxels in X, Y, Z.
        //   g_voxelGrid->m_numX is the number of voxels in the X direction.
        //   g_voxelGrid->m_numY is the number of voxels in the Y direction.
        //   g_voxelGrid->m_numZ is the number of voxels in the Z direction.
        //   g_voxelGrid->m_spacing is the size of each voxel.
        //
        // Inside the triple for-loop, check if the voxel is inside or outside the
        // mesh. Use the g_voxelGrid->isInside(...) method to set whether that voxel
        // is inside or outside. E.g.,
        //   g_voxelGrid->isInside(0,0,0) = false;

        // <TRIPLE FOR-LOOP HERE>

        // Temp varaible for the current corner
        CompFab::Vec3 t_corner = g_voxelGrid->m_corner;

        // Temp varaible for the current center
        CompFab::Vec3 t_origin;

        for (unsigned int i_x = 0; i_x < g_voxelGrid->m_numX; i_x++)
        {
            for (unsigned int i_y = 0; i_y < g_voxelGrid->m_numY; i_y++)
            {
                for (unsigned int i_z = 0; i_z < g_voxelGrid->m_numZ; i_z++)
                {

                    // Finding the corner of the current voxel.
                    t_corner.m_x = g_voxelGrid->m_corner.m_x + i_x*g_voxelGrid->m_spacing;
                    t_corner.m_y = g_voxelGrid->m_corner.m_y + i_y*g_voxelGrid->m_spacing;
                    t_corner.m_z = g_voxelGrid->m_corner.m_z + i_z*g_voxelGrid->m_spacing;

                    // Finding the origin of the current voxel.
                    t_origin.m_x = t_corner.m_x + 0.5*g_voxelGrid->m_spacing;
                    t_origin.m_y = t_corner.m_y + 0.5*g_voxelGrid->m_spacing;
                    t_origin.m_z = t_corner.m_z + 0.5*g_voxelGrid->m_spacing;

                    // Will say "inside" if 4 or more of the raycasts determine a voxel to be inside.
                    g_voxelGrid->isInside(i_x, i_y, i_z) = (castRays(t_origin) >= 4);
                }
            }
        }
        
        // Write out voxel data as obj
        saveVoxelsToObj(argv[2]);
    }
    
    delete g_voxelGrid;
}
