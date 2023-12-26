//
//  main.cpp
//  Assignment 5
//
//  Created by Favour Olaifa-Olawale on 2023-11-08.
//

#include <cassert>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#define GL_SILENCE_DEPRECATION

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"

#include "GetGlut.h"
#include "Sleep.h"
#include "Angles.h"
#include "BlockMap.h"
#include "PerlinNoiseField.h"
#include "Heightmap.h"
#include "SnowMap.h"
#include "Map.h"
#include "Neighbourhood.h"
#include "Player.h"
#include "CollisonBox.h"
#include "SpriteFont.h"
#include "TextureManager.h"
#include "TimeManager.h"
#include "SurfaceType.h"
#include "SnowBall.h"
#include "ConnectionGraph.h"
#include "Path.h"

using namespace std;
using namespace ObjLibrary;

void init ();
void initDisplay ();
void initModels ();
void initDebugging ();
// initShowPath();
void initCollisionBoxes ();
void printBoxCount ();
void printNoiseField ();
void printEdgeGrid ();
Heightmap calculateNoiseGrid (const PerlinNoiseField& field,
                              int base_x, int base_z);
Heightmap calculateEdgeGrid (const Neighbourhood& neighbourhood,
                             int desired_value);

void keyboard (unsigned char key, int x, int y);
void keyboardUp (unsigned char key, int x, int y);
unsigned char fixKey (unsigned char key);
void special (int special_key, int x, int y);
void specialUp (int special_key, int x, int y);

void update ();
void updateForKeyboard ();

void reshape (int w, int h);
void display ();
void drawSkybox (const Vector3& eye);
void drawAxes ();
void drawMap ();
void drawPlayer ();
void drawOpponent ();
void drawDebuggingCube ();
void drawDebuggingHeights (const Heightmap& grid,
                           double min_value, double max_value);
void drawCollisonBox();
void loadBlockMap();
bool isCollision (const Box& box1, const Box& box2);
void checkCollisions ();
bool pushBox(Box& movable, const Box& fixed);
void drawHUD();
void makePlayerSnowball();
void makeOppSnowball();
void DrawProgressBar();
void drawSnowballinPlayerHand(double hand_x, double hand_y, double hand_z);
void drawSnowballinOppHand(double hand_x, double hand_y, double hand_z);

void PhysicsUpdate();//doGameUpdates
void FrictionCheckPlayer();
void FrictionCheckOppnent();
void drawFlyingSnowball(Snowball snowball);
void drawConnectionGraph();
void AiUpdate();
void followPath(double max_speed, double factor_per_update);
double PitchCal(const Vector3& Thrower,const Vector3& Target, double max_speed );


const unsigned int KEY_UP_ARROW    = 256;
const unsigned int KEY_DOWN_ARROW  = 257;
const unsigned int KEY_LEFT_ARROW  = 258;
const unsigned int KEY_RIGHT_ARROW = 259;
const unsigned int KEY_COUNT       = 260;
bool key_pressed[KEY_COUNT];

const string RESOURCE_DIRECTORY = "Resources/";

DisplayList skybox_list;
DisplayList ground_list;
DisplayList block_list;

DisplayList player_jump_list;
DisplayList player_stand_list;
DisplayList opponent_stand_list;
DisplayList opponent_jump_list;
DisplayList snowball_list;

Map map;
vector<Box>BlockMap_Box;
Box groundCollisionBox;

Player player;
Player opponent;
Box player_Box;
Box opponent_Box;
//Box snowball_Box;
SpriteFont font;
TimeManager timeManager;
Snowball PlayerSnowball;
Snowball OppSnowball;
ConnectionGraph connectiongraph;
Path OppPath;


int window_width  = 1024;
int window_height = 768;
int PlayerNumSnowballs = 0;
int OppNumSnowballs = 0; // Initialize to 2 snowballs
float PlayerSnowballProgress = 0.0f; // Initialize to 0.0 seconds
float OppSnowballProgress = 0.0f;
int PlayerHitCount = 0;
int OppHitCount = 0;


const int SNOW_RADIUS_MIN     =  1;
const int SNOW_RADIUS_DEFAULT =  5;
const int SNOW_RADIUS_MAX     = 10;
int snow_radius = SNOW_RADIUS_DEFAULT;

const unsigned int CUBE_SHOW_LARGE  = 0;
const unsigned int CUBE_SHOW_SMALL  = 1;
const unsigned int CUBE_SHOW_HIGHER = 2;
const unsigned int CUBE_SHOW_LOWER  = 3;
const unsigned int CUBE_SHOW_COUNT  = 4;

const unsigned int PATH_NOTHING  = 0;
const unsigned int PATH_CONNECTION_GRAPH  = 1;
const unsigned int PATH_OPPONENT_PATH = 2;
const unsigned int PATH_COUNT  = 3;

bool IS_DEBUGGING_CUBE_WIREFORM = false;
bool displayCollisionBoxes = false;
bool is_debugging_cube;
unsigned int debugging_cube_show;
bool displayFrameRateInfo = false;
//bool ConnectionGraphDraw = false;
unsigned int path_show = 0;
bool OpponentisOutside = false;

bool gamePaused = false;
bool PlayerIsOnGround = true;
bool OppIsOnGround = true;

vector<Vector3> FindSnowballNodes;
vector<Vector3> ThrowSnowballNodes;

int main (int argc, char* argv[])
{
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(0, 0);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutCreateWindow("Assignment 5");
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(special);
    glutSpecialUpFunc(specialUp);
    glutIdleFunc(update);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);

    init();
    
               
       
    glutMainLoop();  // contains an infinite loop, so it never returns
    return 1;
}

void init ()
{
    initDisplay();
    initModels();
    initDebugging();
    TimeManager();//init timemanager
    Snowball();
    map = Map(RESOURCE_DIRECTORY + "map_maze.txt",
              block_list, RESOURCE_DIRECTORY + "snow1.bmp");
    player.reset(map.getPlayerStart(PLAYER_HALF_HEIGHT));
    opponent.reset(map.getOpponentStart(PLAYER_HALF_HEIGHT));
    initCollisionBoxes();
    loadBlockMap();
    printBoxCount();
    printNoiseField();
    printEdgeGrid();
    //ConnectionGraph();
    connectiongraph = ConnectionGraph(map);
    FindSnowballNodes = map.findSnowballNodes();
    ThrowSnowballNodes = map.throwSnowballNodes();
}

