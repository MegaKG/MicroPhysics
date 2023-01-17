//MicroPhysics 2D Physics Engine
//V1.0
//Copyright Kaelan Grainger 2023

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

//Constants
const double GravityConstant = 0.00001;
const double ResistanceForce = 0.0;

class Body {
    private:
        long ID;

        double X;
        double Y;

        double Mass;

        double XVelocity;
        double YVelocity;

        double PendingXVelocity;
        double PendingYVelocity;

        char WholeX; //Is an X Plane, locks the body in place
        char WholeY; //Is a Y Plane, locks the body in place
    
    public:
        Body(long ID, double X, double Y, double Mass, double XVelocity, double YVelocity, char WholeX, char WholeY){
            this->X = X;
            this->Y = Y;
            this->XVelocity = XVelocity;
            this->YVelocity = YVelocity;
            this->WholeX = WholeX;
            this->WholeY = WholeY;
            this->Mass = Mass;
            this->ID = ID;
        }

        void updateVelocity(double XCH, double YCH){
            PendingXVelocity += XCH;
            PendingYVelocity += YCH;
            printf("New Velocity %lf %lf\n",PendingXVelocity,PendingYVelocity);
        }

        void setVelocity(double XCH, double YCH){
            PendingXVelocity = XCH;
            PendingYVelocity = YCH;
            printf("Set Velocity %lf %lf\n",PendingXVelocity,PendingYVelocity);
        }

        long getID(){
            return this->ID;
        }

        double getX(){
            return this->X;
        }
        double getY(){
            return this->Y;
        }

        double getXV(){
            return this->XVelocity;
        }

        double getYV(){
            return this->YVelocity;
        }

        double getMass(){
            return this->Mass;
        }

        char isXPlane(){
            return this->WholeX;
        }

        char isYPlane(){
            return this->WholeY;
        }

        void tick(double TimeFrame){
            XVelocity = PendingXVelocity;
            YVelocity = PendingYVelocity;

            if ((WholeX == 0) && (WholeY == 0)){
                X += (XVelocity * TimeFrame);
                Y += (YVelocity * TimeFrame);
            }
            else {
                //Fix our Velocities
                XVelocity = 0;
                YVelocity = 0;
            }
            
        }

        

        void PrettyPrint(){
            printf("Object %li:\n\tPosition: (%lf,%lf)\n\tSpeed: %lf m/s\n\tMass: %lf\n",this->ID,this->X,this->Y,pow(pow(this->XVelocity,2) + pow(this->YVelocity,2),0.5),this->Mass);
        }

        
};

class Engine {
    private:
        long BodyCount;
        Body** MyBodies;

        int sign(double Input){
            if (Input < 0){
                return -1;
            }
            return 1;
        }

        /*double relativeSpeed(Body* A, Body* B){
            //How fast is A Going Relative to B
            //printf("Relative Speed %li -> %li is %lf\n",A->getID(),B->getID(),-1*(pow(pow(A->getXV(),2) + pow(A->getYV(),2),0.5) - pow(pow(B->getXV(),2) + pow(B->getYV(),2),0.5)));
            //return -1*(pow(pow(A->getXV(),2) + pow(A->getYV(),2),0.5) - pow(pow(B->getXV(),2) + pow(B->getYV(),2),0.5));

            if ((A->isXPlane()) || (B->isXPlane())){
                //Return the Absolute Distance Y
                return pow(pow(A->getYV() - B->getYV(),2),0.5) * sign(A->getY() - B->getY());;
            }
            else if ((A->isYPlane()) || (B->isYPlane())){
                //Return the Absolute Distance X
                return pow(pow(A->getXV() - B->getXV(),2),0.5) * sign(A->getX() - B->getX());
            }
            else {
                return pow(pow(A->getXV()-B->getXV(),2) + pow(A->getYV()-B->getYV(),2),0.5) * sign(A->getX() - B->getX()) * sign(A->getY() - B->getY());
            }
            //return 1;


        }*/

