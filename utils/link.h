//
//  link.h
//
//  Created by iklee on 2019. 4. 24..
//

#ifndef LINK_H
#define LINK_H

#include <cmath>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cube.h>
#include <shader.h>

using namespace std;

class Link {
public:
    string name;        // name of the link
    glm::vec3 color;      // color of the link
    bool isRoot;          // true if this link is the root link
    int nChild;           // number of children links
    Link **child;         // pointer to the children links
    glm::vec3 size;       // size of the link
    glm::vec3 offset;     // translational offset
    glm::vec3 trans1;     // begin translation vector (in terms of the parent's local origin)
    glm::vec3 trans2;     // end translation vector (in terms of the parent's local origin)
    glm::vec3 rot1;       // starting angles (x, y, z) of joint rotation (in radians)
    glm::vec3 rot2;       // ending angles (x, y, z) of joint rotation (in radians)
    glm::quat q1;         // starting quaternion converted from rot1
    glm::quat q2;         // ending quaternion converted from rot2
    Cube *cube;           // cube object for this link
    
    Link(string name, glm::vec3 color, bool isRoot, int nChild,
         glm::vec3 size, glm::vec3 offset,
         glm::vec3 trans1, glm::vec3 trans2,
         glm::vec3 rot1, glm::vec3 rot2) {     // angles in degree
        this->name = name;
        this->color = color;
        this->isRoot = isRoot;
        this->nChild = nChild;
        this->child = new Link*[nChild];
        for (int i = 0; i < nChild; i++) this->child[i] = NULL;
        this->size = size;
        this->offset = offset;
        this->trans1 = trans1;
        this->trans2 = trans2;
        this->rot1 = glm::radians(rot1);
        this->rot2 = glm::radians(rot2);
        this->q1 = glm::quat(rot1);
        this->q2 = glm::quat(rot2);
        //this->cube = new Cube(size[0], size[1], size[2]);
        this->cube = new Cube(offset[0], offset[1], offset[2], size[0], size[1], size[2]);
    };
    
    void draw(Shader *shader) {
        shader->use();
        shader->setVec3("objectColor", color);
        cube->draw(shader);
    }
    
    void print() {
        cout << "LINK[ " << name << "]" << endl;
    };
};


#endif /* link_hpp */
