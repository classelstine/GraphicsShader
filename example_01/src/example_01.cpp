#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>

//include header file for glfw library so that we can use OpenGL
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "example_01.h"



#define PI 3.14159265 // Should be used from mathlib

using namespace std;

//****************************************************
// Global Variables
//****************************************************
GLfloat translation[3] = {0.0f, 0.0f, 0.0f};
int auto_strech = false;
int Width_global = 400;
int Height_global = 400;


//******
//HARD CODED INPUTS -- PLEASE CHANGE
//***
float KA = 0.5;
float KD = 0.5;
float KS = 0.5;
float SPU = 0.5;
float SPV = 0.5;
bool is_isotropic = true;

Light light1 = Light(false, 0.5, 0.5, 0.5, 10, 10, 10);
vector<Light> total_lights = {light1};

//****************************************************
// Simple init function
//****************************************************
void initializeRendering()
{
    glfwInit();
}
  
// must make sure this stays in the bounds of color

//****************************************************
// A routine to set a pixel by drawing a GL point.  This is not a
// general purpose routine as it assumes a lot of stuff specific to
// this example.
//****************************************************
void setPixel(float x, float y, GLfloat r, GLfloat g, GLfloat b) {
    glColor3f(r, g, b);
    glVertex2f(x+0.5, y+0.5);  // The 0.5 is to target pixel centers
    // Note: Need to check for gap bug on inst machines.
}

//Note: p1 is the head and p2 is the tail of the vector 
void points_to_vector(Point p1, Point p2, Vector *v) { 
  v->x = p1.x - p2.x; 
  v->y = p1.y - p2.y;
  v->z = p1.z - p2.z;
} 

void scale_vector(float c, Vector v, Vector *n) {
    n->x = c * v.x;
    n->y = c * v.y;
    n->z = c * v.z;
}

float dot(Vector v1, Vector v2) {
   return (v1.x * v2.x) + (v1.y * v1.y) + (v1.z * v1.z);
}

void add_vector(Vector v1, Vector v2, Vector *v) {
    v->x = v1.x + v2.x;
    v->y = v1.y + v2.y;
    v->z = v1.z + v2.z;
}

// must make sure this stays in the bounds of color
void scale_color(float c, Color c1, Color *c2) {
    c2->red = c * c1.red;
    c2->green = c * c1.green;
    c2->blue = c * c1.blue;
}

//****************************************************
// Keyboard inputs
//****************************************************
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key) {
            
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
        case GLFW_KEY_Q: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
        case GLFW_KEY_LEFT :
            if (action) translation[0] -= 0.01f * Width_global; break;
        case GLFW_KEY_RIGHT:
            if (action) translation[0] += 0.01f * Width_global; break;
        case GLFW_KEY_UP   :
            if (action) translation[1] += 0.01f * Height_global; break;
        case GLFW_KEY_DOWN :
            if (action) translation[1] -= 0.01f * Height_global; break;
        case GLFW_KEY_F:
            if (action) auto_strech = !auto_strech; break;
        case GLFW_KEY_SPACE: break;
            
        default: break;
    }
    
}


//****************************************************
// Draw a filled circle.
//****************************************************
void drawCircle(float centerX, float centerY, float radius) {
    // Draw inner circle
    glBegin(GL_POINTS);

    // We could eliminate wasted work by only looping over the pixels
    // inside the sphere's radius.  But the example is more clear this
    // way.  In general drawing an object by loopig over the whole
    // screen is wasteful.

    int minI = max(0,(int)floor(centerX-radius));
    int maxI = min(Width_global-1,(int)ceil(centerX+radius));

    int minJ = max(0,(int)floor(centerY-radius));
    int maxJ = min(Height_global-1,(int)ceil(centerY+radius));

    for (int i = 0; i < Width_global; i++) {
        for (int j = 0; j < Height_global; j++) {

            // Location of the center of pixel relative to center of sphere
            float x = (i+0.5-centerX);
            float y = (j+0.5-centerY);

            float dist = sqrt(sqr(x) + sqr(y));

            if (dist <= radius) {

                // This is the front-facing Z coordinate
                float z = sqrt(radius*radius-dist*dist);
                Color pixel_color = Color();
                phong(x, y, z, &pixel_color);

                // given x, y, z, and light_x, light_y, and light z:
                // find abient, diffuse, and specular parts
                // solve for normal, light, and viewing vectors
                // find refectance
                // solve for each part of phong
                // total phong
                
                setPixel(i, j, pixel_color.red, pixel_color.green, pixel_color.blue);

                // This is amusing, but it assumes negative Color values are treated reasonably.
                // setPixel(i,j, x/radius, y/radius, z/radius );
                
                // Just for fun, an example of making the boundary pixels yellow.
                // if (dist > (radius-1.0)) {
                //     setPixel(i, j, 1.0, 1.0, 0.0);
                // }
            }
        }
    }

    glEnd();
}


