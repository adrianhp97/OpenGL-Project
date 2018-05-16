/*
 * This code is based on a code skeleton for loading vertex and fragment
 * programs written by Stefan Gustavsson 2004.
 *
 * Everything that has to do with the particle system was added
 * by Tommy Hinks (thinks@hotmail.com) 2005.
*/
#include <stdio.h> // Standard input/output
#include <io.h> // For reading from files
#include <iostream> // For output to console window
#include <cmath> // Trigonometry
#include <cstdlib>
#include <time.h> // To seed randomizer
#include <GL/glfw.h>
#include "glext.h" // Include local glext.h
#include "ParticleList.h" // Linked list for particles
/* Global variables for all the nice ARB extensions we need */
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
PFNGLUNIFORM1IVARBPROC glUniform1ivARB = NULL;
PFNGLUNIFORM1FVARBPROC glUniform1fvARB = NULL;
//============================
// GL_ARB_point_parameters
//============================
PFNGLPOINTPARAMETERFARBPROC glPointParameterfARB = NULL;
PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = NULL;
//========================================================
// Global Variables for Particle System
// (Many of which can be interactively controlled by user)
//========================================================
GLboolean g_bShader = GL_TRUE; // Hardware shading or not
GLfloat g_fPointSize = 43.0f; // Particle size
float g_fTurbYVel = 1.0f; // The speed of the smoke pillar in y-dir
GLfloat g_fMaxPartAge = 3.0f; // Particles with age > g_fMaxPartAge are killed
GLfloat g_fAlpha = 0.028f; // Alpha scaling
// Is the smoke warm, fast rising and grows thicker or
// is it more like steam that rises slowly and fades away
// 1.0f = steam
// 0.0f = smoke
float g_fSmokeType = 1.0f;
// Noise, default values
float g_fNoiseAmp = 0.04f;
float g_fNoiseFreq = 0.1f;
// Smoke color
GLfloat g_fSmokeColor[] = {0.5f, 0.5f, 0.5f}; // Color of smoke 
// Use 3 coordinates in case we might want to spawn particles
// somewhere else than xz-plane.
#define SOURCE_POINTS 45
GLfloat g_fSourcePoints[SOURCE_POINTS][3];
// Particle vel is always up to begin with
GLfloat g_fStartingVel[] = { 0.0f, 1.0f, 0.0f };
GLboolean first_run = GL_TRUE;
// GLSL won't compile larger
// number of uniforms on GeForce FX5700...
const int TABSIZE = 32;
const GLint permTable[TABSIZE] = {
 5, 27, 9, 2, 7, 25, 22, 17, 8, 20, 16, 0, 10,
 11, 28, 3, 18, 24, 15, 6, 23, 13, 26, 31, 12, 1,
 29, 4, 21, 19, 14, 30
};
// Vectors of unit length
const GLfloat gradTable[TABSIZE*3] = {
 -0.3429, 0.8639, -0.3689,
 0.3442, 0.1373, -0.9288,
 -0.09915, 0.009929, -0.995,
 -0.3476, 0.1406, -0.927,
 0.4784, 0.6453, -0.5956,
 0.2621, 0.07418, -0.9622,
 0.2826, 0.08755, -0.9552,
 0.4016, 0.7979, 0.4496,
 0.05402, 0.9971, -0.0541,
 0.4527, 0.2878, 0.8439,
 0.1532, 0.976, 0.155,
 -0.3934, 0.8085, 0.4376,
 -0.4952, 0.431, 0.7543,
 0.2852, 0.08934, 0.9543,
 0.02181, 0.9995, 0.02181,
 -0.4823, 0.6317, 0.6069,
 0.4659, 0.6816, 0.5643,
 0.2916, 0.9061, 0.3064,
 -0.1259, 0.9839, 0.1269,
 -0.1961, 0.96, 0.2001,
 0.3097, 0.1074, -0.9448,
 -0.4533, 0.2889, -0.8433,
 0.3542, 0.1471, 0.9235,
 0.06125, 0.9962, -0.06137,
 -0.4957, 0.4345, -0.752,
 0.4675, 0.6773, 0.5681,
 0.1413, 0.02038, 0.9898,
 0.2045, 0.04372, -0.9779,
 0.1352, 0.9814, 0.1364,
 0.1559, 0.9751, 0.1579,
 0.2437, 0.9366, 0.2519,
 0.3711, 0.8351, 0.4061,
};
ParticleList *g_PartList; // Pointer to our ParticleList
GLfloat g_fMaxPointSize = 0.0f; // This is set in GLinit()
GLfloat g_fMinPointSize = 0.0f; // This is set in GLinit()
//========================================================
int running = GL_TRUE; // Main loop exits when this is set to GL_FALSE
GLuint textureID;
GLhandleARB programObj;
GLhandleARB vertexShader;
GLhandleARB fragmentShader;
// Locations in memory for uniform shader variables
GLint location_testTexture = -1;
GLint location_time = -1; 
GLint location_max_age = -1;
GLint location_alpha = -1;
GLint location_perm = -1;
GLint location_grad = -1;
GLint location_smoke_type = -1;
GLint location_point_size = -1;
GLint location_noise_amp = -1;
GLint location_noise_freq = -1;
const char *vertexShaderStrings[1];
const char *fragmentShaderStrings[1];
GLint vertexCompiled;
GLint fragmentCompiled;
GLint shadersLinked;
char str[4096]; // For error messages from the GLSL compiler and linker
/*
 * printError() - Signal an error.
 * Prints to console window
 */
