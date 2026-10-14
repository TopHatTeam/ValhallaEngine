// -------------------------------------------------
//
//  MIT License
//
//  v_material.hpp
//  Valhalla Engine Editor
//
//  Created by Andrew Skatzes on 10/13/26.
//
// ------------------------------------------------

#pragma once 

#include "v_types.hpp"

namespace ValhallaEngine::Material 
{
    enum class VMaterialType
    {
        VMT_UNDEFINED,
        VMT_DIFFUSE,
        VMT_SPECULAR,
        VMT_METALLIC, 
        VMT_ROUGHNESS,
        VMT_NORMAL,
        VMT_EMISSIVE,
        VMT_AMBIENT_OCCLUSION,
        VMT_HEIGHT,
        VMT_SUBSURFACE_SCATTERING 
    };

    enum class VMaterialBlendMode 
    {
        VMBM_UNDEFINED,
        VMBM_OPAQUE,
        VMBM_TRANSPARENT,
        VMBM_ADDITIVE
    };

    enum class VMaterialFunctionInputType
    {
        VMFIT_UNDEFINED,
        VMFIT_SCALAR, 
        VMFIT_VECTOR2,
        VMFIT_VECTOR3,
        VMFIT_VECTOR4,
        VMFIT_TEXTURE2D,
        VMFIT_TEXTURECUBE,
        VMFIT_BOOLEAN, 
    };

    enum class VMaterialTextureColorChannel
    {
        VMTCC_UNDEFINED,
        VMTCC_RED,
        VMTCC_GREEN,
        VMTCC_BLUE,
        VMTCC_ALPHA
    };

    struct VMaterialInput
    {
        class UMaterialFunction* Function;
        int OutputIndex;
        const char* InputName;

        bitfield MaskR:1;
        bitfield MaskG:1;
        bitfield MaskB:1;
        bitfield MaskA:1;

        bool IsConnected() const 
        {
            return Function != nullptr;
        }
    };

    class VMaterial 
    {
    public:
        VMaterialInput DiffuseColor;
        VMaterialInput SpecularColor;
        VMaterialInput Metallic;
        VMaterialInput Roughness;
        VMaterialInput EmissiveColor; 
        VMaterialInput AmbientOcclusion;
        VMaterialInput Height;
        VMaterialInput Normal;

    };

    class VMaterialFunction
    {
    public:
        virtual ~VMaterialFunction();

        virtual int CompileFunction();
        virtual const char* GetInputName(int InputIndex) const;
        virtual bool IsInputRequired(int InputIndex) const;

        virtual bool UsesLeftSidePins() const;
        virtual bool UsesRightSidePins() const;

        virtual uword GetWidth();
        virtual uword GetHeight();
    };

    class VMaterialFunctionAbs : public VMaterialFunction
    {
    public:
        VMaterialInput Input;

        ~VMaterialFunctionAbs() override;

        int CompileFunction() override;

        bool IsInputRequired(int InputIndex) const override;

        bool UsesLeftSidePins() const override
        {
            return true;
        }

        bool UsesRightSidePins() const override
        {
            return true;
        }
    };

    class VMaterialFunctionCeil : public VMaterialFunction
    {
    public:
        VMaterialInput Input;

        ~VMaterialFunctionCeil() override;

        int CompileFunction() override;

        bool IsInputRequired(int InputIndex) const override;

        bool UsesLeftSidePins() const override
        {
            return true;
        }

        bool UsesRightSidePins() const override
        {
            return true;
        }
    };

    class VMaterialFunctionClamp : public VMaterialFunction
    {
    public:
        VMaterialInput Input;
        VMaterialInput Max;
        VMaterialInput Min;

        ~VMaterialFunctionClamp() override;

        int CompileFunction() override;

        bool IsInputRequired(int InputIndex) const override;

        bool UsesLeftSidePins() const override
        {
            return true;
        }

        bool UsesRightSidePins() const override
        {
            return true;
        }
    };

    class VMaterialFunctionAdd : public VMaterialFunction 
    {
    public:
        VMaterialInput InputA;
        VMaterialInput InputB;

        ~VMaterialFunctionAdd() override;

        int CompileFunction() override;

        bool IsInputRequired(int InputIndex) const override;

        bool UsesLeftSidePins() const override 
        {
            return true;
        }

        bool UsesRightSidePins() const override
        {
            return true;
        }
    };

    class VMaterialFunctionMultiply : public VMaterialFunction
    {
    public:
        VMaterialInput InputA;
        VMaterialInput InputB;

        ~VMaterialFunctionMultiply() override;

        int CompileFunction() override;

        bool IsInputRequired(int InputIndex) const override;

        bool UsesLeftSidePins() const override
        {
            return true;
        }

        bool UsesRightSidePins() const override
        {
            return true;
        }
    };
}