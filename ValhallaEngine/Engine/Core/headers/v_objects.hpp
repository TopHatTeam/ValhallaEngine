// -------------------------------------------------
//
//  MIT License
//
//  v_objects.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 10/12/26.
//
// -------------------------------------------------

#pragma once

namespace ValhallaEngine::VCore
{
    class AVObject
    {
    public:
        virtual ~AVObject();
        
        virtual bool Init(bool bRendered, bool bLowLevel);
    };

    class AVPawn : public AVObject
    {
    public:
        ~AVPawn() override;
        
        bool Init(bool bRendered, bool bLowLevel) override;
        
        virtual bool AssignedPlayer(bool bPlayer);
        
        virtual bool AssignedNPC(bool bNPC);
    };

    class AVPawnNPC : public AVPawn
    {
    public:
        ~AVPawnNPC();
        
    };
}
