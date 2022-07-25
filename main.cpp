#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <string>
#include "tga.h"
using namespace std;

float proporzione = 4.0; // proporzione del labirinto
// gluLookAt
float eyeX = 0.0, eyeY = 0.0, centerX = 0.0, centerY = 0.0, eyeZ = 1.0;
float alpha = 0.0, delta_alpha = 3, pgreco = 3.14159, speed = 0.2;

// variabili di gioco
bool win = false, easy = false, wall = false, lose = false, fullScreen = false;

int rotate = 0;
// init         
GLfloat ambientLight [ ] = { 0.5f, 0.5f , 0.5f , 1.0f}; // slide
GLfloat diffuseLight [ ] = { 0.7f , 0.7f , 0.7f , 1.0f } ;
GLfloat lightPos [ ] = { -50.0f, 50.0f , 100.0f , 1.0f } ;
GLfloat fog_color [ ] = {0.4, 0.4, 0.4, 0 } ; 

GLfloat normalUp[3] = {0, 0, 1};
GLfloat normalFront[3] = {1, 1, 0};
GLfloat normalRight[3] = {-1, 1, 0};
GLfloat normalLeft[3] = {1, -1, 0};
// mappa
const int dim = 30;
int mappa[dim][dim] = {0};
// texture
const int nTexture = 5;
GLuint textures[nTexture];
const char *textureFile[nTexture] = {"texture/floor.tga", "texture/wall.tga", "texture/ceiling.tga", "texture/cubo.tga", "texture/earth.tga"};

// timer
int timer = 300; // 300 sec , 5 minuti
string strTimer = "";

int timerCloseWindow = 50;
string strTimerCloseWindow = "";

void inizializza();
void init();
void reshape(int, int);
void topDisplay();
void mainDisplay();
void drawLabyrinth();
void handlerCube(float, float);
void drawPlayer(float, float);
void texture();
void keyboardSpecial(int, int, int);
void keyboard(unsigned char, int, int);
void TimeRotation(int);
void Timer(int);

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 800);

    int mainWindow = glutCreateWindow("Progetto Informatica Grafica");
    srand(time(0));
    inizializza();

    init();
    glutDisplayFunc(mainDisplay);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyboardSpecial);
    glutReshapeFunc(reshape);
    TimeRotation(25);
    Timer(1000);
    texture();

    int subTopWindow = glutCreateSubWindow(mainWindow, 0, 0, 2000, 20);
    glutDisplayFunc(topDisplay);

    glutMainLoop();
    glDeleteTextures(nTexture, textures);
}

// inizializzo la mappa la posizione iniziale
int getRandInt()
{
    int n = rand() % (dim - 1);
    return n;
}

void initMap()
{
    ifstream in("mappa/mappa.txt");
    for (unsigned int i = 0; i < dim; i++)
    {
        for (unsigned int j = 0; j < dim; j++)
        {
            in >> mappa[i][j];
        }
    }
    in.close();
}

void inizializza()
{
    initMap();
    int x = getRandInt();
    int y = getRandInt();
    while (mappa[x][y] == 1 || mappa[x][y] == 2 || mappa[x][y] == 3)
    {
        x = getRandInt();
        y = getRandInt();
    }
    cout << "[ " << x << " - " << y << " ]" << endl;
    eyeX = x * proporzione;
    eyeY = y * proporzione;

    centerX = eyeX + cos((alpha * pgreco) / 180);
    centerY = eyeY + sin((alpha * pgreco) / 180);
}

// init e reshape
void init()
{
    glClearColor(0.4, 0.4, 0.4, 0);

    glEnable(GL_LIGHTING);

    // glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_LIGHT0);

    glFogfv(GL_FOG_COLOR, fog_color);
    glFogf(GL_FOG_START, 3);
    glFogf(GL_FOG_END, 10);
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glEnable(GL_FOG);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// mainDisplay e topDisplay
void mainDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    gluLookAt(eyeX, eyeY, eyeZ, centerX, centerY, 1.0, 0.0, 0.0, 1.0);
    drawLabyrinth();

    if (easy)
    {
        float x = eyeX / proporzione;
        float y = eyeY / proporzione;
        drawPlayer(x, y);
    }

    glPopMatrix();
    glutSwapBuffers();
}

void drawPlayer(float j, float i)
{   
    i = i * proporzione;
    j = j * proporzione;
    float ip = i + proporzione / 4;
    float jp = j + proporzione / 4;
    glBindTexture(GL_TEXTURE_2D, textures[3]);

    glBegin(GL_QUADS);

    // Parte superiore dei muri => "coperchio cubo"
    glNormal3fv(normalUp);
    glTexCoord2f(1, 0);
    glVertex3f(j, i, proporzione);
    glTexCoord2f(1, 1);
    glVertex3f(j, ip, proporzione);
    glTexCoord2f(0, 1);
    glVertex3f(jp, ip, proporzione);
    glTexCoord2f(0, 0);
    glVertex3f(jp, i, proporzione);
    glEnd();
}

