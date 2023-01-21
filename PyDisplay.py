#!/usr/bin/env python3
import sys
import turtle
import json

turtle.penup()
turtle.tracer(0,0)
turtle.ht()
while True:
    D = sys.stdin.readline().strip('\n')
    print(D)
    if D == '':
        break
    else:
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
            
