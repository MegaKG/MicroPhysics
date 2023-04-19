#include <iostream>
#include "MicroPhysics_OpenCL.h"

int main(){
    //Surface of the Moon
    Body* Platform = NewBody(0,0,0,1000,1,0,0,1,0);
    
    Body* Person = NewBody(1,0,30,6,3,0,0,0,0);

    Body* Person2 = NewBody(2,10,30,2,1,0.5,0,0,0);
    


    Engine* MyEngine = new Engine(3);
    
    MyEngine->insertBody(Platform);
    MyEngine->insertBody(Person);
    MyEngine->insertBody(Person2);
    

    while (true){
        printf("{FRAME}\n");
        MyEngine->JsonBodies();
        MyEngine->tick(0.1);        
    }
}