void printError(const char *errtype, const char *errmsg) {
 std::cerr << errtype << ", " << errmsg << std::endl;
}
// Print current parameter values
void printState()
{
 std::cerr << std::endl;
 std::cerr << "-----Parameters--------------" << std::endl;
 std::cerr << "Point Size: " << g_fPointSize << std::endl;
 std::cerr << "Max particle age [s]: " << g_fMaxPartAge << std::endl;
 std::cerr << "Y+ velocity: " << g_fTurbYVel << std::endl;
 std::cerr << "Alpha scaling: " << g_fAlpha << std::endl;
 std::cerr << "Noise Amp: " << g_fNoiseAmp << std::endl;
 std::cerr << "Noise Freq: " << g_fNoiseFreq << std::endl;
 if(g_fSmokeType == 1)
 std::cerr << "Smoke type: steam" << std::endl;
 else
 std::cerr << "Smoke type: smoke" << std::endl;
 if(g_bShader)
 std::cerr << "Shaders: on" << std::endl;
 else
 std::cerr << "Shaders: off" << std::endl;
 std::cerr << "Color[3]: [ " << g_fSmokeColor[0] << ", " << g_fSmokeColor[1] << ", " << g_fSmokeColor[2] << " ]" << std::endl;
 std::cerr << "-----------------------------" << std::endl;
 std::cerr << std::endl;
}
void printHelp()
{
 std::cerr << std::endl;
 std::cerr << "-------Help---------" << std::endl;
 std::cerr << "z/x +/- point size" << std::endl;
 std::cerr << "q/w +/- particle age" << std::endl;
 std::cerr << "y/u +/- Y-velocity" << std::endl;
 std::cerr << "a/s +/- alpha scaling" << std::endl;
 std::cerr << "j/k +/- Noise amplitude" << std::endl;
 std::cerr << "n/m +/- Noise frequency" << std::endl;
 std::cerr << "r/g/b Cycle color channels" << std::endl;
 std::cerr << "0 reset" << std::endl;
 std::cerr << "1 shaders on/off" << std::endl;
 std::cerr << "e smoke/steam" << std::endl;
 std::cerr << "--------------------" << std::endl;
 std::cerr << std::endl;
}
void GLFWCALL KeyFun( int key, int action )
{
 if( action == GLFW_PRESS )
 {
 switch( key )
 { 
 // Increase/decrease particle size
 case 'z':
 g_fPointSize += 1.0f;
 if(g_fPointSize > g_fMaxPointSize)
 {
 g_fPointSize = g_fMaxPointSize;
 }
 printState();
 break;
 case 'x':
 g_fPointSize -= 1.0f;
 if(g_fPointSize < g_fMinPointSize)
 {
 g_fPointSize = g_fMinPointSize;
 }
 printState();
 break;
 // Increase/decrease y-velocity
 case 'y':
 g_fTurbYVel += 0.1f;
 printState();
 break;
 case 'u':
 g_fTurbYVel -= 0.1f;
 if(g_fTurbYVel < 0.1f)
 {
 g_fTurbYVel = 0.1f;
 }
 printState();
 break;
 // Increase/decrease alpha scaling
 case 'a':
 g_fAlpha += 0.001f;
 printState();
 break;
 case 's':
 g_fAlpha -= 0.001f;
 if(g_fAlpha < 0.001f)
 {
 g_fAlpha = 0.001f;
 }
 printState();
 break;
 // Increase/decrease particle age
 case 'q':
 g_fMaxPartAge += 0.1f;
 printState();
 break;
 case 'w':
 g_fMaxPartAge -= 0.1f;
 if(g_fMaxPartAge < 0.1f)
 {
 g_fMaxPartAge = 0.1f;
 }
 printState();
 break;
 // Increase/decrease noise amplitude
 case 'j':
 g_fNoiseAmp += 0.01f;
 printState();
 break;
 case 'k':
 g_fNoiseAmp -= 0.01f;
 if(g_fNoiseAmp < 0.0f)
 {
 g_fNoiseAmp = 0.0f;
 }
 printState();
 break;
 // Increase/decrease noise frequency
 case 'n':
 g_fNoiseFreq += 0.01f; 
 printState();
 break;
 case 'm':
 g_fNoiseFreq -= 0.01f;
 if(g_fNoiseFreq < 0.0f)
 {
 g_fNoiseFreq = 0.0f;
 }
 printState();
 break;
 // Change smoke type
 case 'e':
 g_fSmokeType = (g_fSmokeType == 0.0f) ? 1.0f:0.0f;
 printState();
 break;
 // Cycle through red color channel
 case 'r':
 g_fSmokeColor[0] += 0.1f;
 if(g_fSmokeColor[0] > 1.01f)
 g_fSmokeColor[0] = 0.0f;
 printState();
 break;
 // Cycle through green color channel
 case 'g':
 g_fSmokeColor[1] += 0.1f;
 if(g_fSmokeColor[1] > 1.01f)
 g_fSmokeColor[1] = 0.0f;
 printState();
 break;
 // Cycle through blue color channel
 case 'b':
 g_fSmokeColor[2] += 0.1f;
 if(g_fSmokeColor[2] > 1.01f)
 g_fSmokeColor[2] = 0.0f;
 printState();
 break;
 case '1':
 g_bShader = (g_bShader) ? GL_FALSE:GL_TRUE;
 printState();
 break;

 // Reset button
 case '0':
 g_fPointSize = 43.0f;
 g_fTurbYVel = 1.0f;
 g_fPointSize = 50.0f;
 g_fMaxPartAge = 3.0f;
 g_fAlpha = 0.02f;
 g_fNoiseAmp = 0.04f;
 g_fNoiseFreq = 0.1f;
 g_fSmokeType = 1.0f;
 g_fSmokeColor[0] = g_fSmokeColor[1] = g_fSmokeColor[2] = 0.5f;
 printState();
 break;
 // Print help
 case 'h':
 printHelp();
 break;
 default:
 break;
 }
 }
}
/*
 * loadExtensions() - Load OpenGL extensions for anything above OpenGL
 * version 1.1. (This is a requirement from Windows, not from OpenGL.)
 */