        int willCollide(Body* A, Body* B, double TimeFrame){
            
            if ((A->isXPlane()) || (B->isXPlane())){
                if (abs(A->getYV() - B->getYV())*TimeFrame > abs(A->getY() - B->getY()) ){
                    return 1;
                }
                return 0;
            }
            else if ((A->isYPlane()) || (B->isYPlane())){
                if (abs(A->getXV() - B->getXV())*TimeFrame > abs(A->getX() - B->getX()) ){
                    return 1;
                }
                return 0;
            }
            else {
                if ((abs(A->getYV() - B->getYV())*TimeFrame > abs(A->getY() - B->getY()) ) && (abs(A->getXV() - B->getXV())*TimeFrame > abs(A->getX() - B->getX()) )){
                    return 1;
                }
                return 0;
            }

        }

        double distanceFrom(Body* Target, Body* Peer){
            printf("A: (%lf,%lf) B: (%lf,%lf)\n",Peer->getX(),Peer->getY(),Target->getX(),Target->getY());
            if ((Peer->isXPlane()) || (Peer->isXPlane())){
                //Return the Absolute Distance Y
                
                return pow(pow(Peer->getY() - Target->getY(),2),0.5);
            }
            else if ((Peer->isYPlane()) || (Target->isYPlane())){
                //Return the Absolute Distance X
                
                return pow(pow(Peer->getX() - Target->getX(),2),0.5);
            }
            else {
                return pow(pow(Peer->getX() - Target->getX(),2) + pow(Peer->getY() - Target->getY(),2),0.5);
            }
        }

    public:
        Engine(long BodyCount){
            this->BodyCount = BodyCount;
            this->MyBodies = (Body**)malloc(sizeof(Body*) * BodyCount);
            for (long i = 0; i < this->BodyCount; i++){
                this->MyBodies[i] = NULL;
            }
        }

        void insertBody(Body* MyBody){
            for (long i = 0; i < this->BodyCount; i++){
                if (this->MyBodies[i] == NULL){
                    this->MyBodies[i] = MyBody;
                    break;
                }
            }
        }

        Body* getBody(long ID){
            for (long i = 0; i < BodyCount; i++){
                if (MyBodies[i] != NULL){
                    if (MyBodies[i]->getID() == ID){
                        return MyBodies[i];
                    }

                }
                
            }
            return NULL;
        }

        void ListBodies(){
            for (long i = 0; i < BodyCount; i++){
                if (MyBodies[i] != NULL){
                    printf("Body %li Pointer %p\n",MyBodies[i]->getID(),MyBodies[i]);
                }
            }
        }

