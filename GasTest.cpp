#include <iostream>
#include "MicroPhysics.h"
#define GasNum 50

double randomDouble(double Min, double Max){
    return (((double)random()/RAND_MAX) * (Max-Min)) + Min;
}

int main(){
    //Surface of the Moon
    Body* Platform = new Body(0,0,0,1000,1,0,0,1,0);
    
    Engine* MyEngine = new Engine(GasNum+1);
    
    MyEngine->insertBody(Platform);

    for (int i = 0; i < GasNum; i++){
        MyEngine->insertBody(new Body(i+1,randomDouble(-30,30),randomDouble(1,30),randomDouble(1,10),1,0,0,0,0));
    }


    while (true){
        printf("{FRAME}\n");
        Platform->jsonPrint();

        for (int i = 0; i < GasNum; i++){
            MyEngine->getBody(i+1)->jsonPrint();
        }
      
        MyEngine->tick(0.1);


        
    }
}