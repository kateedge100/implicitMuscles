#include "GLWindow.h"
#include "marchingcube.h"

#include <iostream>
#include <QColorDialog>
#include <QGLWidget>
#include <QImage>
#include <QScreen>

//----------------------------------------------------------------------------------------------------------------------

GLWindow::GLWindow( QWidget *_parent ) : QOpenGLWidget( _parent )
{
  // set this widget to have the initial keyboard focus
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  this->resize( _parent->size() );
  m_camera.setInitialMousePos(0,0);
  m_camera.setTarget(0.0f, 0.0f, -2.0f);
  m_camera.setEye(0.0f, 0.0f, 0.0f);
  m_rotating = false;

  m_vboFlag=false;
  m_vaoFlag=false;

  m_outputMeshNo = 0;

}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::initializeGL()
{
#ifdef __linux__

  // this needs to be after the context creation, otherwise it GLEW will crash
  //std::cout <<"linux \n";
  glewExperimental = GL_TRUE;
  glewInit();
  //	GLenum error = glGetError();
#endif
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_MULTISAMPLE );
  glEnable( GL_TEXTURE_2D );
  glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
  glViewport( 0, 0, devicePixelRatio(), devicePixelRatio() );

  m_M = new MarchingCube(2,1);

  // dynamic
  m_M->addMesh(1,"models/muscle1.obj", false);
  m_M->addMesh(2,"models/muscle2.obj", false);
  //m_M->addMesh(3,"models/cube3.obj", false);

  // static
  m_M->addMesh(1,"models/bone.obj", true);

  // pass vertices to shader
  init();

  m_MV = glm::translate( m_MV, glm::vec3(0.0f, 0.0f, -2.0f) );
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::resizeGL( int _w, int _h )
{

}

//----------------------------------------------------------------------------------------------------------------------

