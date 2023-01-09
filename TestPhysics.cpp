#include <iostream>
#include "MicroPhysics.h"

int main(){
    //Surface of the Moon
    Body* Platform = new Body(0,0,0,1000,0,0,1,0);
    printf("Init Platform\n");
    Body* Person = new Body(1,0,10,6,0,0,0,0);
    printf("Init Person\n");


    Engine* MyEngine = new Engine(2);
    printf("Init Engine\n");

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