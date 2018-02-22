#ifndef MARCHINGCUBE_H
#define MARCHINGCUBE_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>

#include <QOpenGLWidget>
#include <QResizeEvent>
#include <QEvent>

#include "signed_distance_field_from_mesh.hpp"


/// @author Xiasong Yang
/// @brief Holds vertex data
struct VertData
{
  float nx; // normal from obj mesh
  float ny;
  float nz;
  float x; // position from obj
  float y;
  float z;
};

/// @author Xiasong Yang
/// @brief Holds triangle points
typedef struct {
   glm::vec3 p[3];
} TRIANGLE;

/// @author Xiasong Yang
/// @brief Holds grid data used for marching cubes
typedef struct {
   glm::vec3 p[8];
   double val[8];
} GRIDCELL;


class MarchingCube
{

public:

    /// @brief Constructor initialising number of dynamic and static objects
    /// @author Kate Edge
    MarchingCube(int noDynamic, int noStatic);

    /// @brief Destructor
    /// @author Kate Edge
    ~MarchingCube();

    double isolevel;
    glm::vec3 p1,p2;
    double valp1,valp2;

    /// @brief Interpolate between vertices
    /// @author Xiasong Yang
    glm::vec3 VertexInterp(double isolevel,glm::vec3 p1,glm::vec3 p2,double valp1,double valp2);

    /// @brief The volume data dimension
    //----------------------------------------------------------------------------------------------------------------------
    unsigned int    volume_width;
    unsigned int    volume_height;
    unsigned int    volume_depth;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The volume data
    //----------------------------------------------------------------------------------------------------------------------
    float           *volumeData;
    unsigned int m_volume_size;
    //----------------------------------------------------------------------------------------------------------------------   
    /// @brief Polygonize prepared isosurfaces
    /// @author Xiasong Yang
    void Polygonize(int modelNo, bool _static);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The number of vertices in the object
    //----------------------------------------------------------------------------------------------------------------------
    unsigned long int m_nVerts;
    //----------------------------------------------------------------------------------------------------------------------
    /// @author Xiasong Yang
    /// Implements marching cubes on the isosurface
    unsigned int MarchingTriangles(GRIDCELL g, float iso, std::vector<TRIANGLE> &triList);
    //----------------------------------------------------------------------------------------------------------------------
    /// @author Xiasong Yang
    /// Computes the normal from 3 vertices of a triange
    glm::vec3 computeTriangleNormal(TRIANGLE &itr);
    //----------------------------------------------------------------------------------------------------------------------
    /// @author Kate Edge
    /// Prepares the sdf volume for marching cubes
    bool PrepareVolume(int meshNo, bool _static);
    //----------------------------------------------------------------------------------------------------------------------
    std::vector <VertData> m_vboMesh;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Holds the triangle normal as a vector
    glm::vec3 m_triNormal;
    //----------------------------------------------------------------------------------------------------------------------
    /// @briaf Holds vertices of the polygonized meshes
    std::vector<float> m_verts;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Holds the normals of the vertices of the polygonized meshes
    std::vector<float> m_vertsNormal;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The offset value applied to the meshes
    float m_offset;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief An array of the vertices of the meshes at each offset level, used for quickly updating offset
    std::vector<float> m_offsetArray [10][4];
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief An array of the vertices normals of the meshes at each offset level, used for quickly updating offset
    std::vector<float> m_normalOffsetArray [10][4];
    //----------------------------------------------------------------------------------------------------------------------
    typedef sdf::signed_distance_field_from_mesh mesh;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Number of dynamic meshes initialized in the compiler
    int m_noDynamic = 0;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Number of static meshes initialized in the compiler
    int m_noStatic = 0;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Array to store the sdf value of the dynamic meshes
    mesh m_dynObj[3];
    //----------------------------------------------------------------------------------------------------------------------
     /// @brief Array to store the sdf value of the static meshes
    mesh m_staticObj[1];
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Offsets each of the dynamic meshes by m_offset about the other meshes
    /// @author Kate Edge
    float offsetMesh(glm::vec3 pos, int objNo);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Used to add a mesh to m_dynObj or m_staticObj using its file path
    /// @author Kate Edge
    void addMesh(int _id, const char *_meshPath, bool _static);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Calls offset and polygize functions for each mesh
    /// @author Kate Edge
    void run();

    /// \brief write exports the vertices and normals into a new obj file at the specified destination,
    /// @author Alberto La Scala
    static void write(const std::vector<float> &_vertices, const std::vector<float> &_normals, std::string _destination);



};

#endif // MARCHINGCUBE_H



