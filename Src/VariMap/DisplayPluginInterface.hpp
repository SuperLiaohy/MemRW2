//
// Created by liaohy on 1/5/26.
//

#pragma once

class VariComponent;
class DisplayPluginInterface {
public:
    DisplayPluginInterface(VariComponent* vari=nullptr):vari(vari){}
    virtual ~DisplayPluginInterface() = default;

    virtual void update() = 0;
    VariComponent* vari;

};