#include <iostream>
#include "MicroPhysics_OpenCL.h"
#define GasNum 50

double randomDouble(double Min, double Max){
    return (((double)random()/RAND_MAX) * (Max-Min)) + Min;
}


int main(){
    //Surface of the Moon
    Body* Platform = NewBody(0,0,-32,1000,1,0,0,1,0);
    
    Engine* MyEngine = new Engine(GasNum+1);
    
    MyEngine->insertBody(Platform);

    for (int i = 0; i < GasNum; i++){
        MyEngine->insertBody(NewBody(i+1,randomDouble(-30,30),randomDouble(-30,30),randomDouble(1,10),1,randomDouble(-0.1,0.1),randomDouble(-0.1,0.1),0,0));
    }


    while (true){
        printf("{FRAME}\n");
        MyEngine->JsonBodies();
      
        MyEngine->tick(0.1);


        
    }
}
