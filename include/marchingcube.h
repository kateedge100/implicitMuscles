#ifndef MARCHINGCUBE_H
#define MARCHINGCUBE_H
#include <glm.hpp>
#include <iostream>
#include <fstream>
#include <vector>

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
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if a VBO has been created
    //----------------------------------------------------------------------------------------------------------------------
    bool m_vbo;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief flag to indicate if a VBO has been created
    //----------------------------------------------------------------------------------------------------------------------
    bool m_vao;

    void createVAO();

    /// @brief The number of vertices in the object
    //----------------------------------------------------------------------------------------------------------------------
    unsigned long int m_nVerts;


    unsigned int MarchingTriangles(GRIDCELL g, float iso, std::vector<TRIANGLE> &triList);

    glm::vec3 computeTriangleNormal(TRIANGLE &itr);







};

#endif // MARCHINGCUBE_H