GLWindow::~GLWindow()
{

}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::mouseMove(QMouseEvent * _event)
{
  m_camera.handleMouseMove( _event->pos().x(), _event->pos().y() );

  update();
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::mouseClick(QMouseEvent * _event)
{
  m_camera.handleMouseClick(_event->pos().x(), _event->pos().y(), _event->type(), _event, 0);

  update();
}

//----------------------------------------------------------------------------------------------------------------------

void GLWindow::init()
{
  std::string shadersAddress = "shaders/";
  m_shader = Shader( "m_shader", shadersAddress + "phong_vert.glsl", shadersAddress + "simplefrag.glsl" );




  glLinkProgram( m_shader.getShaderProgram() );
  glUseProgram( m_shader.getShaderProgram() );


  // if we have already created a VBO just return.
  if(m_vaoFlag == true)
  {
      std::cout<<"VAO exist so returning\n";
      return;
  }


  glGenVertexArrays( 1, &m_vao );
  glBindVertexArray( m_vao );

  // buffer for bone shader
  glGenBuffers( 1, &m_vbo );
  glGenBuffers( 1, &m_nbo );




  // polygonizes the input mesh
  m_M->run();


  // copy vertices from 2d array to 1d array based on offset level for rendering
  for(int i = 0; i<m_M->m_noDynamic + m_M->m_noStatic; i++)
  {

      for(uint j =0; j < m_M->m_offsetArray[0][i].size(); ++j )
      {

         m_M->m_renderArray.push_back(m_M->m_offsetArray[0][i][j]);
         m_M->m_renderNormalArray.push_back(m_M->m_normalOffsetArray[0][i][j]);

      }

  }

  m_amountVertexData = m_M->m_renderArray.size();

  // load vertices
  glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
  glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
  // pass bone vertex
  glBufferSubData( GL_ARRAY_BUFFER, 0, m_amountVertexData * sizeof(float), &m_M->m_renderArray[0]);
  // pass cube vertex
  //glBufferSubData( GL_ARRAY_BUFFER, m_M->m_offsetArray[0][0].size() * sizeof(float), m_M->m_offsetArray[0][1].size() * sizeof(float), &m_M->m_offsetArray[0][1][0]);


  // pass vertices to shader
  GLint pos = glGetAttribLocation(m_shader.getShaderProgram(), "VertexPosition" );
  glEnableVertexAttribArray( pos );
  glVertexAttribPointer( pos, 3, GL_FLOAT, GL_FALSE, 0, 0 );


  // load normals
  glBindBuffer( GL_ARRAY_BUFFER,	m_nbo );
  glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, m_amountVertexData* sizeof(float), &m_M->m_renderNormalArray[0]);
  //glBufferSubData( GL_ARRAY_BUFFER, m_M->m_normalOffsetArray[0][0].size()* sizeof(float), m_M->m_normalOffsetArray[0][1].size() * sizeof(float), &m_M->m_normalOffsetArray[0][1][0]);


  // pass normals to shader
  GLint n = glGetAttribLocation( m_shader.getShaderProgram(), "VertexNormal" );
  glEnableVertexAttribArray( n );
  glVertexAttribPointer( n, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  m_vaoFlag = true;


  // link matrices with shader locations
  m_MVAddress = glGetUniformLocation( m_shader.getShaderProgram(), "MV" );
  m_MVPAddress = glGetUniformLocation( m_shader.getShaderProgram(), "MVP" );
  m_NAddress = glGetUniformLocation( m_shader.getShaderProgram(), "N" );
  m_timeAddress = glGetUniformLocation( m_shader.getShaderProgram(), "Time" );
  m_colorAddress = glGetUniformLocation( m_shader.getShaderProgram(), "Color" );

}

//------------------------------------------------------------------------------------------------------------------------------

int GLWindow::muscleTotalVertices()
{
    int totalVertices = 0;

    for(int i = 0; i< m_M->m_noDynamic; i++)
    {
        totalVertices += m_M->m_offsetArray[int(m_offsetUI*5)][i].size();
    }

    return totalVertices;

}

//------------------------------------------------------------------------------------------------------------------------------

int GLWindow::boneTotalVertices()
{
    int totalVertices = 0;

    for(int i = m_M->m_noDynamic; i< m_M->m_noDynamic + m_M->m_noStatic; i++)
    {
        totalVertices += m_M->m_offsetArray[int(m_offsetUI*5)][i].size();
    }

    return totalVertices;

}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::paintGL()
{
  glViewport( 0, 0, width(), height() );
  glClearColor( 1, 1, 1, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  renderScene();


  update();
}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::renderScene()
{
  glViewport( 0, 0, width()*devicePixelRatio(), height()*devicePixelRatio() ); //fix for retina screens
  glClearColor( 1, 1, 1, 1.0f );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  m_camera.update();
  m_projection = glm::perspective( glm::radians( 60.0f ),
                                   static_cast<float>( width() ) / static_cast<float>( height() ), 0.1f, 100.0f );
  m_view = glm::lookAt( glm::vec3( 0.0f, 0.0f, 5.0f ), glm::vec3( 0.0f, 0.0f, 0.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );

  if ( m_rotating )
    m_MV = glm::rotate( m_MV, glm::radians( -1.0f ), glm::vec3( 0.0f, 1.0f, 0.0f ) );
  m_MVP = m_projection * m_camera.viewMatrix() * m_MV;
  glm::mat3 N = glm::mat3( glm::inverse( glm::transpose( m_MV ) ) );

  glUniformMatrix4fv( m_MVPAddress, 1, GL_FALSE, glm::value_ptr( m_MVP ) );
  glUniformMatrix4fv( m_MVAddress, 1, GL_FALSE, glm::value_ptr( m_MV ) );

  glUniformMatrix3fv( m_NAddress, 1, GL_FALSE, glm::value_ptr( N ) );

  glm::vec3 color = {183.0f/255, 80.0f/255, 80.0f/255};

  glUniform3fv( m_colorAddress, 1, glm::value_ptr( color ) );


  glDrawArrays( GL_TRIANGLES, 0, muscleTotalVertices()/3 );

  color = {242.0f/255, 232.0f/255, 213.0f/255};

  glUniform3fv( m_colorAddress, 1, glm::value_ptr( color ) );


  glDrawArrays( GL_TRIANGLES, muscleTotalVertices()/3 , boneTotalVertices()/3 );
}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::updateOffset(double _offset)
{
    m_offsetUI = _offset;

    m_M->m_renderArray.clear();
    m_M->m_renderNormalArray.clear();



    // copy vertices from 2d array to 1d array based on offset level for rendering
    for(int i = 0; i<m_M->m_noDynamic + m_M->m_noStatic; i++)
    {

        for(uint j =0; j < m_M->m_offsetArray[int(m_offsetUI*5)][i].size(); ++j )
        {

           m_M->m_renderArray.push_back(m_M->m_offsetArray[int(m_offsetUI*5)][i][j]);
           m_M->m_renderNormalArray.push_back(m_M->m_normalOffsetArray[int(m_offsetUI*5)][i][j]);

        }

    }




    m_amountVertexData = m_M->m_renderArray.size();

    std::cout<<"Offset Updated!\nNew offset is "<<_offset<<"\n";

    // load vertices
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
    glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_amountVertexData * sizeof(float), &m_M->m_renderArray[0]);

    // pass vertices to shader
    GLint pos = glGetAttribLocation( m_shader.getShaderProgram(), "VertexPosition" );
    glEnableVertexAttribArray( pos );
    glVertexAttribPointer( pos, 3, GL_FLOAT, GL_FALSE, 0, 0 );

    // load normals
    glBindBuffer( GL_ARRAY_BUFFER,	m_nbo );
    glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_amountVertexData * sizeof(float), &m_M->m_renderNormalArray[0]);

    // pass normals to shader
    GLint n = glGetAttribLocation( m_shader.getShaderProgram(), "VertexNormal" );
    glEnableVertexAttribArray( n );
    glVertexAttribPointer( n, 3, GL_FLOAT, GL_FALSE, 0, 0 );

}

void GLWindow::outputMesh()
{
    std::string outputName = "outputMesh";
    std::string outputFormat = ".obj";
    std::string meshNo = std::to_string(m_outputMeshNo);

    for(int i = 0; i< m_M->m_noDynamic + m_M->m_noStatic; i++)
    {
        if(i<m_M->m_noDynamic)
        {
            outputName = "muscleMesh";

        }
        else
        {
            outputName = "boneMesh";
        }
        m_M->write(m_M->m_offsetArray[int(m_offsetUI*5)][i],m_M->m_normalOffsetArray[int(m_offsetUI*5)][i], outputName+"_"+std::to_string(i)+"_"+meshNo+outputFormat);

    }


    m_outputMeshNo ++;

    std::cout<<"Mesh saved as obj\n";

}
