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



// a simple structure to hold our vertex data
// had to move this outside the method as g++ complains about it
// clang doesn't have a problem tho
struct VertData
{
  float nx; // normal from obj mesh
  float ny;
  float nz;
  float x; // position from obj
  float y;
  float z;
};

typedef struct {
   glm::vec3 p[3];
} TRIANGLE;

typedef struct {
   glm::vec3 p[8];
   double val[8];
} GRIDCELL;

typedef struct {
    glm::vec3 v[10];//v[10*m_nVerts];
    glm::vec3 n[10];//n[10*m_nVerts];


}OFFSET;

class MarchingCube
{

public:
    MarchingCube();
    ~MarchingCube();

    double isolevel;
    glm::vec3 p1,p2;
    double valp1,valp2;

    glm::vec3 VertexInterp(double isolevel,glm::vec3 p1,glm::vec3 p2,double valp1,double valp2);

    /// @brief  load an volume data from a file
    /// @param[in]  &_vol Volume Data File name
    /// @author xiasong
    //----------------------------------------------------------------------------------------------------------------------
    bool LoadVolumeFromFile(std::string _vol);
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
    std::vector <float> Polygonize(int lineNo);
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief The number of vertices in the object
    //----------------------------------------------------------------------------------------------------------------------
    unsigned long int m_nVerts;
    //----------------------------------------------------------------------------------------------------------------------
    unsigned int MarchingTriangles(GRIDCELL g, float iso, std::vector<TRIANGLE> &triList);
    //----------------------------------------------------------------------------------------------------------------------
    glm::vec3 computeTriangleNormal(TRIANGLE &itr);
    //----------------------------------------------------------------------------------------------------------------------
    bool PrepareVolume(int lineFunc);
    //----------------------------------------------------------------------------------------------------------------------
    float getSphereValue(float x, float y, float z);
    //----------------------------------------------------------------------------------------------------------------------
    std::vector <VertData> m_vboMesh;
    //----------------------------------------------------------------------------------------------------------------------
    glm::vec3 m_triNormal;
    //----------------------------------------------------------------------------------------------------------------------
    // float vector version of vbo_mesh to pass into VBO
    std::vector<float> m_verts;
    std::vector<float> m_vertsNormal;
    //----------------------------------------------------------------------------------------------------------------------
    float unionOperation( float d1, float d2 ) { return std::min(d1,d2); }
    //----------------------------------------------------------------------------------------------------------------------
    float distanceToSegment(glm::vec3 a, glm::vec3 b, glm::vec3 pos);
    //----------------------------------------------------------------------------------------------------------------------
    float distanceToLine1(float x, float y, float z);
    float distanceToLine2(float x, float y, float z);
    //----------------------------------------------------------------------------------------------------------------------
    float line1(float x, float y, float z);
    float line2(float x, float y, float z);
    //----------------------------------------------------------------------------------------------------------------------
    float m_offset;
    //----------------------------------------------------------------------------------------------------------------------
    std::vector<float> m_offsetArray [10];
    //----------------------------------------------------------------------------------------------------------------------
    void createOffsetArray();
    //----------------------------------------------------------------------------------------------------------------------
    float sdfMesh(std::vector<float> _verts);
    //----------------------------------------------------------------------------------------------------------------------








};

#endif // MARCHINGCUBE_H



