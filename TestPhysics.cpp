#include <iostream>
#include "MicroPhysics.h"

int main(){
    //Surface of the Moon
    Body* Platform = new Body(0,0,0,1000,0,0,1,0);
    printf("Init Platform %p Size of %li\n",Platform,sizeof(*Platform));
    Body* Person = new Body(1,0,10,6,0,0,0,0);
    printf("Init Person %p Size of %li\n",Person,sizeof(*Person));


    Engine* MyEngine = new Engine(2);
    printf("Init Engine %p Size of %li\n",MyEngine,sizeof(*MyEngine));

    MyEngine->insertBody(Platform);
    MyEngine->insertBody(Person);
    printf("Inserted Bodies into Engine\n");

    while (true){
        printf("-------------[FRAME]------------\n");
        Platform->PrettyPrint();
        printf("\n");
        Person->PrettyPrint();

        MyEngine->tick(0.1);


        
    }
}