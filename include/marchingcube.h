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


class MarchingCube
{
public:
    MarchingCube();
    ~MarchingCube();

    double isolevel;
    glm::vec3 p1,p2;
    double valp1,valp2;

    int Polygonise(GRIDCELL grid,double isolevel,TRIANGLE *triangles);

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
    /// @brief flag to indicate if a VBO has been created
    //----------------------------------------------------------------------------------------------------------------------
    bool m_vboFlag;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if a VBO has been created
    //----------------------------------------------------------------------------------------------------------------------
    bool m_vaoFlag;

    GLuint m_vbo;
    GLuint m_vao;
    GLuint m_nbo;

    void createVAO();

    /// @brief The number of vertices in the object
    //----------------------------------------------------------------------------------------------------------------------
    unsigned long int m_nVerts;


    unsigned int MarchingTriangles(GRIDCELL g, float iso, std::vector<TRIANGLE> &triList);

    glm::vec3 computeTriangleNormal(TRIANGLE &itr);

    bool PrepareVolume(const double _offset);

    float getFunctionValue(float x, float y, float z, const double _offset);

    std::vector <VertData> m_vboMesh;

    glm::vec3 m_triNormal;

    // float vector version of vbo_mesh to pass into VBO
    std::vector<float> m_verts;









};

#endif // MARCHINGCUBE_H