// printbitmap
// Prints the given string at the given raster position
// using GLUT bitmap fonts.
// You probably don't want any rotations in the model/view
// transformation when calling this function.
void printbitmap(const string msg, double x, double y)
{
    glColor3f(1.0f, 1.0f, 1.0f); // sets color
    glRasterPos2d(x, y);
    for (string::const_iterator ii = msg.begin(); ii != msg.end(); ++ii)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *ii);
    }
}

void topDisplay()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    if (!lose && !win)
    {
        glClearColor(0.0, 0.0, 1.0, 0.0); // dark blue

        printbitmap("Timer:", -0.99, -0.5);
        printbitmap(strTimer, -0.92, -0.5);

        printbitmap("sec  |  f : FULL/NORMAL SCREEN  |  w : WALL/NOT WALL  |  e : EASY/NORMAL  |  esc : QUIT", -0.88, -0.5);
    }
    else if (win)
    {
        glClearColor(0.0, 0.5, 0.0, 0.0); // "dark" green
        printbitmap("HAI VINTO      |    esc : QUIT", -0.99, -0.5);
        printbitmap("TIMER CHIUSURA FINESTRA: ", -0.70, -0.5);
        printbitmap(strTimerCloseWindow, -0.42, -0.5);
        printbitmap("sec  |  r : RESTART", -0.38, -0.5);
    }
    else if (lose)
    {
        glClearColor(1.0, 0.0, 0.0, 0.0); // red

        printbitmap("HAI PERSO      |    esc : QUIT", -0.99, -0.5);
        printbitmap("TIMER CHIUSURA FINESTRA: ", -0.70, -0.5);
        printbitmap(strTimerCloseWindow, -0.42, -0.5);
        printbitmap("sec  |  r : RESTART", -0.38, -0.5);
    }

    glPopMatrix();

    glutSwapBuffers();
    glutPostRedisplay();
}

