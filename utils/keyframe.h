//
//  keyframe.h
//

#ifndef __KEYFRAME_H__
#define __KEYFRAME_H__

#include <iostream>
#include <cmath>


using namespace std;

class Key
{
public:
    float time;   // in seconds
    float value;
};

class KeyFraming
{
public:
    int nKeys;
    Key* keys;
    
    // constructor
    KeyFraming(int nKeys) {
        this->nKeys = nKeys;
        keys = new Key[nKeys];
    };
    
    // set k'th key
    void setKey(int k, float time, float value) {
        keys[k].time = time;
        keys[k].value = value;
    };
    
    // get value at time t by linear interpolation
    float getValLinear(float t) {
        float val = 0.0f;
        
        if (t > keys[nKeys-1].time) {
            cout << "ERROR: getValLinear time error, t:" << t
                << " end time:" << keys[nKeys-1].time << endl;
            exit(-1);
        }
        else if (t == keys[nKeys-1].time) {
            return keys[nKeys-1].value;
        }
        
        for (int k = 0; k < nKeys - 1; k++) {
            if (keys[k].time == t) return keys[k].value;
            if (t < keys[k+1].time) {
                val = ((t - keys[k].time) / (keys[k+1].time - keys[k].time)) *
                (keys[k+1].value - keys[k].value) + keys[k].value;
                break;
            }
        }
        
        return val;
    };
    
    // print the KeyFraming class object
    void print() {
        cout << "KeyFraming: nKeys = " << nKeys << endl;
        for (int i = 0; i < nKeys; i++) {
            cout << "  key[" << i << "] = " << keys[i].time << " " << keys[i].value << endl;
        }
    };
    
};


#endif //__KEYFRAME_H__
