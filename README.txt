Implicit Bodybuild: Research Project


In order to compile the folder glm must be contained in the project directory.

OpenGLTemplate created by ALberto La Scala used for window setup and openGL scene.

Original file found here: https://github.com/albelax/OpenGLTemplate

Instructions for user:

The number of meshes inputted must be set in the constructor for MarchingCube, this can be found in the function GlWindow::initializeGL().

MarchingCube(noOfDynamic,noOfStatic)

The max number of dynamic objects is currently 3 and the max number of static is 1.

The required meshes also need to be added using the addMesh() function, its parameters are shown below.

addMesh(meshID, meshDestination, static)

meshID: The id for the mesh, the first mesh will be 1 and the second 2 etc. ID numbers are independent for static and dynamic objects so the first dynamic and static mesh both have id 1.

meshDestination: A string indicating where the mesh is located, example meshes found in models folder.

static: A booleon indicating whether the mesh is static or not, if static enter true.


Current setup polygonizes the meshes at 5 different offsets automatically. Causes longer compilation time but fast updates to offset when compiled. 