// texture
void texture()
{
    GLbyte *pBytes = 0;
    GLint iWidth, iHeight, iComponents;
    GLenum eFormat;
    GLubyte i;

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glGenTextures(nTexture, textures);

    // Carica texture
    for (i = 0; i < nTexture; i++)
    {

        glBindTexture(GL_TEXTURE_2D, textures[i]);

        pBytes = gltLoadTGA(textureFile[i], &iWidth, &iHeight, &iComponents,
                            &eFormat);
        glTexImage2D(GL_TEXTURE_2D, 0, iComponents, iWidth, iHeight, 0, eFormat,
                     GL_UNSIGNED_BYTE, pBytes);

        free(pBytes);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
}

// keyboard movimenti e collisioni e keybord
bool collision(bool flag)
{
    float speed2 = speed + 0.2;
    int x;
    int y;
    if (flag == true)
    {
        x = (int)(((eyeX) / (proporzione)) + speed2 * cos((alpha * pgreco) / 180));
        y = (int)(((eyeY) / (proporzione)) + speed2 * sin((alpha * pgreco) / 180));
    }
    else
    {
        x = (int)(((eyeX) / (proporzione)) - speed2 * cos((alpha * pgreco) / 180));
        y = (int)(((eyeY) / (proporzione)) - speed2 * sin((alpha * pgreco) / 180));
    }
    if (mappa[x][y] == 3)
    {
        return true;
    }
    else if (mappa[x][y] == 1 && !wall)
    {
        return true;
    }
    else if ( mappa[x+1][y+1] == 2 ||  mappa[x + 1][y] == 2 || mappa[x][y + 1] == 2 || mappa[x][y] == 2)
    {
        win = true;
        return true;
    }
    return false;
}
void keyboardSpecial(int key, int w, int h)
{
    if (!win && !lose)
    {
        switch (key)
        {
        case GLUT_KEY_RIGHT:
            alpha -= delta_alpha;
            centerX = eyeX + cos((alpha * pgreco) / 180);
            centerY = eyeY + sin((alpha * pgreco) / 180);
            glutPostRedisplay();
            break;

        case GLUT_KEY_LEFT:
            alpha += delta_alpha;
            centerX = eyeX + cos((alpha * pgreco) / 180);
            centerY = eyeY + sin((alpha * pgreco) / 180);
            glutPostRedisplay();
            break;
        case GLUT_KEY_UP:
            if (!collision(true))
            {
                eyeX = eyeX + speed * cos((alpha * pgreco) / 180);
                eyeY = eyeY + speed * sin((alpha * pgreco) / 180);
                centerX = eyeX + cos((alpha * pgreco) / 180);
                centerY = eyeY + sin((alpha * pgreco) / 180);
                glutPostRedisplay();
            }

            break;

        case GLUT_KEY_DOWN:
            if (!collision(false))
            {
                eyeX = eyeX - speed * cos((alpha * pgreco) / 180);
                eyeY = eyeY - speed * sin((alpha * pgreco) / 180);
                centerX = eyeX + cos((alpha * pgreco) / 180);
                centerY = eyeY + sin((alpha * pgreco) / 180);
                glutPostRedisplay();
            }

            break;

        default:
            break;
        }
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'w':
        if (wall)
            wall = false;
        else
            wall = true;
        glutPostRedisplay();
        break;

    case 'f':
        if (!fullScreen)
        {
            fullScreen = true;
            glutFullScreen();
        }
        else if (fullScreen)
        {
            fullScreen = false;

            glutReshapeWindow(900, 800);
        }
        glutPostRedisplay();
        break;
    case 'e':
        if (easy == false)
        {
            easy = true;
            eyeZ = 19.2;
            glDisable(GL_FOG);
        }
        else
        {
            easy = false;
            eyeZ = 1.0;
            glEnable(GL_FOG);
        }
        glutPostRedisplay();
        break;

    default:
        break;
    }
    if(lose || win )
        if(key == 'r') {
            inizializza();
            lose = false;
            win = false;
            timer = 300;
            timerCloseWindow = 50;
        }
    if (key == 27)
        exit(0);
}

// drawLabirynt
void drawFloor(int i, int j)
{

    i = i * proporzione;
    j = j * proporzione;
    int ip = i + proporzione;
    int jp = j + proporzione;

    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(jp, i, 0.0);

    glTexCoord2f(1, 0);
    glVertex3f(j, i, 0.0);

    glTexCoord2f(1, 1);
    glVertex3f(j, ip, 0.0);

    glTexCoord2f(0, 1);
    glVertex3f(jp, ip, 0.0);
    glEnd();
}

void drawCeiling(int i, int j)
{
    i = i * proporzione;
    j = j * proporzione;
    int ip = i + proporzione;
    int jp = j + proporzione;

    glBindTexture(GL_TEXTURE_2D, textures[2]);

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0);
    glVertex3f(jp, i, proporzione);

    glTexCoord2f(1, 0);
    glVertex3f(j, i, proporzione);

    glTexCoord2f(1, 1);
    glVertex3f(j, ip, proporzione);

    glTexCoord2f(0, 1);
    glVertex3f(jp, ip, proporzione);
    glEnd();
}

