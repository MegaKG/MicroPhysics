#!/usr/bin/env python3
import sys
import turtle
import json

turtle.penup()
turtle.tracer(0,0)
turtle.ht()


while True:
    
    try:
        D = sys.stdin.buffer.readline().decode('utf-8').strip('\n').strip('\r')
        print(D)
    
        if D == '{FRAME}':
            turtle.update()
            turtle.clear()
        else:
            
                data = json.loads(D)
                X = data['Position'][0]*10
                Y = data['Position'][1]*10

                R = data['Radius']*10

                turtle.setpos(X,Y)
                turtle.pendown()
                turtle.circle(R)
                turtle.penup()
    except:
        pass
            
