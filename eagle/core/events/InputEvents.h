//
// Created by Novak on 05/06/2019.
//

#ifndef EAGLE_INPUTEVENTS_H
#define EAGLE_INPUTEVENTS_H

EG_BEGIN

struct OnMouseMove {
    double x, y;
};

struct OnMouseButton {
    int key, action, mods;
};

struct OnMouseScrolled {
    float x, y;
};

struct OnKey {
    int key, action, mods;
};

struct OnKeyTyped {
    unsigned int key;

};

EG_END

#endif //EAGLE_INPUTEVENTS_H