        void tick(double TimeFrame){
            //ListBodies();
            
            double xforce, yforce;
            double dx, dy;
            double scalex, scaley;
            double force;
            double distance;

            for (long BodySelector = 0; BodySelector < BodyCount; BodySelector++){
                if (MyBodies[BodySelector] != NULL){
                    force = 0;
                    xforce = 0;
                    yforce = 0;

                    for (long PeerSelector = 0; PeerSelector < BodyCount; PeerSelector++){
                        //Ensure we are not looking at the same body as both a target and a peer
                        if (MyBodies[PeerSelector] != NULL){
                            
                            if (MyBodies[BodySelector]->getID() != MyBodies[PeerSelector]->getID()){
                                
                                distance = distanceFrom(MyBodies[BodySelector],MyBodies[PeerSelector]);
                                printf("distance %li -> %li is %lf\n",MyBodies[BodySelector]->getID(),MyBodies[PeerSelector]->getID(),distance);
                                if (willCollide(MyBodies[BodySelector],MyBodies[PeerSelector],TimeFrame)){
                                    printf("Collision!\n");
                                    //Body 1
                                    printf("Collision Inputs: A: %li XV %lf YV %lf M %lf B: %li XV %lf YV %lf M %lf\n",
                                    MyBodies[BodySelector]->getID(),MyBodies[BodySelector]->getXV(),MyBodies[BodySelector]->getYV(),MyBodies[BodySelector]->getMass(),
                                    MyBodies[PeerSelector]->getID(),MyBodies[PeerSelector]->getXV(),MyBodies[PeerSelector]->getYV(),MyBodies[PeerSelector]->getMass()
                                    );
                                    MyBodies[BodySelector]->setVelocity(
                                        (((MyBodies[BodySelector]->getMass() - MyBodies[PeerSelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[BodySelector]->getXV()) + (((2 * MyBodies[PeerSelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[PeerSelector]->getXV()),
                                        (((MyBodies[BodySelector]->getMass() - MyBodies[PeerSelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[BodySelector]->getYV()) + (((2 * MyBodies[PeerSelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[PeerSelector]->getYV())
                                    );

                                    printf("Collision Velocity: %lf %lf\n",
                                    (((MyBodies[BodySelector]->getMass() - MyBodies[PeerSelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[BodySelector]->getXV()) + (((2 * MyBodies[PeerSelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[PeerSelector]->getXV()),
                                        (((MyBodies[BodySelector]->getMass() - MyBodies[PeerSelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[BodySelector]->getYV()) + (((2 * MyBodies[PeerSelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[PeerSelector]->getYV())
                                    );

                                    //Body 2 is Updated on their cycle
                                    //MyBodies[PeerSelector]->setVelocity(
                                    //    (((2 * MyBodies[BodySelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[BodySelector]->getXV()) + (((MyBodies[PeerSelector]->getMass() - MyBodies[BodySelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[PeerSelector]->getXV()),
                                    //    (((2 * MyBodies[BodySelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[BodySelector]->getYV()) + (((MyBodies[PeerSelector]->getMass() - MyBodies[BodySelector]->getMass())/(MyBodies[BodySelector]->getMass() + MyBodies[PeerSelector]->getMass())) * MyBodies[PeerSelector]->getYV())
                                    //);

                                    


                                }
                                else {
                                    // F = GMm/r^2
                                    force = (GravityConstant * MyBodies[BodySelector]->getMass() * MyBodies[PeerSelector]->getMass()) / pow(distance,2);
                                    //printf("Found force %lf\n",force);

                                    //Convert to Unit Vector - Use the distance X and Y as a unit vector for the force vector
                                    dx = MyBodies[PeerSelector]->getX() - MyBodies[BodySelector]->getX();
                                    dy = MyBodies[PeerSelector]->getY() - MyBodies[BodySelector]->getY();
                                    //printf("dx %lf dy %lf\n",dx,dy);

                                    if (MyBodies[PeerSelector]->isXPlane() || MyBodies[BodySelector]->isXPlane()){
                                        scalex = 0;
                                    }
                                    else {
                                        scalex = dx / distance;
                                    }
                                    if (MyBodies[PeerSelector]->isYPlane() || MyBodies[BodySelector]->isYPlane()){
                                        scaley = 0;
                                    }
                                    else {
                                        scaley = dy / distance;
                                    }
                                    xforce += scalex * force;
                                    yforce += scaley * force;
                                    //printf("scalex %lf scaley %lf FX %lf FY %lf\n",scalex,scaley,scalex*force,scaley*force);
                                    //printf("XForce: %lf YForce: %lf\n",xforce,yforce);

                                }
                                
                            }
                        }
                    }

                    //Apply Drag

                    //Convert to Unit Vector - Use the forces X and Y as a unit vector for the drag vector
                    dx = xforce;
                    dy = yforce;
                    
                    if ((dx != 0) || (dy != 0)){
                        scalex = dx / pow(pow(dx,2) + pow(dy,2),0.5);
                        scaley = dy / pow(pow(dx,2) + pow(dy,2),0.5);
                        

                        xforce -= scalex * ResistanceForce;
                        yforce -= scaley * ResistanceForce;

                        
                    }

                    //Apply Accelerations to Velocity F=MA
                    MyBodies[BodySelector]->updateVelocity(
                            (xforce/MyBodies[BodySelector]->getMass())/TimeFrame,
                            (yforce/MyBodies[BodySelector]->getMass())/TimeFrame
                    );

                

                
                }
            }

            //Now Tick
            for (long BodySelector = 0; BodySelector < BodyCount; BodySelector++){
                if (MyBodies[BodySelector] != NULL){
                    MyBodies[BodySelector]->tick(TimeFrame); 
                }
            }


        }
    
};


