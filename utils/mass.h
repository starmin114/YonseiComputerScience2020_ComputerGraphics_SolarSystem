//
//  mass.h
//
//  Created by iklee on 2018. 5. 21.
//  Modified by iklee on 2019. 5. 15.
//  Copyright © 2018년 iklee. All rights reserved.
//

#ifndef mass_h
#define mass_h

#include <GL/glew.h>
#include <stdio.h>
#include <shader.h>


#define GRAVITY_ACCEL       (-5.0)

class Mass {
public:
    float m;     // mass
    float p[3];  // position
    float v[3];  // velocity
    float a[3];  // acceleration
    float force[3]; // force
    
    Mass(float m) {
        this->m = m;
        createBuffers();
        updateBuffers(); 
    };
    
    void setPosition(float x, float y, float z) {
        this->p[0] = x;
        this->p[1] = y;
        this->p[2] = z;
    };
    
    void setVelocity(float vx, float vy, float vz) {
        this->v[0] = vx;
        this->v[1] = vy;
        this->v[2] = vz;
    };
    
    void setAcceleration(float ax, float ay, float az) {
        this->a[0] = ax;
        this->a[1] = ay;
        this->a[2] = az;
    };
    
    void setColor(float r, float g, float b) {
        this->colors[0] = r;
        this->colors[1] = g;
        this->colors[2] = b;
    }
    
    void euler(float timeT, float deltaT, float fx, float fy, float fz) {
       
        force[0] = fx;
        force[1] = fy + (m * GRAVITY_ACCEL);
        force[2] = fz;
        
        for (int i = 0; i < 3; i++) {
            p[i] = p[i] + (v[i] * deltaT);
            v[i] = v[i] + (a[i] * deltaT);
            a[i] = force[i] / m;
        }
    };
    
    void draw(Shader *shader, float r, float g, float b) {
        setColor(r, g, b);
        updateBuffers();
        shader->use();
        glPointSize(3.0);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, 1);
        glBindVertexArray(0);
    }

private:
    unsigned int VAO;
    unsigned int VBO[2];      // VBO[0]: for position, VBO[1]: for color
    float colors[3];
    
    void createBuffers() {
        
        glGenVertexArrays(1, &VAO);
        glGenBuffers(2, VBO);
        
        glBindVertexArray(VAO);
        
        // reserve space for position attributes
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(p), 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // reserve space for color attributes
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(colors), 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
        
    }
    
    void updateBuffers() {
        
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(p), p);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(colors), colors);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
    };
    
};

#endif /* mass_hpp */
