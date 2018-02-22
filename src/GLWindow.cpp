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

  m_M = new MarchingCube(2,0);

  // dynamic
  m_M->addMesh(1,"models/cube1.obj", false);
  m_M->addMesh(2,"models/cube2.obj", false);

  // static
  //m_M->addMesh(1,"models/bone.obj", true);


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
  //m_shader = Shader( "m_shader", shadersAddress + "phong_vert.glsl", shadersAddress + "simplefrag.glsl" );
  Shader boneShader = Shader( "bone_shader", shadersAddress + "phong_vert.glsl", shadersAddress + "muscle_frag.glsl" );

  //glLinkProgram( m_shader.getShaderProgram() );
  //glUseProgram( m_shader.getShaderProgram() );

  glLinkProgram( boneShader.getShaderProgram() );
  glUseProgram( boneShader.getShaderProgram() );


  // if we have already created a VBO just return.
  if(m_vaoFlag == true)
  {
      std::cout<<"VAO exist so returning\n";
      return;
  }

  glGenVertexArrays( 1, &m_vao );
  glBindVertexArray( m_vao );
  glGenBuffers( 1, &m_vbo );
  glGenBuffers( 1, &m_nbo );

  // polygonizes the input mesh
  m_M->run();

  m_amountVertexData = m_M->m_offsetArray[0][0].size();

  // load vertices
  glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
  glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, m_M->m_offsetArray[0][0].size() * sizeof(float), &m_M->m_offsetArray[0][0][0]);


  // pass vertices to shader
  GLint pos = glGetAttribLocation(boneShader.getShaderProgram(), "VertexPosition" );
  glEnableVertexAttribArray( pos );
  glVertexAttribPointer( pos, 3, GL_FLOAT, GL_FALSE, 0, 0 );


  // load normals
  glBindBuffer( GL_ARRAY_BUFFER,	m_nbo );
  glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
  glBufferSubData( GL_ARRAY_BUFFER, 0, m_M->m_offsetArray[0][0].size() * sizeof(float), &m_M->m_normalOffsetArray[0][0][0]);



  // pass normals to shader
  GLint n = glGetAttribLocation( boneShader.getShaderProgram(), "VertexNormal" );
  glEnableVertexAttribArray( n );
  glVertexAttribPointer( n, 3, GL_FLOAT, GL_FALSE, 0, 0 );

  m_vaoFlag = true;

  // link matrices with shader locations
  m_MVAddress = glGetUniformLocation( boneShader.getShaderProgram(), "MV" );
  m_MVPAddress = glGetUniformLocation( boneShader.getShaderProgram(), "MVP" );
  m_NAddress = glGetUniformLocation( boneShader.getShaderProgram(), "N" );
  m_timeAddress = glGetUniformLocation( boneShader.getShaderProgram(), "Time" );






  //------------------------------------------------------------------------------------


//  m_shader = Shader( "m_shader", shadersAddress + "phong_vert.glsl", shadersAddress + "simplefrag.glsl" );
//  //Shader boneShader = Shader( "bone_shader", shadersAddress + "phong_vert.glsl", shadersAddress + "muscle_frag.glsl" );

//  glLinkProgram( m_shader.getShaderProgram() );
//  glUseProgram( m_shader.getShaderProgram() );

////  glLinkProgram( boneShader.getShaderProgram() );
////  glUseProgram( boneShader.getShaderProgram() );


////  // if we have already created a VBO just return.
////  if(m_vaoFlag == true)
////  {
////      std::cout<<"VAO exist so returning\n";
////      return;
////  }

////  glGenVertexArrays( 1, &m_vao );
////  glBindVertexArray( m_vao );
////  glGenBuffers( 1, &m_vbo );
////  glGenBuffers( 1, &m_nbo );


//  m_amountVertexData = m_M->m_offsetArray[0][1].size();

//  // load vertices
//  glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
//  glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
//  glBufferSubData( GL_ARRAY_BUFFER, 0, m_M->m_offsetArray[0][1].size() * sizeof(float), &m_M->m_offsetArray[0][1][0]);


//  // pass vertices to shader
//  GLint pos2 = glGetAttribLocation(m_shader.getShaderProgram(), "VertexPosition" );
//  glEnableVertexAttribArray( pos2 );
//  glVertexAttribPointer( pos2, 3, GL_FLOAT, GL_FALSE, 0, 0 );


//  // load normals
//  glBindBuffer( GL_ARRAY_BUFFER,	m_nbo );
//  glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
//  glBufferSubData( GL_ARRAY_BUFFER, 0, m_M->m_offsetArray[0][1].size() * sizeof(float), &m_M->m_normalOffsetArray[0][1][0]);



//  // pass normals to shader
//  GLint n2 = glGetAttribLocation( m_shader.getShaderProgram(), "VertexNormal" );
//  glEnableVertexAttribArray( n2);
//  glVertexAttribPointer( n2, 3, GL_FLOAT, GL_FALSE, 0, 0 );

//  m_vaoFlag = true;

//  // link matrices with shader locations
//  m_MVAddress = glGetUniformLocation( m_shader.getShaderProgram(), "MV" );
//  m_MVPAddress = glGetUniformLocation( m_shader.getShaderProgram(), "MVP" );
//  m_NAddress = glGetUniformLocation( m_shader.getShaderProgram(), "N" );
//  m_timeAddress = glGetUniformLocation( m_shader.getShaderProgram(), "Time" );






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

  glDrawArrays( GL_TRIANGLES, 0 , ( m_amountVertexData / 3 ) );
}

//------------------------------------------------------------------------------------------------------------------------------

void GLWindow::updateOffset(double _offset)
{
    m_offsetUI = _offset;

    m_amountVertexData = m_M->m_offsetArray[int(_offset*5)][0].size();

    std::cout<<"Offset Updated!\nNew offset is "<<_offset<<"\n";

    // load vertices
    glBindBuffer( GL_ARRAY_BUFFER, m_vbo );
    glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_amountVertexData * sizeof(float), &m_M->m_offsetArray[int(_offset*5)][0][0]);

    // pass vertices to shader
    GLint pos = glGetAttribLocation( m_shader.getShaderProgram(), "VertexPosition" );
    glEnableVertexAttribArray( pos );
    glVertexAttribPointer( pos, 3, GL_FLOAT, GL_FALSE, 0, 0 );

    // load normals
    glBindBuffer( GL_ARRAY_BUFFER,	m_nbo );
    glBufferData( GL_ARRAY_BUFFER, m_amountVertexData * sizeof(float), 0, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, m_amountVertexData * sizeof(float), &m_M->m_normalOffsetArray[int(_offset*5)][0][0]);

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
