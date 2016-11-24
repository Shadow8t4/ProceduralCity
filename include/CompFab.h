//
//  CompFab.h
//  voxelizer
//
//  Created by David Levin on 2/3/14.
//  Modified by Shinjiro Sueda on 8/20/16.
//
//

#ifndef voxelizer_CompFab_h
#define voxelizer_CompFab_h

#define EPSILON 1e-9

#include <cmath>

namespace CompFab
{
    
    //Data Types
    typedef struct Vec3Struct
    {
        
        Vec3Struct();
        Vec3Struct(double x, double y, double z);
        
        union
        {
            double m_pos[3];
            struct { double m_x,m_y,m_z; };
        };
        
        inline double & operator[](unsigned int index) { return m_pos[index]; }
        inline const double & operator[](unsigned int index) const { return m_pos[index]; }
        inline void operator+=(const Vec3Struct &a)
        {
            m_x += a.m_x;
            m_y += a.m_y;
            m_z += a.m_z;
        }
        
        void normalize();
        
    }Vec3;
    
    //Data Types
    typedef struct Vec3iStruct
    {
        
        Vec3iStruct();
        Vec3iStruct(int x, int y, int z);
        union
        {
            int m_pos[3];
            struct {int m_x,m_y,m_z;};
        };
        
        inline int & operator[](unsigned int index) { return m_pos[index]; }
        inline const int & operator[](unsigned int index) const { return m_pos[index]; }
        
    }Vec3i;
    
    //Data Types
    typedef struct Vec2fStruct
    {
        
        Vec2fStruct();
        Vec2fStruct(float x, float y);
        
        union
        {
            float m_pos[2];
            struct { float m_x,m_y; };
        };
        
        inline float & operator[](unsigned int index) { return m_pos[index]; }
        inline const float & operator[](unsigned int index) const { return m_pos[index]; }
        
    }Vec2f;
    
    
    //NOTE: Ray direction must be normalized
    typedef struct RayStruct
    {
        
        RayStruct();
        RayStruct(Vec3 &origin, Vec3 &direction);
        
        Vec3 m_origin;
        Vec3 m_direction;
        
    } Ray;
    
    typedef struct TriangleStruct
    {
        
        TriangleStruct(Vec3 &v1, Vec3 &v2,Vec3 &v3);
        
        Vec3 m_v1, m_v2, m_v3;
        
    } Triangle;
    
    //Some useful operations
    //Compute v1 - v2
    Vec3 operator-(const Vec3 &v1, const Vec3 &v2);
    
    Vec3 operator+(const Vec3 &v1, const Vec3 &v2);
    
    //Cross Product
    Vec3 operator%(const Vec3 &v1, const Vec3 &v2);
    
    //Dot Product
    double operator*(const Vec3 &v1, const Vec3 &v2);
    
    
    //Grid structure for Voxels
    typedef struct VoxelGridStruct
    {
        //Square voxels only
        VoxelGridStruct(Vec3 lowerLeft, unsigned int dimX, unsigned int dimY, unsigned int dimZ, double spacing);
        virtual ~VoxelGridStruct();
        
        inline bool & isInside(unsigned int i, unsigned int j, unsigned int k)
        {
            return m_insideArray[k*(m_numX*m_numY)+j*m_numY + i];
        }
        
        unsigned int m_numX, m_numY, m_numZ; // Number of voxels in each direction
        unsigned int m_size; // Total number of voxels
        bool *m_insideArray; // An array of all voxels
        double m_spacing; // spacing between voxels
        Vec3 m_corner; // corner of the whole grid with minimum x, minimum y, and minimum z
        
    } VoxelGrid;
}



#endif
