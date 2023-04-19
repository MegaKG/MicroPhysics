#include <graphics.h>
#include <stdio.h>
#include <stdlib.h>
#include "MicroPhysics_OpenCL.h"
#include <iostream>

#undef printf
int main(){
    //Surface of the Moon
    Body* Platform = NewBody(0,0,0,1000,1,0,0,1,0);
    printf("Init Platform %p Size of %li\n",Platform,sizeof(*Platform));
    Body* Person = NewBody(1,50,30,6,3,0,0,0,0);
    printf("Init Person %p Size of %li\n",Person,sizeof(*Person));


    Engine* MyEngine = new Engine(2);
    printf("Init Engine %p Size of %li\n",MyEngine,sizeof(*MyEngine));

    MyEngine->insertBody(Platform);
    MyEngine->insertBody(Person);
    printf("Inserted Bodies into Engine\n");

    int gd = DETECT, gm;
    initgraph(&gd, &gm, NULL);
    //printf("Init Screen\n");


    while (true){
        //printf("-------------[FRAME]------------\n");
        //Platform->PrettyPrint();
        //printf("\n");
        //Person->PrettyPrint();

        MyEngine->tick(0.1);

        cleardevice();
        
        //Render Platform
        Platform = MyEngine->getBody(0);
        line(0,Platform->Y,getmaxx(),Platform->Y);

        //Render Person
        Person = MyEngine->getBody(1);
        circle(Person->X,Person->Y,Person->Radius);

        //if (grgetch() == 'q'){
        //    break;
        //}
        
    }
    closegraph();

}