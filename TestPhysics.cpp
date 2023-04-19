#include <iostream>
#include "MicroPhysics_OpenCL.h"

int main(){
    //Surface of the Moon
    Body* Platform = NewBody(0,0,0,1000,1,0,0,1,0);
    printf("Init Platform %p Size of %li\n",Platform,sizeof(*Platform));
    Body* Person = NewBody(1,0,30,6,3,0,0,0,0);
    printf("Init Person %p Size of %li\n",Person,sizeof(*Person));


    Engine* MyEngine = new Engine(2);
    printf("Init Engine %p Size of %li\n",MyEngine,sizeof(*MyEngine));

    MyEngine->insertBody(Platform);
    MyEngine->insertBody(Person);
    printf("Inserted Bodies into Engine\n");

    while (true){
        printf("-------------[FRAME]------------\n");
        PrettyPrint(MyEngine->getBody(0));
        printf("\n");
        PrettyPrint(MyEngine->getBody(1));
        printf("\n");
        MyEngine->tick(0.1);


        
    }
}