//*****
//PHONG SHADING APPLIED
// px, py, pz : location of the pixel
// n : norm vector
// light_vecs : list of light vectors normalized
// light_cols : list of light colors 
// view : view vector
//*****
void phong(float px, float py, float pz, Color *pixel_color) {
    Color tmp_pixel_color = Color();
    Vector view = Vector(0,0,1);
    Point cur_point = Point(px, py, pz);
    Vector normal = Vector(px, py, pz);
    normal.normalize();
    Color ambient = Color(0.0, 0.0, 0.0);
    Color diffuse = Color(0.0, 0.0, 0.0);
    Color specular = Color(0.0, 0.0, 0.0);
    for(int d =0; d < total_lights.size(); d++) {
      Light cur_light = total_lights[d];
      Vector light_vec = Vector();
      Color light_col = cur_light.color;
      if(cur_light.is_direct()) {
        light_vec = Vector(cur_light.x, cur_light.y, cur_light.z);
      } else {
        Point cur_light_pt = Point(cur_light.x, cur_light.y, cur_light.z);
        points_to_vector(cur_light_pt, cur_point, &light_vec);
      }

      Vector reflect = Vector();
      reflectance(light_vec, normal, &reflect); 
      Color new_ambient = Color();
      scale_color(KA, light_col, &new_ambient);
      ambient.add_color(new_ambient);

      Color new_diffuse = Color();
      float l_n = dot(light_vec, normal);
      float positive_dot = max(l_n,(float)  0.0);
      scale_color(positive_dot * KD, light_col, &new_diffuse); 
      diffuse.add_color(new_diffuse);

      Color new_specular = Color();
      float tmp = pow(max(dot(reflect, view),(float)  0), SPU) * KS;
      scale_color(tmp, light_col, &new_specular);
      specular.add_color(new_specular);
    }
  tmp_pixel_color.add_color(ambient); 
  tmp_pixel_color.add_color(diffuse); 
  tmp_pixel_color.add_color(specular); 
  pixel_color->red = tmp_pixel_color.red;
  pixel_color->green = tmp_pixel_color.green;
  pixel_color->blue = tmp_pixel_color.blue;
}

void reflectance(Vector light_source, Vector normal, Vector *reflectance) { 
    Vector negative_norm = Vector();
    scale_vector(-1, light_source, &negative_norm); 
    float tmp = 2.0*dot(light_source, normal); 
    Vector scaled_norm = Vector();
    scale_vector(tmp, light_source, &scaled_norm); 
    Vector result_vector = Vector(); 
    add_vector(negative_norm, scaled_norm, &result_vector); 
} 


//****************************************************
// function that does the actual drawing of stuff
//***************************************************
void display( GLFWwindow* window )
{
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f ); //clear background screen to black
    
    glClear(GL_COLOR_BUFFER_BIT);                // clear the Color buffer (sets everything to black)
    
    glMatrixMode(GL_MODELVIEW);                  // indicate we are specifying camera transformations
    glLoadIdentity();                            // make sure transformation is "zero'd"
    
    //----------------------- code to draw objects --------------------------
    glPushMatrix();
    glTranslatef (translation[0], translation[1], translation[2]);
    drawCircle(Width_global / 2.0 , Height_global / 2.0 , min(Width_global, Height_global) * 0.8 / 2.0);
    glPopMatrix();
    
    glfwSwapBuffers(window);
    
}

//****************************************************
// function that is called when window is resized
//***************************************************
void size_callback(GLFWwindow* window, int width, int height)
{
    // Get the pixel coordinate of the window
    // it returns the size, in pixels, of the framebuffer of the specified window
    glfwGetFramebufferSize(window, &Width_global, &Height_global);
    
    glViewport(0, 0, Width_global, Height_global);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Width_global, 0, Height_global, 1, -1);
    
    display(window);
}


//****************************************************
// the usual stuff, nothing exciting here
//****************************************************
int main(int argc, char *argv[]) {
    //This initializes glfw
    initializeRendering();
    
    GLFWwindow* window = glfwCreateWindow( Width_global, Height_global, "CS184", NULL, NULL );
    if ( !window )
    {
        cerr << "Error on window creating" << endl;
        glfwTerminate();
        return -1;
    }
    
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if ( !mode )
    {
        cerr << "Error on getting monitor" << endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent( window );
    
    // Get the pixel coordinate of the window
    // it returns the size, in pixels, of the framebuffer of the specified window
    glfwGetFramebufferSize(window, &Width_global, &Height_global);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, Width_global, 0, Height_global, 1, -1);
    
    glfwSetWindowTitle(window, "CS184");
    glfwSetWindowSizeCallback(window, size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    while( !glfwWindowShouldClose( window ) ) // infinite loop to draw object again and again
    {   // because once object is draw then window is terminated
        display( window );
        
        if (auto_strech){
            glfwSetWindowSize(window, mode->width, mode->height);
            glfwSetWindowPos(window, 0, 0);
        }
        
        glfwPollEvents();
        
    }

    return 0;
}