void drawWall(int i, int j)
{

    i = i * proporzione;
    j = j * proporzione;
    int ip = i + proporzione;
    int jp = j + proporzione;
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glBegin(GL_QUADS);

    // Parte superiore dei muri => "coperchio cubo"
    glNormal3fv(normalUp);
    glTexCoord2f(1, 0);
    glVertex3f(j, i, proporzione);
    glTexCoord2f(1, 1);
    glVertex3f(j, ip, proporzione);
    glTexCoord2f(0, 1);
    glVertex3f(jp, ip, proporzione);
    glTexCoord2f(0, 0);
    glVertex3f(jp, i, proporzione);

    glNormal3fv(normalFront);
    glTexCoord2f(1, 1);
    glVertex3f(jp, ip, proporzione);
    glTexCoord2f(0, 1);
    glVertex3f(j, ip, proporzione);
    glTexCoord2f(0, 0);
    glVertex3f(j, ip, 0.0);
    glTexCoord2f(1, 0);
    glVertex3f(jp, ip, 0.0);

    glNormal3fv(normalLeft);
    glTexCoord2f(1, 1);
    glVertex3f(j, ip, proporzione);
    glTexCoord2f(0, 1);
    glVertex3f(j, i, proporzione);
    glTexCoord2f(0, 0);
    glVertex3f(j, i, 0.0);
    glTexCoord2f(1, 0);
    glVertex3f(j, ip, 0.0);

    // lato

    glNormal3fv(normalFront);
    glTexCoord2f(0, 0);
    glVertex3f(j, i, 0.0);
    glTexCoord2f(1, 0);
    glVertex3f(jp, i, 0.0);
    glTexCoord2f(1, 1);
    glVertex3f(jp, i, proporzione);
    glTexCoord2f(0, 1);
    glVertex3f(j, i, proporzione);

    // lato
    glNormal3fv(normalRight);
    glTexCoord2f(0, 1);
    glVertex3f(jp, ip, proporzione);
    glTexCoord2f(1, 1);
    glVertex3f(jp, i, proporzione);
    glTexCoord2f(1, 0);
    glVertex3f(jp, i, 0.0);
    glTexCoord2f(0, 0);
    glVertex3f(jp, ip, 0.0);
    glEnd();
}
void drawCube(int i)
{
    
    GLfloat x = 0.5, z = 1.5;

    glBindTexture(GL_TEXTURE_2D, textures[i]);
    glBegin(GL_QUADS);
    glNormal3fv(normalUp);
    glTexCoord2f(1, 0);
    glVertex3f(x, -x, x);
    glTexCoord2f(1, 1);
    glVertex3f(x, -x, z);
    glTexCoord2f(0, 1);
    glVertex3f(-x, -x, z);
    glTexCoord2f(0, 0);
    glVertex3f(-x, -x, x);

    glNormal3fv(normalFront);
    glTexCoord2f(1, 1);
    glVertex3f(-x, -x, x);
    glTexCoord2f(0, 1);
    glVertex3f(-x, -x, z);
    glTexCoord2f(0, 0);
    glVertex3f(-x, x, z);
    glTexCoord2f(1, 0);
    glVertex3f(-x, x, x);

    glNormal3fv(normalLeft);
    glTexCoord2f(1, 1);
    glVertex3f(-x, x, x);
    glTexCoord2f(0, 1);
    glVertex3f(-x, x, z);
    glTexCoord2f(0, 0);
    glVertex3f(x, x, z);
    glTexCoord2f(1, 0);
    glVertex3f(x, x, x);

    glNormal3fv(normalFront);
    glTexCoord2f(0, 0);
    glVertex3f(x, x, x);
    glTexCoord2f(1, 0);
    glVertex3f(x, x, z);
    glTexCoord2f(1, 1);
    glVertex3f(x, -x, z);
    glTexCoord2f(0, 1);
    glVertex3f(x, -x, x);

    glNormal3fv(normalFront);
    glTexCoord2f(0, 0);
    glVertex3f(x, -x, z);
    glTexCoord2f(1, 0);
    glVertex3f(-x, -x, z);
    glTexCoord2f(1, 1);
    glVertex3f(-x, x, z);
    glTexCoord2f(0, 1);
    glVertex3f(x, x, z);

    // lato
    glNormal3fv(normalRight);
    glTexCoord2f(0, 1);
    glVertex3f(x, -x, x);
    glTexCoord2f(1, 1);
    glVertex3f(-x, -x, x);
    glTexCoord2f(1, 0);
    glVertex3f(-x, x, x);
    glTexCoord2f(0, 0);
    glVertex3f(x, x, x);

    glEnd();
}

void trasformIntoSphere()
{
    glBindTexture(GL_TEXTURE_2D, textures[4]);

    glTranslatef(0, 0, 1);

    GLUquadric *q = gluNewQuadric();
    gluQuadricTexture(q, true);
    gluSphere(q, 1.08, 200, 200);
}

void handlerCube(float i, float j)
{

    glPushMatrix();

    glTranslatef((j * proporzione), (i * proporzione), 0.5);
    glRotatef(GLfloat(rotate), 0.0, 0.0, 0.1);

    if (win)
    {
        trasformIntoSphere();
    }
    else if(!win)
    {
        drawCube(3);
    }

    glPopMatrix();

    glutPostRedisplay();
}

void drawLabyrinth()
{
    for (unsigned int i = 0; i < dim; ++i)
    {
        for (unsigned int j = 0; j < dim; ++j)
        {
            drawFloor(i, j);
            if (eyeZ <= 1.0)
                drawCeiling(j, i);
            if (mappa[i][j] == 3)
            {
                drawWall(j, i);
            }
            else if (mappa[i][j] == 1 && !wall)
            {
                drawWall(j, i);
            }
            if (mappa[i][j] == 2)
            {
                handlerCube(j, i);
            }
        }
    }
}

// timer

void TimeRotation(int secondi)
{
    rotate = (rotate + 4) % 360;

    glutTimerFunc(secondi, TimeRotation, secondi);
}

void Timer(int sec)
{
    if (!win && !lose)
    {
        if (timer > 0)
        {
            timer--;
        }
        else
        {
            lose = true;
        }
    }
    if (lose || win)
    {
        if (timerCloseWindow > 0)
            timerCloseWindow--;
        else
            exit(0);
    }
    strTimer = to_string(timer);
    strTimerCloseWindow = to_string(timerCloseWindow);
    glutTimerFunc(sec, Timer, sec);
}