void initDisplay ()
{
    glClearColor(0.5, 0.5, 0.5, 0.0);
    glColor3f(0.0, 0.0, 0.0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glutPostRedisplay();
}

void initModels ()
{
    skybox_list = ObjModel(RESOURCE_DIRECTORY + "Skybox.obj").getDisplayList();
    ground_list = ObjModel(RESOURCE_DIRECTORY + "ground.obj").getDisplayList();
    block_list  = ObjModel(RESOURCE_DIRECTORY + "crate.obj").getDisplayList();
    //block_list  = ObjModel(RESOURCE_DIRECTORY + "weighted_cube.obj").getDisplayList();
    //player_list = ObjModel(RESOURCE_DIRECTORY + "cbabe_stand.obj").getDisplayList();
    player_jump_list    = ObjModel(RESOURCE_DIRECTORY + "cbabe_jump.obj" ).getDisplayList();
    player_stand_list  = ObjModel(RESOURCE_DIRECTORY + "cbabe_stand.obj" ).getDisplayList();
    opponent_stand_list = ObjModel(RESOURCE_DIRECTORY + "guard_stand.obj").getDisplayList();
    opponent_jump_list = ObjModel(RESOURCE_DIRECTORY + "guard_jump.obj").getDisplayList();
    snowball_list = ObjModel(RESOURCE_DIRECTORY + "snowball.obj").getDisplayList();
    font.load(RESOURCE_DIRECTORY +"Font.bmp");
}

void initDebugging ()
{
    is_debugging_cube   = false;
    debugging_cube_show = CUBE_SHOW_LARGE;
    path_show = PATH_NOTHING;
}


void initCollisionBoxes ()
{
        double half_size_x = 0.25;
        double half_size_y = 0.8;
        player_Box.half_size = Vector3(half_size_x, half_size_y,half_size_x);
        player_Box.center=player.getPosition();
        player_Box.is_colliding = false;
        
        groundCollisionBox.half_size.x = 500.0;  // Half size along the X-axis
        groundCollisionBox.half_size.y = 500.0;  // Half size along the Y-axis
        groundCollisionBox.half_size.z = 500.0;  // Half size along the Z-axis
        groundCollisionBox.center.x = 0.0;    // Center along the X-axis (at the origin)
        groundCollisionBox.center.y = -500.0; // Center 500 meters below the origin along the Y-axis
        groundCollisionBox.center.z = 0.0;    // Center along the Z-axis (at the origin)

        opponent_Box.half_size = Vector3(half_size_x, half_size_y,half_size_x);
        opponent_Box.center=opponent.getPosition();
        opponent_Box.is_colliding = false;
        
        PlayerSnowball.initSnowBallCollisionBoxes();
        OppSnowball.initSnowBallCollisionBoxes();
}


void printBoxCount ()
{
    unsigned int box_count = 0;
    for(int x = 0; x < map.getSizeX(); x++)
        for(int z = 0; z < map.getSizeZ(); z++)
            box_count += map.getBlockHeight(x, z);
    cout << "Total boxes: " << box_count << endl;
}

void printNoiseField ()
{
    static const unsigned int PRINT_SIZE = 25;
    static const char CHAR_VALUES[] =
    {    ' ', ' ', ' ', ' ', ' ', ' ',
        '.', '-', '=', 'o', 'O', 'H', 'M',
        '@', '@', '@', '@', '@', '@', };
    static const unsigned int CHAR_VALUE_COUNT = sizeof(CHAR_VALUES) / sizeof(CHAR_VALUES[0]);
    static const float CHAR_HALF_COUNT = CHAR_VALUE_COUNT * 0.5f;
    cout << "Perlin noise:" << endl;
    PerlinNoiseField field(10.0f, CHAR_HALF_COUNT);
    for(unsigned int y = 0; y < PRINT_SIZE; y++)
    {
        for(unsigned int x = 0; x < PRINT_SIZE; x++)
        {
            if(x % 10 == 0 && y % 10 == 0)
                cout << "+ ";
            else
            {
                float real_value = field.perlinNoise((float)(x), (float)(y)) + CHAR_HALF_COUNT + 0.5f;
                int    int_value = (int)(real_value);
                assert(int_value >= 0);
                assert(int_value <  CHAR_VALUE_COUNT);
                cout << CHAR_VALUES[int_value] << " ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

void printEdgeGrid ()
{
    static const unsigned int PRINT_SIZE = HEIGHTMAP_VERTEX_SIZE;
    Neighbourhood neighbourhood( 1,  0,  0,
                                 0,      1,
                                -1, -1,  0);
    Heightmap grid = calculateEdgeGrid(neighbourhood, 1);
    cout << "Edge distances:" << endl;
    for(unsigned int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
    {
        for(unsigned int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
        {
            int int_value = (int)(grid.getAt(x, z) * (PRINT_SIZE - 1));
            assert(int_value >= 0);
            assert(int_value <  PRINT_SIZE);
            cout << int_value << " ";
        }
        cout << endl;
    }
    cout << endl;
}

Heightmap calculateNoiseGrid (const PerlinNoiseField& field,
                              int base_x, int base_z)
{
    Heightmap grid;
    for(int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
        for(int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
        {
            float x_frac = x / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
            float z_frac = z / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
            grid.setAt(x, z, field.perlinNoise(base_x + x_frac, base_z + z_frac));
        }
    return grid;
}

Heightmap calculateEdgeGrid (const Neighbourhood& neighbourhood,
                             int desired_value)
{
    Heightmap grid;
    for(unsigned int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
        for(unsigned int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
        {
            float x_frac   = x / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
            float z_frac   = z / (HEIGHTMAP_VERTEX_SIZE - 1.0f);
            float distance = neighbourhood.getEdgeDistance(x_frac, z_frac, desired_value);
            grid.setAt(x, z, distance);
        }
    return grid;
}



void keyboard (unsigned char key, int x, int y)
{
    key = fixKey(key);

    switch(key)
    {
    case 27: // on [ESC]
        exit(0); // normal exit
        break;
    case '1':
        if(!key_pressed['1'])
            is_debugging_cube = !is_debugging_cube;
        break;
    case '2':
        if(!key_pressed['2'])
            debugging_cube_show = (debugging_cube_show + 1) % CUBE_SHOW_COUNT;
        break;
    case 'R':
        if(!key_pressed['R'] && !gamePaused)
            player.reset(map.getPlayerStart(PLAYER_HALF_HEIGHT));
        break;
    case '-':
        if(!key_pressed['-'])
            if(snow_radius > SNOW_RADIUS_MIN)
                snow_radius--;
        break;
    case '=':
        if(!key_pressed['='])
            if(snow_radius < SNOW_RADIUS_MAX)
                snow_radius++;
        break;
    case '3':
        if(!key_pressed['3'])
            displayCollisionBoxes = !displayCollisionBoxes;
        break;
    case 'F':
        if(!key_pressed['F'] && !gamePaused)
            if(PlayerNumSnowballs>0)
            {
                //throwSnowball();
                PlayerNumSnowballs--;
                PlayerSnowballProgress=0.0f;
                PlayerSnowball.ThrowSnowBall(player);
                //snowball.SnowBallUpdate()
            }
        break;
    case '4':
         if (!key_pressed['4']) {
             displayFrameRateInfo = !displayFrameRateInfo;
         }
         break;
    case '5':
          
         if (!key_pressed['5']) {
             path_show = (path_show + 1) % PATH_COUNT;
         }
            
         break;
    case 'P':
        if (!key_pressed['P']) {
               gamePaused = !gamePaused;
        }
        break;
     case ' ':
            if (!gamePaused) {
                surfacetype surface_Type;
                surface_Type = map.getPlayerSurfaceType(player_Box);
                if (surface_Type == GROUND || surface_Type == BLOCK || surface_Type == SNOW) {
                    double gravityAcceleration = 5.5 ; // 9.8 m/s² downwards
                    Vector3 dummy (0,gravityAcceleration,0);
                    player.addVelocity(dummy);
                }
            }
            break;
    case 'T':
        if (!key_pressed['T']) {
            Vector3 cubePosition = player.getPosition() + player.getForward() * 1.4;
            int cubeX = static_cast<int>(floor(cubePosition.x));
            int cubeZ = static_cast<int>(floor(cubePosition.z));
           // int cubeY = map.getBlockHeight(cubeX, cubeZ);
            
            if(map.isInMap(opponent.getPosition().x, opponent.getPosition().z) && map.isInMap(cubeX, cubeZ))
            {
                unsigned int opponentNodeIndex = connectiongraph.getNodeIndexAt(opponent.getPosition().x,opponent.getPosition().z);
                unsigned int cubeNodeIndex = connectiongraph.getNodeIndexAt(cubeX,cubeZ);
                
                OppPath = connectiongraph.aStarSearch(opponentNodeIndex, cubeNodeIndex);
               
            }
            //cout<<"h";
        }
        break;
   }
//   test

    key_pressed[key] = true;
}

void keyboardUp (unsigned char key, int x, int y)
{
    key = fixKey(key);
    key_pressed[key] = false;
}

unsigned char fixKey (unsigned char key)
{
    key = toupper(key);
    if(key == '_')
        key = '-';
    if(key == '+')
        key = '=';
    return key;
}

void special (int special_key, int x, int y)
{
    switch(special_key)
    {
    case GLUT_KEY_UP:
        key_pressed[KEY_UP_ARROW] = true;
        break;
    case GLUT_KEY_DOWN:
        key_pressed[KEY_DOWN_ARROW] = true;
        break;
    case GLUT_KEY_LEFT:
        key_pressed[KEY_LEFT_ARROW] = true;
        break;
    case GLUT_KEY_RIGHT:
        key_pressed[KEY_RIGHT_ARROW] = true;
        break;
    }
}

void specialUp (int special_key, int x, int y)
{
    switch(special_key)
    {
    case GLUT_KEY_UP:
        key_pressed[KEY_UP_ARROW] = false;
        break;
    case GLUT_KEY_DOWN:
        key_pressed[KEY_DOWN_ARROW] = false;
        break;
    case GLUT_KEY_LEFT:
        key_pressed[KEY_LEFT_ARROW] = false;
        break;
    case GLUT_KEY_RIGHT:
        key_pressed[KEY_RIGHT_ARROW] = false;
        break;
    }
}



void update ()
{
        for(unsigned int i = 0; i <  timeManager.getMaxUpdatesPerFrame() && timeManager.isUpdateWaiting(); i++)
        {
            
            if (!gamePaused)
            {
                
                AiUpdate();
                PhysicsUpdate();
              
                
            }
            timeManager.markNextUpdate();
            if (key_pressed['U']) {
                   //  Physics runs slowly while [U] is pressed, no spiral of death
                   sleep(1.0 / 50.0);
               }
           
       
    }
    timeManager.sleepUntilNextUpdate();
    
    
    // update your variables here
    
   // sleep(1.0 / 60.0);
    glutPostRedisplay();
}

void AiUpdate()
{
   
    float maxSpeed=5.0;
    float interFactor =0.2;
    surfacetype surface_Type;
    surface_Type = map.getPlayerSurfaceType(opponent_Box);
   // srand(time(0));
        
        if(surface_Type==SNOW)
        {
            maxSpeed=3.0;
            interFactor=0.5;
        }else if(surface_Type==BLOCK)
        {
            maxSpeed=5.0;
            interFactor=0.2;
        }
        else if (surface_Type==GROUND)
        {
            maxSpeed=5.0;
            interFactor=0.05;
        }
        else
        {
            maxSpeed=5.0;
            interFactor=0.02;
        }
    
    if (map.isInMap(opponent.getPosition().x, opponent.getPosition().z))
    {
        OpponentisOutside=false;
        Vector3 footposition=opponent.getPosition();
        footposition.y= footposition.y-PLAYER_HALF_HEIGHT;
        unsigned int  DummyForFoot = connectiongraph.getNodeIndexAt(footposition.x,footposition.z);
        
        if(!OppPath.isEmpty())
        {
            followPath( maxSpeed,  interFactor);
            if(!opponent.getVelocity().isZero())
            {
                double yaw_radians = atan2(opponent.getVelocity().x, opponent.getVelocity().z) - 90.0;
                
                opponent.setYawDegrees(radiansToDegrees(yaw_radians));
            }
        }
        
         if (OppPath.isEmpty() && OppNumSnowballs == 0)
        {

            if (map.hasSnowAtPosition(footposition)== true)
            {
                //cout<<"Making Snowball "<<endl;
                makeOppSnowball();
            }
            else {
                //srand(time(0));
                unsigned int index = rand() % FindSnowballNodes.size();
                Vector3 destination = FindSnowballNodes[index];
                unsigned int  DummyForDestination = connectiongraph.getNodeIndexAt(destination.x,destination.z);
                OppPath =connectiongraph.aStarSearch(DummyForFoot, DummyForDestination);
            }
        }
         
        if (OppPath.isEmpty() && OppNumSnowballs == 1)
        {
            
            if (!map.isHighIshNode(opponent.getPosition().x, opponent.getPosition().z))
            {
                cout<<"Opp is going to a high node"<<endl;
               
                unsigned int index = rand() % ThrowSnowballNodes.size();
                Vector3 throW = ThrowSnowballNodes[index];
                unsigned int  DummyForThrow = connectiongraph.getNodeIndexAt(throW.x,throW.z);
                OppPath =connectiongraph.aStarSearch(DummyForFoot, DummyForThrow);
 
            }
            else{
                bool Ballthrown = OppSnowball.ThrowOppSnowBall(opponent,player);
                if (Ballthrown==true)
                {
                    cout<<"Success Opponent Thrown Snowball"<<endl;
                    OppNumSnowballs--;
                    OppSnowballProgress=0.0f;
                }
                else
                {
                    cout<<"Failure Opponenet cannot throw Snowball.. Making a new Path"<<endl;
                    unsigned int index = rand() % FindSnowballNodes.size();
                    Vector3 destination = FindSnowballNodes[index];
                    unsigned int  DummyForDestination = connectiongraph.getNodeIndexAt(destination.x,destination.z);
                    OppPath =connectiongraph.aStarSearch(DummyForFoot, DummyForDestination);
                }
            }
        }
       
    }
    else
    {
        OpponentisOutside = true;
        followPath(maxSpeed,  interFactor);
    }
    
}

void PhysicsUpdate()//doGameUpdates
{
    
    updateForKeyboard();
    FrictionCheckPlayer();
    FrictionCheckOppnent();
    player.updatePosition(timeManager.getUpdateDeltaTime());
    opponent.updatePosition(timeManager.getUpdateDeltaTime());
    player_Box.center=player.getPosition();
    opponent_Box.center=opponent.getPosition();
    checkCollisions ();
    Vector3 footposition=player.getPosition();
    footposition.y= footposition.y-PLAYER_HALF_HEIGHT;
    if(map.hasSnowAtPosition(footposition)==true && player.getVelocity().isZero())
    {
        makePlayerSnowball();
    }
    else {
        PlayerSnowballProgress=0.0;
    }
    
   
    PlayerSnowball.SnowBallUpdate(timeManager.getUpdateDeltaTime());
    OppSnowball.SnowBallUpdate(timeManager.getUpdateDeltaTime());
     

}


void updateForKeyboard ()
{
    float maxSpeed=5.0;
    float interFactor =0.2;
    surfacetype surface_Type;
    surface_Type = map.getPlayerSurfaceType(player_Box);
 
        
        if(surface_Type==SNOW)
        {
            maxSpeed=3.0;
            interFactor=0.5;
        }else if(surface_Type==BLOCK)
        {
            maxSpeed=5.0;
            interFactor=0.2;
        }
        else if (surface_Type==GROUND)
        {
            maxSpeed=5.0;
            interFactor=0.05;
        }
        else
        {
            maxSpeed=5.0;
            interFactor=0.02;
        }

    
    if(key_pressed['W'])
    {
 
        player.accelerateToVelocity(maxSpeed,interFactor,player.getForward());
        PlayerSnowballProgress=0.0;
    }
    if(key_pressed['S'])
    {
        
        player.accelerateToVelocity(maxSpeed,interFactor,-player.getForward());
    }
    if(key_pressed['A'])
    {
        
        player.accelerateToVelocity(maxSpeed,interFactor,-player.getRight());
    }
    if(key_pressed['D'])
    {
        
        player.accelerateToVelocity(maxSpeed,interFactor,player.getRight());
    }
    
    if(key_pressed[KEY_UP_ARROW])
        player.addPitchDegrees( 5.0);
    if(key_pressed[KEY_DOWN_ARROW])
        player.addPitchDegrees(-5.0);
    if(key_pressed[KEY_LEFT_ARROW])
        player.addYawDegrees( 5.0);
    if(key_pressed[KEY_RIGHT_ARROW])
        player.addYawDegrees(-5.0);
}



void reshape (int w, int h)
{
    glViewport (0, 0, w, h);
    window_width  = w;
    window_height = h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);
    
    glutPostRedisplay();
}

void display ()
{
    if (key_pressed['Y']) {
        // Display runs slowly while [Y] is pressed, physics runs at same rate (by eyeball test)
        sleep(1.0 / 50.0);
    }
    timeManager.markNextFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // clear the screen - any drawing before here will not display at all
    
    Vector3 look_at = player.getPosition();
    look_at.y += 0.5;
    
    //change if you want camera angle to change look_at.y += 0.5;
    Vector3 eye = look_at - player.getForward() * 2.5;
    
    glLoadIdentity();
    gluLookAt(    eye.x,     eye.y,     eye.z,   // (x, y, z) camera eye position
              look_at.x, look_at.y, look_at.z,   // (x, y, z) camera look at position
              0.0,       1.0,       0.0);  // (x, y, z) camera up direction
    
    // camera is now set up - any drawing before here will display incorrectly
    
    drawSkybox(eye);  // must be first
    ground_list.draw();
    drawAxes();
    drawMap();
    drawPlayer();
    drawDebuggingCube();
    drawCollisonBox();
    drawHUD();
    drawOpponent();
    drawFlyingSnowball(PlayerSnowball);
    drawFlyingSnowball(OppSnowball);
    drawConnectionGraph();
    
    
//    // send the current image to the screen - any drawing after here will not display
    glutSwapBuffers();
}

void drawSkybox (const Vector3& eye)
{
    glPushMatrix();
    glTranslated(eye.x, eye.y, eye.z);
    glDepthMask(GL_FALSE);
    skybox_list.draw();
    glDepthMask(GL_TRUE);
    glPopMatrix();
    
}

void drawAxes ()
{
    // display positive X, Y, and Z axes near origin
    glBegin(GL_LINES);
    glColor3d(1.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(2.0, 0.0, 0.0);
    glColor3d(0.0, 1.0, 0.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 2.0, 0.0);
    glColor3d(0.0, 0.0, 1.0);
    glVertex3d(0.0, 0.0, 0.0);
    glVertex3d(0.0, 0.0, 2.0);
    glEnd();
}

void drawMap ()
{
    int draw_x = (int)(player.getPosition().x + 0.5);
    int draw_z = (int)(player.getPosition().z + 0.5);
    
    map.drawBlocks();
    if(snow_radius == SNOW_RADIUS_MAX)
        map.drawSnow(draw_x, draw_z, 999999);
    else
        map.drawSnow(draw_x, draw_z, snow_radius);
}

void drawPlayer ()
{
    const Vector3& position = player.getPosition();
    glPushMatrix();
    glTranslated(position.x, position.y, position.z);
    glRotated(player.getYawDegrees(), 0.0, 1.0, 0.0);
    //player_list.draw();
    if(PlayerIsOnGround)
    {
        player_stand_list.draw();
        if(PlayerNumSnowballs>0)
        {
            drawSnowballinPlayerHand(0.25, 0.1, 0.2);
            if(PlayerNumSnowballs==2)
            {
                drawSnowballinPlayerHand(0.18, 0.03, -0.15);
            }
            
        }
    }
    else
    {
        player_jump_list.draw();
        if(PlayerNumSnowballs>0)
        {
            drawSnowballinPlayerHand(0.45, 0.75, 0.35);
            if(PlayerNumSnowballs==2)
            {
                drawSnowballinPlayerHand(0.45, 0.75, -0.35);
            }
            
        }
    }
    glPopMatrix();
    
    
}

void drawOpponent ()
{
    const Vector3& position = opponent.getPosition();
    glPushMatrix();
    glTranslated(position.x, position.y, position.z);
    glRotated(opponent.getYawDegrees(), 0.0, 1.0, 0.0);
    if(OppIsOnGround==true)
    {
        opponent_stand_list.draw();
        if(OppNumSnowballs == 1)
        {
            drawSnowballinOppHand(0.2, 0.4, 0.18);
            
        }
    }else
    {
        opponent_jump_list.draw();
        if(OppNumSnowballs == 1)
        {
            drawSnowballinOppHand(0.5, 0.8, 0.45);
            
        }
    }
    
    
    glPopMatrix();
    
}

void drawDebuggingCube ()
{
    if(is_debugging_cube)
    {
        Vector3 ahead = player.getPosition() + player.getForward() * 1.4;
        int ahead_x = (int)(floor(ahead.x));
        int ahead_z = (int)(floor(ahead.z));
        
        int ahead_y = 0;
        if(map.isInMap(ahead_x, ahead_z))
            ahead_y = map.getBlockHeight(ahead_x, ahead_z);
        
        glColor3d(0.0, 1.0, 0.0);
        glPushMatrix();
        glTranslated(ahead_x + 0.5, ahead_y + 0.5, ahead_z + 0.5);
        glutWireCube(1.0);
        
        Heightmap grid;
        double min_value = 0.0;
        double max_value = 1.0;
        switch(debugging_cube_show)
        {
            case CUBE_SHOW_LARGE:
                grid = calculateNoiseGrid(map.getNoiseLarge(), ahead_x, ahead_z);
                min_value = map.getNoiseLarge().getAmplitude() * -0.5;
                max_value = map.getNoiseLarge().getAmplitude() *  0.5;
                break;
            case CUBE_SHOW_SMALL:
                grid = calculateNoiseGrid(map.getNoiseSmall(), ahead_x, ahead_z);
                min_value = map.getNoiseSmall().getAmplitude() * -0.5;
                max_value = map.getNoiseSmall().getAmplitude() *  0.5;
                break;
            case CUBE_SHOW_HIGHER:
                grid = calculateEdgeGrid(map.getNeighbourhood(ahead_x, ahead_z), 1);
                min_value = 0.0;
                max_value = 1.0;
                break;
            case CUBE_SHOW_LOWER:
                grid = calculateEdgeGrid(map.getNeighbourhood(ahead_x, ahead_z), -1);
                min_value = 0.0;
                max_value = 1.0;
                break;
        }
        drawDebuggingHeights(grid, min_value, max_value);
        
        glPopMatrix();
    }
}

void drawDebuggingHeights (const Heightmap& grid,
                           double value_min, double value_max)
{
    assert(value_min < value_max);
    
    double value_range = value_max - value_min;
    assert(value_range > 0.0);
    
    if(IS_DEBUGGING_CUBE_WIREFORM)
    {
        // simpler value grid (point-based)
        
        glPointSize(5.0);
        glBegin(GL_POINTS);
        for(int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
            for(int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
            {
                double draw_x = x / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
                double draw_z = z / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
                
                double value = (grid.getAt(x, z) - value_min) / value_range;
                glColor3d(1.0, value, 0.0);
                glVertex3d(draw_x, value - 0.5, draw_z);
            }
        glEnd();
    }
    else
    {
        // nicer-looking value grid (wireform)
        
        // lines parallel to X axis
        for(int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
        {
            glBegin(GL_LINE_STRIP);
            for(int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
            {
                double draw_x = x / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
                double draw_z = z / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
                
                double value = (grid.getAt(x, z) - value_min) / value_range;
                glColor3d(1.0, value, 0.0);
                glVertex3d(draw_x, value - 0.5, draw_z);
            }
            glEnd();
        }
        
        // lines parallel to Z axis
        for(int z = 0; z < HEIGHTMAP_VERTEX_SIZE; z++)
        {
            glBegin(GL_LINE_STRIP);
            for(int x = 0; x < HEIGHTMAP_VERTEX_SIZE; x++)
            {
                double draw_x = x / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
                double draw_z = z / (HEIGHTMAP_VERTEX_SIZE - 1.0) - 0.5;
                
                double value = (grid.getAt(x, z) - value_min) / value_range;
                glColor3d(1.0, value, 0.0);
                glVertex3d(draw_x, value - 0.5, draw_z);
            }
            glEnd();
        }
    }
}


void drawCollisonBox()
{
    if (displayCollisionBoxes)
    {
        player_Box.drawBox(player_Box);
        
        for (int i = 0; i < BlockMap_Box.size(); i++)
        {
            BlockMap_Box[i].drawBlockMap(BlockMap_Box[i]);
        }
        opponent_Box.drawBox(opponent_Box);
        PlayerSnowball.snowBall_Box.drawBox(PlayerSnowball.snowBall_Box);
        OppSnowball.snowBall_Box.drawBox(OppSnowball.snowBall_Box);
    }
}


void loadBlockMap()
{
    
    for (int row = 0; row < map.getSizeX(); row++)
    {
        for (int col = 0; col < map.getSizeZ(); col++)
        {
            if (map.getBlockHeight(row, col) > 0)
            {
                Box collisionBox;
                float getblockheight=map.getBlockHeight(row, col);
                
                collisionBox.half_size.x=0.5;
                collisionBox.half_size.y=(getblockheight/2);
                collisionBox.half_size.z=0.5;
                collisionBox.center.x=row+0.5;
                collisionBox.center.y= (getblockheight/2);
                collisionBox.center.z=col+0.5;
                
                BlockMap_Box.push_back(collisionBox);
            }
            
        }
    }
    
}



bool isCollision (const Box& box1, const Box& box2)
{
    // add code here
    Vector3 min1 = box1.center - box1.half_size;
    Vector3 max1 = box1.center + box1.half_size;
    Vector3 min2 = box2.center - box2.half_size;
    Vector3 max2 = box2.center + box2.half_size;
    
    if(min1.x >= max2.x)
        return false;
    if(min2.x >= max1.x)
        return false;
    if(min1.y >= max2.y)
        return false;
    if(min2.y >= max1.y)
        return false;
    if(min1.z >= max2.z)
        return false;
    if(min2.z >= max1.z)
        return false;
    return true;
}


void checkCollisions ()
{
    
    if (isCollision(player_Box, groundCollisionBox))
    {
    
        pushBox(player_Box, groundCollisionBox);
        player.setPosition(player_Box.center);
    }
    
    
    for (int i=0;i<BlockMap_Box.size();i++)
    {
        if (isCollision(player_Box, BlockMap_Box[i]))
        {
            //cout<<"say something"<<endl;
            //boxes[i].is_colliding = true;
            if (pushBox(player_Box, BlockMap_Box[i]) == true) //vertically
            {
                Vector3 dummy = player.getVelocity();
                dummy.y=0.0;
                player.setVelocity(dummy);

            }
            else{ //horizontal collision
                Vector3 dummy = player.getVelocity();
                dummy.x=0.0;
                dummy.z=0.0;
                player.setVelocity(dummy);
            }
                
            
            player.setPosition(player_Box.center);
        }
    }
    
    
    if (isCollision(player_Box, opponent_Box))
    {
        PlayerSnowballProgress = 0.0f;
        pushBox(player_Box, opponent_Box);
        player.setPosition(player_Box.center);
    }
    
    if (isCollision(PlayerSnowball.snowBall_Box, groundCollisionBox))
    {
        PlayerSnowball.setPosition(Vector3(0,-999999,0));
    }
    
    
    for (int i=0;i<BlockMap_Box.size();i++)
    {
        if (isCollision(PlayerSnowball.snowBall_Box, BlockMap_Box[i]))
        {
            PlayerSnowball.setPosition(Vector3(0,-999999,0));
        }
        
        if (isCollision(OppSnowball.snowBall_Box, BlockMap_Box[i]))
        {
            OppSnowball.setPosition(Vector3(0,-999999,0));
        }
        
        
    }
    
    
    if (isCollision(PlayerSnowball.snowBall_Box, opponent_Box))
    {
        PlayerHitCount++;
        PlayerSnowball.setPosition(Vector3(0,-999999,0));
        Vector3 dummy = (PlayerSnowball.getVelocity() * 0.3);
        opponent.addVelocity(dummy);
    }
   
    //opponent
    
    if (isCollision(opponent_Box, groundCollisionBox))
    {
        pushBox(opponent_Box, groundCollisionBox);
        opponent.setPosition(opponent_Box.center);
    }
    
    
    for (int i=0;i<BlockMap_Box.size();i++)
    {
        if (isCollision(opponent_Box, BlockMap_Box[i]))
        {

            if (pushBox(opponent_Box, BlockMap_Box[i]) == true) //vertically
            {
                Vector3 dummy = opponent.getVelocity();
                dummy.y=0.0;
                opponent.setVelocity(dummy);

            }
            else{ //horizontal collision
                Vector3 dummy = opponent.getVelocity();
                dummy.x=0.0;
                dummy.z=0.0;
                opponent.setVelocity(dummy);
            }
            opponent.setPosition(opponent_Box.center);
        }
    }
    
    if (isCollision(OppSnowball.snowBall_Box, player_Box))
    {
        OppHitCount++;
        OppSnowball.setPosition(Vector3(0,-999999,0));
        Vector3 dummy = (OppSnowball.getVelocity() * 0.3);
        player.addVelocity(dummy);

    }
    
}



bool pushBox(Box& movable, const Box& fixed)
{
    // Determine the half-size sums as a Vector3
    Vector3 halfSizeSum = movable.half_size + fixed.half_size;
    
    // Determine the distance between the centers along each axis
    double dx = fabs(movable.center.x - fixed.center.x);
    double dy = fabs(movable.center.y - fixed.center.y);
    double dz = fabs(movable.center.z - fixed.center.z);
    
    // Determine how much overlap there is along each axis
    double overlapX = halfSizeSum.x - dx;
    double overlapY = halfSizeSum.y - dy;
    double overlapZ = halfSizeSum.z - dz;
    //cout<<overlapX<<" "<<overlapY<<"  "<<overlapZ<<endl;
    // Check which axis has less overlap
    if (overlapX < overlapY && overlapX < overlapZ)
    {
        // Move along the X-axis
        if (movable.center.x < fixed.center.x)
        {
            // Move movable to the left
            movable.center.x = fixed.center.x - halfSizeSum.x;
        }
        else
        {
            // Move movable to the right
            movable.center.x = fixed.center.x + halfSizeSum.x;
        }
        return false;
    }
    else if (overlapY < overlapZ)
    {
        // cout<<"chainng Y"<<endl;
        if (movable.center.y < fixed.center.y)  // Move along the Y-axis
        {
            // Move movable up
            //cout<<"up"<<endl;
            movable.center.y = fixed.center.y - halfSizeSum.y;
        }
        else
        {
            //cout<<"down"<<endl;
            // Move movable down
            movable.center.y = fixed.center.y + halfSizeSum.y;
        }
        //cout<<movable.center.y<<endl;
        return true;
    }
    else
    {
        if (movable.center.z < fixed.center.z)  // Move along the Y-axis
        {
            // Move movable down
            movable.center.z = fixed.center.z - halfSizeSum.z;
        }
        else
        {
            // Move movable up
            movable.center.z = fixed.center.z + halfSizeSum.z;
        }
        return false;
    }
}



void drawHUD()
{
    
    stringstream ss;
    ss << "No. of snowballs: " << setw(2) <<  PlayerNumSnowballs;
    string black_string = ss.str();
    
    int black_width = font.getWidth(black_string);
    int left_x  = 200 - black_width;
    // int right_x = black_width -200;
    
    stringstream Phits_ss;
    Phits_ss << "Player Hit Count: " << setw(2) << PlayerHitCount;
    string Phits_string = Phits_ss.str();
    int Phits_width = font.getWidth(Phits_string);
    
    stringstream Opphits_ss;
    Opphits_ss << "Opponent Hit Count: " << setw(2) << OppHitCount;
    string Opphits_string = Opphits_ss.str();
    int Opphits_width = font.getWidth(Opphits_string);
    
    stringstream frame_count_ss;
    frame_count_ss << "Frame count: " << timeManager.getFrameCount();
    
    
    float game_duration = timeManager.getGameDuration();
    stringstream game_time_ss;
    game_time_ss << "Game running time: " << game_duration;
    
    
    float average_frames_per_second = timeManager.getFrameRateAverage();
    stringstream average_frame_rate_ss;
    average_frame_rate_ss << "Average frame rate: " << average_frames_per_second;
    
    
    float instantaneous_frames_per_second = timeManager.getFrameRateInstantaneous();
    stringstream instantaneous_frame_rate_ss;
    instantaneous_frame_rate_ss << "Instantaneous frame rate: " << instantaneous_frames_per_second;
    
    stringstream smoothed_frame_rate_ss;
    smoothed_frame_rate_ss << "Smoothed frame rate: " << timeManager.getFrameRateSmoothed();
    
    stringstream update_count_ss;
    update_count_ss << "Update count: " << timeManager.getUpdateCount();
    
    float average_updates_per_second = timeManager.getUpdateRateAverage();
    stringstream average_update_rate_ss;
    average_update_rate_ss << "Average update rate: " << average_updates_per_second;
    
    // Display instantaneous update rate 24 pixels below the average update rate
    stringstream instantaneous_update_rate_ss;
    instantaneous_update_rate_ss << "Instantaneous update rate: " << timeManager.getUpdateRateInstantaneous();
    
    stringstream smoothed_update_rate_ss;
    smoothed_update_rate_ss << "Smoothed update rate: " << timeManager.getUpdateRateSmoothed();
    
    
    
    SpriteFont::setUp2dView(window_width, window_height);
    glDisable(GL_DEPTH_TEST);
    
    if(displayFrameRateInfo)
    {
        font.draw(frame_count_ss.str(), 16, 48,0 ,0 ,0);
        font.draw(game_time_ss.str(), 16, 16,0,0,0);
        font.draw(average_frame_rate_ss.str(), 16, 72,0,0,0);
        font.draw(instantaneous_frame_rate_ss.str(), 16, 96,0,0,0);
        font.draw(smoothed_frame_rate_ss.str(), 16, 120,0,0,0);
        font.draw(update_count_ss.str(), 16, 152,0,0,0);
        font.draw(average_update_rate_ss.str(), 16, 176,0,0,0);
        font.draw(instantaneous_update_rate_ss.str(), 16, 200,0,0,0);
        font.draw(smoothed_update_rate_ss.str(), 16, 224,0,0,0);
    }
    //
    if(gamePaused)
    {
        font.draw("Game Is Paused", 16, window_height - 116,0,2,0);
    }
    font.draw(black_string, left_x,  window_height-60,   0,   0,   0);
    //font.draw(hits_string, 200 - hits_width ,10 , 0, 1, 0);
    font.draw(Phits_string, 200 - Phits_width ,window_height-90 , 0, 1, 0);
    font.draw(Opphits_string, 500 -Opphits_width ,window_height-90 , 0, 1, 0);
    DrawProgressBar();
    glEnable(GL_DEPTH_TEST);
    SpriteFont::unsetUp2dView();
    
    
}


void makePlayerSnowball()
{
    
         if (PlayerNumSnowballs < 2) {
            // Player is not moving and has fewer than 2 snowballs
            // Increment the progress by the sleep time once each frame
            PlayerSnowballProgress += (1.0f / 60.0f); //  60 FPS, adjust as needed
            // Check if the progress reaches 3.0 seconds
            if (PlayerSnowballProgress >= 3.0f) {
                // Reset the progress to 0.0 and increment the snowball count
                PlayerSnowballProgress = 0.0f;
                PlayerNumSnowballs++; // Increment snowball count
            }
        }
    
}


void makeOppSnowball()
{
    if (OppNumSnowballs < 1) {
       // Player is not moving and has fewer than 2 snowballs
       // Increment the progress by the sleep time once each frame
        //cout<<"Making snowball in "<<OppSnowballProgress<<endl;
       OppSnowballProgress += (1.0f / 60.0f); //  60 FPS, adjust as needed
       // Check if the progress reaches 3.0 seconds
       if (OppSnowballProgress >= 3.0f) {
           // Reset the progress to 0.0 and increment the snowball count
           OppSnowballProgress = 0.0f;
           OppNumSnowballs++; // Increment snowball count
       }
        
   }
}


void DrawProgressBar()
{
    if (PlayerSnowballProgress > 0.0f)
    {
        int x0=0;
        int x1=window_width;
        int y0=window_height-24;
        int y1=window_height-0;
        
        // Draw the dark background bar at the bottom left
        glColor3ub(0, 0, 0);
        glBegin(GL_POLYGON);
        glVertex2i(x0, y0);                             // Bottom-left
        glVertex2i(x0, y1);                            // Top-left
        glVertex2i(x1, y1);                  // Top-right
        glVertex2i(x1, y0);                   // Bottom-right
        glEnd();
        
        // Calculate the width of the white progress bar based on the player's progress
        int padding = 2; // Padding on each side
        int maxBarWidth = window_width - 2 * padding;
        int bar_width = static_cast<int>((PlayerSnowballProgress/3) * maxBarWidth);
        
        
        // Draw the white progress bar on top of the dark background
        glColor3ub(255, 255, 255);
        glBegin(GL_POLYGON);
        glVertex2i(x0+padding, y0+2);                        // Bottom-left of white bar
        glVertex2i(x0+padding, y1-2);                       // Top-left of white bar
        glVertex2i(x0+padding + bar_width,y1-2);           // Top-right of white bar
        glVertex2i(x0+padding + bar_width,y0+ 2);            // Bottom-right of white bar
        glEnd();
    }
    
}

void drawSnowballinPlayerHand(double hand_x, double hand_y, double hand_z)
{
    glPushMatrix();
 // glTranslated(player.getPosition().x, player.getPosition().y,player.getPosition().z);
    glTranslated(hand_x, hand_y, hand_z);
    glRotated(player.getYawDegrees(), 0.0, 1.0, 0.0);
    glScaled (0.1, 0.1, 0.1);
    snowball_list.draw();
    glPopMatrix();
}

void drawSnowballinOppHand(double hand_x, double hand_y, double hand_z)
{
    glPushMatrix();
 // glTranslated(player.getPosition().x, player.getPosition().y,player.getPosition().z);
    glTranslated(hand_x, hand_y, hand_z);
    glRotated(opponent.getYawDegrees(), 0.0, 1.0, 0.0);
    glScaled (0.1, 0.1, 0.1);
    snowball_list.draw();
    glPopMatrix();
}

void drawFlyingSnowball(Snowball snowball)
{
    glPushMatrix();
    glTranslated(snowball.getPosition().x, snowball.getPosition().y, snowball.getPosition().z);
    glScaled(0.1,0.1,0.1);
    glRotated(player.getYawDegrees(), 1.0, 0.0, 0.0);
    snowball_list.draw();// from -1.0 to +1.0
    glPopMatrix();
}




void FrictionCheckPlayer()
{
    surfacetype surface_Type;
    surface_Type = map.getPlayerSurfaceType(player_Box);
   // surface_Type = map.getPlayerSurfaceType(opponent_Box);
    
    if (surface_Type == GROUND || surface_Type == BLOCK || surface_Type == SNOW) //apply friction
    {
        double maxFrictionAcceleration = 25.0 * timeManager.getUpdateDeltaTime();
       
     
        if(surface_Type==SNOW)
        {
             maxFrictionAcceleration = 25.0 * timeManager.getUpdateDeltaTime();
        }else if(surface_Type==BLOCK)
        {
            maxFrictionAcceleration = 15.0 * timeManager.getUpdateDeltaTime();
        }
            else if (surface_Type==GROUND)
            {
                maxFrictionAcceleration = 2.0 * timeManager.getUpdateDeltaTime();
            }
        Vector3 friction = -player.getVelocity();
        friction.truncate(maxFrictionAcceleration);
        
        player.addVelocity(friction);
        PlayerIsOnGround=true;
       }
    else //dont apply friction
    {
        double gravityAcceleration = -9.8 * timeManager.getUpdateDeltaTime(); // 9.8 m/s² downwards
        Vector3 dummy (0,gravityAcceleration,0);
        player.addVelocity(dummy);
        PlayerIsOnGround=false;
    }
    
}



void FrictionCheckOppnent()
{
    surfacetype surface_Type;
    surface_Type = map.getPlayerSurfaceType(opponent_Box);
   // surface_Type = map.getPlayerSurfaceType(opponent_Box);
    
    if (surface_Type == GROUND || surface_Type == BLOCK || surface_Type == SNOW) //apply friction
    {
        double maxFrictionAcceleration = 25.0 * timeManager.getUpdateDeltaTime();
       
     
        if(surface_Type==SNOW)
        {
             maxFrictionAcceleration = 25.0 * timeManager.getUpdateDeltaTime();
        }else if(surface_Type==BLOCK)
        {
            maxFrictionAcceleration = 15.0 * timeManager.getUpdateDeltaTime();
        }
            else if (surface_Type==GROUND)
            {
                maxFrictionAcceleration = 2.0 * timeManager.getUpdateDeltaTime();
            }
        Vector3 friction = -opponent.getVelocity();
        friction.truncate(maxFrictionAcceleration);
        opponent.addVelocity(friction);
       // cout<<"after Snowball with oppornet Collision"<<opponent.getVelocity()<<endl;
        OppIsOnGround=true;
       }
    else //dont apply friction
    {
        double gravityAcceleration = -9.8 * timeManager.getUpdateDeltaTime(); // 9.8 m/s² downwards
        Vector3 dummy (0,gravityAcceleration,0);
        opponent.addVelocity(dummy);
        OppIsOnGround=false;
    }
    
}



void drawConnectionGraph()
{
        switch(path_show)
        {
               
            case PATH_NOTHING:
               // cout<<"work"<<endl;
                break;
            case PATH_CONNECTION_GRAPH:
                connectiongraph.drawNodes();
                connectiongraph.drawLinks();
                break;
            case PATH_OPPONENT_PATH :
                    if(OppPath.isEmpty())
                    {
                        connectiongraph.drawNodes();
                        }
                    else
                    {
                        OppPath.drawPath(connectiongraph);
                        OppPath.drawSearchValues(connectiongraph);
                
                    }
                    
                break;
        }

}


void followPath(double max_speed, double factor_per_update) {

    surfacetype surface_Type;
    surface_Type = map.getPlayerSurfaceType(opponent_Box);
    //cout<<OppNumSnowballs<<endl;
    if (!OppPath.isEmpty() && OpponentisOutside==false) {
        const Vector3& second_node_position = OppPath.getSecondNodePosition(connectiongraph);
        
        if(second_node_position.getDistanceXZ(opponent.getPosition()) < 0.25)
        {
            OppPath.removeFirstLinkAndSetNewStartNode();
            OppPath.resetLinkTimer();
           // cout<<"if"<<OppPath.getLinkTimer()<<endl;
        }
        else{
            OppPath.updateLinkTimer(timeManager.getUpdateDeltaTime());
           // cout<<"else"<<OppPath.getLinkTimer()<<endl;
            if (OppPath.getLinkTimer() > 3.0) {
                //reset path
                OppPath=Path();
                        }
        }

        // Get the position of the new first node
        const Vector3& new_second_node_position = OppPath.getSecondNodePosition(connectiongraph);
        
        unsigned int first_link_type = OppPath.getLinkTypeToSecondNode();
        
      
        
        if (first_link_type == LINK_TYPE_UP_1 || first_link_type == LINK_TYPE_JUMP3 || first_link_type == LINK_TYPE_JUMP2) {
            
            if (surface_Type == GROUND || surface_Type == BLOCK || surface_Type == SNOW) {
                Vector3 test = opponent.getVelocity()*0.5;
                test.y=5.5;
                opponent.setVelocity(test);
                OppPath.FrontToAdjacent();
            }
            
        }
       
            opponent.accelerateToNode(max_speed, factor_per_update,new_second_node_position);

    
    }
        if(OpponentisOutside == true)
        {
            Vector3 dummy;
            dummy.x=static_cast<int>(floor(map.getSizeX()/2));
            dummy.z=static_cast<int>(floor(map.getSizeZ()/2));
            opponent.accelerateToNode(max_speed, factor_per_update,dummy);
        }
    
}



