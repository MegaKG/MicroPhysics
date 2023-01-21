#include <iostream>
#include "MicroPhysics.h"

int main(){
    //Surface of the Moon
    Body* Platform = new Body(0,0,0,1000,1,0,0,1,0);
    
    Body* Person = new Body(1,0,30,6,3,0,0,0,0);

    Body* Person2 = new Body(2,10,30,2,1,0.5,0,0,0);
    


    Engine* MyEngine = new Engine(2);
    
    MyEngine->insertBody(Platform);
    MyEngine->insertBody(Person);
    MyEngine->insertBody(Person2);
    

    while (true){
        printf("{FRAME}\n");
        Platform->jsonPrint();
        //printf("\n");
        Person->jsonPrint();
        Person2->jsonPrint();

        MyEngine->tick(0.1);


        
    }
}