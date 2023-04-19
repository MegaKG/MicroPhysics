

__constant double GravityConstant = 0.00001;
__constant double ResistanceForce = 0;

typedef struct __attribute__ ((packed)) _Body {
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

    double Radius;
} CLBody;



double distanceFrom(CLBody Target, CLBody Peer){
    if ((Peer.WholeX == 1) || (Peer.WholeX == 1)){
        //Return the Absolute Distance Y
        
        return pow(pow(Peer.Y - Target.Y,2),0.5);
    }
    else if ((Peer.WholeY == 1) || (Target.WholeY == 1)){
        //Return the Absolute Distance X
        
        return pow(pow(Peer.X - Target.X,2),0.5);
    }
    else {
        return pow(pow(Peer.X - Target.X,2) + pow(Peer.Y - Target.Y,2),0.5);
    }
}

int signof(double Input){
    if (Input < 0){
        return -1;
    }
    return 1;
}

double sabs(double Input){
    return Input * signof(Input);
}

int willCollide(CLBody A, CLBody B, double TimeFrame){
    double distance = distanceFrom(A,B);
    double totalRadius = A.Radius + B.Radius;

    return (distance < totalRadius);
}

__kernel void doMaths(__global CLBody* MyBodies, __global double* TimeFrameIn, __global int* ListLengthIn){
    double TimeFrame = TimeFrameIn[0];
    int ListLength = (long)ListLengthIn[0];
    //printf("ATimeFrame %f ListLength %i\n",TimeFrame,ListLength);
    //printf("Start Obj\n");

    int BodySelector = get_global_id(0);

    double xforce, yforce;
    double dx, dy;
    double scalex, scaley;
    double force;
    double distance;

    force = 0;
    xforce = 0;
    yforce = 0;

    for (long PeerSelector = 0; PeerSelector < ListLength; PeerSelector++){
        //printf("peer %i\n",PeerSelector);
        if (MyBodies[BodySelector].ID != MyBodies[PeerSelector].ID){           
            distance = distanceFrom(MyBodies[BodySelector],MyBodies[PeerSelector]);
            //printf("Dist From %i to %i is %f\n",MyBodies[BodySelector].ID,MyBodies[PeerSelector].ID,distance);
            
            if (willCollide(MyBodies[BodySelector],MyBodies[PeerSelector],TimeFrame)){
                MyBodies[BodySelector].PendingXVelocity = (((MyBodies[BodySelector].Mass - MyBodies[PeerSelector].Mass)/(MyBodies[BodySelector].Mass + MyBodies[PeerSelector].Mass)) * MyBodies[BodySelector].XVelocity) + (((2 * MyBodies[PeerSelector].Mass)/(MyBodies[BodySelector].Mass + MyBodies[PeerSelector].Mass)) * MyBodies[PeerSelector].XVelocity);
                MyBodies[BodySelector].PendingYVelocity = (((MyBodies[BodySelector].Mass - MyBodies[PeerSelector].Mass)/(MyBodies[BodySelector].Mass + MyBodies[PeerSelector].Mass)) * MyBodies[BodySelector].YVelocity) + (((2 * MyBodies[PeerSelector].Mass)/(MyBodies[BodySelector].Mass + MyBodies[PeerSelector].Mass)) * MyBodies[PeerSelector].YVelocity);
                

            }
            else {
                // F = GMm/r^2
                force = (GravityConstant * MyBodies[BodySelector].Mass * MyBodies[PeerSelector].Mass) / pow(distance,2);
                
                //printf("Force %f\n",force);

                //Convert to Unit Vector - Use the distance X and Y as a unit vector for the force vector
                dx = MyBodies[PeerSelector].X - MyBodies[BodySelector].X;
                dy = MyBodies[PeerSelector].Y - MyBodies[BodySelector].Y;


                if ( (MyBodies[PeerSelector].WholeX == 1) || (MyBodies[BodySelector].WholeX == 1)){
                    scalex = 0;
                }
                else {
                    scalex = dx / distance;
                }
                if ( (MyBodies[PeerSelector].WholeY == 1) || (MyBodies[BodySelector].WholeY == 1)){
                    scaley = 0;
                }
                else {
                    scaley = dy / distance;
                }
                //printf("scaley %f scalex %f force %f\n",scaley,scalex,force);
                xforce += scalex * force;
                yforce += scaley * force;
                

            }
            
        }

    }

    //Convert to Unit Vector - Use the forces X and Y as a unit vector for the drag vector
    dx = xforce;
    dy = yforce;
    //printf("DX: %f DY %f\n",dx,dy);
    
    if ((dx != 0) || (dy != 0)){
        scalex = dx / pow(pow(dx,2) + pow(dy,2),0.5);
        scaley = dy / pow(pow(dx,2) + pow(dy,2),0.5);
        
        if (sabs(scalex* ResistanceForce) <= sabs(xforce)){
            xforce -= scalex * ResistanceForce;
        }
        else {
            xforce = 0;
        }

        if (sabs(scaley* ResistanceForce) <= sabs(yforce)){
            yforce -= scaley * ResistanceForce;
        }
        else {
            yforce = 0;
        }
        
    }
    //printf("OUT DX: %f DY %f\n",dx,dy);

    //Apply Accelerations to Velocity F=MA
    MyBodies[BodySelector].PendingXVelocity += (xforce/MyBodies[BodySelector].Mass)/TimeFrame;
            
    MyBodies[BodySelector].PendingYVelocity += (yforce/MyBodies[BodySelector].Mass)/TimeFrame;


}

__kernel void stepBodies(__global CLBody* Input, __global double* TimeFrameIn, __global double* ListLengthIn){
    double TimeFrame = TimeFrameIn[0];
    double ListLength = ListLengthIn[0];
    int myID = get_global_id(0);

    //printf("My Body %i X %f Y %f VX %f VY %f\n",Input[myID].ID,Input[myID].X,Input[myID].Y,Input[myID].XVelocity,Input[myID].YVelocity);
    //printf("TimeFrame %f ListLength %f\n",TimeFrame,ListLength);
    
    
    

    Input[myID].XVelocity = Input[myID].PendingXVelocity;
    Input[myID].YVelocity = Input[myID].PendingYVelocity;
    //printf("Step Velocity %f %f for %i\n",Input[myID].XVelocity,Input[myID].YVelocity,myID);

    if ((Input[myID].WholeX == 0) && (Input[myID].WholeY == 0)){
        Input[myID].X += (Input[myID].XVelocity * TimeFrame);
        Input[myID].Y += (Input[myID].YVelocity * TimeFrame);
    }
    else {
        //Fix our Velocities
        Input[myID].XVelocity = 0;
        Input[myID].YVelocity = 0;
    }
}