void loadExtensions() {
 //These extension strings indicate that the OpenGL Shading Language,
 // version 1.00, and GLSL shader objects are supported.
 if(!glfwExtensionSupported("GL_ARB_shading_language_100")) 
 {
 printError("GL init error", "GL_ARB_shading_language_100 extension was not found");
 return;
 }
 if(!glfwExtensionSupported("GL_ARB_shader_objects"))
 {
 printError("GL init error", "GL_ARB_shader_objects extension was not found");
 return;
 }
 else
 {
 glCreateProgramObjectARB =
(PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
 glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
 glUseProgramObjectARB =
(PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
 glCreateShaderObjectARB =
(PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
 glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
 glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
 glGetObjectParameterivARB =
(PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
 glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
 glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
 glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
 glGetUniformLocationARB =
(PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
 glUniform4fARB = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
 glUniform1fARB = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
 glUniform1iARB = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");
 glUniform1ivARB = (PFNGLUNIFORM1IVARBPROC)wglGetProcAddress("glUniform1ivARB");
 glUniform1fvARB = (PFNGLUNIFORM1FVARBPROC)wglGetProcAddress("glUniform1fvARB");
 // Point Sprites
 glPointParameterfARB = (PFNGLPOINTPARAMETERFARBPROC)wglGetProcAddress("glPointParameterfARB");
 glPointParameterfvARB = (PFNGLPOINTPARAMETERFVARBPROC)wglGetProcAddress("glPointParameterfvARB");
 if( !glCreateProgramObjectARB || !glDeleteObjectARB || !glUseProgramObjectARB ||
 !glCreateShaderObjectARB || !glCreateShaderObjectARB || !glCompileShaderARB ||
 !glGetObjectParameterivARB || !glAttachObjectARB || !glGetInfoLogARB ||
 !glLinkProgramARB || !glGetUniformLocationARB || !glUniform4fARB ||
 !glUniform1fARB || !glUniform1iARB || !glPointParameterfARB || !glPointParameterfvARB
 || !glUniform1ivARB || !glUniform1fvARB)
 {
 printError("GL init error", "One or more GL_ARB_shader_objects functions were not found");
 return;
 }
 }
}
//=========================================================
// Read and load shaders
//=========================================================
/*
 * readShaderFile(filename) - read a shader source string from a file
 */
unsigned char* readShaderFile(const char *filename) {
 FILE *file = fopen(filename, "r");
 if(file == NULL)
 {
 printError("ERROR", "Cannot open shader file!");
 return 0;
 }
 int bytesinfile = filelength(fileno(file));
 unsigned char *buffer = (unsigned char*)malloc(bytesinfile);
 int bytesread = fread( buffer, 1, bytesinfile, file);
 buffer[bytesread] = 0; // Terminate the string with 0
 fclose(file);

 return buffer;
}
/* 
 * createShaders() - create, load, compile and link the GLSL shader objects.
 */
void createShaders() {
 // Create the vertex shader.
 vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
 unsigned char *vertexShaderAssembly = readShaderFile("vertex_shader.vert");
 vertexShaderStrings[0] = (char*)vertexShaderAssembly;
 glShaderSourceARB( vertexShader, 1, vertexShaderStrings, NULL );
 glCompileShaderARB( vertexShader);
 free((void *)vertexShaderAssembly);
 glGetObjectParameterivARB( vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,
 &vertexCompiled );
 if(vertexCompiled == GL_FALSE)
 {
 glGetInfoLogARB(vertexShader, sizeof(str), NULL, str);
 printError("Vertex shader compile error", str);
 }
 // Create the fragment shader.
 fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );
 unsigned char *fragmentShaderAssembly = readShaderFile( "fragment_shader.frag" );
 fragmentShaderStrings[0] = (char*)fragmentShaderAssembly;
 glShaderSourceARB( fragmentShader, 1, fragmentShaderStrings, NULL );
 glCompileShaderARB( fragmentShader );
 free((void *)fragmentShaderAssembly);
 glGetObjectParameterivARB( fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,
 &fragmentCompiled );
 if(fragmentCompiled == GL_FALSE)
 {
 glGetInfoLogARB( fragmentShader, sizeof(str), NULL, str );
 printError("Fragment shader compile error", str);
 }
 // Create a program object and attach the two compiled shaders.
 programObj = glCreateProgramObjectARB();
 glAttachObjectARB( programObj, vertexShader );
 glAttachObjectARB( programObj, fragmentShader );
 // Link the program object and print out the info log.
 glLinkProgramARB( programObj );
 glGetObjectParameterivARB( programObj, GL_OBJECT_LINK_STATUS_ARB, &shadersLinked );
 if( shadersLinked == GL_FALSE )
 {
 glGetInfoLogARB( programObj, sizeof(str), NULL, str );
 printError("Program object linking error", str);
 }
 // Locate the uniform shader variables so we can set them later:
 // a texture ID "testTexture" and a float "time".
 location_testTexture = glGetUniformLocationARB( programObj, "testTexture" );
 if(location_testTexture == -1)
 printError("Binding error","Failed to locate uniform variable 'testTexture'.");

 location_time = glGetUniformLocationARB( programObj, "time" );
 if(location_time == -1)
 printError("Binding error", "Failed to locate uniform variable 'time'.");

 location_max_age = glGetUniformLocationARB( programObj, "MaxPartAge" );
 if(location_max_age == -1)
 printError("Binding error", "Failed to locate uniform variable 'MaxPartAge'.");

 location_alpha = glGetUniformLocationARB( programObj, "alpha" );
 if(location_alpha == -1)
 printError("Binding error", "Failed to locate uniform variable 'alpha'.");

 location_perm = glGetUniformLocationARB( programObj, "permTable");
 if(location_perm == -1)
 printError("Binding error", "Failed to locate uniform variable 'permTable'.");
 
 location_grad = glGetUniformLocationARB( programObj, "gradTable");
 if(location_grad == -1)
 printError("Binding error", "Failed to locate uniform variable 'gradTable'.");

 location_noise_amp = glGetUniformLocationARB( programObj, "noiseAmp");
 if(location_noise_amp == -1)
 printError("Binding error", "Failed to locate uniform variable 'noiseAmp'.");
 location_noise_freq = glGetUniformLocationARB( programObj, "noiseFreq");
 if(location_noise_freq == -1)
 printError("Binding error", "Failed to locate uniform variable 'noiseFreq'.");

 location_smoke_type = glGetUniformLocationARB( programObj, "smokeType");
 if(location_smoke_type == -1)
 printError("Binding error", "Failed to locate uniform variable 'smokeType'.");

 location_point_size = glGetUniformLocationARB( programObj, "pointSize");
 if(location_point_size == -1)
 printError("Binding error", "Failed to locate uniform variable 'pointSize'.");
}
/*
 * setupCamera() - set up the OpenGL projection and (model)view matrices
 */
void setupCamera() {
 int width, height;

 // Get window size. It may start out different from the requested
 // size, and will change if the user resizes the window.
 glfwGetWindowSize( &width, &height );

 height = (height > 0)? height : 1;
 //if(height<=0) height=1; // Safeguard against iconified/closed window
 // Set viewport. This is the pixel rectangle we want to draw into.
 glViewport( 0, 0, width, height ); // The entire window
 glMatrixMode(GL_PROJECTION);
 glLoadIdentity();
 // 45 degrees FOV, same aspect ratio as viewport, depth range 0 to 10
 gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 0.0f, 10.0f );

 glMatrixMode( GL_MODELVIEW );
 glLoadIdentity();
 gluLookAt( 0.0f, 2.0f, 2.0f, // Eye position
 0.0f, 2.0f, 0.0f, // View point
 0.0f, 1.0f, 0.0f ); // Up vector
}
// Update particle velocities according to vector field
void updateVel(Node *part, float dt)
{
 part->setVel(0, part->getPos(2) * part->getPos(1)*2.0f); // Set x-vel
 part->setVel(1, g_fTurbYVel); // Set y-vel
 part->setVel(2, -(part->getPos(0)) * part->getPos(1)*2.0f); // Set z-vel
}
// Move, render and delete particles
void updateParticles(float time, float dt)
{
 // Insert particles....
 // Loop over source points
 for(int i = 0; i < SOURCE_POINTS; i++)
 {
 g_PartList->insert(g_fSourcePoints[i], g_fStartingVel);
 }
 // Setup rendering
 glEnable( GL_POINT_SPRITE_ARB );
 glBegin( GL_POINTS );
 
 // Create pointer to header node
 Node *partptr = g_PartList->getHeader();

 while(partptr->getNext() != 0)
 {
 Node *current = partptr->getNext();
 // Age the particle
 // Age first so we don't calculate unnecessary positions
 current->setAge(current->getAge() + dt);
 if(current->getAge() > g_fMaxPartAge)
 { // If particle has aged too much, kill it
 // Also delete all nodes behind it, since
 // they will be even older...(or as old)
 g_PartList->del(partptr);
 // Break the loop, because all particles after
 // this one will also be too old...
 break;
 }
 else
 {
 // Move particle according to it's velocity
 current->setPos(current->getPos(0) + dt*current->getVel(0),
 current->getPos(1) + dt*current->getVel(1),
 current->getPos(2) + dt*current->getVel(2));

 // Send position and color to be rendered
 glColor4f(g_fSmokeColor[0], g_fSmokeColor[1], g_fSmokeColor[2], current->getAge());
 glVertex3fv((GLfloat *)current->getPos());

 // Update particle velocity
 updateVel(current, dt);
 }
 // Move on to next particle...
 partptr = partptr->getNext();
 }
 glEnd();
 glDisable( GL_POINT_SPRITE_ARB );
}
//======================
// Software shading
// This is an over-simplified version of the glsl shader.
// It doesn't have the same functionality but the calculations
// are similar, though not identical, in fact, a whole lot more
// calculations are done in the shader version. These functions are for
// comparability only, speed comparability. This could well have
// been a separate program which would equally well illustrate the
// differences.
//======================
int CPUperm(int x)
{
 // Ugly abs()-hack...
 int i = int(abs(x%(TABSIZE - 1)));
 return permTable[i];
}
int CPUindex(int tx, int ty, int tz)
{
 return CPUperm(tx + CPUperm(ty + CPUperm(tz)));
}
void CPUglattice(int tx, int ty, int tz, float *out)
{
 int i = CPUindex(tx, ty, tz);
 out[0] = gradTable[i*3];
 out[1] = gradTable[i*3 + 1];
 out[2] = gradTable[i*3 + 2];
}
void CPUhnoise(float *in, float *out)
{ 
 // Hack to avoid negative numbers
 in[0] = in[0] + 100.0f;
 in[1] = in[1] + 100.0f;
 in[2] = in[2] + 100.0f;

 // Integer part is first decimal as integer
 // due to small scale scene.
 int ix = int(floor(10.0f*(in[0] - floor(in[0]))));
 int iy = int(floor(10.0f*(in[1] - floor(in[1]))));
 int iz = int(floor(10.0f*(in[2] - floor(in[2]))));

 // Fraction vector
 float fx = 10.0f*in[0] - ix;
 float fy = 10.0f*in[1] - iy;
 float fz = 10.0f*in[2] - iz;
 float vertices[6][3];
 // x-comp
 CPUglattice( (ix + 1), iy, iz , vertices[0]);
 CPUglattice( (ix - 1), iy, iz , vertices[1]);

 // y-comp
 CPUglattice( ix, (iy + 1), iz , vertices[2]);
 CPUglattice( ix, (iy - 1), iz , vertices[3]);

 // z-comp
 CPUglattice( ix, iy, (iz + 1) , vertices[4]);
 CPUglattice( ix, iy, (iz - 1) , vertices[5]);
 // Very simple 'interpolation', should be enough...
 out[0] = g_fNoiseAmp*(fx*vertices[0][0] + (1.0f - fx)*vertices[1][0]);
 out[1] = g_fNoiseAmp*(fy*vertices[2][1] + (1.0f - fy)*vertices[3][1]);
 out[2] = g_fNoiseAmp*(fz*vertices[4][2] + (1.0f - fz)*vertices[5][2]);
}
// Virtually the same as updateParticles()
void CPUupdatePart(float time, float dt)
{
 for(int i = 0; i < SOURCE_POINTS; i++)
 {
 g_PartList->insert(g_fSourcePoints[i], g_fStartingVel);
 }
 glPointSize(g_fPointSize);
 glEnable( GL_POINT_SPRITE_ARB );
 glBegin( GL_POINTS );
 Node *partptr = g_PartList->getHeader();

 while(partptr->getNext() != 0)
 {
 Node *current = partptr->getNext();
 current->setAge(current->getAge() + dt);
 if(current->getAge() > g_fMaxPartAge)
 {
 g_PartList->del(partptr);
 break;
 }
 else
 {
 float noise[] = { 0.0f, 0.0f, 0.0f };
 CPUhnoise(current->getPos(), noise);

 current->setPos(current->getPos(0) + dt*current->getVel(0),
 current->getPos(1) + dt*current->getVel(1),
 current->getPos(2) + dt*current->getVel(2));

 // We don't render anything, just observe framerate
 updateVel(current, dt);
 }
 partptr = partptr->getNext();
 } 
 glEnd();
 glDisable( GL_POINT_SPRITE_ARB );
}
//======================
/*
 * renderScene() - a wrapper to drawScene() to switch shaders on and off
 */
void renderScene( float ut, float dt )
{
 float t = (float)glfwGetTime(); // Get elapsed time
 if(g_bShader)
 {
 // Identify the texture to use.
 // For some reason, this uniform variable must be set BEFORE the
 // activation of the program object. This seems like a driver bug.
 if( location_testTexture != -1 )
 glUniform1iARB( location_testTexture, textureID );

 // Use vertex and fragment shaders.
 glUseProgramObjectARB( programObj );

 // Update the uniform time variable.
 // For some reason, this uniform variable must be set AFTER the
 // activation of the program object. This seems like a driver bug.
 if(location_time != -1 )
 glUniform1fARB( location_time, t );

 // These must be changed while the shaders are active?
 if(location_max_age != -1)
 glUniform1fARB( location_max_age, g_fMaxPartAge );
 if(location_alpha != -1)
 glUniform1fARB( location_alpha, g_fAlpha );
 if(location_noise_amp != -1)
 glUniform1fARB( location_noise_amp, g_fNoiseAmp );
 if(location_noise_freq != -1)
 glUniform1fARB( location_noise_freq, g_fNoiseFreq );
 if(location_point_size != -1)
 glUniform1fARB( location_point_size, g_fPointSize );
 if(location_smoke_type != -1)
 glUniform1fARB( location_smoke_type, g_fSmokeType );

 // These are never changed, so they are just set once
 // (They would have to be re-set if we re-linked shaders)
 if(first_run)
 {
 if(location_perm != -1)
 glUniform1ivARB( location_perm, TABSIZE, permTable);
 if(location_grad != -1)
 glUniform1fvARB( location_grad, TABSIZE*3, gradTable);
 first_run = GL_FALSE;
 }

 updateParticles(ut, dt);

 // Deactivate the shaders.
 glUseProgramObjectARB(0);
 }
 else
 {
 // Render without shaders
 CPUupdatePart(ut, dt);
 }
}
// (A lot of the GL-state is set here and only once,
// this may be done since the primitives and rendering
// of our scene will not change much. Otherwise it is good
// programming practice to enable and disable state after
// a certain rendering.)
int GLinit()
{ 
 glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

 // Culling with bill-boards, how does it work?
 // Probably has no effect...
 // glEnable(GL_CULL_FACE);

 glGenTextures(1, &textureID); // Generate a unique texture ID
 glBindTexture(GL_TEXTURE_2D, textureID); // Activate the texture
 glfwLoadTexture2D("particle_w.tga", GLFW_BUILD_MIPMAPS_BIT); // Load image
 glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);

 // Specify point sprite texture coordinate replacement mode
 glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

 glEnable(GL_TEXTURE_2D); // Enable texturing
 // Setup blending
 glEnable( GL_DEPTH_TEST );
 glDepthMask( GL_FALSE );
 glEnable(GL_BLEND);
 glBlendFunc( GL_SRC_ALPHA, GL_ONE);

 // Load the extensions for GLSL - note that this has to be done
 // *after* the window has been opened, or we won't have a GL context
 // to query for those extensions and connect to instances of them.
 loadExtensions();
 // Create the two shaders
 createShaders();

 int c, d;
 glGetObjectParameterivARB(programObj, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &c);
 glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS_ARB, &d);

 std::cerr << "Vendor " << (char *)glGetString(GL_VENDOR) << std::endl;
 std::cerr << "Version " << (char *)glGetString(GL_VERSION) << std::endl;
 std::cerr << "Max uniforms: " << d << std::endl;
 std::cerr << "Uniforms used: " << c << std::endl;
 std::cerr << "Loading..." << std::endl;

 glGetFloatv( GL_POINT_SIZE_MAX_ARB, &g_fMaxPointSize );
 glGetFloatv( GL_POINT_SIZE_MIN_ARB, &g_fMinPointSize );
 glPointParameterfARB( GL_POINT_SIZE_MAX_ARB, g_fMaxPointSize );
 glPointParameterfARB( GL_POINT_SIZE_MIN_ARB, g_fMinPointSize );

 // Enable vertex shader point size control
 glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
 // Seed randomizer
 srand(time(NULL));

 int count = 0;
 float rand_max = (float)RAND_MAX;
 // Vary radius
 for(int i = 3; i > 0; i = i - 1)
 {
 // Loop over angles
 for(int k = 0; k < 360; k = k + 24)
 {
 g_fSourcePoints[count][0] = (((float)(rand())/rand_max)*0.05 + (float)i/10.0f)*sin((float)k*M_PI/180.0f);
 g_fSourcePoints[count][1] = 0.0f;
 g_fSourcePoints[count][2] = (((float)(rand())/rand_max)*0.05 + (float)i/10.0f)*cos((float)k*M_PI/180.0f);
 count++;
 }
 }
 std::cerr << "Source Points: "<< count << std::endl;

 // Create particle list.
 // Has to be done after loading the extensions for some reason,
 // at least when the extensions give errors.
 g_PartList = new ParticleList(); 
 std::cerr << "Init complete" << std::endl;
 std::cerr << "Press 'h' for help!" << std::endl;
 printState();
 return 0; // Init ok
}

int main(int argc, char *argv[])
{
 double t,t1,t0,fps;
 float dt; // For timing particle motion
 float yrot = 0.0f;
 int frames;
 char titlestr[ 200 ];
 // Initialise GLFW
 glfwInit();

 // Open the OpenGL window
 if( !glfwOpenWindow(640, 480, 8,8,8,8, 32,0, GLFW_WINDOW) )
 {
 glfwTerminate(); // glfwOpenWindow failed, quit the program.
 delete g_PartList;
 return EXIT_FAILURE;
 }
 if( GLinit() )
 {
 glfwTerminate();
 delete g_PartList;
 return EXIT_FAILURE;
 }

 glfwSetCharCallback( KeyFun ); // Callback for keyboard
 // Do not wait for screen refresh between frames.
 // = vsync off.
 glfwSwapInterval(0);

 frames = 0; // Framecounter
 t0 = glfwGetTime(); // Initial time
 t1 = t0;

 // Main loop
 while(running)
 {
 // Clear the color buffer and the depth buffer.
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 t = glfwGetTime(); // Get new time
 dt = (float)(t-t1); // New time - Old time = How long time has passed
 t1 = t; // Old time = New Time
 // Calculate and display FPS (frames per second)...
 // if a second has passed since last time this was done
 if( (t-t0) > 1.0 || frames == 0 )
 {
 fps = (double)frames / (t-t0);
 sprintf( titlestr, "Smoke [ %i particles, %.1f FPS ] ('h' for help)", g_PartList->getNodeCount() ,fps );
 glfwSetWindowTitle( titlestr );
 t0 = t;
 frames = 0;
 }
 frames = frames + 1;

 // Set up the camera projection.
 setupCamera();

 // Rotate scene
 yrot = (yrot > 360.0f)?(yrot = yrot - 360.0f):(yrot = yrot + 5.0f*dt);
 glRotatef(yrot, 0.0f, 1.0f, 0.0f);

 // Draw the scene. 
 renderScene((float)t, dt);

 // Swap buffers, i.e. display the image and prepare for next frame.
 glfwSwapBuffers();

 // Check if the ESC key was pressed or the window was closed.
 if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
 running = GL_FALSE;
 }
 // Close the OpenGL window,terminate GLFW and free memory
 glfwTerminate();
 delete g_PartList;
 return EXIT_SUCCESS;
} 